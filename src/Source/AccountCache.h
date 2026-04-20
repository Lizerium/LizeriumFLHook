/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 16:23:13
 * Version: 1.0.468
 */

#pragma once
#include <unordered_map>
#include <string>
#include <mutex>

class AccountCache {
public:
    static AccountCache& Instance() {
        static AccountCache instance;
        return instance;
    }

    // Добавить или обновить игрока
    void AddOrUpdate(uint iClientID, const std::wstring& wscDir) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_[iClientID] = wscDir;
    }

    // Получить wscDir по клиенту
    bool Get(uint iClientID, std::wstring& outDir) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(iClientID);
        if (it != cache_.end()) {
            outDir = it->second;
            return true;
        }
        return false;
    }

    // Вывести содержимое кеша
    std::list<std::wstring> GetCacheList() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::list<std::wstring> result;
        for (const auto& kv : cache_) {
            result.push_back(std::to_wstring(kv.first) + L"|" + kv.second);
        }
        return result;
    }

    // Удалить при выходе игрока
    void Remove(uint iClientID) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(iClientID);
    }

private:
    AccountCache() = default;
    std::unordered_map<uint, std::wstring> cache_;
    std::mutex mutex_;
};
