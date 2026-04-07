/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 07 апреля 2026 10:57:40
 * Version: 1.0.22
 */

// AlleyPlugin for FLHookPlugin
// February 2015 by Alley
//
// Этот CPP управляет функцией, позволяющей игрокам не подключаться к чему-либо в течение x секунд.
// 
// Это свободное программное обеспечение; вы можете распространять его и/или изменять
// по своему усмотрению без каких - либо ограничений.Если вы это сделаете, я был бы признателен
// за уведомление и/или упоминание где-либо.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Включает в себя
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <float.h>
#include <FLHook.h>
#include <plugin.h>
#include <list>
#include <set>
#include <sstream>
#include <iostream>

#include <PluginUtilities.h>
#include "PlayerRestrictions.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Сооружения и всякое такое дерьмо, йоу
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static list<uint> idlist;
static list<uint> listAllowedShips;
static map<uint, wstring> MapActiveSirens;

static int duration = 60;
static map<uint, int> mapActiveNoDock;

static list<uint> baseblacklist;

static list<wstring> superNoDockedShips;

FILE *Logfile = fopen("./flhook_logs/nodockcommand.log", "at");

void Logging(const char *szString, ...)
{
	char szBufString[1024];
	va_list marker;
	va_start(marker, szString);
	_vsnprintf(szBufString, sizeof(szBufString) - 1, szString, marker);

	if (Logfile) {
		char szBuf[64];
		time_t tNow = time(0);
		struct tm *t = localtime(&tNow);
		strftime(szBuf, sizeof(szBuf), "%d/%m/%Y %H:%M:%S", t);
		fprintf(Logfile, "%s %s\n", szBuf, szBufString);
		fflush(Logfile);
		fclose(Logfile);
	}
	else {
		ConPrint(L"Failed to write nodockcommand log! This might be due to inability to create the directory - are you running as an administrator?\n");
	}
	Logfile = fopen("./flhook_logs/nodockcommand.log", "at");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Settings Loading
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADOCK::LoadSettings()
{
	// The path to the configuration file.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\alley.cfg";

	INI_Reader ini;
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("nodockcommand"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("id"))
					{
						idlist.push_back(CreateID(ini.get_value_string(0)));
					}
				}
			}
			else if (ini.is_header("nodockexemption"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("no"))
					{
						baseblacklist.push_back(CreateID(ini.get_value_string(0)));
					}
				}
			}
			else if (ini.is_header("config"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("duration"))
					{
						duration = ini.get_value_int(0);
					}
				}
			}
			else if (ini.is_header("supernodockedships"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("ship"))
					{
						superNoDockedShips.push_back((const wchar_t*)ini.get_value_wstring());
					}
				}
			}
		}
		ini.close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Логика
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADOCK::Timer()
{
	for (map<uint, int>::iterator i = mapActiveNoDock.begin(); i != mapActiveNoDock.end(); ++i)
	{
		if (i->second == 0)
		{
			const wchar_t* wszCharname = (const wchar_t*)Players.GetActiveCharacterName(i->first);
			if (wszCharname) {
				wstring wscMsg = GetLocalized(i->first, "MSG_1506");
				wscMsg = ReplaceStr(wscMsg, L"%time", GetTimeString(false));
				wscMsg = ReplaceStr(wscMsg, L"%victim", wszCharname);
				PrintLocalUserCmdText(i->first, wscMsg, 10000);
			}

			mapActiveNoDock.erase(i->first);
		}
		else
		{
			i->second = i->second - 1;
		}
	}
}

void ADOCK::ClearClientInfo(uint iClientID)
{
	listAllowedShips.remove(iClientID);
	MapActiveSirens.erase(iClientID);
}

