/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 апреля 2026 10:59:03
 * Version: 1.0.24
 */

// AlleyPlugin for FLHookPlugin
// December 2014 by Alley
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
#include <list>
#include <map>
#include <algorithm>
#include <FLHook.h>
#include <plugin.h>
#include <PluginUtilities.h>
#include "PlayerRestrictions.h"
#include <sstream>
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <vector>

namespace pt = boost::posix_time;
static int set_iPluginDebug = 0;

FILE *GiftLogfile = fopen("./flhook_logs/alley_gifts.log", "at");

void GiftLogging(const char *szString, ...)
{
	char szBufString[1024];
	va_list marker;
	va_start(marker, szString);
	_vsnprintf(szBufString, sizeof(szBufString) - 1, szString, marker);

	if (GiftLogfile) {
		char szBuf[64];
		time_t tNow = time(0);
		struct tm *t = localtime(&tNow);
		strftime(szBuf, sizeof(szBuf), "%d/%m/%Y %H:%M:%S", t);
		fprintf(GiftLogfile, "%s %s\n", szBuf, szBufString);
		fflush(GiftLogfile);
		fclose(GiftLogfile);
	}
	else {
		ConPrint(L"Failed to write gift log! This might be due to inability to create the directory - are you running as an administrator?\n");
	}
	GiftLogfile = fopen("./flhook_logs/alley_gifts.log", "at");
}

#define POPUPDIALOG_BUTTONS_LEFT_YES 1
#define POPUPDIALOG_BUTTONS_CENTER_NO 2
#define POPUPDIALOG_BUTTONS_RIGHT_LATER 4
#define POPUPDIALOG_BUTTONS_CENTER_OK 8

static map<uint, string> notradelist;
static map<uint, string> notractorbeamlist;
static list<uint> MarkedPlayers;
//Добавил это из-за идиотизма
static list<uint> MarkUsageTimer;

char szCurDir[MAX_PATH];

/// Код возврата, указывающий FLHook, хотим ли мы, чтобы обработка запроса продолжалась.
PLUGIN_RETURNCODE returncode;

void LoadSettings();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	srand((uint)time(0));
	// Если мы загружаемся из командной строки во время запуска FLHook, то
	// set_scCfgFile не будет пустым, поэтому загружайте настройки только так, как FLHook 
	// вызывает загрузку настроек при запуске FLHook и .rehash.
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

/// Hook вызовет эту функцию после вызова функции плагина, чтобы посмотреть, продолжим ли мы обработку
EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

FILE *PMLogfile = fopen("./flhook_logs/generatedids.log", "at");
//FILE *JSON_playersonline = fopen("./flhook/playersonline.json", "w");

void PMLogging(const char *szString, ...)
{
	char szBufString[1024];
	va_list marker;
	va_start(marker, szString);
	_vsnprintf(szBufString, sizeof(szBufString) - 1, szString, marker);

	if (PMLogfile) {
		char szBuf[64];
		time_t tNow = time(0);
		struct tm *t = localtime(&tNow);
		strftime(szBuf, sizeof(szBuf), "%d/%m/%Y %H:%M:%S", t);
		fprintf(PMLogfile, "%s %s\n", szBuf, szBufString);
		fflush(PMLogfile);
		fclose(PMLogfile);
	}
	else {
		ConPrint(L"Failed to write generatedids log! This might be due to inability to create the directory - are you running as an administrator?\n");
	}
	PMLogfile = fopen("./flhook_logs/generatedids.log", "at");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Настройки загрузки
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
map<uint, float> healingMultipliers;
map<uint, uint> healingAdditions;

void LoadSettings()
{
	PMLogging("-------------------- starting server --------------------");
	returncode = DEFAULT_RETURNCODE;

	// The path to the configuration file.
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\alley_restrictions.cfg";

	INI_Reader ini;
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("notrade"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("tr"))
					{
						notradelist[CreateID(ini.get_value_string(0))] = ini.get_value_string(1);
					}
				}
			}
		}
		ini.close();
	}

	scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\alley_no_tractor_beam.cfg";

	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("notrade"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("tr"))
					{
						notractorbeamlist[CreateID(ini.get_value_string(0))] = ini.get_value_string(1);
					}
				}
			}
		}
		ini.close();
	}

	string scHealingCfgFile = string(szCurDir) + "\\..\\DATA\\EQUIPMENT\\healingrates.cfg";
	if (ini.open(scHealingCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("HealingRate"))
			{
				list<uint> shipclasses;
				float multiplier = 1.0f;
				uint addition = 0;
				while (ini.read_value())
				{
					if (ini.is_value("target_shipclass"))
					{
						shipclasses.push_back(ini.get_value_int(0));
					}
					else if (ini.is_value("addition"))
					{
						addition = ini.get_value_int(0);
					}
					else if (ini.is_value("multiplier"))
					{
						multiplier = ini.get_value_float(0);
					}
				}
				foreach(shipclasses, uint, shipclass)
				{
					healingMultipliers[*shipclass] = multiplier;
					healingAdditions[*shipclass] = addition;
				}
			}
		}
		ini.close();
	}

	AP::LoadSettings();
	ADOCK::LoadSettings();
	SCI::LoadSettings();
	REP::LoadSettings();

	//Init();
}

