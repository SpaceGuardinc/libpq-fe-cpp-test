#include "ITable.hpp"
#include "IPGSQLDatabase.hpp"
#include <libpq-fe.h>
#include <sstream>
#include "logger.hpp"

namespace ssec {
    namespace orm {
        template<typename T>
        void ITable<T>::createTable(
            const char* const instruction,
            const std::shared_ptr<IPGSQLDatabase>& conn
        ) {
            auto db = conn->getDatabase();  // Get shared_ptr to database

            // Ensure we extract the pointer to PGconn*
            PGconn* pg_conn = db.get();  // Extract PGconn* from shared_ptr

            conn->tr_lock();

            try {
                PGresult* res = PQexec(pg_conn, instruction);  // Use PGconn* here
                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    LOGGER_ERROR("Failed to create table: %s", PQerrorMessage(pg_conn));  // Pass PGconn* to PQerrorMessage
                    PQclear(res);
                    throw std::runtime_error("Failed to create table");
                }
                PQclear(res);
            } catch (const std::exception& e) {
                conn->tr_unlock();
                throw;
            }

            conn->tr_unlock();
        }

        template<typename T>
        void ITable<T>::insert(const std::vector<Field>& values) {
            auto db = getDatabase();
            PGconn* pg_conn = db.get();  // Extract PGconn* from shared_ptr
            std::stringstream ss;
            ss << "INSERT INTO " << getTable() << " (";

            size_t idx = 0;
            for (const auto& field : values) {
                ss << field.desc.name;
                if (idx != values.size() - 1) ss << ", ";
                idx++;
            }
            ss << ") VALUES (";
            for (size_t idx = 0; idx < values.size(); ++idx) {
                const auto& field = values[idx];
                if (field.desc.type == Field::FIELD_TYPE::INTEGER) {
                    ss << *static_cast<const int*>(field.value);  // Cast to int
                } else if (field.desc.type == Field::FIELD_TYPE::STRING) {
                    ss << "'" << static_cast<const std::string*>(field.value)->c_str() << "'";  // Cast to string
                }
                if (idx != values.size() - 1) ss << ", ";
            }
            ss << ")";

            PGresult* res = PQexec(pg_conn, ss.str().c_str());  // Use PGconn* here
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                LOGGER_ERROR("Failed to insert data: %s", PQerrorMessage(pg_conn));  // Pass PGconn* to PQerrorMessage
                PQclear(res);
                throw std::runtime_error("Failed to insert data");
            }

            PQclear(res);
        }
    }
}
