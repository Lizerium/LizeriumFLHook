/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 29 марта 2026 16:56:36
 * Version: 1.0.2
 */

#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include <fstream>
#include "FLHookCustomLibs/json.hpp" // nlohmann::json
using json = nlohmann::json;

struct LocalizationEntry {
    std::unordered_map<std::string, std::wstring> translations;
};

class LocalizationManager {
public:
    static LocalizationManager& Instance() {
        static LocalizationManager instance;
        return instance;
    }

    bool LoadFromFile(const std::string& path) {
        std::ifstream f(path.c_str());
        if (!f.is_open()) return false;

        json j;
        f >> j;

        std::lock_guard<std::mutex> lock(mutex_);
        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            LocalizationEntry entry;
            for (json::iterator jt = it.value().begin(); jt != it.value().end(); ++jt) {
                entry.translations[jt.key()] = stowsUTF8(jt.value());
            }
            entries_[it.key()] = entry;
        }

        return true;
    }

    std::wstring Get(uint iClientID, const std::string& key, const std::string& locale) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = entries_.find(key);
        if (it != entries_.end()) {
            auto lit = it->second.translations.find(locale);
            if (lit != it->second.translations.end())
                return lit->second;

            lit = it->second.translations.find("ru");
            if (lit != it->second.translations.end())
                return lit->second;
        }
        return L"";
    }

private:
    LocalizationManager() = default;
    std::unordered_map<std::string, LocalizationEntry> entries_;
    std::mutex mutex_;
};
