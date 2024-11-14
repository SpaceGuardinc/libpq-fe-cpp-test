#ifndef IPGSQLDATABASE_HPP
#define IPGSQLDATABASE_HPP

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <libpq-fe.h>
#include "IPGDatabase.hpp"
#include "misc.hpp"

        class IPGSQLDatabase : public IDatabase<IPGSQLDatabase> {
		
        protected:
            IPGSQLDatabase(const std::string& conninfo);

        public:
            virtual ~IPGSQLDatabase() = 0;
	    
        public:
            bool connect();
            void disconnect();
            void executeQuery(const std::string& query);
            PGconn* getConnection() const;

        private:
            bool _connect();
            void _disconnect();
            PGconn* _getConnection() const;

        private:
           std::string conninfo_;
           PGconn* conn_;
	   mutable std::mutex db_mutex_;
        };

#endif

