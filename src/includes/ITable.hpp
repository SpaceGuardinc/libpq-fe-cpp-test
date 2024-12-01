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
            virtual ~ITable() = 0;

        protected:
            std::vector<std::vector<std::shared_ptr<void>>> select(
                const std::vector<Field>& fields, 
                const std::vector<Field>& conditions
            );

            std::shared_ptr<T> getDatabase() const;

            void delet(const std::vector<Field>& conditions);
            void update(
                const std::vector<Field>& values,
                const std::vector<Field>& conditions
            );

            static void createTable(const char* const instruction, const std::shared_ptr<IDatabase<T>>& conn);
            void insert(const std::vector<Field>& values);

        protected:
            const std::string& getTable() const { return table_; }

        private:
            std::string table_;
            std::shared_ptr<IDatabase<T>> conn_;
        };

        template<>
        void ITable<IPGSQLDatabase>::createTable(const char* const instruction, const std::shared_ptr<IDatabase<IPGSQLDatabase>>& conn);

	template<>
        std::vector<std::vector<std::shared_ptr<void>>> ITable<PGconn>::select(
            const std::vector<ssec::orm::Field>& fields,
            const std::vector<ssec::orm::Field>& conditions
        );


    }
}
#include "ITable.tpp"

#endif  // __ITABLE_HPP__

