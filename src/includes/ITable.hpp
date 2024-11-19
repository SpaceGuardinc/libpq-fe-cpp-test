#ifndef __ITABLE_HPP__
#define __ITABLE_HPP__


#include <memory>
#include <string>
#include <vector>
#include "IDatabase.hpp"
#include "IPGSQLDatabase.hpp"

namespace ssec {
    namespace orm {

        struct Field {
            using field_name_t = std::string;
            enum class FIELD_TYPE {
                STRING,
                INTEGER,
                DOUBLE,
                BOOLEAN,
            };
            using field_value_t = const void*;
            struct Desc {
                field_name_t name;
                FIELD_TYPE type;
            };
            Desc desc;
            field_value_t value;
        };

        template <typename T>
        class ITable {
        public:
            ITable(const std::string& table, const std::shared_ptr<IDatabase<T>>& conn)
                : table_(table), conn_(conn) {}
            virtual ~ITable();

            std::shared_ptr<T> getDatabase() const;

            // Define function signatures for createTable and insert
            void createTable(const char* const instruction, const std::shared_ptr<IPGSQLDatabase>& conn);
            void insert(const std::vector<Field>& values);

        protected:
            const std::string& getTable() const { return table_; }

        private:
            std::string table_;
            std::shared_ptr<IDatabase<T>> conn_;
        };
    }
}


#include "ITable.tpp"

#endif  // __ITABLE_HPP__

