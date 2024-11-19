#ifndef PGMISC_HPP
#define PGMISC_HPP

#include <string>
#include <stdexcept>

namespace os {
    namespace misc {
    	std::string formatPostgresConnString(const std::string& dbname, const std::string& user);
		/*
        std::string formatPostgresConnString(const std::string& host, const std::string& dbname, const std::string& user, const std::string& password);
	*/
    }
}

#endif // PGMISC_HPP