void __stdcall SetVisitedState(unsigned int iClientID, uint objHash, int p3)
{
	returncode = DEFAULT_RETURNCODE;
}

void SetFuse(uint iClientID, uint fuse, float lifetime)
{
	IObjInspectImpl *obj = HkGetInspect(iClientID);
	if (obj)
	{
		HkLightFuse((IObjRW*)obj, fuse, 0.0f, lifetime, 0.0f);
	}
}

void UnSetFuse(uint iClientID, uint fuse)
{
	IObjInspectImpl *obj = HkGetInspect(iClientID);
	if (obj)
	{
		HkUnLightFuse((IObjRW*)obj, fuse, 0.0f);
	}
}

bool PirateCmd(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	uint iBaseID;
	pub::Player::GetBase(iClientID, iBaseID);
	if (!iBaseID)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0050"));
		return true;
	}

	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	HkSetRep(wscCharname, L"fc_pirate", 1.0f);

	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0051"));
	pub::Audio::PlaySoundEffect(iClientID, CreateID("ui_gain_level"));
	pub::Audio::PlaySoundEffect(iClientID, CreateID("dx_s070x_0801_Jacobi"));
	return true;
}

bool GiftCmd(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	HK_ERROR err;
	// Укажите ошибку, если команда, по-видимому, отформатирована неправильно 
	// и прекратите обработку, но сообщите FLHook, что мы обработали команду.
	if (wscParam.size() == 0)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0052"));
		PrintUserCmdText(iClientID, usage);
		return true;
	}

	uint iBaseID;
	pub::Player::GetBase(iClientID, iBaseID);
	if (!iBaseID)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0053"));
		return true;
	}

	// Узнайте текущее количество кредитов у игрока
	// и убедитесь, что у персонажа достаточно наличных денег.
	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	wstring wscCash = GetParam(wscParam, L' ', 0);
	int Cash = ToInt(wscCash);

	if (Cash <= 0)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0052"));
		PrintUserCmdText(iClientID, usage);
		return true;
	}

	int iCash = 0;
	if ((err = HkGetCash(wscCharname, iCash)) != HKE_OK)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0055") + HkErrGetText(err));
		return true;
	}
	if (Cash > 0 && iCash < Cash)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0056"));
		return true;
	}

	// Снимайте наличные, если мы берем за это плату.
	if (Cash > 0)
		HkAddCash(wscCharname, 0 - Cash);

	string timestamp = pt::to_iso_string(pt::second_clock::local_time());
	wstring wscMsg = GetLocalized(iClientID, "MSG_1509");
	wscMsg = ReplaceStr(wscMsg, L"%stamp", stows(timestamp).c_str());
	wscMsg = ReplaceStr(wscMsg, L"%name", wscCharname.c_str());
	wscMsg = ReplaceStr(wscMsg, L"%money", boost::lexical_cast<std::wstring>(Cash));

	PrintUserCmdText(iClientID, wscMsg.c_str());
	string scText = wstos(wscMsg);
	GiftLogging("%s", scText.c_str());

	pub::Audio::PlaySoundEffect(iClientID, CreateID("ui_gain_level"));
	pub::Audio::PlaySoundEffect(iClientID, CreateID("dx_s076x_0401_juni"));

	return true;
}

void AdminCmd_GenerateID(CCmds* cmds, wstring argument)
{
	if (cmds->rights != RIGHT_SUPERADMIN)
	{
		cmds->Print(L"ERR No permission\n");
		return;
	}

	uint thegeneratedid = CreateID(wstos(argument).c_str());

	string s;
	stringstream out;
	out << thegeneratedid;
	s = out.str();

	wstring wscMsg = L"string <%sender> would equal to <%d> as internal id";
	wscMsg = ReplaceStr(wscMsg, L"%sender", argument.c_str());
	wscMsg = ReplaceStr(wscMsg, L"%d", stows(s).c_str());
	string scText = wstos(wscMsg);
	cmds->Print(L"OK %s\n", wscMsg.c_str());
	PMLogging("%s", scText.c_str());

	return;
}

