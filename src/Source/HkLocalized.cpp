/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 июля 2026 12:57:45
 * Version: 1.0.560
 */

#include "FLHook/Hook.h"


wstring GetLocalized(uint iClientID, const string& wscKey)
{
	string localized = "en";
	if (iClientID != -1)
	{
		std::wstring playerIdentifier;
		bool stateExistId = AccountCache::Instance().Get(iClientID, playerIdentifier);
		if(stateExistId)
			localized = DatabaseManager::Instance().GetUserLocale(wstos(playerIdentifier));
	}

	return LocalizationManager::Instance().Get(iClientID, wscKey, localized);
}