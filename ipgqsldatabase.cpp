// ipgqsldatabase.cpp
#include "ipgqsldatabase.hpp"
#include <fstream>
#include <sstream>

IPGQSLDatabase::IPGQSLDatabase(const std::string& configFile) : conn(nullptr), res(nullptr) {
    dbname = readConfig(configFile, "dbname");
    user = readConfig(configFile, "user");
}

IPGQSLDatabase::~IPGQSLDatabase() {
    if (conn != nullptr) {
        PQfinish(conn);
    }
}

void IPGQSLDatabase::connect() {
    std::string conninfo = "dbname=" + dbname + " user=" + user;
    conn = PQconnectdb(conninfo.c_str());
    
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn);
        exitNicely();
    } else {
        std::cout << "You connected to DB\n";
    }
}

void IPGQSLDatabase::executeQuery(const std::string& query) {
    res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn);
        PQclear(res);
        exitNicely();
    }
}

void IPGQSLDatabase::showResult() {
    int i_fnum = PQfnumber(res, "id");
    int t_fnum = PQfnumber(res, "first_name");
    int b_fnum = PQfnumber(res, "second_name");

    for (int i = 0; i < PQntuples(res); ++i) {
        char* iptr = PQgetvalue(res, i, i_fnum);
        char* tptr = PQgetvalue(res, i, t_fnum);
        char* bptr = PQgetvalue(res, i, b_fnum);

        std::cout << "ID: " << iptr << ", ";
        std::cout << "First name: " << tptr << ", ";
        std::cout << "Second name: " << bptr << std::endl;
    }
}

void IPGQSLDatabase::exitNicely() {
    if (res != nullptr) {
        PQclear(res);
    }
    PQfinish(conn);
    exit(1);
}

std::string IPGQSLDatabase::readConfig(const std::string& filename, const std::string& key) {
    std::ifstream configFile(filename);
    std::string line, foundValue;
    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        std::string k, v;
        if (std::getline(iss, k, '=') && std::getline(iss, v) && k == key) {
            foundValue = v;
            break;
        }
    }
    return foundValue;
}
