namespace ssec {
    namespace orm {
        template<typename T>
        IPGTable<T>::~IPGTable() {}

        template<typename T>
        inline std::shared_ptr<T> IPGTable<T>::getDatabase() const {
            return conn_->getDatabase();
        }
    }
}

