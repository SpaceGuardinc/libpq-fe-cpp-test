#include "PGmisc.hpp"
#include "IPGSQLDatabase.hpp"
#include <string>
#include <iostream>
#include <vector>

int main() {
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
    	std::vector<std::string> result = db.executeQuery(query);

    	for (const auto& str : result) {
		std::cout << str << std::endl;
    	}
	db.disconnect();
	
    } catch (const std::exception& e) {
	    os::misc::logError("Error: " + std::string(e.what()));
    }

    return 0;
}
