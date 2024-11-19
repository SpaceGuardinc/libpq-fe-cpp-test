#ifndef __ITABLE_HPP__
#define __ITABLE_HPP__

#include "IDatabase.hpp"
#include <string>
#include <memory>
#include <vector>

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
        template<typename T>
        class ITable {
        protected:
            ITable(const std::string& table, const std::shared_ptr<IDatabase<T>>& conn) : table_(table), conn_(conn) {}

            virtual ~ITable() = 0;
        protected:
            static void createTable(const char* const instruction, const std::shared_ptr<IDatabase<T>>& conn);

        protected:
            void insert(const std::vector<Field>& values);
            void delet(const std::vector<Field>& conditions);
            void update(
                const std::vector<Field>& values,
                const std::vector<Field>& conditions
            );

            std::vector<std::vector<std::shared_ptr<void>>> select(
                const std::vector<Field::Desc>& fields,
                const std::vector<Field>& conditions = std::vector<Field>()
            );

        protected:
           const std::string& getTable() const { return table_; }
           std::shared_ptr<T> getDatabase() const;

        private:
           std::string table_;
           std::shared_ptr<IDatabase<T>> conn_;
        };

        template<>
        void ITable<SQLite::Database>::createTable(const char* const instruction, const std::shared_ptr<IDatabase<SQLite::Database>>& conn);

        template<>
        void ITable<SQLite::Database>::insert(const std::vector<Field>& values);

        template<>
        void ITable<SQLite::Database>::delet(const std::vector<Field>& conditions);

        template<>
        void ITable<SQLite::Database>::update(
            const std::vector<Field>& values,
            const std::vector<Field>& conditions
        );

        template<>
        std::vector<std::vector<std::shared_ptr<void>>> ITable<SQLite::Database>::select(
            const std::vector<Field::Desc>& fields,
            const std::vector<Field>& conditions
        );

        template class ITable<SQLite::Database>;
    }
}

#include "ITable.tpp"

#endif  // __ITABLE_HPP__

