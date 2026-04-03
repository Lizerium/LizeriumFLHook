/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 03 апреля 2026 11:33:45
 * Version: 1.0.18
 */

#pragma once
#include "FLHookCustomLibs/sqlite3.h"
#include <string>

class DatabaseManager {
public:
    static DatabaseManager& Instance() {
        static DatabaseManager instance;
        return instance;
    }

    bool Init(const std::string& dbFile);
    bool InitLocalization();

    bool SetUserLocale(const std::string& identifier, const std::string& locale);
    std::string GetUserLocale(const std::string& identifier);
    bool InsertUser(const std::string& identifier);
    bool UserExists(const std::string& identifier);
    void PrintUsers();

private:
    DatabaseManager();  // приватный конструктор
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    sqlite3* db;
};