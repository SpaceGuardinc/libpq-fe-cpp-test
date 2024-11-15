#ifndef __CORE_ORM_IPGTABLE_HPP__
#define __CORE_ORM_IPGTABLE_HPP__

#include "IPGSQLDatabase.hpp"
#include <vector>
#include <string>
#include <memory>


namespace ssec {
    namespace orm {
    
    }
}

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
class IPGTable {

protected:
    IPGTable(const std::string& table, const std::shared_ptr<IPGSQLDatabase>& conn) 
        : table_(table), conn_(conn) {}

    virtual ~IPGTable() = 0;

protected:
    static void createTable(const char* const instruction, const std::shared_ptr<IPGSQLDatabase>& conn);

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

    std::shared_ptr<IPGSQLDatabase> getDatabase() const;

private:
    std::string table_;
    std::shared_ptr<IPGSQLDatabase> conn_;
};

#include "IPGTable.tpp"

#endif  // __CORE_ORM_IPGTABLE_HPP__

