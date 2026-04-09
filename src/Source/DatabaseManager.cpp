/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 апреля 2026 10:59:03
 * Version: 1.0.24
 */

#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager() : db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    if (db) sqlite3_close(db);
}

bool DatabaseManager::Init(const std::string& dbFile) {
    if (sqlite3_open(dbFile.c_str(), &db)) {
        std::cerr << "Ошибка открытия БД: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "identifier TEXT NOT NULL);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка SQL: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool DatabaseManager::InitLocalization() {
    const char* sqlLocales =
        "CREATE TABLE IF NOT EXISTS locales ("
        "code TEXT PRIMARY KEY,"
        "name TEXT NOT NULL);";

    const char* sqlUserLocales =
        "CREATE TABLE IF NOT EXISTS user_locales ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "identifier TEXT UNIQUE NOT NULL,"
        "locale_code TEXT NOT NULL,"
        "FOREIGN KEY(locale_code) REFERENCES locales(code));";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sqlLocales, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка создания таблицы locales: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, sqlUserLocales, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка создания таблицы user_locales: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    // Добавим базовые локали, если их ещё нет
    const char* insertDefaults =
        "INSERT OR IGNORE INTO locales(code, name) VALUES"
        "('ru','Русский'),"
        "('en','English');";

    if (sqlite3_exec(db, insertDefaults, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка добавления базовых локалей: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

std::string DatabaseManager::GetUserLocale(const std::string& identifier) {
    std::string result = "ru"; // по умолчанию
    std::string sql = "SELECT locale_code FROM user_locales WHERE identifier='" + identifier + "';";

    auto callback = [](void* data, int argc, char** argv, char**) -> int {
        if (argc > 0 && argv[0]) {
            *static_cast<std::string*>(data) = argv[0];
        }
        return 0;
        };

    sqlite3_exec(db, sql.c_str(), callback, &result, nullptr);
    return result;
}

bool DatabaseManager::SetUserLocale(const std::string& identifier, const std::string& locale) {
    // Проверка, существует ли локаль
    std::string checkLocaleSQL = "SELECT COUNT(*) FROM locales WHERE code='" + locale + "';";
    int exists = 0;
    auto callback = [](void* data, int argc, char** argv, char**) -> int {
        if (argc > 0 && argv[0]) {
            *static_cast<int*>(data) = std::stoi(argv[0]);
        }
        return 0;
        };
    if (sqlite3_exec(db, checkLocaleSQL.c_str(), callback, &exists, nullptr) != SQLITE_OK || exists == 0) {
        std::cerr << "Локаль " << locale << " не найдена в базе." << std::endl;
        return false;
    }

    // Вставка или обновление
    std::string sql =
        "INSERT INTO user_locales(identifier, locale_code) VALUES('" + identifier + "','" + locale + "') "
        "ON CONFLICT(identifier) DO UPDATE SET locale_code=excluded.locale_code;";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка установки локали: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool DatabaseManager::UserExists(const std::string& identifier) {
    std::string sql = "SELECT COUNT(*) FROM users WHERE identifier='" + identifier + "';";
    int count = 0;

    auto callback = [](void* data, int argc, char** argv, char**) -> int {
        if (argc > 0 && argv[0]) {
            *static_cast<int*>(data) = std::stoi(argv[0]);
        }
        return 0;
        };

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), callback, &count, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка проверки пользователя: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false; // лучше считать, что "нет" при ошибке
    }

    return count > 0;
}


bool DatabaseManager::InsertUser(const std::string& identifier) {
    std::string sql = "INSERT INTO users(identifier) VALUES('" + identifier + "');";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка вставки: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void DatabaseManager::PrintUsers() {
    const char* sql = "SELECT id, identifier FROM users;";
    auto callback = [](void*, int argc, char** argv, char** colNames) -> int {
        for (int i = 0; i < argc; i++) {
            std::cout << colNames[i] << "=" << (argv[i] ? argv[i] : "NULL") << " | ";
        }
        std::cout << std::endl;
        return 0;
        };

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, callback, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка SELECT: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}
