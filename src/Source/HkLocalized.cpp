/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 26 апреля 2026 09:57:30
 * Version: 1.0.474
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