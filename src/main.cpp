#include "misc/misc.hpp"
#include "includes/IPGSQLDatabase.hpp"
#include <string>
#include <iostream>
#include <vector>
#include "logger/logger.hpp"

int main() {
	ssec::logger::init_logger(ssec::logger::LEVEL::ERROR, ssec::logger::TYPE::TO_STDERR);
	//std::string host = "";
	std::string dbname = "test";
	std::string user = "spaceguard";
	//std::string password = "";

	//std::string conn_str = misc::formatPostgresConnString(host, dbname, user, password);
	std::string conn_str = os::misc::formatPostgresConnString(dbname, user);

	try {
		std::shared_ptr<ssec::orm::IPGSQLDatabase> db = std::make_shared<ssec::orm::IPGSQLDatabase>(conn_str);
		// Записать в переменную table_name
		std::string query = "SELECT * FROM public.users;";
		std::vector<std::string> result = db->executeQuery(query);

		for (const auto& str : result) {
			std::cout << str << std::endl;
		}
		db->disconnect();

	} catch (const std::exception& e) {
		auto& log = ssec::logger::instance();
		log.fatal("Unhandled exception: %s", e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
