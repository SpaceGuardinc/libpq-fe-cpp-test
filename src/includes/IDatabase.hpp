#ifndef __IDATABASE_HPP__
#define __IDATABASE_HPP__

#include <libpq-fe.h>
#include <string>
#include <memory>
#include <mutex>

namespace ssec {
    namespace orm {
        template<typename T>
        class IDatabase {
        protected:
            IDatabase() = default;

            IDatabase(const IDatabase&) = delete;
            IDatabase(IDatabase&&) = delete;

            IDatabase& operator=(const IDatabase&) = delete;
            IDatabase& operator=(IDatabase&&) = delete;

            virtual ~IDatabase() = 0;
        public:
            virtual void createDatabase(bool rewrite = false) = 0;
            virtual void removeDatabase() = 0;
	    virtual std::shared_ptr<IDatabase<PGconn>> getDatabase() const = 0;
	    virtual bool haveDatabase() const = 0;

        public:
           void tr_lock();
           void tr_unlock();

        private:
           mutable std::mutex tr_mutex_;
        };
    }
}

#include "IDatabase.tpp"

#endif  // __IPGDATABASE_HPP__