void ADOCK::PlayerLaunch(unsigned int iShip, unsigned int client)
{
	// Получите информацию о местоположении и списке груза.
	int iHoldSize;
	list<CARGO_INFO> lstCargo;
	HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(client), lstCargo, iHoldSize);

	foreach(lstCargo, CARGO_INFO, i)
	{
		if (i->bMounted)
		{
			// это хорошее удостоверение личности
			list<uint>::iterator iter = idlist.begin();
			while (iter != idlist.end())
			{
				if (*iter == i->iArchID)
				{
					listAllowedShips.push_back(client);
					//PrintUserCmdText(client, GetLocalized(iClientID, "MSG_0031"));
					break;
				}
				iter++;
			}
		}
	}

	//PrintUserCmdText(client, GetLocalized(iClientID, "MSG_0032"));
	//PrintUserCmdText(client, GetLocalized(iClientID, "MSG_0033"), duration);
}

bool ADOCK::NoDockCommand(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	bool isAllowed = false;

	list<uint>::iterator iter = listAllowedShips.begin();
	while (iter != listAllowedShips.end())
	{
		if (*iter == iClientID)
		{
			isAllowed = true;
			break;
		}
		iter++;
	}

	if (isAllowed == false)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0034"));
		return true;
	}
	if (isAllowed == true)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0035"));

		uint iShip = 0;
		pub::Player::GetShip(iClientID, iShip);
		if (!iShip) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0036"));
			return true;
		}

		uint iTarget = 0;
		pub::SpaceObj::GetTarget(iShip, iTarget);

		if (!iTarget) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0037"));
			return true;
		}

		uint iClientIDTarget = HkGetClientIDByShip(iTarget);
		if (!HkIsValidClientID(iClientIDTarget))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0038"));
			return true;
		}

		wstring wscTargetCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientIDTarget);

		for (map<uint, int>::iterator i = mapActiveNoDock.begin(); i != mapActiveNoDock.end(); ++i)
		{
			if (i->first == iClientIDTarget)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0039"), duration);
				PrintUserCmdText(iClientIDTarget, GetLocalized(iClientID, "MSG_0040"), duration);
				i->second = duration;
				return true;
			}
		}

		mapActiveNoDock[iClientIDTarget] = duration;

		//10k космическое сообщение

		stringstream ss;
		ss << duration;
		string strduration = ss.str();

		wstring wscMsg = GetLocalized(iClientID, "MSG_1507");
		wscMsg = ReplaceStr(wscMsg, L"%time", GetTimeString(false));
		wscMsg = ReplaceStr(wscMsg, L"%player", (const wchar_t*)Players.GetActiveCharacterName(iClientID));
		wscMsg = ReplaceStr(wscMsg, L"%victim", wscTargetCharname.c_str());
		wscMsg = ReplaceStr(wscMsg, L"%duration", stows(strduration).c_str());
		PrintLocalUserCmdText(iClientID, wscMsg, 10000);

		//internal log
		wstring wscMsgLog = GetLocalized(iClientID, "MSG_1508");
		wscMsgLog = ReplaceStr(wscMsgLog, L"%sender", wscCharname.c_str());
		wscMsgLog = ReplaceStr(wscMsgLog, L"%victim", wscTargetCharname.c_str());
		string scText = wstos(wscMsgLog);
		Logging("%s", scText.c_str());

		return true;
	}

	return true;
}

bool ADOCK::IsDockAllowed(uint iShip, uint iDockTarget, uint iClientID)
{
	boolean supernodocked = false;
	wstring curCharName = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	list<wstring>::iterator sndIter = superNoDockedShips.begin();

	while (sndIter != superNoDockedShips.end())
	{
		if (*sndIter == curCharName)
		{

			supernodocked = true;
			break;
		}
		sndIter++;
	}

	if (supernodocked)
	{
		uint iID;
		pub::SpaceObj::GetDockingTarget(iDockTarget, iID);
		Universe::IBase *base = Universe::get_base(iID);

		if (base)
		{
			pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0041"));
			return false;
		}
	}

	bool aminice = true;

	// он находится под действием nodock 
	for (map<uint, int>::iterator i = mapActiveNoDock.begin(); i != mapActiveNoDock.end(); ++i)
	{
		if (i->first == iClientID)
		{
			//обнаружено несоответствие требованиям
			aminice = false;
			//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0042"));
		}
	}

	// вместо сложного кода мы просто проверяем, хороший ли он. 
	// Если да, то мы игнорируем остальную часть кода.
	if (aminice == true)
	{
		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0043"));
		return true;
	}

	uint iName;
	pub::SpaceObj::GetDockingTarget(iDockTarget, iName);

	// если он не очень приятный, мы проверяем, подвержена ли основа эффекту nodock.
	list<uint>::iterator iter = baseblacklist.begin();
	while (iter != baseblacklist.end())
	{
		if (*iter == iName)
		{
			//мы нашли эту базу в черном списке. следовательно, nodock будет работать. не позволяйте ему подключаться.
			pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0044"));
			return false;
			break;
		}
		iter++;
	}

	//в противном случае это, вероятно, не сработает
	return true;
}

