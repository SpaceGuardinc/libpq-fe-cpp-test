namespace ssec {
    namespace orm {
        template<typename T>
        ITable<T>::~ITable() {}

        template<typename T>
        inline std::shared_ptr<T> ITable<T>::getDatabase() const {
            return conn_->getDatabase();
        }
    }
}

