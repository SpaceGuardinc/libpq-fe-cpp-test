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
            PGconn* getConnection() const;

        private:
            bool _connect();
            void _disconnect();
            PGconn* _getConnection() const;

            bool _haveConnection() const;

            // Строка подключения и указатель на подключение
            std::string conninfo_; ///< Строка подключения к базе данных.
            PGconn* conn_; ///< Указатель на подключение к базе данных.
            mutable std::mutex db_mutex_;
        };
	std::string formatPostgresConnString(const std::string& dbname, const std::string& user);
    }
}


#endif  // __IPGSQLDATABASE_HPP__

