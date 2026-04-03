/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 03 апреля 2026 11:33:45
 * Version: 1.0.18
 */

#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

PLUGIN_RETURNCODE returncode;

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return true;
}

EXPORT void UserCmd_Help(uint iClientID, const wstring &wscParam)
{
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1316"), GetLocalized(iClientID, "MSG_1317"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1318"), GetLocalized(iClientID, "MSG_1319"));
}

void UserCmd_Bank(uint iClientID, const wstring &wscParam)
{
	CAccount *acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	scUserStore = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1320"), GetLocalized(iClientID, "MSG_1321"));
		return;
	}
	int availCash = 0;
	HkGetCash(ARG_CLIENTID(iClientID), availCash);
	int amount = ToInt(GetParam(wscParam, ' ', 0));

	long long samout = IniGetLL(scUserStore, "Bank", "money", 0);
	if (amount == 0)
	{
		PrintUserCmdTextColorKV(iClientID, std::to_wstring(samout), GetLocalized(iClientID, "MSG_1524"));
		return;
	}
	if (amount <= 0)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1322"), GetLocalized(iClientID, "MSG_1323"));
		return;
	}
	if (amount > availCash)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1324"), GetLocalized(iClientID, "MSG_1325"));
		return;
	}
	long long amountfix = static_cast<long long>(amount) / 1000000;
	long long samoutfix = samout / 1000000;

	if (amountfix > 9223372036854775807 - samoutfix)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1326"), GetLocalized(iClientID, "MSG_1327"));
		return;
	}

	long long load = amountfix + samoutfix;
	long long load1 = 1900000000;

	if (load > load1)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1328"), GetLocalized(iClientID, "MSG_1329"));
		return;
	}
	else
	{
		HkAddCash(ARG_CLIENTID(iClientID), -amount);
		IniDelSection(scUserStore, "Bank");
		IniWrite(scUserStore, "Bank", "money", lltos(samout + static_cast<long long>(amount)));
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1330"), GetLocalized(iClientID, "MSG_1331"));
	}
}

void UserCmd_wBank(uint iClientID, const wstring &wscParam)
{
	CAccount *acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	scUserStore = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
	int availCash = 0;
	HkGetCash(ARG_CLIENTID(iClientID), availCash);
	int amount = ToInt(GetParam(wscParam, ' ', 0));

	long long samout = IniGetLL(scUserStore, "Bank", "money", 0);
	if (amount == 0)
	{
		PrintUserCmdTextColorKV(iClientID, std::to_wstring(samout), GetLocalized(iClientID, "MSG_1524"));
		return;
	}
	if (amount <= 0)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1332"), GetLocalized(iClientID, "MSG_1333"));
		return;
	}
	if (amount > samout)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1334"), GetLocalized(iClientID, "MSG_1335"));
		return;
	}

	int availCashfix = availCash / 1000000;
	int amountfix = amount / 1000000;
	int load = availCashfix + amountfix;
	int load1 = 1900;
	if (load > load1)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1336"), GetLocalized(iClientID, "MSG_1337"));
		return;
	}
	else
	{
		HkAddCash(ARG_CLIENTID(iClientID), amount);
		IniDelSection(scUserStore, "Bank");
		IniWrite(scUserStore, "Bank", "money", lltos(samout - static_cast<long long>(amount)));
		PrintUserCmdTextColorKV(iClientID, to_wstring(amount), GetLocalized(iClientID, "MSG_1525"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void(*_UserCmdProc)(uint, const wstring &);

struct USERCMD
{
	wchar_t *wszCmd;
	_UserCmdProc proc;
};

USERCMD UserCmds[] =
{
	{ L"/bank",					UserCmd_Bank},
	{ L"/bankw",			    UserCmd_wBank},
};

EXPORT bool UserCmd_Process(uint iClientID, const wstring &wscCmd)
{
	wstring wscCmdLower = ToLower(wscCmd);
	for (uint i = 0; (i < sizeof(UserCmds) / sizeof(USERCMD)); i++)
	{
		if (wscCmdLower.find(ToLower(UserCmds[i].wszCmd)) == 0)
		{
			wstring wscParam = L"";
			if (wscCmd.length() > wcslen(UserCmds[i].wszCmd))
			{
				if (wscCmd[wcslen(UserCmds[i].wszCmd)] != ' ')
					continue;
				wscParam = wscCmd.substr(wcslen(UserCmds[i].wszCmd) + 1);
			}
			UserCmds[i].proc(iClientID, wscParam);
			returncode = SKIPPLUGINS_NOFUNCTIONCALL; // we handled the command, return immediatly
			return true;
		}
	}
	returncode = DEFAULT_RETURNCODE; // we did not handle the command, so let other plugins or FLHook kick in
	return false;
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "Shared bank plugin by kosacid";
	p_PI->sShortName = "banker";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Help, PLUGIN_UserCmd_Help, 0));
	return p_PI;
}