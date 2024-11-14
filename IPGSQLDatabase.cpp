#include "IPGSQLDatabase.hpp"
#include "misc.hpp"

        IPGSQLDatabase::IPGSQLDatabase(const std::string& conninfo)
            : IDatabase<PGconn>(), conninfo_(conninfo) {
            if (!_haveConnection()) {
                _connect();
            }
        }

        IPGSQLDatabase::~IPGSQLDatabase() {
            disconnect();
        }

        bool IPGSQLDatabase::connect() {
            std::lock_guard<std::mutex> lg(db_mutex_);
            return _connect();
        }

        void IPGSQLDatabase::disconnect() {
            std::lock_guard<std::mutex> lg(db_mutex_);
            _disconnect();
        }

        void IPGSQLDatabase::executeQuery(const std::string& query) {
            std::lock_guard<std::mutex> lg(db_mutex_);
            if (!_haveConnection()) {
                throw std::runtime_error("Database not connected");
            }

            PGresult* res = PQexec(conn_, query.c_str());

            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                misc::handleError("Query execution failed: " + std::string(PQerrorMessage(conn_)));
                PQclear(res);
                return;
            }

            int nFields = PQnfields(res);
            for (int i = 0; i < PQntuples(res); ++i) {
                for (int j = 0; j < nFields; ++j) {
                    std::cout << PQgetvalue(res, i, j) << " ";
                }
                std::cout << std::endl;
            }

            PQclear(res);
        }

        PGconn* IPGSQLDatabase::getConnection() const {
            return _getConnection();
        }

        bool IPGSQLDatabase::_connect() {
            if (_haveConnection()) {
                return true;
            }

            conn_ = PQconnectdb(conninfo_.c_str());

            if (PQstatus(conn_) != CONNECTION_OK) {
                misc::handleError("Connection to database failed: " + std::string(PQerrorMessage(conn_)));
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

        PGconn* IPGSQLDatabase::_getConnection() const {
            return conn_;
        }

        bool IPGSQLDatabase::_haveConnection() const {
            return conn_ != nullptr;
        }

