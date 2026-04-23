/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 апреля 2026 06:54:28
 * Version: 1.0.471
 */

// FLHook Plugin to hold a miscellaneous collection of commands and 
// other such things that don't fit into other plugins
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Main.h"

// A return code to indicate to FLHook if we want the hook processing to continue.
PLUGIN_RETURNCODE returncode;

void LoadSettings();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	srand(static_cast<uint>(time(nullptr)));
	// If we're being loaded from the command line while FLHook is running then
	// set_scCfgFile will not be empty so load the settings as FLHook only
	// calls load settings on FLHook startup and .rehash.
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (set_scCfgFile.length() > 0)
			LoadSettings();
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
	}
	return true;
}

/// Hook will call this function after calling a plugin function to see if we the
/// processing to continue
EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Structures and Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// См. Main.h для любых определений структуры/класса.
// Это только для деклараций


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Loading Settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;
	// Reserved for future use
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckIsInBase(uint iClientID)
{
	uint iBaseID;
	pub::Player::GetBase(iClientID, iBaseID);
	if (!iBaseID)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0773"));
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// /refresh - Обновляет временные метки файла персонажа для всех кораблей на учетной записи.
bool UserCmd_RefreshCharacters(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	if (!CheckIsInBase(iClientID))
		return true;

	FILETIME ft;
	SYSTEMTIME st;
	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	char toWrite[128];
	sprintf_s(toWrite, "%u,%u", ft.dwHighDateTime, ft.dwLowDateTime);

	CAccount *acc = HkGetAccountByClientID(iClientID);
	CAccountListNode *characterList = acc->pFirstListNode;
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0774"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0775"));	// Разрыв строки, чтобы это выглядело красиво
	int iCharactersRefreshed = 0;		// Номер, за которым нужно следить
	HK_ERROR err;						// Если это ошибка, мы хотим сообщить им.

	// Сохраните текущий символ на случай, если по какой-либо причине он еще не был сохранен.
	// Предотвращает любую потенциальную потерю данных.
	HkSaveChar(reinterpret_cast<const wchar_t*>(Players.GetActiveCharacterName(iClientID)));

	// Нам нужно отслеживать имя первого персонажа, чтобы оно не повторялось вечно.
	wstring wscFirstCharacterName;
	while (characterList)
	{
		// Нам нужно убедиться, что имя персонажа не равно нулю (что иногда случается из-за Фрилансера?)
		if (!characterList->wszCharname)
		{
			// Если это произойдет, перейдите к следующему.
			characterList = characterList->next;
			continue;
		}

		wstring wscCharacterName;
		try {
			wscCharacterName = reinterpret_cast<wchar_t*>(characterList->wszCharname);
		}
		catch (...) {
			// Если это произойдет, перейдите к следующему.
			characterList = characterList->next;
			continue;
		}

		// Сохраняйте только имя
		if (iCharactersRefreshed == 0)
			wscFirstCharacterName = wscCharacterName;

		// Если это не имя, нам нужно убедиться, что мы не зациклились на списке еще раз.
		else
			if (wscCharacterName == wscFirstCharacterName)
				break; // Завершите цикл, если имена совпадают.

		iCharactersRefreshed++;
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0776"), wscCharacterName.c_str());
		if ((err = HkFLIniWrite(wscCharacterName, L"tstamp", stows(toWrite))) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0777"), HkErrGetText(err).c_str());
			return true;
		}

		// Повторите цикл снова
		characterList = characterList->next;
	}
	// Укажите количество обновленных символов.
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0778"), iCharactersRefreshed);

	// Откажитесь от них, чтобы предотвратить потерю сохраненных данных и убедиться, что все идет по плану.
	HkKickReason(reinterpret_cast<const wchar_t*>(Players.GetActiveCharacterName(iClientID)), 
		GetLocalized(iClientID, "MSG_1548"));
	return true;
}

// /frelancer - дает пользователю IFF фрилансера (нейтральная ко всем фракция)
bool UserCmd_FreelancerIFF(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	if (!CheckIsInBase(iClientID))
		return true;

	HK_ERROR err;
	if ((err = HkSetRep(reinterpret_cast<const wchar_t*>(Players.GetActiveCharacterName(iClientID)), L"fc_freelancer", 1.0f)) != HKE_OK)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0777"), HkErrGetText(err).c_str());
		return true;
	}

	PrintUserCmdText(iClientID, 
		GetLocalized(iClientID, "MSG_0780"));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Actual Code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Clean up when a client disconnects */
void ClearClientInfo(uint iClientID)
{
	returncode = DEFAULT_RETURNCODE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Client command processing
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef bool(*_UserCmdProc)(uint, const wstring &, const wstring &, const wchar_t*);

struct USERCMD
{
	wchar_t *wszCmd;
	_UserCmdProc proc;
	wchar_t *usage;
};

USERCMD UserCmds[] =
{
	{ L"/refresh", UserCmd_RefreshCharacters, L"" },
	{ L"/refresh*", UserCmd_RefreshCharacters, L"" },
	{ L"/freelancer", UserCmd_FreelancerIFF, L"" },
	{ L"/freelancer*", UserCmd_FreelancerIFF, L"" },
};

/**
This function is called by FLHook when a user types a chat string. We look at the
string they've typed and see if it starts with one of the above commands. If it
does we try to process it.
*/
bool UserCmd_Process(uint iClientID, const wstring &wscCmd)
{
	returncode = DEFAULT_RETURNCODE;

	wstring wscCmdLineLower = ToLower(wscCmd);

	// If the chat string does not match the USER_CMD then we do not handle the
	// command, so let other plugins or FLHook kick in. We require an exact match
	for (uint i = 0; (i < sizeof(UserCmds) / sizeof(USERCMD)); i++)
	{

		if (wscCmdLineLower.find(UserCmds[i].wszCmd) == 0)
		{
			// Extract the parameters string from the chat string. It should
			// be immediately after the command and a space.
			wstring wscParam = GetLocalized(iClientID, "MSG_0775");
			if (wscCmd.length() > wcslen(UserCmds[i].wszCmd))
			{
				if (wscCmd[wcslen(UserCmds[i].wszCmd)] != ' ')
					continue;
				wscParam = wscCmd.substr(wcslen(UserCmds[i].wszCmd) + 1);
			}

			// Dispatch the command to the appropriate processing function.
			if (UserCmds[i].proc(iClientID, wscCmd, wscParam, UserCmds[i].usage))
			{
				// We handled the command tell FL hook to stop processing this
				// chat string.
				returncode = SKIPPLUGINS_NOFUNCTIONCALL; // we handled the command, return immediatly
				return true;
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions to hook
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "Miscellaneous Commands by a lot of different people.";
	p_PI->sShortName = "misc";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;

	p_PI->lstHooks.emplace_back(reinterpret_cast<FARPROC*>(&LoadSettings), PLUGIN_LoadSettings, 0);
	p_PI->lstHooks.emplace_back(reinterpret_cast<FARPROC*>(&ClearClientInfo), PLUGIN_ClearClientInfo, 0);
	p_PI->lstHooks.emplace_back(reinterpret_cast<FARPROC*>(&UserCmd_Process), PLUGIN_UserCmd_Process, 0);

	return p_PI;
}
