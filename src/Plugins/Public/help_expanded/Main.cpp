/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 апреля 2026 10:59:03
 * Version: 1.0.24
 */

// Expanded Help Menu - Customisable Help Commands
// By Laz, w/ help from @Alex.
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <list>
#include <map>
#include <algorithm>
#include <FLHook.h>
#include <plugin.h>
#include <PluginUtilities.h>
#include "Main.h"

#include "../hookext_plugin/hookext_exports.h"

/// A return code to indicate to FLHook if we want the hook processing to continue.
PLUGIN_RETURNCODE returncode;

void LoadSettings();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	srand((uint)time(0));
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

struct HELPSTRUCT
{
	wstring wscTitle; // What will the title of our text box be called?
	wstring wscContent; // What content will it hold?
};

bool bPluginEnabled = true; // So we can disable it with ease.
map<wstring, HELPSTRUCT> mapHelp;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Loading Settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LoadSettings()
{
	mapHelp.clear();
	returncode = DEFAULT_RETURNCODE;

	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\laz_help.cfg";
	bPluginEnabled = IniGetB(scPluginCfgFile, "Config", "Enabled", true); // Allow things to be quickly disabled

	HELPSTRUCT helpstruct;
	list<INISECTIONVALUE> iniSection; // Create a new list to store our values
	IniGetSection(scPluginCfgFile, "Commands", iniSection); // Which header will we use to fill the values

	foreach(iniSection, INISECTIONVALUE, iter) // Loop through the list
	{
		if (iter->scKey == "help") // Imitate ini reader
		{
			wstring getValueString = stows(iter->scValue); // Get around the character limit glitch

			int firstComma = getValueString.find(','); // Get index of first comma
			int secondComma = getValueString.substr(firstComma + 1).find(','); // Get index of second comma
			wstring wscParam = getValueString.substr(0, firstComma); // Our param for the command
			helpstruct.wscTitle = Trim(getValueString.substr(firstComma + 1, secondComma)); // The title for our text box
			helpstruct.wscContent = Trim(getValueString.substr(firstComma + secondComma + 2)); // The content of our text box. XML String.

			mapHelp[wscParam] = helpstruct;
		}
	}
	ConPrint(L"HELP MENUS: Loaded %u help menus\n", mapHelp.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HelpInfo(uint iClientID)
{
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0673"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0674"));
	for (map<wstring, HELPSTRUCT>::iterator iter = mapHelp.begin(); iter != mapHelp.end(); iter++)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1352"), iter->first.c_str());
	}
}

bool ValidityCheck(uint iClientID)
{
	if (!bPluginEnabled) // Убедитесь, что плагин не отключен через cfg.
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0675"));
		return false;
	}

	// Не позволяет людям блокировать себя с помощью команды, когда они мертвы..
	if (HkIsOnDeathMenu(iClientID))
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0676")); // Фиктивное сообщение
		return false;
	}
	return true;
}

void TextboxPopUp(uint iClientID, const wstring &idTitle, const wstring &idContentXML)
{
	FmtStr caption(0, 0);					// Создать новый заголовок текстового поля
	//caption.begin_mad_lib(ToInt(idTitle));	// Заполните его информационной карточкой, которую мы изменили.
	caption.end_mad_lib();
	FmtStr message(0, 0);						// Создать новое сообщение
	message.begin_mad_lib(ToInt(idTitle));	// Заполните это сообщение информационной карточкой, которую мы изменили.
	message.end_mad_lib();

	pub::Player::PopUpDialog(iClientID, caption, message, POPUPDIALOG_BUTTONS_CENTER_OK);
	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0677"), idTitle);	// Отобразить это сообщение в игре
}

// Нужна отдельная карта для специальных инфокарт, которые не контролируются этим плагином.
// При этом уже были инфокарты для "Getting Started", и "Server Rules" созданны,
// Так что мы будем использовать их и не обращать внимания на любые потенциальные ошибки с помощью HkChangeIDSString.
void RulesOrStarted(uint iClientID, const wstring &wscTitle, const int infocardNumber)
{
	//HkChangeIDSString(iClientID, 500000, wscTitle);

	FmtStr caption(0, 0);
	//caption.begin_mad_lib(500000);
	caption.end_mad_lib();

	FmtStr message(0, 0);
	message.begin_mad_lib(infocardNumber);
	message.end_mad_lib();

	pub::Player::PopUpDialog(iClientID, caption, message, POPUPDIALOG_BUTTONS_CENTER_OK);

	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0678"));
}

bool UserCmd_Rules(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	if (!ValidityCheck(iClientID))
		return false;

	RulesOrStarted(iClientID, GetLocalized(iClientID, "MSG_1528"), 500009);
	return true;
}

bool UserCmd_GettingStarted(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	if (!ValidityCheck(iClientID))
		return false;

	RulesOrStarted(iClientID, GetLocalized(iClientID, "MSG_1529"), 500010);
	return true;
}

bool UserCmd_HelpMenu(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	if (!ValidityCheck(iClientID)) // Подробнее см. функцию
		return false;

	wstring wscHelpParam = ToLower(GetParam(wscParam, ' ', 0)); // Какой список помощи мы пытаемся получить?

	if (!wscHelpParam.length()) // Если параметр не был указан, выполните итерацию по списку.
	{
		HelpInfo(iClientID);
		return false;
	}

	if (mapHelp.find(wscHelpParam) != mapHelp.end())
	{
		HELPSTRUCT &xml = mapHelp[wscHelpParam]; // Загрузите правильную структуру
		TextboxPopUp(iClientID, xml.wscTitle, xml.wscContent); // Отправьте его, чтобы он превратился в окно сообщения
		return true;
	}

	return true;
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
	{ L"/helpmenu", UserCmd_HelpMenu, L"Usage: /helpmenu" },
	{ L"/helpmenu*", UserCmd_HelpMenu, L"Usage: /helpmenu" },
	{ L"/start", UserCmd_GettingStarted, L"Usage: /start" },
	{ L"/start*", UserCmd_GettingStarted, L"Usage: /start" },
	{ L"/rules", UserCmd_Rules, L"Usage: /rules" },
	{ L"/rules*", UserCmd_Rules, L"Usage: /rules" },
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
			wstring wscParam = L"";
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

namespace HkIServerImpl
{
	EXPORT void __stdcall PlayerLaunch(unsigned int iShip, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;

		bool exist = CheckUserExistInDb(iClientID);

		if (exist == false)
		{
			// показываю что для начала надо знать на сервере
			RulesOrStarted(iClientID, GetLocalized(iClientID, "MSG_1529"), 500010);
			// фиксирую пользователя в базе данных
			AddUserInDb(iClientID);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions to hook
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "Expanded Help Menu by Laz";
	p_PI->sShortName = "exhelp";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::PlayerLaunch, PLUGIN_HkIServerImpl_PlayerLaunch, 0));

	return p_PI;
}
