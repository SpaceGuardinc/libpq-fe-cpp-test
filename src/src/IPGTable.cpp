#include "ITable.hpp"
#include "IPGSQLDatabase.hpp"
#include "logger.hpp"
#include <sstream>
#include <stdexcept>
#include <libpq-fe.h>
#include <iostream>
#include <cstring>

namespace ssec {
	namespace orm {
		/*
		   void ITable::createTable(
		   const char* const instruction,
		   const std::shared_ptr<IPGSQLDatabase>& conn
		   ) {
		   auto db = conn->getDatabase();

		   conn->tr_lock();

		   try {
		   PGresult* res = PQexec(db, instruction);
		   if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		   LOGGER_ERROR("Failed to create table: %s", PQerrorMessage(db));
		   PQclear(res);
		   throw std::runtime_error("Failed to create table");
		   }
		   PQclear(res);
		   } catch (const std::exception& e) {
		   conn->tr_unlock();
		   throw;
		   }

		   conn->tr_unlock();
		   }
		   */

		template<>
			void ITable<PGconn>::insert(const std::vector<Field>& values) {
				auto ipgsql_conn = std::dynamic_pointer_cast<IPGSQLDatabase>(conn_);
				if (!ipgsql_conn) {
					throw std::runtime_error("Invalid database connection type");
				}

				PGconn* pg_conn = PQconnectdb(ipgsql_conn->getConnInfo().c_str());
				if (PQstatus(pg_conn) != CONNECTION_OK) {
					ssec::logger::instance().error("Connection to database failed: %s", PQerrorMessage(pg_conn));
					PQfinish(pg_conn);
					throw std::runtime_error("Database connection failed");
				}

				std::stringstream ss;
				ss << "INSERT INTO " << getTable() << " (";

				size_t idx = 0;
				for (const auto& field : values) {
					ss << field.desc.name;
					if (idx != values.size() - 1) {
						ss << ", ";
					}
					idx++;
				}

				ss << ") VALUES (";
				for (size_t i = 0; i < values.size(); i++) {
					ss << "$" << (i + 1);
					if (i != values.size() - 1) {
						ss << ", ";
					}
				}
				ss << ");";

				std::vector<const char*> param_values(values.size());
				idx = 0;
				for (const auto& field : values) {
					if (field.value) {
						switch (field.desc.type) {
							case Field::FIELD_TYPE::STRING:
								param_values[idx] = static_cast<const char*>(field.value);
								break;
							case Field::FIELD_TYPE::INTEGER: {
												 auto str = std::to_string(*static_cast<const int*>(field.value));
												 param_values[idx] = strdup(str.c_str());
												 break;
											 }
							case Field::FIELD_TYPE::DOUBLE: {
												auto str = std::to_string(*static_cast<const double*>(field.value));
												param_values[idx] = strdup(str.c_str());
												break;
											}
							case Field::FIELD_TYPE::BOOLEAN: {
												 auto str = std::to_string(*static_cast<const bool*>(field.value) ? 1 : 0);
												 param_values[idx] = strdup(str.c_str());
												 break;
											 }
							default:
											 throw std::runtime_error("Unsupported field type for insert");
						}
					} else {
						param_values[idx] = nullptr;
					}
					idx++;
				}

				PGresult* res = PQexecParams(
						pg_conn,
						ss.str().c_str(),
						values.size(),
						nullptr,
						param_values.data(),
						nullptr,
						nullptr,
						0
						);

				if (PQresultStatus(res) != PGRES_COMMAND_OK) {
					ssec::logger::instance().error("Insert query failed: %s", PQerrorMessage(pg_conn));
					PQclear(res);
					PQfinish(pg_conn);
					throw std::runtime_error("Insert query failed");
				}

				ssec::logger::instance().info("Insert query executed successfully");
				PQclear(res);
				PQfinish(pg_conn);

				/*
				for (auto& param : param_values) {
					if (param) {
						free(const_cast<char*>(param));
						param = nullptr;
					}
				}
				*/
			}



