#include "IPGSQLDatabase.hpp"
#include "misc.hpp"
#include "logger.hpp"
#include <iostream>
#include <vector>

namespace ssec {
    namespace orm {

        std::string formatPostgresConnString(const std::string& dbname, const std::string& user) {
            return "dbname=" + dbname + " user=" + user;
        }

        IPGSQLDatabase::IPGSQLDatabase(const std::string& conninfo)
            : conninfo_(conninfo), conn_(nullptr) {}

        IPGSQLDatabase::~IPGSQLDatabase() {
            _disconnect();  
        }

        bool IPGSQLDatabase::connect() {
            std::lock_guard<std::mutex> lg(db_mutex_);
            return _connect();
        }

        void IPGSQLDatabase::disconnect() {
            std::lock_guard<std::mutex> lg(db_mutex_);
            _disconnect();
        }

        std::vector<std::string> IPGSQLDatabase::executeQuery(const std::string& query) {
            std::lock_guard<std::mutex> lg(db_mutex_);

            PGconn* conn = PQconnectdb(conninfo_.c_str());
            if (PQstatus(conn) != CONNECTION_OK) {
                ssec::logger::instance().error("Connection to database failed: %s", PQerrorMessage(conn));
                PQfinish(conn);  
                throw std::runtime_error("Database connection failed");
            }

            PGresult* res = PQexec(conn, query.c_str());
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                ssec::logger::instance().error("Query execution failed: %s", PQerrorMessage(conn));
                PQclear(res);
                PQfinish(conn);  
                throw std::runtime_error("Query execution failed");
            }

            std::vector<std::string> result;
            int nFields = PQnfields(res);
            for (int i = 0; i < PQntuples(res); ++i) {
                std::string row;
                for (int j = 0; j < nFields; ++j) {
                    if (j > 0) row += ", ";
                    row += PQgetvalue(res, i, j);
                }
                result.push_back(row);
            }

            PQclear(res);
            PQfinish(conn);  

            return result;
        }

        void IPGSQLDatabase::createDatabase(bool rewrite) {
            if (rewrite) {
                executeQuery("DROP DATABASE IF EXISTS my_database");
            }
            executeQuery("CREATE DATABASE my_database");
        }

        void IPGSQLDatabase::removeDatabase() {
            executeQuery("DROP DATABASE IF EXISTS my_database");
        }

        std::shared_ptr<IDatabase<PGconn>> IPGSQLDatabase::getDatabase() const {
            return std::make_shared<IPGSQLDatabase>(conninfo_);
        }

        bool IPGSQLDatabase::haveDatabase() const {
            return _haveConnection();
        }

        bool IPGSQLDatabase::_connect() {
            if (_haveConnection()) {
                return true;
            }

            conn_ = PQconnectdb(conninfo_.c_str());

            if (PQstatus(conn_) != CONNECTION_OK) {
                ssec::logger::instance().error("Connection to database failed: %s", PQerrorMessage(conn_));
                return false;
            }

            return true;
        }

        void IPGSQLDatabase::_disconnect() {
            if (_haveConnection()) {
                PQfinish(conn_);
                conn_ = nullptr;
            }
        }

        bool IPGSQLDatabase::_haveConnection() const {
            return conn_ != nullptr;
        }
    }
}

