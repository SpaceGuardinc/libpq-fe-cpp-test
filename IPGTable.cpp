#include "IPGTable.hpp"
#include "IPGSQLDatabase.hpp"
#include <libpq-fe.h>
#include <sstream>
#include "logger/logger.hpp"

namespace ssec {
    namespace orm {
        void IPGTable::createTable(
            const char* const instruction,
            const std::shared_ptr<IPGSQLDatabase>& conn
        ) {
            auto db = conn->getDatabase();

            conn->tr_lock();

            try {
                PGresult* res = PQexec(db, instruction);
                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    LOGGER_ERROR("Failed to create table: %s", PQerrorMessage(db));
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

        void IPGTable::insert(const std::vector<Field>& values) {
            auto db = getDatabase();
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
                    ss << field.value.intValue;
                } else if (field.desc.type == Field::FIELD_TYPE::STRING) {
                    ss << "'" << field.value.strValue << "'";
                }
                if (idx != values.size() - 1) ss << ", ";
            }
            ss << ")";

            PGresult* res = PQexec(db, ss.str().c_str());
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                LOGGER_ERROR("Failed to insert data: %s", PQerrorMessage(db));
                PQclear(res);
                throw std::runtime_error("Failed to insert data");
            }

            PQclear(res);
        }
    }
}

