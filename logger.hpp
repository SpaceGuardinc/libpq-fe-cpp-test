#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <cstdarg>

namespace ssec {
	class logger {
	public:
		enum class LEVEL {
			TRACE,
			DEBUG,
			INFO,
			WARNING,
			ERROR,
			FATAL,
		};
		enum class TYPE {
			TO_STDERR,
			TO_FILE,
			TO_SYSLOG
		};

	private:
		logger(LEVEL level, TYPE type, const std::string& path = "");

		logger(const logger&) = delete;
		logger(logger&&) = delete;

		logger& operator=(const logger&) = delete;
		logger& operator=(logger&&) = delete;

	public:
		~logger();

	public:
		static void init_logger(LEVEL lvl, TYPE type, const std::string& path = "");
		static void destroy_logger() noexcept;
		static logger& instance() { return *logger_; }

	public:
		#ifndef REPORT_DEV

		void log(LEVEL lvl, const char* format, ...);
		void trace(const char* format, ...);
		void debug(const char* format, ...);
		void info(const char* format, ...);
		void warning(const char* format, ...);
		void error(const char* format, ...);
		void fatal(const char* format, ...);

		#else

		void log(const char* file, int line, LEVEL lvl, const char* format, ...);
		void trace(const char* file, int line, const char* format, ...);
		void debug(const char* file, int line, const char* format, ...);
		void info(const char* file, int line, const char* format, ...);
		void warning(const char* file, int line, const char* format, ...);
		void error(const char* file, int line, const char* format, ...);
		void fatal(const char* file, int line, const char* format, ...);

		#endif

		LEVEL getLevel() const noexcept { return lvl_; }
		void setLevel(LEVEL lvl) noexcept { lvl_ = lvl; }

	public:
		static logger* logger_;

	private:
		LEVEL lvl_;
		TYPE type_;

		FILE* logfile_;
		const std::string path_;

	#ifdef REPORT_DEV
	private:
		static std::unordered_map<size_t, std::string> thread_ids;
		static std::mutex							   thread_ids_mutex;

	public:
		static void _thread_id_add(const std::string& name) {
			auto						thread_id = std::hash<std::thread::id> {}(std::this_thread::get_id());
			std::lock_guard<std::mutex> lg(logger::thread_ids_mutex);
			logger::thread_ids[thread_id] = name;
		}
	#endif

	public:
		static std::string logLvlToString(LEVEL lvl) noexcept;
		static LEVEL logLevelFromStr(const std::string& s);

		static const char* logLvlToAnsiColor(LEVEL lvl, bool start = true) noexcept;

		static int logLvlToSyslogLevel(LEVEL lvl) noexcept;

	public:
		#ifndef REPORT_DEV
		void logfmt(LEVEL lvl, const char* format, std::va_list args);
		#else
		void logfmt(const char* file, int line, LEVEL lvl, const char* format, std::va_list args);
		#endif
	};

	#ifndef REPORT_DEV

		#define LOGGER_LOG(priority, format, args...) logger::instance().log(priority, format, ##args)

		#define LOGGER_TRACE(format, args...) // logger::instance().trace(format, ##args)
		#define LOGGER_DEBUG(format, args...) logger::instance().debug(format, ##args)
		#define LOGGER_INFO(format, args...) logger::instance().info(format, ##args)
		#define LOGGER_WARNING(format, args...) logger::instance().warning(format, ##args)
		#define LOGGER_ERROR(format, args...) logger::instance().error(format, ##args)
		#define LOGGER_FATAL(format, args...) logger::instance().fatal(format, ##args)

		#define LOGGER_LOGFMT(priority, format, args) logger::instance().logfmt(priority, format, args)

		#define LOGGER_ADD_THREAD_ID(name) pthread_setname_np(pthread_self(), name)

	#else

		#define LOGGER_LOG(priority, format, args...) logger::instance().log(__FILE__, __LINE__, priority, format, ##args)

		#define LOGGER_TRACE(format, args...) logger::instance().trace(__FILE__, __LINE__, format, ##args)
		#define LOGGER_DEBUG(format, args...) logger::instance().debug(__FILE__, __LINE__, format, ##args)
		#define LOGGER_INFO(format, args...) logger::instance().info(__FILE__, __LINE__, format, ##args)
		#define LOGGER_WARNING(format, args...) logger::instance().warning(__FILE__, __LINE__, format, ##args)
		#define LOGGER_ERROR(format, args...) logger::instance().error(__FILE__, __LINE__, format, ##args)
		#define LOGGER_FATAL(format, args...) logger::instance().fatal(__FILE__, __LINE__, format, ##args)

		#define LOGGER_LOGFMT(priority, format, args) logger::instance().logfmt(__FILE__, __LINE__, priority, format, args)

		#define LOGGER_ADD_THREAD_ID(name) logger::_thread_id_add(name); pthread_setname_np(pthread_self(), name)

	#endif
}

#endif // __LOGGER_HPP__

