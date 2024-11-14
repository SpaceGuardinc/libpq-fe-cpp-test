        // Деструктор
        template <typename T>
        IDatabase<T>::~IDatabase() {}

        // Метод блокировки транзакции
        template<typename T>
        inline void IDatabase<T>::tr_lock() {
            tr_mutex_.lock();
        }

        // Метод разблокировки транзакции
        template<typename T>
        inline void IDatabase<T>::tr_unlock() {
            tr_mutex_.unlock();
        }

