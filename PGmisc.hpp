#ifndef PGMISC_HPP
#define PGMISC_HPP

#include <string>
#include <stdexcept>

namespace os {
    namespace misc {
        void removeFile(const std::string& path);

        void handleError(const std::string& msg);

        bool pathExists(const std::string& path);

        void logError(const std::string& errorMessage);
    	std::string formatPostgresConnString(const std::string& dbname, const std::string& user);
		/*
        std::string formatPostgresConnString(const std::string& host, const std::string& dbname, const std::string& user, const std::string& password);
	*/
    }
}

#endif // PGMISC_HPP

