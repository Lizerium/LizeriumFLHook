/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 28 апреля 2026 14:27:20
 * Version: 1.0.476
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