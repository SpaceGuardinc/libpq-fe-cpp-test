namespace ssec {
    namespace orm {

        template <typename T>
        IDatabase<T>::~IDatabase() {}

        template<typename T>
        inline void IDatabase<T>::tr_lock() {
            tr_mutex_.lock();
        }

        template<typename T>
        inline void IDatabase<T>::tr_unlock() {
            tr_mutex_.unlock();
        }
    }
}

