/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 апреля 2026 06:54:28
 * Version: 1.0.471
 */

// Player Control plugin for FLHookPlugin
// Feb 2010 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.
//
// This file includes code that was not written by me but I can't find
// the original author (I know they posted on the-starport.net about it).

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

#include <PluginUtilities.h>
#include "Main.h"

#include "Shlwapi.h"

#include <FLCoreServer.h>
#include <FLCoreCommon.h>

namespace Restart
{
	//список публичных рестартов с русскоязычным названием фракции
	static list<RestartPublicItem> PublicRestarts;

	//список приватных рестартов с русскоязычным названием фракции
	static list<RestartPrivateItem> PrivateRestarts;

	//пароль для доступу к списку закрытых фракций
	static string passwordPrivate;

	// Игроки с рангом выше этого значения не могут использовать команду restart.
	static int set_iMaxRank;

	// Игроки с денежными средствами выше этого значения не могут использовать команду перезапуска.
	static int set_iMaxCash;
	
	vector<string> split_string(const string& str, char delimiter) {
		vector<string> result;
		string current_word;
		for (char c : str) {
			if (c == delimiter) {
				if (!current_word.empty()) {
					result.push_back(current_word);
					current_word.clear();
				}
			}
			else {
				current_word += c;
			}
		}
		if (!current_word.empty()) {
			result.push_back(current_word);
		}
		return result;
	}

