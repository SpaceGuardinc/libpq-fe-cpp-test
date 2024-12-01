#include "includes/IPGSQLDatabase.hpp"
#include "includes/ITable.hpp"
#include "logger/logger.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace ssec {
	namespace orm {
		class UserTable : public ITable<PGconn> {
			public:
				UserTable(const std::string& table, const std::shared_ptr<ssec::orm::IPGSQLDatabase>& conn)
					: ITable<PGconn>(table, conn) {}

				using ITable<PGconn>::select;
		};
	}
}

int main() {
	ssec::logger::init_logger(ssec::logger::LEVEL::ERROR, ssec::logger::TYPE::TO_STDERR);
	std::string dbname = "test";
	std::string user = "spaceguard";

	std::string conn_str = ssec::orm::formatPostgresConnString(dbname, user);

	try {
		std::shared_ptr<ssec::orm::IPGSQLDatabase> db = std::make_shared<ssec::orm::IPGSQLDatabase>(conn_str);

		/*
		 * examle how to use query
		 */

		std::string query = "SELECT * FROM public.users;";
		std::vector<std::string> result_query = db->executeQuery(query);

		for (const auto& str : result_query) {
			std::cout << str << std::endl;
		}


		/*
		 * example how to use select
		 */
		ssec::orm::UserTable table("users", db);

		std::vector<ssec::orm::Field> fields = {
			ssec::orm::Field{"id", ssec::orm::Field::FIELD_TYPE::INTEGER, nullptr},
			ssec::orm::Field{"first_name", ssec::orm::Field::FIELD_TYPE::STRING, nullptr},
			ssec::orm::Field{"second_name", ssec::orm::Field::FIELD_TYPE::STRING, nullptr}
		};

		std::vector<ssec::orm::Field> conditions = {};

		auto result = table.select(fields, conditions);

		for (const auto& row : result) {
			size_t column_index = 0;
			for (const auto& field_ptr : row) {
				switch (column_index) {
					case 0: { 
							int id = *std::static_pointer_cast<int>(field_ptr);
							std::cout << "ID: " << id << "\n";
							break;
						}
					case 1: { 
							std::string first_name = *std::static_pointer_cast<std::string>(field_ptr);
							std::cout << "First Name: " << first_name << "\n";
							break;
						}
					case 2: { 
							std::string last_name = *std::static_pointer_cast<std::string>(field_ptr);
							std::cout << "Last Name: " << last_name << "\n";
							break;
						}
					default:
						std::cerr << "Unexpected column index: " << column_index << "\n";
						break;
				}
				++column_index; 
			}
		}


	} catch (const std::exception& e) {
		auto& log = ssec::logger::instance();
		log.fatal("Unhandled exception: %s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
