#include "logger.hpp"

#include <syslog.h>

#include <cstring>
#include <memory>

namespace ssec {
    logger* logger::logger_ = NULL;
    void logger::init_logger(logger::LEVEL lvl, logger::TYPE type, const std::string& path) {
        if (!logger_)
            logger_ = new logger(lvl, type, path);
    }

    void logger::destroy_logger() noexcept {
        if (logger_)
            delete logger_;
    }
   logger::logger(logger::LEVEL lvl, logger::TYPE type, const std::string& path) : lvl_(lvl), type_(type), path_(path) {
        switch (type_) {
            case TYPE::TO_STDERR:
                logfile_ = stderr;
                break;
            case TYPE::TO_FILE:
                logfile_ = std::fopen(path.c_str(), "r");
                if (logfile_ == NULL) {
                    // throw LoggerException::FopenLoggerError("Cannot open file, errno: " + std::to_string(errno));
                }
                break;
            case TYPE::TO_SYSLOG:
                openlog("ARGUS_CGI", LOG_NDELAY | LOG_PID, LOG_DAEMON);
                break;
        }
    }

    logger::~logger() {
        switch (type_) {
            case TYPE::TO_STDERR:
                break;
            case TYPE::TO_FILE:
                std::fclose(logfile_);
                break;
            case TYPE::TO_SYSLOG:
                closelog();
                break;
        }
    }

    std::string logger::logLvlToString(logger::LEVEL lvl) noexcept {
            switch (lvl) 
            {
                case LEVEL::TRACE:
                    return "TRACE";
                case LEVEL::DEBUG:
                    return "DEBUG";
                case LEVEL::INFO:
                    return "INFO";
                case LEVEL::WARNING:
                    return "WARNING";
                case LEVEL::ERROR:
                    return "ERROR";
                case LEVEL::FATAL:
                    return "FATAL";
                default:
                    return "?";
            }
        }

    logger::LEVEL logger::logLevelFromStr(const std::string& s) {
        if (s == "TRACE")
            return LEVEL::TRACE;
        else if (s == "DEBUG")
            return LEVEL::DEBUG;
        else if (s == "INFO")
            return LEVEL::INFO;
        else if (s == "WARNING")
            return LEVEL::WARNING;
        else if (s == "ERROR")
            return LEVEL::ERROR;
        else if (s == "FATAL")
            return LEVEL::FATAL;
        else
            throw ;//LoggerException::UnknownLoggerLevelError();
    }

    const char* logger::logLvlToAnsiColor(logger::LEVEL lvl, bool start) noexcept {
        if (!start) {
            return "\033[0m";
        }

        switch (lvl) {
            case logger::LEVEL::TRACE:
                return "\x1b[35m";
            case logger::LEVEL::DEBUG:
                return "\x1b[36m";
            case logger::LEVEL::INFO:
                return "\x1b[32m";
            case logger::LEVEL::WARNING:
                return "\x1b[33m";
            case logger::LEVEL::ERROR:
                return "\x1b[31m";
            case logger::LEVEL::FATAL:
                return "\x1b[31m";
            default:
                return "?";
        }
    }

    int logger::logLvlToSyslogLevel(logger::LEVEL lvl) noexcept {
        switch (lvl) {
            case logger::LEVEL::TRACE:
            case logger::LEVEL::DEBUG:
                return LOG_DEBUG;

            case logger::LEVEL::INFO:
                return LOG_INFO;

            case logger::LEVEL::WARNING:
                return LOG_WARNING;

            case logger::LEVEL::ERROR:
                return LOG_ERR;

            case logger::LEVEL::FATAL:
                return LOG_CRIT;

            default:
                return LOG_ERR;
        }
    }

    static std::string timeToString(const time_t& time) {
        static const auto TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
        char time_char[30];
        struct tm time_tm;
        gmtime_r(&time, &time_tm);

        if (strftime(time_char, sizeof(time_char)-1, TIME_FORMAT, &time_tm))
            return time_char;

        throw ;//LoggerException::StrftimeLoggerError("strftime returned 0, errno:" + std::to_string(errno));
    }

    #ifndef REPORT_DEV

    void logger::logfmt(LEVEL lvl, const char* format, std::va_list args) {
        if (lvl >= lvl_)
        {
            timespec tp;
            clock_gettime(CLOCK_REALTIME, &tp);
            auto curr_t = timeToString(tp.tv_sec);

            int sz_add_info = std::snprintf(
                nullptr, 0, "%s[%s.%03li] [%-7s]: ",
                logLvlToAnsiColor(lvl),
                curr_t.c_str(),
                (tp.tv_nsec / (1000 * 1000)),
                logLvlToString(lvl).c_str()
            ) + 1;
            std::unique_ptr<char[]> buf_add_info(new char[sz_add_info]);
            std::snprintf(buf_add_info.get(), sz_add_info, "%s[%s.%03li] [%-7s]: ",
                logLvlToAnsiColor(lvl),
                curr_t.c_str(),
                (tp.tv_nsec / (1000 * 1000)),
                logLvlToString(lvl).c_str()
            );

            std::va_list args2;
            va_copy(args2, args);

            int sz_info = std::vsnprintf(nullptr, 0, format, args) + 1;
            std::unique_ptr<char[]> buf_info(new char[sz_info]);
            std::vsnprintf(buf_info.get(), sz_info, format, args2);

            int sz_end_line = std::snprintf(nullptr, 0, "%s\n", logLvlToAnsiColor(lvl, false)) + 1;
            std::unique_ptr<char[]> buf_end_line(new char[sz_end_line]);
            std::snprintf(buf_end_line.get(), sz_end_line, "%s\n", logLvlToAnsiColor(lvl, false));

            std::string output = std::string(buf_add_info.get(), buf_add_info.get() + sz_add_info - 1) +
                                    std::string(buf_info.get(), buf_info.get() + sz_info - 1) +
                                    std::string(buf_end_line.get(), buf_end_line.get() + sz_end_line - 1);

            switch (type_) 
            {
                case TYPE::TO_SYSLOG:
                    if (lvl >= LEVEL::WARNING) {
                        syslog(logLvlToSyslogLevel(lvl), output.c_str());
                    }
                    break;
                case TYPE::TO_STDERR:
                case TYPE::TO_FILE:
                    std::fprintf(logfile_, output.c_str());
                    std::fflush(logfile_);
                    break;
            }
        }
    }

