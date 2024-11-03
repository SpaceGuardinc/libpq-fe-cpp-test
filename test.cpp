#include <iostream>
#include <stdlib.h>
#include <libpq-fe.h>
#include <cstdint>

static void exit_nicely(PGconn *conn);
static void show_result(PGresult *res);

int main(int argc, char **argv){

	
	const char *conninfo;
	PGconn     *conn;
	PGresult   *res;
	int        nFields;
	int	   i,
		   j;
	const char *paramValues[1];
	int 	   paramLenght[1];
	int        paramFormants[1];
	uint32_t   binaryIntVal;

	if (argc > 1) {
		conninfo = argv[1];
	} else {
		conninfo = "dbname=test user=spaceguard";
	}

	std::cout << "Connection info: " << conninfo << std::endl;


	
	conn = PQconnectdb(conninfo);
	
	if (PQstatus(conn) != CONNECTION_OK){
		std::cerr << "%s" << PQerrorMessage(conn);
		//fprintf(stderr, "%s", PQerrorMessage(conn));
		exit_nicely(conn);
	} else {
		printf("You connected to DB\n");
	}

	res = PQexec(conn, "SET search_path = testlibpq3");
	if (PQresultStatus(res) != PGRES_COMMAND_OK){
		std::cerr << "SET failded:  %s" << PQerrorMessage(conn);
		//fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);	
	}	
	PQclear(res);

	paramValues[0] = "vasya";

	res = PQexecParams(conn,
			"SELECT * FROM public.users;",
			0, 
			NULL,
			NULL, 
			NULL,
			NULL,
			0);

	if (PQresultStatus(res) != PGRES_TUPLES_OK){
		std::cerr << "SELECT failed: %s" << PQerrorMessage(conn);
		//fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
	}
	
	show_result(res);
	

}




static void exit_nicely(PGconn *conn) {
	PQfinish(conn);
	exit(1);
}


static void show_result(PGresult *res){
	int i,
	    j;
	int i_fnum,
	    t_fnum,
	    b_fnum;
	
	i_fnum = PQfnumber(res, "id");
	t_fnum = PQfnumber(res, "first_name");
	b_fnum = PQfnumber(res, "second_name");

	for (i = 0; i < PQntuples(res); i++){
		char *iptr;
		char *tptr;
		char *bptr;
		int   blen;
		int   ival;

		iptr = PQgetvalue(res, i, i_fnum);
		tptr = PQgetvalue(res, i, t_fnum);
		bptr = PQgetvalue(res, i, b_fnum);

		std::cout << "ID: " << iptr << ", ";
		std::cout << "First name: " << tptr << ", ";
		std::cout << "Second name: " << bptr << std::endl;
	}
}