		template<>
			std::vector<std::vector<std::shared_ptr<void>>> ITable<PGconn>::select(
					const std::vector<Field>& fields,
					const std::vector<Field>& conditions
					) {
				auto ipgsql_conn = std::dynamic_pointer_cast<IPGSQLDatabase>(conn_);
				if (!ipgsql_conn) {
					throw std::runtime_error("Invalid database connection type");
				}

				PGconn* pg_conn = PQconnectdb(ipgsql_conn->getConnInfo().c_str());
				if (PQstatus(pg_conn) != CONNECTION_OK) {
					ssec::logger::instance().error("Connection to database failed: %s", PQerrorMessage(pg_conn));
					PQfinish(pg_conn);
					throw std::runtime_error("Database connection failed");
				} else {
					//std::cout << "Connection successful!" << std::endl;
				}

				std::stringstream ss;
				ss << "SELECT ";
				size_t idx = 0;
				for (const auto& field : fields) {
					ss << field.desc.name;
					if (idx != fields.size() - 1) {
						ss << ", ";
					}
					idx++;
				}

				ss << " FROM " << getTable();
				if (!conditions.empty()) {
					ss << " WHERE ";
					idx = 0;
					for (const auto& condition : conditions) {
						ss << condition.desc.name << " = $" << (idx + 1);
						if (idx != conditions.size() - 1) {
							ss << " AND ";
						}
						idx++;
					}
				}
				ss << ";";

				PGresult* res = PQexecParams(
						pg_conn,
						ss.str().c_str(),
						conditions.size(),
						nullptr,
						nullptr,
						nullptr,
						nullptr,
						0
						);

				if (PQresultStatus(res) != PGRES_TUPLES_OK) {
					ssec::logger::instance().error("Query execution failed: %s", PQerrorMessage(pg_conn));
					PQclear(res);
					PQfinish(pg_conn);
					throw std::runtime_error("Query execution failed");
				} else {
					//std::cout << "Query executed successfully, rows: " << PQntuples(res) << std::endl;
				}

				std::vector<std::vector<std::shared_ptr<void>>> result;
				int nRows = PQntuples(res);
				int nFields = PQnfields(res);

				if (nFields != fields.size()) {
					ssec::logger::instance().error("Field count does not match result columns");
					PQclear(res);
					PQfinish(pg_conn);
					throw std::runtime_error("Field count mismatch");
				}


				for (int i = 0; i < nRows; ++i) {
					std::vector<std::shared_ptr<void>> row;
					for (int j = 0; j < nFields; ++j) {
						std::shared_ptr<void> value = nullptr;
						if (!PQgetisnull(res, i, j)) {
							try {
								const char* field_value = PQgetvalue(res, i, j);
								if (field_value) {

									switch (fields[j].desc.type) {
										case Field::FIELD_TYPE::STRING: {
															value = std::make_shared<std::string>(field_value);
															break;
														}
										case Field::FIELD_TYPE::INTEGER: {
															 value = std::make_shared<int>(std::stoi(field_value));
															 break;
														 }
										case Field::FIELD_TYPE::DOUBLE: {
															value = std::make_shared<double>(std::stod(field_value));
															break;
														}
										case Field::FIELD_TYPE::BOOLEAN: {
															 value = std::make_shared<bool>(std::stoi(field_value) != 0);
															 break;
														 }
										default:
														 ssec::logger::instance().error("Unknown field type encountered in row %d, column %d", i, j);
														 break;
									}
								} else {
									ssec::logger::instance().error("Null value encountered in row %d, column %d", i, j);
								}
							} catch (const std::exception& e) {
								ssec::logger::instance().error("Error processing row %d, column %d: %s", i, j, e.what());
							}
						} else {
							ssec::logger::instance().error("Null value encountered in row %d, column %d", i, j);
						}

						row.push_back(value);
					}
					result.push_back(row);
				}

				PQclear(res);
				PQfinish(pg_conn);
				return result;
			}
	}
}