    void logger::log(logger::LEVEL lvl, const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(lvl, format, args);
        va_end(args);
    }

    void logger::trace(const char* format, ...)  {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(logger::LEVEL::TRACE, format, args);
        va_end(args);
    }

    void logger::debug(const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(logger::LEVEL::DEBUG, format, args);
        va_end(args);
    }

    void logger::info(const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(logger::LEVEL::INFO, format, args);
        va_end(args);
    }

    void logger::warning(const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(logger::LEVEL::WARNING, format, args);
        va_end(args);
    }

    void logger::error(const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(logger::LEVEL::ERROR, format, args);
        va_end(args);
    }

    void logger::fatal(const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(logger::LEVEL::FATAL, format, args);
        va_end(args);
    }

    #else

    void logger::logfmt(const char* file, int line, LEVEL lvl, const char* format, std::va_list args) {
        if (lvl >= lvl_)
        {
            timespec tp;
            clock_gettime(CLOCK_REALTIME, &tp);
            auto curr_t = timeToString(tp.tv_sec);

            std::string sfile(file);
            sfile = sfile.substr(sfile.rfind("/")) + ":" + std::to_string(line);

            auto thread_id = std::hash<std::thread::id> {}(std::this_thread::get_id());
            {
                std::lock_guard<std::mutex> lg(logger::thread_ids_mutex);
                if (logger::thread_ids.find(thread_id) == logger::thread_ids.cend()) {
                    static size_t counter		  = 0;
                    logger::thread_ids[thread_id] = std::to_string(counter++);
                }
            }

            int sz_add_info = std::snprintf(
                nullptr, 0, "%s[%s.%03li] [%-7s] {%-25s} {THREAD_ID: %-15.15s}: ",
                logLvlToAnsiColor(lvl),
                curr_t.c_str(),
                (tp.tv_nsec / (1000 * 1000)),
                logLvlToString(lvl).c_str(),
                sfile.c_str(),
                logger::thread_ids[thread_id].c_str()
            ) + 1;
            
            std::unique_ptr<char[]> buf_add_info(new char[sz_add_info]);
            std::snprintf(buf_add_info.get(), sz_add_info, "%s[%s.%03li] [%-7s] {%-25s} {THREAD_ID: %-15.15s}: ",
                logLvlToAnsiColor(lvl),
                curr_t.c_str(),
                (tp.tv_nsec / (1000 * 1000)),
                logLvlToString(lvl).c_str(),
                sfile.c_str(),
                logger::thread_ids[thread_id].c_str()
            );

            std::va_list args2;
            va_copy(args2, args);

            int sz_info = std::vsnprintf(nullptr, 0, format, args) + 1;

            std::unique_ptr<char[]> buf_info(new char[sz_info]);
            std::vsnprintf(buf_info.get(), sz_info, format, args2);

            int sz_end_line = std::snprintf(nullptr, 0, "%s\n", logLvlToAnsiColor(lvl, false)) + 1;
            std::unique_ptr<char[]> buf_end_line(new char[sz_end_line]);
            std::snprintf(buf_end_line.get(), sz_end_line, "%s\n", logLvlToAnsiColor(lvl, false));

            std::string output = std::string(buf_add_info.get(), buf_add_info.get() + sz_add_info - 1) +
                                    std::string(buf_info.get(), buf_info.get() + sz_info - 1) +
                                    std::string(buf_end_line.get(), buf_end_line.get() + sz_end_line - 1);

            switch (type_) 
            {
                case TYPE::TO_SYSLOG: {
                    if (lvl >= LEVEL::WARNING) {
                        syslog(logLvlToSyslogLevel(lvl), output.c_str());
                    }
                    break;
                }
                case TYPE::TO_STDERR:
                case TYPE::TO_FILE: {
                    std::fprintf(logfile_, output.c_str());
                    std::fflush(logfile_);
                    break;
                }
            }
        }
    }

    void logger::log(const char* file, int line, logger::LEVEL lvl, const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(file, line, lvl, format, args);
        va_end(args);
    }

    void logger::trace(const char* file, int line, const char* format, ...)  {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(file, line, logger::LEVEL::TRACE, format, args);
        va_end(args);
    }

    void logger::debug(const char* file, int line, const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(file, line, logger::LEVEL::DEBUG, format, args);
        va_end(args);
    }

    void logger::info(const char* file, int line, const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(file, line, logger::LEVEL::INFO, format, args);
        va_end(args);
    }

    void logger::warning(const char* file, int line, const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(file, line, logger::LEVEL::WARNING, format, args);
        va_end(args);
    }

    void logger::error(const char* file, int line, const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(file, line, logger::LEVEL::ERROR, format, args);
        va_end(args);
    }

    void logger::fatal(const char* file, int line, const char* format, ...) {
        std::va_list args;
        va_start(args, format);
        logger::logfmt(file, line, logger::LEVEL::FATAL, format, args);
        va_end(args);
    }
    #endif
}