	void Restart::LoadSettingsRestart(const string& scPluginRestartCfgFile)
	{
		PublicRestarts.clear();
		PrivateRestarts.clear();

		INI_Reader ini;
		if (ini.open(scPluginRestartCfgFile.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header("FactionsRestart"))
				{
					RestartPublicItem restartPublicItem;
					RestartPrivateItem restartPrivateItem;
					while (ini.read_value())
					{
						if (ini.is_value("password"))
						{
							passwordPrivate = ini.get_value_string(0);
						}
						if (ini.is_value("public"))
						{
							restartPublicItem.fileName = ini.get_value_string(0);
							restartPublicItem.factionName = ini.get_value_string(1);
							PublicRestarts.push_back(restartPublicItem);
						}
						if (ini.is_value("private"))
						{
							vector<string> parts = split_string(ini.get_value_string(0), '_');
							restartPrivateItem.fileName = parts[0];
							restartPrivateItem.password = parts[1];
							restartPrivateItem.factionName = ini.get_value_string(1);
							PrivateRestarts.push_back(restartPrivateItem);
						}
					}
				}
			}
			ini.close();
		}
	}

	void Restart::LoadSettings(const string &scPluginCfgFile)
	{
		set_iMaxRank = IniGetI(scPluginCfgFile, "Restart", "MaxRank", 5);
		set_iMaxCash = IniGetI(scPluginCfgFile, "Restart", "MaxCash", 1000000);
	}

	/// <summary>
	/// Показывает список доступных рестартов персонажа
	/// </summary>
	bool Restart::UserCmd_ShowRestarts(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscFactionPrivate = GetParam(wscParam, ' ', 0);
		int count = 0;

		if (!wscFactionPrivate.length())
		{
			foreach(PublicRestarts, RestartPublicItem, it)
			{
				count++;
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1373") + to_wstring(count) + L"] ", stows(it->factionName));
			}
		}
		else
		{
			if (wscFactionPrivate == stows(passwordPrivate))
			{
				foreach(PrivateRestarts, RestartPrivateItem, it)
				{
					count++;
					PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1374") + to_wstring(count) + L"] ", stows(it->factionName) + L" | " + stows(it->password));
				}
			}
			else
			{
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1375"), GetLocalized(iClientID, "MSG_1376"));
			}
		}
		return true;
	}

	struct RESTARTCALLER
	{
		wstring wscCharname;
		string scRestartFile;
		wstring wscDir;
		wstring wscCharfile;
	};

	struct RESTART
	{
		wstring wscCharname;
		string scRestartFile;
		wstring wscDir;
		wstring wscCharfile;
	};
	std::list<RESTARTCALLER> pendingRestarts;

	bool Restart::UserCmd_Restart(uint iClientID, const wstring& wscCmd, const wstring& wscParam, const wchar_t* usage)
	{
		if (!set_bEnableRestart)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1326"), GetLocalized(iClientID, "MSG_1623"));
			return false;
		}

		wstring wscFaction = GetParam(wscParam, ' ', 0);
		wstring wscFactionPrivate = GetParam(wscParam, ' ', 1);

		if (!wscFaction.length())
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1377"), GetLocalized(iClientID, "MSG_1378"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		// Получите имя персонажа для этого подключения.
		RESTARTCALLER restartcall;
		restartcall.wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		int faction = ToInt(wstos(wscFaction));
		int count = 0;
		char szCurDir[MAX_PATH];
		GetCurrentDirectory(sizeof(szCurDir), szCurDir);

		if (!wscFactionPrivate.length())
		{
			foreach(PublicRestarts, RestartPublicItem, it)
			{
				count++;
				if (count != faction) {
					//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1379"), to_wstring(count) + L"|" + to_wstring(faction));
					continue;
				}
				//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1380"), to_wstring(count) + L"|" + to_wstring(faction) + L"|" + stows(it->factionName));
				restartcall.scRestartFile = string(szCurDir) + "\\flhook_plugins\\restart\\" + it->fileName + ".fl";
				break;
			}
		}
		else
		{
			foreach(PrivateRestarts, RestartPrivateItem, it)
			{
				count++;
				if (count != faction) {
					//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1381"), to_wstring(count) + L"|" + to_wstring(faction));
					continue;
				}
				if (wscFactionPrivate == stows(it->password))
				{
					//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1382"), to_wstring(count) + L"|" + to_wstring(faction) + L"|" + stows(it->fileName) + L"|" + stows(it->password));
					restartcall.scRestartFile = string(szCurDir) + "\\flhook_plugins\\restart\\private\\" + it->fileName + "_" + it->password + ".fl";
				}
				else
				{
					PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1383"), GetLocalized(iClientID, "MSG_1384"));
				}
				break;
			}
		}

		if (!restartcall.scRestartFile.length())
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1385"), GetLocalized(iClientID, "MSG_1386"));
			return true;
		}

		// Сохранение персонажей заставляет античит проверять и исправлять множество других проблем. 
		HkSaveChar(iClientID);
		if (!HkIsValidClientID(iClientID))
			return true;

		uint iBaseID;
		pub::Player::GetBase(iClientID, iBaseID);
		if (!iBaseID)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1387"), GetLocalized(iClientID, "MSG_1388"));
			return true;
		}

		// Получить ранг для лимита MaxRank
		int iRank = 0;
		HkGetRank(restartcall.wscCharname, iRank);
		if (iRank == 0 || iRank > set_iMaxRank)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1389"), GetLocalized(iClientID, "MSG_1390") + to_wstring(set_iMaxRank) + L"");
			return true;
		}

		int iCash = 0;
		HkGetCash(restartcall.wscCharname, iCash);
		if (iCash > set_iMaxCash)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1391"), GetLocalized(iClientID, "MSG_1392") + to_wstring(set_iMaxCash) + L"");
			return true;
		}

		CAccount* acc = Players.FindAccountFromClientID(iClientID);
		if (acc)
		{

			HkGetAccountDirName(acc, restartcall.wscDir);
			HkGetCharFileName(restartcall.wscCharname, restartcall.wscCharfile);
			//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1393"), restartcall.wscDir);
			//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1394"), restartcall.wscCharfile);
			pendingRestarts.push_back(restartcall);
			HkKickReason(restartcall.wscCharname, GetLocalized(iClientID, "MSG_1551"));
		}
		return true;
	}

	void Timer()
	{
		while (pendingRestarts.size())
		{
			RESTARTCALLER restart = pendingRestarts.front();
			if (HkGetClientIdFromCharname(restart.wscCharname) != -1)
				return;
			pendingRestarts.pop_front();

			try
			{
				// Перезапись существующего файла символов
				string scCharFile = scAcctPath + wstos(restart.wscDir) + "\\" + wstos(restart.wscCharfile) + ".fl";
				string scTimeStampDesc = IniGetS(scCharFile, "Player", "description", "");
				string scTimeStamp = IniGetS(scCharFile, "Player", "tstamp", "0");
				//AddLog("NOTICE: User restart %s ", scCharFile);

				if (!::CopyFileA(restart.scRestartFile.c_str(), scCharFile.c_str(), FALSE))
				{
					throw ("copy template");
				}

				flc_decode(scCharFile.c_str(), scCharFile.c_str());
				IniWriteW(scCharFile, "Player", "name", restart.wscCharname);
				IniWrite(scCharFile, "Player", "description", scTimeStampDesc);
				IniWrite(scCharFile, "Player", "tstamp", scTimeStamp);
				AddLog("NOTICE 777: User restart %s ", scCharFile);

				if (!set_bDisableCharfileEncryption)
					flc_encode(scCharFile.c_str(), scCharFile.c_str());

				AddLog("NOTICE: User restart %s for %s", restart.scRestartFile.c_str(), wstos(restart.wscCharname).c_str());
			}
			catch (char* err)
			{
				AddLog("ERROR: User restart failed (%s) for %s", err, wstos(restart.wscCharname).c_str());
			}
			catch (...)
			{
				AddLog("ERROR: User restart failed for %s", wstos(restart.wscCharname).c_str());
			}
		}
	}
}