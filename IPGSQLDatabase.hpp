/**
 * @file IPGSQLDatabase.hpp
 * @author mikhailmaslennikov
 * @brief Интерфейс объекта, описывающего работу с PostgreSQL БД.
 * @version 0.1
**/


#ifndef __CORE_ORM_IPGSQLDATABASE_HPP__
#define __CORE_ORM_IPGSQLDATABASE_HPP__

#include "IPGDatabase.hpp"
#include <libpq-fe.h>
#include <memory>
#include <string>
#include <mutex>


    /**
     * @brief Отношения между C++-классами и базами данных в ООП-паттерне.
     **/

        /**
         * @brief Интерфейс объекта, описывающего работу с PostgreSQL.
         **/
        class IPGSQLDatabase : public IDatabase<IPGSQLDatabase> {

        /**
         * @name Конструкторы, операторы присваивания, деструктор.
         * @{
         **/

        protected:
            IPGSQLDatabase(const std::string& conninfo);

        public:
            virtual ~IPGSQLDatabase() = 0;

        /**
         * @}
         **/

        public:
            /**
             * @brief Создать соединение с БД.
             * 
             * @details thread-safe
             * 
             * @returns true если соединение успешно установлено, false иначе.
             **/
            bool connect();

            /**
             * @brief Закрыть соединение с БД.
             * 
             * @details thread-safe
             **/
            void disconnect();

            /**
             * @brief Выполнить запрос к БД.
             * 
             * @param query SQL-запрос.
             * 
             * @details thread-safe
             **/
            void executeQuery(const std::string& query);

            /**
             * @brief Получить указатель на объект подключения к PostgreSQL.
             * 
             * @returns Указатель на объект подключения.
             **/
            PGconn* getConnection() const;

        private:
            /**
             * @brief Создать соединение с БД.
             * 
             * @returns true если соединение успешно, false иначе.
             * 
             * @warning non thread-safe
             **/
            bool _connect();

            /**
             * @brief Закрыть соединение с БД.
             * 
             * @warning non thread-safe
             **/
            void _disconnect();

            /**
             * @brief Получить указатель на объект подключения к PostgreSQL.
             * 
             * @returns Указатель на объект подключения.
             * 
             * @warning non thread-safe
             **/
            PGconn* _getConnection() const;

        private:
            /**
             * @brief Строка подключения к БД.
             **/
            std::string conninfo_;

            /**
             * @brief Указатель на подключение к PostgreSQL.
             **/
            PGconn* conn_;

            /**
             * @brief Мьютекс для синхронизации операций с БД.
             **/
            mutable std::mutex db_mutex_;
        };


#endif  // __CORE_ORM_IPGSQLDATABASE_HPP__

