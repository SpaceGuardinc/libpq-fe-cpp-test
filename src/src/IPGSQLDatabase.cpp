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
			: conninfo_(conninfo) {}

		IPGSQLDatabase::~IPGSQLDatabase() {}

		std::vector<std::string> IPGSQLDatabase::executeQuery(const std::string& query) {
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


		std::shared_ptr<PGconn> IPGSQLDatabase::getDatabase() const {
			PGconn* conn = PQconnectdb(conninfo_.c_str());
			return std::shared_ptr<PGconn>();
		}

		bool IPGSQLDatabase::haveDatabase() const {
			PGconn* conn = PQconnectdb(conninfo_.c_str());
			if (PQstatus(conn) != CONNECTION_OK) {
				PQfinish(conn);
				return false;
			}
			PQfinish(conn);
			return true;
		}
	}
}

