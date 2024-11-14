#include "misc.hpp"
#include <iostream>
#include <fstream>

namespace misc {
    namespace os {
        void removeFile(const std::string& path) {
            if (std::remove(path.c_str()) != 0) {
                logError("Error removing file: " + path);
            }
        }

        bool pathExists(const std::string& path) {
            std::ifstream file(path);
            return file.good();
        }

        void logError(const std::string& errorMessage) {
            std::cerr << "ERROR: " << errorMessage << std::endl;
        }
    }

    std::string formatPostgresConnString(const std::string& dbname, const std::string& user) {
        return " dbname=" + dbname + " user=" + user;


    //std::string formatPostgresConnString(const std::string& host, const std::string& dbname, const std::string& user, const std::string& password) {
        //return "host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password;
    }
}


