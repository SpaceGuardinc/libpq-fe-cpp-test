#include "misc.hpp"
#include "IPGSQLDatabase.hpp"
#include <string>
#include <iostream>

int main() {
    //std::string host = "";
    std::string dbname = "test";
    std::string user = "spaceguard";
    //std::string password = "";

    //std::string conn_str = misc::formatPostgresConnString(host, dbname, user, password);
    std::string conn_str = misc::formatPostgresConnString(dbname, user);

    try {
        IPGSQLDatabase db;
	db.connect(conn_str);

	// Записать в переменную table_name
	std::string query = "SELECT * FROM table_name;";

	auto result = db.executeQuery(query);

	std::cout << "Query result: " << result << std::endl;

	db.disconnect();
	
    } catch (const std::exception& e) {
	    misc::os::logError("Error: " + std::string(e.what()));
    }

    return 0;
}



