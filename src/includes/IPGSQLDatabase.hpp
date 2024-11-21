#ifndef __IPGSQLDATABASE_HPP__
#define __IPGSQLDATABASE_HPP__

#include "IDatabase.hpp"
#include <libpq-fe.h>
#include <memory>
#include <string>
#include <mutex>
#include <vector>

namespace ssec {
    namespace orm {
        class IPGSQLDatabase : public IDatabase<PGconn> {
        public:
            IPGSQLDatabase(const std::string& conninfo_);
            virtual ~IPGSQLDatabase();
            void createDatabase(bool rewrite = false) override;
            void removeDatabase() override;
            std::shared_ptr<IDatabase<PGconn>> getDatabase() const override;
            bool haveDatabase() const override;
            bool connect();
            void disconnect();
            static std::string formatPostgresConnString(const std::string& dbname, const std::string& user);
            std::vector<std::string> executeQuery(const std::string& query);

        private:
            bool _connect();
            void _disconnect();
            bool _haveConnection() const;

            std::string conninfo_; 
            PGconn* conn_; 
            mutable std::mutex db_mutex_;
        };
	std::string formatPostgresConnString(const std::string& dbname, const std::string& user);
    }
}

#endif  // __IPGSQLDATABASE_HPP__

