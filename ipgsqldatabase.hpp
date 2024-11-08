// ipgqsldatabase.hpp
#ifndef IPGSQLDATABASE_HPP
#define IPGSQLDATABASE_HPP

#include <iostream>
#include <libpq-fe.h>
#include <string>

class IPGSQLDatabase {
public:
    IPGSQLDatabase(const std::string& configFile);
    ~IPGSQLDatabase();

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

#endif // IPGSQLDATABASE_HPP

