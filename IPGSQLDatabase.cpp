#include "IPGSQLDatabase.hpp"
#include "PGmisc.hpp"
#include <iostream>
#include <vector>


namespace ssec {
	namespace orm {

		// Конструктор
		IPGSQLDatabase::IPGSQLDatabase(const std::string& conninfo)
			: IDatabase<PGconn>(), conninfo_(conninfo) {
				if (!_haveConnection()) {
					_connect();
				}
			}

		// Деструктор
		IPGSQLDatabase::~IPGSQLDatabase() {}

		// Функция подключения
		bool IPGSQLDatabase::connect() {
			std::lock_guard<std::mutex> lg(db_mutex_);
			return _connect();
		}

		// Функция отключения
		void IPGSQLDatabase::disconnect() {
			std::lock_guard<std::mutex> lg(db_mutex_);
			_disconnect();
		}

		// Выполнение запроса
		std::vector<std::string> IPGSQLDatabase::executeQuery(const std::string& query) {
			std::lock_guard<std::mutex> lg(db_mutex_);
			if (!_haveConnection()) {
				throw std::runtime_error("Database not connected");
			}

			PGresult* res = PQexec(conn_, query.c_str());
			if (PQresultStatus(res) != PGRES_TUPLES_OK) {
				os::misc::handleError("Query execution failed: " + std::string(PQerrorMessage(conn_)));
				PQclear(res);
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
			return result;
		}


		// Получение подключения
		PGconn* IPGSQLDatabase::getConnection() const {
			return _getConnection();
		}

		// Реализация функций интерфейса
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

		// Внутреннее подключение
		bool IPGSQLDatabase::_connect() {
			if (_haveConnection()) {
				return true;
			}

			conn_ = PQconnectdb(conninfo_.c_str());

			if (PQstatus(conn_) != CONNECTION_OK) {
				os::misc::handleError("Connection to database failed: " + std::string(PQerrorMessage(conn_)));
				return false;
			}

			return true;
		}

		// Отключение
		void IPGSQLDatabase::_disconnect() {
			if (_haveConnection()) {
				PQfinish(conn_);
				conn_ = nullptr;
			}
		}

		// Получение подключения
		PGconn* IPGSQLDatabase::_getConnection() const {
			return conn_;
		}

		// Проверка наличия подключения
		bool IPGSQLDatabase::_haveConnection() const {
			return conn_ != nullptr;
		}
	}
}

