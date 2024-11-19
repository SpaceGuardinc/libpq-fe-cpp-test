#include "misc.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>

namespace os {
	namespace misc {
			std::string formatPostgresConnString(const std::string& dbname, const std::string& user) {
			return " dbname=" + dbname + " user=" + user;
		}
		/*
		   std::string formatPostgresConnString(const std::string& host, const std::string& dbname, const std::string& user, const std::string& password) {
		   return "host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password;
		   */
	}
}

