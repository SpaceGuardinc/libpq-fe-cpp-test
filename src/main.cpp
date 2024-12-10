/*
 *
 * This main.cpp accualy docs for my project
 *
 */

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

				using ITable<PGconn>::delet;
				using ITable<PGconn>::update;
				using ITable<PGconn>::insert;
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

		ssec::orm::UserTable table("users", db);

		/*
		 * examle how to use query

		std::string query = "SELECT * FROM public.users;";
		std::vector<std::string> result_query = db->executeQuery(query);

		for (const auto& str : result_query) {
			std::cout << str << std::endl;
		}

		*/

		/*
		 * examle how to use insert into

		 int id = 15;
		 std::string first_name = "Kate";
		 std::string second_name = "Vacum";

		 std::vector<ssec::orm::Field> values = {
		 ssec::orm::Field{"id", ssec::orm::Field::FIELD_TYPE::INTEGER, (const void*) &id},
		 ssec::orm::Field{"first_name", ssec::orm::Field::FIELD_TYPE::STRING, (const void*) first_name.c_str()},
		 ssec::orm::Field{"second_name", ssec::orm::Field::FIELD_TYPE::STRING, (const void*) second_name.c_str()}
		 };

		 table.insert(values);

*/


		/*
		 * example how to use update 

		 int id = 15;



		 std::vector<ssec::orm::Field> update_values = {
		 ssec::orm::Field{"first_name", ssec::orm::Field::FIELD_TYPE::STRING, (const void*)"Katrin"}
		 };

		 std::vector<ssec::orm::Field> conditions_for_update = {
		 ssec::orm::Field{"id", ssec::orm::Field::FIELD_TYPE::INTEGER, (const void*)&id}
		 };

		 table.update(update_values, conditions_for_update);
		 */


		/*
		 * example how to use delete


		int id = 15;
		std::vector<ssec::orm::Field> conditions_for_delete = {
			ssec::orm::Field{"id", ssec::orm::Field::FIELD_TYPE::INTEGER, (const void*)&id}
		};
		table.delet(conditions_for_delete);

		 */

		/*
		 * example how to use select

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

		 */



	} catch (const std::exception& e) {
		auto& log = ssec::logger::instance();
		log.fatal("Unhandled exception: %s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
