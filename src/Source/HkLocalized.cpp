/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 03 сентября 2026 07:19:01
 * Version: 1.0.601
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