void AdminCmd_missiontest1(CCmds* cmds, wstring argument)
{
	if (cmds->rights != RIGHT_SUPERADMIN)
	{
		cmds->Print(L"ERR No permission\n");
		return;
	}

	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);

		wstring wscXML = cmds->ArgStrToEnd(1);
		wstring wscPlayerInfo = L"<RDL><PUSH/><TEXT>" + wscXML + L"</TEXT><PARA/><PARA/><POP/></RDL>";

		HkChangeIDSString(iClientID, 500000, GetLocalized(iClientID, "MSG_1510"));
		HkChangeIDSString(iClientID, 526999, wscPlayerInfo);

		FmtStr caption(0, 0);
		caption.begin_mad_lib(500000);
		caption.end_mad_lib();

		FmtStr message(0, 0);
		message.begin_mad_lib(526999);
		message.end_mad_lib();

		pub::Player::PopUpDialog(iClientID, caption, message, POPUPDIALOG_BUTTONS_CENTER_OK);
		pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("mission_data_received"));
	}

	cmds->Print(L"OK\n");
	return;
}

void AdminCmd_missiontest2(CCmds* cmds, wstring argument)
{
	if (cmds->rights != RIGHT_SUPERADMIN)
	{
		cmds->Print(L"ERR No permission\n");
		return;
	}

	const wchar_t *wszTargetName = argument.c_str();

	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);
		HkChangeIDSString(iClientID, 526999, wszTargetName);

		FmtStr caption(0, 0);
		caption.begin_mad_lib(526999);
		caption.end_mad_lib();

		pub::Player::DisplayMissionMessage(iClientID, caption, MissionMessageType::MissionMessageType_Type1, true);
	}

	cmds->Print(L"OK\n");

	return;
}

void AdminCmd_missiontest2b(CCmds* cmds, wstring argument)
{
	if (cmds->rights != RIGHT_SUPERADMIN)
	{
		cmds->Print(L"ERR No permission\n");
		return;
	}

	HKPLAYERINFO adminPlyr;
	if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
	{
		cmds->Print(L"ERR Not in space\n");
		return;
	}

	uint iShip;
	pub::Player::GetShip(adminPlyr.iClientID, iShip);

	Vector pos;
	Matrix rot;
	pub::SpaceObj::GetLocation(iShip, pos, rot);

	uint iSystem;
	pub::Player::GetSystem(adminPlyr.iClientID, iSystem);

	const wchar_t *wszTargetName = argument.c_str();

	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		// Get the this player's current system and location in the system.
		uint iClientID2 = HkGetClientIdFromPD(pPD);
		uint iSystem2 = 0;
		pub::Player::GetSystem(iClientID2, iSystem2);
		if (iSystem != iSystem2)
			continue;

		HkChangeIDSString(iClientID2, 526999, wszTargetName);

		FmtStr caption(0, 0);
		caption.begin_mad_lib(526999);
		caption.end_mad_lib();

		pub::Player::DisplayMissionMessage(iClientID2, caption, MissionMessageType::MissionMessageType_Type1, true);
	}

	HkChangeIDSString(adminPlyr.iClientID, 526999, wszTargetName);

	FmtStr caption(0, 0);
	caption.begin_mad_lib(526999);
	caption.end_mad_lib();

	pub::Player::DisplayMissionMessage(adminPlyr.iClientID, caption, MissionMessageType::MissionMessageType_Type1, true);

	cmds->Print(L"OK\n");

	return;
}

bool UserCmd_MarkObjGroup(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	for (list<uint>::iterator Mark = MarkUsageTimer.begin(); Mark != MarkUsageTimer.end(); Mark++)
	{
		if (*Mark == iClientID)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0057"));
			return true;
		}
	}

	uint iShip, iTargetShip;
	pub::Player::GetShip(iClientID, iShip);
	pub::SpaceObj::GetTarget(iShip, iTargetShip);
	if (!iTargetShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0058"));
		return true;
	}

	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	uint iClientIDTarget = HkGetClientIDByShip(iTargetShip);

	if (!iClientIDTarget)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0059"));
		return true;
	}

	list<GROUP_MEMBER> lstMembers;
	HkGetGroupMembers((const wchar_t*)Players.GetActiveCharacterName(iClientID), lstMembers);

	foreach(lstMembers, GROUP_MEMBER, gm)
	{
		uint iClientShip;
		pub::Player::GetShip(gm->iClientID, iClientShip);
		if (iClientShip == iTargetShip)
			continue;

		wstring wscTargetCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientIDTarget);

		wstring wscMsg = GetLocalized(iClientID, "MSG_1511");
		wscMsg = ReplaceStr(wscMsg, L"%name", wscTargetCharname.c_str());
		wstring wscMsg2 = GetLocalized(iClientID, "MSG_1512");
		wscMsg2 = ReplaceStr(wscMsg2, L"%name", wscTargetCharname.c_str());
		wscMsg2 = ReplaceStr(wscMsg2, L"%player", wscCharname.c_str());

		PrintUserCmdText(gm->iClientID, wscMsg2.c_str());
		HkChangeIDSString(gm->iClientID, 526999, wscMsg);

		FmtStr caption(0, 0);
		caption.begin_mad_lib(526999);
		caption.end_mad_lib();

		//Register all players informed of the mark
		MarkUsageTimer.push_back(gm->iClientID);
		pub::Player::DisplayMissionMessage(gm->iClientID, caption, MissionMessageType::MissionMessageType_Type2, true);
	}

	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0060"));
	return true;
}

