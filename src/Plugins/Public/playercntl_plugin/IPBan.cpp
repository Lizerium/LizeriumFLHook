/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 02 апреля 2026 06:53:11
 * Version: 1.0.17
 */

// Player Control plugin for FLHookPlugin
// Feb 2010 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <float.h>
#include <FLHook.h>
#include <plugin.h>
#include <math.h>
#include <list>
#include <set>

#include "Main.h"
#include <PluginUtilities.h>
#include "wildcards.h"

namespace IPBans
{
	/// Список запретов
	static list<string> set_lstIPBans;
	static list<string> set_lstLoginIDBans;

	struct INFO
	{
		bool bIPChecked;
	};
	static map<uint, INFO> mapInfo;

	/// Возвращает true, если этот клиент находится в запрещенном диапазоне IP-адресов.
	static bool IsBanned(uint iClientID)
	{
		wstring wscIP;
		HkGetPlayerIP(iClientID, wscIP);
		string scIP = wstos(wscIP);

		// Проверьте соответствие диапазона IP-адресов.
		foreach(set_lstIPBans, string, iter)
			if (Wildcard::wildcardfit(iter->c_str(), scIP.c_str()))
				return true;
		// Чтобы избежать общения плагинов с DSAce, потому что у меня не хватило времени, чтобы это работало
		// Я использую какой-то трюк, чтобы получить идентификатор для входа в систему.
		// Прочитайте все файлы идентификаторов входа в аккаунт и найдите тот, у которого IP-адрес совпадает с этим игроком.
		// Если мы найдем подходящий IP-адрес, то у нас есть идентификатор для входа, который мы можем проверить.
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		if (acc)
		{
			bool bBannedLoginID = false;

			wstring wscDir;
			HkGetAccountDirName(acc, wscDir);

			WIN32_FIND_DATA findFileData;

			string scFileSearchPath = scAcctPath + "\\" + wstos(wscDir) + "\\login_*.ini";
			HANDLE hFileFind = FindFirstFile(scFileSearchPath.c_str(), &findFileData);
			if (hFileFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					// Прочитайте идентификатор входа и IP-адрес из записи идентификатора входа.
					string scLoginID = "";
					string scLoginID2 = "";
					string scThisIP = "";
					string scFilePath = scAcctPath + wstos(wscDir) + "\\" + findFileData.cFileName;
					FILE *f = fopen(scFilePath.c_str(), "r");
					if (f)
					{
						char szBuf[200];
						if (fgets(szBuf, sizeof(szBuf), f) != NULL)
						{
							try
							{
								scLoginID = Trim(GetParam(szBuf, '\t', 1).substr(3, string::npos));
								scThisIP = Trim(GetParam(szBuf, '\t', 2).substr(3, string::npos));
								if (GetParam(szBuf, '\t', 3).length() > 4)
									scLoginID2 = Trim(GetParam(szBuf, '\t', 3).substr(4, string::npos));
							}
							catch (...)
							{
								ConPrint(L"ERR Corrupt loginid file $0\n", stows(scFilePath).c_str());
							}
						}
						fclose(f);
					}

					if (set_iPluginDebug > 2)
					{
						ConPrint(L"NOTICE: Checking for ban on IP %s Login ID1 %s ID2 %s Client %d\n",
							stows(scThisIP).c_str(), stows(scLoginID).c_str(), stows(scLoginID2).c_str(), iClientID);
					}

					// Если идентификатор пользователя был прочитан, то проверьте его, чтобы узнать, не был ли он заблокирован
					if (scThisIP == scIP && scLoginID.length())
					{
						foreach(set_lstLoginIDBans, string, iter)
						{
							if (*iter == scLoginID
								|| *iter == scLoginID2)
							{
								ConPrint(L"* Kicking player on ID ban: ip=%s id1=%s id2=%s\n",
									stows(scThisIP).c_str(), stows(scLoginID).c_str(), stows(scLoginID2).c_str());
								bBannedLoginID = true;
								break;
							}
						}
					}
				} while (FindNextFile(hFileFind, &findFileData));
				FindClose(hFileFind);
			}

			if (bBannedLoginID)
				return true;
		}
		return false;
	}

	/// Если идентификатор пользователя был прочитан, то проверьте его, чтобы узнать, не был ли он заблокирован
	/// Каталог учетной записи, указывающий, что клиент может подключиться, 
	/// даже если он находится в ограниченном диапазоне IP-адресов.
	static bool IsAuthenticated(uint iClientID)
	{
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		if (!acc)
			return false;

		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);
		string scUserFile = scAcctPath + wstos(wscDir) + "\\authenticated";
		FILE* fTest = fopen(scUserFile.c_str(), "r");
		if (!fTest)
			return false;

		fclose(fTest);
		return true;
	}


	static void ReloadIPBans()
	{
		// Переменные инициализации
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\ipbans.ini";
		if (set_iPluginDebug)
			ConPrint(L"NOTICE: Loading IP bans from %s\n", stows(scAcctPath).c_str());

		INI_Reader ini;
		set_lstIPBans.clear();
		if (ini.open(scAcctPath.c_str(), false))
		{
			while (ini.read_header())
			{
				while (ini.read_value())
				{
					set_lstIPBans.push_back(ini.get_name_ptr());
					if (set_iPluginDebug)
						ConPrint(L"NOTICE: Adding IP ban %s\n", stows(ini.get_name_ptr()).c_str());
				}
			}
			ini.close();
		}
		ConPrint(L"IP Bans [%u]\n", set_lstIPBans.size());
	}


	static void ReloadLoginIDBans()
	{
		// Переменные инициализации
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\loginidbans.ini";
		if (set_iPluginDebug)
			ConPrint(L"NOTICE: Loading Login ID bans from %s\n", stows(scAcctPath).c_str());

		INI_Reader ini;
		set_lstLoginIDBans.clear();
		if (ini.open(scAcctPath.c_str(), false))
		{
			while (ini.read_header())
			{
				while (ini.read_value())
				{
					set_lstLoginIDBans.push_back(Trim(ini.get_name_ptr()));
					if (set_iPluginDebug)
						ConPrint(L"NOTICE: Adding Login ID ban %s\n", stows(ini.get_name_ptr()).c_str());
				}
			}
			ini.close();
		}
		ConPrint(L"ID Bans [%u]\n", set_lstLoginIDBans.size());
	}


	/// Перезагрузите файл ipbans.
	void IPBans::LoadSettings(const string &scPluginCfgFile)
	{
		ReloadIPBans();
		ReloadLoginIDBans();
	}

	void IPBans::PlayerLaunch(unsigned int iShip, unsigned int iClientID)
	{
		if (!mapInfo[iClientID].bIPChecked)
		{
			mapInfo[iClientID].bIPChecked = true;
			if (IsBanned(iClientID) && !IsAuthenticated(iClientID))
			{
				HkAddKickLog(iClientID, L"IP banned");
				HkMsgAndKick(iClientID, GetLocalized(iClientID, "MSG_1566"), 15000L);
			}
		}
	}


	void IPBans::BaseEnter(unsigned int iBaseID, unsigned int iClientID)
	{
		if (!mapInfo[iClientID].bIPChecked)
		{
			mapInfo[iClientID].bIPChecked = true;
			if (IsBanned(iClientID) && !IsAuthenticated(iClientID))
			{
				HkAddKickLog(iClientID, L"IP banned");
				HkMsgAndKick(iClientID, GetLocalized(iClientID, "MSG_1566"), 7000L);
			}
		}
	}

	/** Запуск автоматической проверки зон */
	void IPBans::AdminCmd_AuthenticateChar(CCmds* cmds, const wstring &wscCharname)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		// init variables
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);

		wstring wscDir;
		if (HkGetAccountDirName(wscCharname, wscDir) != HKE_OK)
		{
			cmds->Print(L"ERR Account not found\n");
			return;
		}

		string scPath = string(szDataPath) + "\\Accts\\MultiPlayer\\" + wstos(wscDir) + "\\authenticated";
		FILE *fTest = fopen(scPath.c_str(), "w");
		if (!fTest)
		{
			cmds->Print(L"ERR Writing authentication file\n");
			return;
		}

		fclose(fTest);
		cmds->Print(L"OK\n");
	}

	void IPBans::ClearClientInfo(uint iClientID)
	{
		mapInfo[iClientID].bIPChecked = false;
	}

	void IPBans::AdminCmd_ReloadBans(CCmds* cmds)
	{
		ReloadLoginIDBans();
		ReloadIPBans();
		cmds->Print(L"OK\n");
	}
}