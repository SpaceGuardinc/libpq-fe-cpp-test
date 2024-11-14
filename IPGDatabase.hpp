#ifndef __CORE_ORM_IDATABASE_HPP__
#define __CORE_ORM_IDATABASE_HPP__

#include <string>
#include <memory>
#include <mutex>

        /**
         * @brief Интерфейс объекта, описывающего базу данных.
         **/
        template<typename T>
        class IDatabase {

        /**
         * @name Конструкторы, операторы присваивания, деструктор.
         * @{
        **/

        protected:
            IDatabase() = default;

            IDatabase(const IDatabase&) = delete;
            IDatabase(IDatabase&&) = delete;

            IDatabase& operator=(const IDatabase&) = delete;
            IDatabase& operator=(IDatabase&&) = delete;

            virtual ~IDatabase() = 0;

        /**
         * @}
         **/

        public:
            /**
             * @brief Создать БД.
             * 
             * @param rewrite   разрешение на перезапись существующей БД.
             * 
             * @details thread-safe
             * 
             * @returns Указатель на внутренний коннектор БД.
            **/
            virtual void createDatabase(bool rewrite = false) = 0;

            /**
             * @brief Удалить БД.
             * 
             * @details thread-safe
            **/
            virtual void removeDatabase() = 0;

            /**
             * @brief Получить указатель на коннектор БД.
             * 
             * @details thread-safe
             * 
             * @returns Указатель на коннектор БД.
            **/
            virtual std::shared_ptr<T> getDatabase() const = 0;

            /**
             * @brief Проверить, существует ли БД.
             * 
             * @details thread-safe
             * 
             * @retval true     БД существует;
             * @retval false    БД не существует.
            **/
            virtual bool haveDatabase() const = 0;

        public:
            /**
             * @brief Блокировка для начала транзакции на объектном уровне в БД.
            **/
            void tr_lock();

            /**
             * @brief Разблокировка для окончания транзакции на объектном уровне в БД.
            **/
            void tr_unlock();

        private:
            /**
             * @brief Мьютекс для взаимодействия с транзакциями в БД.
            **/
            mutable std::mutex tr_mutex_;
        };

#include "IPGDatabase.tpp"

#endif  // __CORE_ORM_IDATABASE_HPP__

