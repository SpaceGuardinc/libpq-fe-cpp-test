// ipgqsldatabase.hpp
#ifndef IPGQSLDATABASE_HPP
#define IPGQSLDATABASE_HPP

#include <iostream>
#include <libpq-fe.h>
#include <string>

class IPGQSLDatabase {
public:
    IPGQSLDatabase(const std::string& configFile);
    ~IPGQSLDatabase();

    void connect();
    void executeQuery(const std::string& query);
    void showResult();

protected:
    void exitNicely();

private:
    PGconn* conn;
    PGresult* res;
    std::string dbname;
    std::string user;

    static std::string readConfig(const std::string& filename, const std::string& key);
};

#endif // IPGQSLDATABASE_HPP