void ADOCK::AdminNoDock(CCmds* cmds, const wstring &wscCharname)
{
	if (cmds->rights != RIGHT_SUPERADMIN)
	{
		cmds->Print(L"ОШИБКА: Без разрешения\n");
		return;
	}

	HKPLAYERINFO targetPlyr;
	uint iClientID = targetPlyr.iClientID;
	 
	if (HkGetPlayerInfo(wscCharname, targetPlyr, false) != HKE_OK)
	{
		cmds->Print(L"ОШИБКА: игрок не найден\n");
		return;
	}

	for (map<uint, int>::iterator i = mapActiveNoDock.begin(); i != mapActiveNoDock.end(); ++i)
	{
		if (i->first == targetPlyr.iClientID)
		{
			cmds->Print(GetLocalized(iClientID, "MSG_0039"), duration);
			PrintUserCmdText(targetPlyr.iClientID, GetLocalized(iClientID, "MSG_0040"), duration);
			i->second = duration;
			return;
		}
	}

	mapActiveNoDock[targetPlyr.iClientID] = duration;

	//10k space message

	stringstream ss;
	ss << duration;
	string strduration = ss.str();

	wstring wscMsg = GetLocalized(iClientID, "MSG_1507");
	wscMsg = ReplaceStr(wscMsg, L"%time", GetTimeString(false));
	wscMsg = ReplaceStr(wscMsg, L"%player", cmds->GetAdminName().c_str());
	wscMsg = ReplaceStr(wscMsg, L"%victim", targetPlyr.wscCharname.c_str());
	wscMsg = ReplaceStr(wscMsg, L"%duration", stows(strduration).c_str());
	PrintLocalUserCmdText(targetPlyr.iClientID, wscMsg, 10000);

	//internal log
	wstring wscMsgLog = GetLocalized(iClientID, "MSG_1508");
	wscMsgLog = ReplaceStr(wscMsgLog, L"%sender", cmds->GetAdminName().c_str());
	wscMsgLog = ReplaceStr(wscMsgLog, L"%victim", targetPlyr.wscCharname.c_str());
	string scText = wstos(wscMsgLog);
	Logging("%s", scText.c_str());

	cmds->Print(L"OK\n");
	return;
}

bool ADOCK::PoliceCmd(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip) {
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0046"));
		return true;
	}

	bool isAllowed = false;

	list<uint>::iterator iter = listAllowedShips.begin();
	while (iter != listAllowedShips.end())
	{
		if (*iter == iClientID)
		{
			isAllowed = true;
			break;
		}
		iter++;
	}

	if (isAllowed == false)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0034"));
		return true;
	}
	if (isAllowed == true)
	{
		if (MapActiveSirens.find(iClientID) != MapActiveSirens.end())
		{
			UnSetFuse(iClientID, CreateID("dsy_police_liberty"));
			MapActiveSirens.erase(iClientID);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0048"));
		}
		else
		{
			SetFuse(iClientID, CreateID("dsy_police_liberty"), 999999);
			MapActiveSirens[iClientID] = L"test";
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0049"));
		}
	}

	return true;
}
