#ifndef MISC_HPP
#define MISC_HPP

#include <string>
#include <stdexcept>

namespace misc {
    namespace os {
        // Функция для удаления файла (можно адаптировать для работы с файлами конфигурации или лога)
        void removeFile(const std::string& path);

        // Функция для проверки существования файла
        bool pathExists(const std::string& path);
        
        // Функция для логирования ошибок
        void logError(const std::string& errorMessage);
    }

    // Функция для обработки строки подключения для PostgreSQL
    //std::string formatPostgresConnString(const std::string& host, const std::string& dbname, const std::string& user, const std::string& password);
    std::string formatPostgresConnString(const std::string& dbname, const std::string& user);
}

#endif // MISC_HPP

