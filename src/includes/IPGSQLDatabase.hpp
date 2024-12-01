#ifndef __IPGSQLDATABASE_HPP__
#define __IPGSQLDATABASE_HPP__

#include "IDatabase.hpp"
#include <libpq-fe.h>
#include <memory>
#include <string>
#include <vector>

namespace ssec {
    namespace orm {
        class IPGSQLDatabase : public IDatabase<PGconn> {
        public:
            IPGSQLDatabase(const std::string& conninfo_);
            virtual ~IPGSQLDatabase();
            void createDatabase(bool rewrite = false) override;
            void removeDatabase() override;
            std::shared_ptr<PGconn> getDatabase() const override;
            bool haveDatabase() const override;
            std::vector<std::string> executeQuery(const std::string& query);

	    std::string getConnInfo() const { return conninfo_; }
        private:
            std::string conninfo_; 
        };

        std::string formatPostgresConnString(const std::string& dbname, const std::string& user);
    }
}

#endif  // __IPGSQLDATABASE_HPP__