bool UserCmd_JettisonAll(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
{
	uint baseID = 0;
	pub::Player::GetBase(iClientID, baseID);
	if (baseID)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0061"));
		return true;
	}

	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	uint iSystem = 0;
	pub::Player::GetSystem(iClientID, iSystem);
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	Vector vLoc = { 0.0f, 0.0f, 0.0f };
	Matrix mRot = { 0.0f, 0.0f, 0.0f };
	pub::SpaceObj::GetLocation(iShip, vLoc, mRot);
	vLoc.x += 30.0;

	list<CARGO_INFO> lstCargo;
	int iRemainingHoldSize = 0;
	uint items = 0;
	if (HkEnumCargo(wscCharname, lstCargo, iRemainingHoldSize) == HKE_OK)
	{
		foreach(lstCargo, CARGO_INFO, item)
		{
			bool flag = false;
			pub::IsCommodity(item->iArchID, flag);
			if (!item->bMounted && flag)
			{
				HkRemoveCargo(wscCharname, item->iID, item->iCount);
				Server.MineAsteroid(iSystem, vLoc, CreateID("lootcrate_ast_loot_metal"), item->iArchID, item->iCount, iClientID);
				items++;
			}
		}
	}
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0062"), items);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef bool(*_UserCmdProc)(uint, const wstring &, const wstring &, const wchar_t*);

struct USERCMD
{
	wchar_t *wszCmd;
	_UserCmdProc proc;
	wchar_t *usage;
};


USERCMD UserCmds[] =
{
	/*{ L"/showrestrictions", UserCmd_ShowRestrictions, L"Usage: /showrestrictions"},
	{ L"/showrestrictions*", UserCmd_ShowRestrictions, L"Usage: /showrestrictions"},*/
	{ L"/nodock", ADOCK::NoDockCommand, L"Usage: /nodock"},
	{ L"/nodock*", ADOCK::NoDockCommand, L"Usage: /nodock"},
	{ L"/police", ADOCK::PoliceCmd, L"Usage: /nodock" },
	{ L"/police*", ADOCK::PoliceCmd, L"Usage: /nodock" },
	{ L"/pirate", PirateCmd, L"Usage: /pirate"},
	{ L"/pirate*", PirateCmd, L"Usage: /pirate"},
	//{ L"/racestart", AP::RacestartCmd, L"Usage: /racestart" },
	//{ L"/gift", GiftCmd, L"Usage: /gift amount"},
	//{ L"/gift*", GiftCmd, L"Usage: /gift amount"},
	{ L"$help", AP::AlleyCmd_Help, L"Usage: $help"},
	{ L"$help*", AP::AlleyCmd_Help, L"Usage: $help"},
	{ L"$chase", AP::AlleyCmd_Chase, L"Usage: $chase <charname>"},
	{ L"$chase*", AP::AlleyCmd_Chase, L"Usage: $chase <charname>"},
	{ L"/marktarget", UserCmd_MarkObjGroup, L"Usage: /marktarget"},
	{ L"/marktarget*", UserCmd_MarkObjGroup, L"Usage: /marktarget"},
	{ L"/jettisonall", UserCmd_JettisonAll, L"Usage: /jettisonall"},
};


/**
Эта функция вызывается FLHook, когда пользователь вводит строку чата. We look at the
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

#define IS_CMD(a) !wscCmd.compare(L##a)

bool ExecuteCommandString_Callback(CCmds* cmds, const wstring &wscCmd)
{
	returncode = DEFAULT_RETURNCODE;

	if (IS_CMD("showrestrictions"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		return true;
	}
	else if (IS_CMD("generateid"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		AdminCmd_GenerateID(cmds, cmds->ArgStrToEnd(1));
		return true;
	}
	else if (IS_CMD("shiptest"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK)
		{
			cmds->Print(L"ERR\n");
			return true;
		}

		Archetype::Ship* TheShipArch = Archetype::GetShip(Players[adminPlyr.iClientID].iShipArchetype);
		PrintUserCmdText(adminPlyr.iClientID, GetLocalized(adminPlyr.iClientID, "MSG_0063"), TheShipArch->iArchID);

		return true;
	}
	else if (IS_CMD("healthtest1"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK)
		{
			cmds->Print(L"ERR\n");
			return true;
		}

		uint iShip;
		pub::Player::GetShip(adminPlyr.iClientID, iShip);

		float curr, max;
		pub::SpaceObj::GetHealth(iShip, curr, max);

		float woop = curr - 2000.0f;
		pub::SpaceObj::SetRelativeHealth(iShip, woop);
		return true;
	}
	else if (IS_CMD("healthtest2"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK)
		{
			cmds->Print(L"ERR\n");
			return true;
		}

		uint iShip;
		pub::Player::GetShip(adminPlyr.iClientID, iShip);

		float curr, max;
		pub::SpaceObj::GetHealth(iShip, curr, max);

		float woop = curr + 2000.0f;

		if (woop > max)
		{
			HkMsgU(L"DEBUG: woop > max");
			return true;
		}
		pub::SpaceObj::SetRelativeHealth(iShip, woop);
		return true;
	}
	else if (IS_CMD("missiontest1"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		AdminCmd_missiontest1(cmds, cmds->ArgStrToEnd(1));
		return true;
	}
	else if (IS_CMD("missiontest2"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		AdminCmd_missiontest2(cmds, cmds->ArgStrToEnd(1));
		return true;
	}
	else if (IS_CMD("missiontest2b"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		AdminCmd_missiontest2b(cmds, cmds->ArgStrToEnd(1));
		return true;
	}
	else if (IS_CMD("nodock"))
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		ADOCK::AdminNoDock(cmds, cmds->ArgCharname(1));
		return true;
	}
	else if (IS_CMD("testfuseobj"))
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return true;
		}

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return true;
		}

		string fuse = wstos(cmds->ArgStrToEnd(1));

		uint space_obj = 0;
		pub::SpaceObj::GetTarget(adminPlyr.iShip, space_obj);
		pub::SpaceObj::LightFuse(space_obj, fuse.c_str(), 0);
		return true;
	}
	else if (IS_CMD("testunfuseobj"))
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return true;
		}

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return true;
		}

		uint space_obj = 0;
		pub::SpaceObj::GetTarget(adminPlyr.iShip, space_obj);

		uint fuse = CreateID(wstos(cmds->ArgStrToEnd(1)).c_str());
		uint dunno;
		IObjInspectImpl *inspect;

		if (GetShipInspect(space_obj, inspect, dunno))
		{
			HkUnLightFuse((IObjRW*)inspect, fuse, 0);
			cmds->Print(L"OK unlighted fuse");
		}

		return true;
	}
	else if (IS_CMD("testselffuseobj"))
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return true;
		}

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return true;
		}

		string fuse = wstos(cmds->ArgStrToEnd(1));

		pub::SpaceObj::LightFuse(adminPlyr.iShip, fuse.c_str(), 0);
		return true;
	}
	else if (IS_CMD("testselfunfuseobj"))
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return true;
		}

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return true;
		}


		uint fuse = CreateID(wstos(cmds->ArgStrToEnd(1)).c_str());
		uint dunno;
		IObjInspectImpl *inspect;

		if (GetShipInspect(adminPlyr.iShip, inspect, dunno))
		{
			HkUnLightFuse((IObjRW*)inspect, fuse, 0);
			cmds->Print(L"OK unlighted fuse");
		}

		return true;
	}
	return false;
}

void __stdcall HkCb_AddDmgEntry_AFTER(DamageList *dmg, unsigned short p1, float damage, enum DamageEntry::SubObjFate fate)
{
	returncode = DEFAULT_RETURNCODE;
	if (iDmgToSpaceID && dmg->get_inflictor_id() && dmg->is_inflictor_a_player())
	{
		uint client = HkGetClientIDByShip(iDmgToSpaceID);
		if (client)
		{
			uint ShootingClient = dmg->get_inflictor_owner_player();
			Archetype::Ship* TheShipArch = Archetype::GetShip(Players[ShootingClient].iShipArchetype);

			if (TheShipArch->iShipClass == 19)
			{
				float curr, max;
				pub::SpaceObj::GetHealth(iDmgToSpaceID, curr, max);
				float expecteddmg = (float)1;
				float projecteddamage = curr - damage;

				//Если ремонтная пушка исцеляет корабль, но это не отображается, значит, она попадает в щит.
				//HkMsgU(L"DEBUG: повреждения, полученные ремонтным кораблем, излечиваются?"); 
				//PrintUserCmdText(клиент, GetLocalized(iClientID, "MSG_0064"), projecteddamage);
				if ((projecteddamage <= 1) && (projecteddamage > 0))
				{
					//Займись исцелением.
					returncode = SKIPPLUGINS_NOFUNCTIONCALL;

					Archetype::Ship* TheShipArchHealed = Archetype::GetShip(Players[client].iShipArchetype);
					float amounttoheal = curr;

					//если ключ НЕ НАЙДЕН в healingMultipliers то не продолжаем
					if (healingMultipliers.find(TheShipArchHealed->iShipClass) == healingMultipliers.end())
					{
						return;
					}
					// здесь нет необходимости проверять healingAdditions, 
					// так как он устанавливается одновременно с healingMultipliers
					amounttoheal = max / 100 * healingMultipliers[TheShipArchHealed->iShipClass] + healingAdditions[TheShipArchHealed->iShipClass];

					float testhealth = curr + amounttoheal;

					if (testhealth > max)
					{
						//HkMsgU(L"DEBUG: Health would be superior to max");
						dmg->add_damage_entry(1, max, (DamageEntry::SubObjFate)0);
						return;
					}
					else
					{
						//HkMsgU(L"DEBUG: Health less max");
						dmg->add_damage_entry(1, testhealth, (DamageEntry::SubObjFate)0);
						return;
					}
				}
				//в противном случае ничего не предпринимайте, значит, это не призыв к исцелению.
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Фактический код
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Очистка при отключении клиента */
void ClearClientInfo(uint iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	AP::ClearClientInfo(iClientID);
	ADOCK::ClearClientInfo(iClientID);
	SCI::ClearClientInfo(iClientID);
}

void __stdcall SystemSwitchOutComplete(unsigned int iShip, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	// Делает игрока неуязвимым, позволяя иногда устанавливать JHs / JGS вблизи минных полей
	// игрок взрывается во время прыжка (в туннеле для прыжков)
	pub::SpaceObj::SetInvincible(iShip, true, true, 0);
	if (AP::SystemSwitchOutComplete(iShip, iClientID))
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
}

void JettisonCargo(unsigned int iClientID, struct XJettisonCargo const &jc)
{
	returncode = DEFAULT_RETURNCODE;
	for (list<EquipDesc>::iterator item = Players[iClientID].equipDescList.equip.begin(); item != Players[iClientID].equipDescList.equip.end(); item++)
	{
		if (item->sID == jc.iSlot)
		{
			//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0065"));
			for (map<uint, string>::iterator i = notradelist.begin(); i != notradelist.end(); ++i)
			{
				if (i->first == item->iArchID)
				{
					returncode = SKIPPLUGINS_NOFUNCTIONCALL;
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0066"), stows(i->second).c_str());
				}
			}
		}
	}
}

void AddTradeEquip(unsigned int iClientID, struct EquipDesc const &ed)
{
	if (notradelist.find(ed.iArchID) != notradelist.end())
	{
		for (map<uint, string>::iterator i = notradelist.begin(); i != notradelist.end(); ++i)
		{
			if (i->first == ed.iArchID)
			{
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0067"), stows(i->second).c_str());
			}
		}
	}
	else
	{
		returncode = DEFAULT_RETURNCODE;
	}
}

void __stdcall BaseEnter_AFTER(unsigned int iBaseID, unsigned int iClientID)
{
	//ClearClientInfo(iClientID);
	returncode = DEFAULT_RETURNCODE;
	//wstring wscIp = L"???";
	//HkGetPlayerIP(iClientID, wscIp);

	//string scText = wstos(wscIp);
	//PMLogging("BaseEnter: %s", scText.c_str());
	AP::BaseEnter_AFTER(iBaseID, iClientID);
}

void __stdcall PlayerLaunch_AFTER(unsigned int iShip, unsigned int client)
{
	//wstring wscIp = L"???";
	//HkGetPlayerIP(client, wscIp);
	//string scText = wstos(wscIp);
	//PMLogging("PlayerLaunch: %s", scText.c_str());

	ADOCK::PlayerLaunch(iShip, client);
	SCI::CheckOwned(client);
	SCI::UpdatePlayerID(client);
}

int __cdecl Dock_Call(unsigned int const &iShip, unsigned int const &iDockTarget, int iCancel, enum DOCK_HOST_RESPONSE response)
{
	returncode = DEFAULT_RETURNCODE;

	uint iClientID = HkGetClientIDByShip(iShip);

	if (iClientID && (response == PROCEED_DOCK || response == DOCK) && iCancel != -1)
	{
		if (!ADOCK::IsDockAllowed(iShip, iDockTarget, iClientID))
		{
			returncode = SKIPPLUGINS_NOFUNCTIONCALL;
			return 0;
		}
		if (!SCI::CanDock(iDockTarget, iClientID))
		{
			returncode = SKIPPLUGINS_NOFUNCTIONCALL;
			return 0;
		}
	}
	return 0;
}

void __stdcall BaseExit(unsigned int iBaseID, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	SCI::CheckItems(iClientID);
}

void __stdcall ReqAddItem(unsigned int iArchID, char const *Hardpoint, int count, float p4, bool bMounted, unsigned int iClientID)
{
	/*
	string HP(Hardpoint);
	if (HP == "BAY")
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0068"));
	}
	else
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0069"));
	}
	*/
}

void __stdcall ReqEquipment(class EquipDescList const &edl, unsigned int iClientID)
{


	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0070"));
	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0071"));
	//returncode = SKIPPLUGINS_NOFUNCTIONCALL;
}

void __stdcall ReqModifyItem(unsigned short iArchID, char const *Hardpoint, int count, float p4, bool bMounted, unsigned int iClientID)
{
	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0072"));
}

void __stdcall ReqRemoveItem(unsigned short slot, int amount, unsigned int iClientID)
{
	/*
	for (EquipDescListItem *item = Players[iClientID].equipDescList.pFirst->next;
		item != Players[iClientID].equipDescList.pFirst; item = item->next)
	{
		if (item->equip.sID == slot)
		{
			if (string(item->equip.szHardPoint.value) == "BAY")
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0073"));
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0074"), item->equip.get_count());
			}
			else
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0075"));
			}
		}
	}
	*/
}

void __stdcall DisConnect(unsigned int iClientID, enum  EFLConnection state)
{
	returncode = DEFAULT_RETURNCODE;
	ClearClientInfo(iClientID);
}

void __stdcall CharacterSelect_AFTER(struct CHARACTER_ID const &charId, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	ClearClientInfo(iClientID);
}

typedef void(*_TimerFunc)();
struct TIMER
{
	_TimerFunc  proc;
	mstime      tmIntervallMS;
	mstime      tmLastCall;
};

struct DangerousPlayer
{
	uint iClientID;
	int ticksToWait; // количество тиков таймера до проверки
};

TIMER Timers[] =
{
	{TractorCheatersCheckerPlayers, 100, 0},
};

EXPORT int __stdcall Update()
{
	returncode = DEFAULT_RETURNCODE;

	for (uint i = 0; (i < sizeof(Timers) / sizeof(TIMER)); i++)
	{
		if ((timeInMS() - Timers[i].tmLastCall) >= Timers[i].tmIntervallMS)
		{
			Timers[i].tmLastCall = timeInMS();
			Timers[i].proc();
		}
	}
	return 0;
}

// Например, таймер срабатывает каждые 50 мс, а нужна задержка 1 секунда:
// ticksToWait = 1000 / 50 = 20
vector<DangerousPlayer> g_dangerousPlayers;
// Бэкап карго
map<uint, list<CARGO_INFO>> g_backupCargo;

void TractorCheatersCheckerPlayers()
{
	for (std::vector<DangerousPlayer>::iterator it = g_dangerousPlayers.begin(); it != g_dangerousPlayers.end(); )
	{
		uint iClientID = it->iClientID;
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		// Ждём нужное количество тиков
		if (it->ticksToWait > 0)
		{
			it->ticksToWait--;
			++it;
			//PrintUserCmdText(iClientID, L"[Cheating Checker] Waiting cargo check: " + wscCharname);
			continue;
		}

		// Получаем текущее содержимое трюма
		list<CARGO_INFO> lstCurrent;
		int iRem = 0;
		HkEnumCargo(ARG_CLIENTID(iClientID), lstCurrent, iRem);

		// Получаем бэкап до тракта
		list<CARGO_INFO>& lstBackup = g_backupCargo[iClientID];

		// Считаем суммарное количество по ArchID
		std::map<uint, int> backupAmount;
		for (list<CARGO_INFO>::iterator b = lstBackup.begin(); b != lstBackup.end(); ++b)
			backupAmount[b->iArchID] += b->iCount;

		std::map<uint, int> currentAmount;
		for (list<CARGO_INFO>::iterator c = lstCurrent.begin(); c != lstCurrent.end(); ++c)
			currentAmount[c->iArchID] += c->iCount;

		// Проверяем запрещённые предметы
		for (std::map<uint, int>::iterator ci = currentAmount.begin(); ci != currentAmount.end(); ++ci)
		{
			uint archID = ci->first;
			int curCount = ci->second;

			if (notractorbeamlist.find(archID) == notractorbeamlist.end())
				continue;

			int prevCount = 0;
			if (backupAmount.find(archID) != backupAmount.end())
				prevCount = backupAmount[archID];

			if (curCount > prevCount)
			{
				int diff = curCount - prevCount;
				//PrintUserCmdText(iClientID, L"[Cheating] Forbidden cargo detected: ArchID:" + to_wstring(archID) +
				//	L" Prev:" + to_wstring(prevCount) + L" Current:" + to_wstring(curCount) +
				//	L" Diff:" + to_wstring(diff));

				int remaining = diff;
				for (list<CARGO_INFO>::iterator c = lstCurrent.begin(); c != lstCurrent.end(); ++c)
				{
					if (c->iArchID != archID)
						continue;

					int removeCount = (c->iCount < remaining) ? c->iCount : remaining;
					wstring wscCharname2 = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
					HkRemoveCargo(wscCharname2, c->iID, removeCount);
					
					/*Archetype::Equipment* eq = Archetype::GetEquipment(c->iArchID);
					const GoodInfo* gi = GoodList::find_by_id(c->iArchID);
					if (!gi)
						continue;
					Archetype::Gun* gun = (Archetype::Gun*)eq;
					if (gun->iArchID)
					{
						if (gi->iType == 0)
						{
							PrintUserCmdText(iClientID,  HkGetWStringFromIDS(gun->iIdsName) + L" = ");
						}
						else PrintUserCmdText(iClientID, HkGetWStringFromIDS(gun->iIdsName) + L" = ");
					}
					else
					{
						PrintUserCmdText(iClientID, HkGetWStringFromIDS(gi->iIDSName) + L" = ");
					}*/
					
					//try {
					//	const GoodInfo* gir = GoodList::find_by_id(c->iArchID);
					//	if (gir)
					//	{
					//		wstring wName;
					//		if (HkGetWStringFromIDS(gir->iIDSName).empty()) {
					//			wName = stows("KIOPKOPKOPKOPKOPKOPKOPKOPKOPKOPKOP"); // или gir->strName или что там в структуре предмета реально хранит текст
					//		}
					//		else {
					//			wName = HkGetWStringFromIDS(gir->iIDSName);
					//		}
					//		PrintUserCmdText(iClientID, L"[Cheating] Forbidden cargo removed: " + wName);

					//		PrintUserCmdText(iClientID, L"[Cheating] Forbidden cargo removed: ");
					//		PrintUserCmdText(iClientID, HkGetWStringFromIDS(gir->iIDSName).c_str());
					//	}
					//}
					//catch (const std::exception& e) {
					//	AddLog("Exception caught in alley: %s", e.what());
					//}
					//catch (...) {
					//	AddLog("Unknown exception caught in alley Update");
					//}
					remaining -= removeCount;
					if (remaining <= 0)
						break;
				}
			
				AddLog("CheatDetect: %s had forbidden cargo removed: ArchID %d, Amount %d",
					wstos(wscCharname).c_str(), archID, diff);
			}
		}

		g_backupCargo.erase(iClientID);
		it = g_dangerousPlayers.erase(it);
	}
}

// Предотвращение попытки захвата груза который запрещено подбирать или трейдить или выбрасывать
void __stdcall TractorObjects(unsigned int iClientID, struct XTractorObjects const& objs)
{
	returncode = DEFAULT_RETURNCODE;
	// Получаем текущее содержимое трюма игрока
	list<CARGO_INFO> lstCargo;
	int iRem = 0;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	// Сохраняем в бэкап
	g_backupCargo[iClientID] = list<CARGO_INFO>(lstCargo.begin(), lstCargo.end());;

	// Добавляем игрока в список опасных
	DangerousPlayer dp;
	dp.iClientID = iClientID;
	dp.ticksToWait = 40; // 1 секунда, если таймер срабатывает каждые 50 мс
	g_dangerousPlayers.push_back(dp);

	//wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	//PrintUserCmdText(iClientID, L"[Cheating Checker] I see you: " + wscCharname);
}

void HkTimerCheckKick()
{
	uint curr_time = (uint)time(0);
	ADOCK::Timer();

	//Каждые 15 секунд стирайте список таймеров.
	if ((curr_time % 15) == 0)
	{
		MarkUsageTimer.clear();
	}

	AP::Timer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions to hook
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "AlleyPlugin by Alley";
	p_PI->sShortName = "alley";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Update, PLUGIN_HkIServerImpl_Update, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkTimerCheckKick, PLUGIN_HkTimerCheckKick, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ExecuteCommandString_Callback, PLUGIN_ExecuteCommandString_Callback, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkCb_AddDmgEntry_AFTER, PLUGIN_HkCb_AddDmgEntry_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&JettisonCargo, PLUGIN_HkIServerImpl_JettisonCargo, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&AddTradeEquip, PLUGIN_HkIServerImpl_AddTradeEquip, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseEnter_AFTER, PLUGIN_HkIServerImpl_BaseEnter_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ClearClientInfo, PLUGIN_ClearClientInfo, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&SystemSwitchOutComplete, PLUGIN_HkIServerImpl_SystemSwitchOutComplete, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&PlayerLaunch_AFTER, PLUGIN_HkIServerImpl_PlayerLaunch_AFTER, 0));
	//	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ShipDestroyed, PLUGIN_ShipDestroyed, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&TractorObjects, PLUGIN_HkIServerImpl_TractorObjects_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Dock_Call, PLUGIN_HkCb_Dock_Call, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseExit, PLUGIN_HkIServerImpl_BaseExit, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqAddItem, PLUGIN_HkIServerImpl_ReqAddItem, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqEquipment, PLUGIN_HkIServerImpl_ReqEquipment, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqModifyItem, PLUGIN_HkIServerImpl_ReqModifyItem, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqRemoveItem, PLUGIN_HkIServerImpl_ReqRemoveItem, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&DisConnect, PLUGIN_HkIServerImpl_DisConnect, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CharacterSelect_AFTER, PLUGIN_HkIServerImpl_CharacterSelect_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&SetVisitedState, PLUGIN_HkIServerImpl_SetVisitedState, 0));

	return p_PI;
}
