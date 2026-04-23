/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 апреля 2026 06:54:28
 * Version: 1.0.471
 */

#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

PLUGIN_RETURNCODE returncode;
AC_DATA Antiecheat[250];
list<INISECTIONVALUE> lstAnticheatpaths;
list<INISECTIONVALUE> lstAnticheatkey;
list<INISECTIONVALUE> lstAnticheatcrcs;
int set_iTimeAC;
wstring set_wsPrison;
list<INISECTIONVALUE> lstKnowdlls;
bool FullLog = false;

EXPORT void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	string set_scBSGFile;
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	set_scBSGFile = string(szCurDir) + "\\flhook_plugins\\anticheat.ini";
	lstAnticheatpaths.clear();
	lstAnticheatkey.clear();
	lstAnticheatcrcs.clear();
	lstKnowdlls.clear();
	IniGetSection(set_scBSGFile, "Anticheatpaths", lstAnticheatpaths);
	IniGetSection(set_scBSGFile, "Anticheatcrcs", lstAnticheatcrcs);
	IniGetSection(set_scBSGFile, "Anticheatkeywords", lstAnticheatkey);
	set_iTimeAC = IniGetI(set_scBSGFile, "General", "TimeAC", 10);
	set_wsPrison = stows(IniGetS(set_scBSGFile, "General", "Prison", ""));
	IniGetSection(set_scBSGFile, "Knowdlls", lstKnowdlls);
}

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		LoadSettings();
	return true;
}

void AdminTest(unsigned int iClientID)
{
	//CAccount *acc = Players.FindAccountFromClientID(iClientID);
	//wstring wscAccDirname;
	//HkGetAccountDirName(acc, wscAccDirname);
	//string scAdminFile = scAcctPath + wstos(wscAccDirname) + "\\flhookadmin.ini";
	//WIN32_FIND_DATA fd;
	//HANDLE hFind = FindFirstFile(scAdminFile.c_str(), &fd);
	//if (hFind != INVALID_HANDLE_VALUE)
	//{
	//	Antiecheat[iClientID].AntiCheat = false;
	//	Antiecheat[iClientID].IsAdmin = true;
	//}
}

namespace HkIServerImpl
{
	EXPORT void __stdcall SubmitChat_AFTER(struct CHAT_ID cId, unsigned long lP1, void const *rdlReader, struct CHAT_ID cIdTo, int iP2)
	{
		returncode = DEFAULT_RETURNCODE;


		//wchar_t wszBuf[1024] = L"";
		//uint iClientID = cId.iID;
		//BinaryRDLReader rdl;
		//uint iRet1;
		//rdl.extract_text_from_buffer((unsigned short*)wszBuf, sizeof(wszBuf), iRet1, (const char*)rdlReader, lP1);
		//wstring wscBuf = wszBuf;
		//g_iTextLen = (uint)wscBuf.length();
		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0089"));

		////AntiCheat .test
		//if (wszBuf[0] == '@' && g_iTextLen > 1 && wszBuf[1] != '@')
		//{
		//	if (Antiecheat[iClientID].IsAdmin)
		//	{
		//		return;
		//	}
		//	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		//	if (wscBuf.find(L"@pass dcurechensky") == 0)
		//	{
		//		Antiecheat[iClientID].AntiCheat = false;
		//		foreach(lstAnticheatpaths, INISECTIONVALUE, path)
		//		{
		//			HkMsg(wscCharname, stows(path->scKey));
		//		}
		//		HkMsg(wscCharname, L"process");
		//		HkMsg(wscCharname, L"time");
		//	}

		//	if (wscBuf.find(L"@filecrc") == 0)
		//	{
		//		bool kick = true;
		//		wstring crc = GetParam(wszBuf, ' ', 2);
		//		foreach(lstAnticheatcrcs, INISECTIONVALUE, path)
		//		{
		//			wstring crc1 = stows(path->scKey);
		//			if (crc == crc1)
		//			{
		//				kick = false;
		//			}
		//			else
		//			{
		//				ConPrint(L"crc cheat %s %s %s\n", wscCharname.c_str(), crc1, stows(path->scKey));
		//			}
		//		}
		//		if (crc.find(L"no file found") == 0)
		//		{
		//			kick = true;
		//		}
		//		if (kick)
		//		{
		//			wstring wscBufErase = wscBuf;
		//			wscBufErase.erase(0, 9);
		//			ConPrint(L"crc cheat %s %s\n", wscCharname.c_str(), wscBufErase.c_str());
		//			HkAddChatLogCRC(iClientID, wscBufErase);
		//			if (set_wsPrison.length() > 0)
		//			{
		//				Antiecheat[iClientID].CrC = true;
		//			}
		//		}
		//	}

		//	if (wscBuf.find(L"@time") == 0)
		//	{
		//		int test = ToInt(GetParam(wszBuf, ' ', 1));
		//		if (test > set_iTimeAC)
		//		{
		//			HkMsg(wscCharname, L"atprocess");
		//			wstring wscBufErase = wscBuf;
		//			wscBufErase.erase(0, 6);
		//			ConPrint(L"timetest %s %u\n", wscCharname.c_str(), test);
		//			HkAddChatLogSpeed(iClientID, wscBufErase);
		//			HkMsgAndKick(iClientID, L"РЎРІСЏР¶РёС‚РµСЃСЊ СЃ Р°РґРјРёРЅРёСЃС‚СЂР°С‚РѕСЂРѕРј", set_iKickMsgPeriod);
		//		}
		//	}

		//	if (wscBuf.find(L"@atprocess") == 0)
		//	{
		//		wstring wscBufErase = wscBuf;
		//		wscBufErase.erase(0, 11);
		//		string test = wstos(wscBufErase);
		//		bool log = true;
		//		foreach(lstKnowdlls, INISECTIONVALUE, dlls)
		//		{
		//			if (test.find(dlls->scKey) != -1)
		//			{
		//				log = false;
		//			}
		//		}
		//		if (log)
		//		{
		//			HkAddChatLogATProc(iClientID, wscBufErase);
		//		}
		//	}

		//	if (wscBuf.find(L"@process") == 0)
		//	{
		//		wstring wscBufErase = wscBuf;
		//		wscBufErase.erase(0, 9);
		//		string test = wstos(wscBufErase);
		//		bool log = false;
		//		foreach(lstAnticheatkey, INISECTIONVALUE, key)
		//		{
		//			if (test.find(key->scKey) != -1)
		//			{
		//				log = true;
		//			}
		//		}
		//		if (log)
		//		{
		//			ConPrint(L"Process cheat detected %s %s\n", wscCharname.c_str(), wscBufErase.c_str());
		//			HkAddChatLogProc(iClientID, wscBufErase); //will only log when keywords are found and display in the console
		//			HkMsgAndKick(iClientID, L"Р’Р°СЃ РІС‹РіРЅР°Р»Рё, РїРѕРёС‰РёС‚Рµ РЅР° С„РѕСЂСѓРјРµ РёРЅС„РѕСЂРјР°С†РёСЋ Рѕ Р·Р°РїСЂРµС‰РµРЅРЅС‹С… РїСЂРёР»РѕР¶РµРЅРёСЏС….", set_iKickMsgPeriod);
		//		}
		//		if (FullLog)
		//		{
		//			HkAddChatLogProc(iClientID, wscBufErase);
		//		}
		//	}
		//}
	}

	typedef void(*_TimerFunc)();
	struct TIMER
	{
		_TimerFunc	proc;
		mstime		tmIntervallMS;
		mstime		tmLastCall;
	};

	TIMER Timers[] =
	{
		{HkTimerAntiCheat,     2000,             0},
		{HkTimerCrC,           9000,             0},
		{HkTimerCheckShip,     10000,            0}	
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

	EXPORT void __stdcall PlayerLaunch_AFTER(unsigned int iShip, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;
		//wstring wscCharname = ToLower((const wchar_t*)Players.GetActiveCharacterName(iClientID));
		//HkMsg(wscCharname, L"test");
		//AdminTest(iClientID);
	}

	EXPORT void __stdcall CharacterSelect_AFTER(struct CHARACTER_ID const & cId, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;
		//wstring wscCharname = ToLower((const wchar_t*)Players.GetActiveCharacterName(iClientID));
		//Antiecheat[iClientID].AntiCheatT = timeInMS() + 20000;
		//Antiecheat[iClientID].AntiCheat = true;
		//Antiecheat[iClientID].IsAdmin = false;
		//Antiecheat[iClientID].CrC = false;
		//AdminTest(iClientID);
		//HkMsg(wscCharname, L"test");
	}
}

//Anticheat kick timer
void HkTimerAntiCheat()
{
	/*struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);
		if (ClientInfo[iClientID].tmF1TimeDisconnect)
			continue;

		DPN_CONNECTION_INFO ci;
		if (HkGetConnectionStats(iClientID, ci) != HKE_OK)
			continue;

		if (timeInMS() > Antiecheat[iClientID].AntiCheatT && Antiecheat[iClientID].AntiCheat)
		{
			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			ConPrint(L"%s No AntiCheat Found\n", wscCharname.c_str());
			HkAddKickLog(iClientID, L"No Anti Cheat Found");
			Antiecheat[iClientID].AntiCheatT = timeInMS() + 50000;
			HkMsgAndKick(iClientID, L"РЎРІСЏР¶РёС‚РµСЃСЊ СЃ Р°РґРјРёРЅРѕРј! Р’РµСЂРѕСЏС‚РЅРѕ client.dll РІ РїР°РїРєРµ EXE(РёРіСЂС‹) РѕС‚СЃСѓС‚СЃС‚РІСѓРµС‚ РёР»Рё Р·Р°Р±Р»РѕРєРёСЂРѕРІР°РЅ, РїРѕР¶Р°Р»СѓР№СЃС‚Р° РґРѕР±Р°РІСЊС‚Рµ РµРіРѕ РІ РёСЃРєР»СЋС‡РµРЅРёСЏ Р°РЅС‚РёРІРёСЂСѓСЃР°.", set_iKickMsgPeriod);
		}
	}*/
}

void HkTimerCrC()
{
	/*struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);
		if (ClientInfo[iClientID].tmF1TimeDisconnect)
			continue;

		DPN_CONNECTION_INFO ci;
		if (HkGetConnectionStats(iClientID, ci) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0090"));
			continue;
		}

		uint iShip = 0;
		pub::Player::GetShip(iClientID, iShip);

		if (Antiecheat[iClientID].CrC && iShip)
		{
			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			HkBeam(wscCharname, set_wsPrison);
			Antiecheat[iClientID].CrC = false;
		}
	}*/
}

/// <summary>
/// РџСЂРѕРІРµСЂСЏРµС‚ РїСЂР°РІРёР»СЊРЅРѕСЃС‚СЊ С…Р°СЂР°РєС‚РµСЂРёСЃС‚РёРє РєРѕСЂР°Р±Р»СЏ Сѓ РїРѕР»СЊР·РѕРІР°С‚РµР»СЏ РЅР° СЃРµСЂРІРµСЂРµ
/// РЎСЂР°РІРЅРёРІР°РµС‚ РµРіРѕ РєРѕСЂР°Р±Р»СЊ СЃ РµРіРѕ РїР°СЂР°РјРµС‚СЂР°РјРё СЃ СѓСЃС‚Р°РЅРѕРІР»РµРЅРЅС‹РјРё РЅР° СЃРµСЂРІРµСЂРµ
/// </summary>
void HkTimerCheckShip()
{
	//struct PlayerData* pPD = 0;
	//while (pPD = Players.traverse_active(pPD))
	//{
	//	uint iClientID = HkGetClientIdFromPD(pPD);

	//	uint srvProcPlayerShip = 0;
	//	//РїРѕР»СѓС‡Р°РµРј РєРѕСЂР°Р±Р»СЊ РёР· С„Р°Р№Р»РѕРІ СЃРµСЂРІРµСЂР°
	//	pub::Player::GetShipID(iClientID, srvProcPlayerShip);
	//	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1287"), to_wstring(srvProcPlayerShip));
	//	
	//	//СЃСЂР°РІРЅРёРІР°РµРј РµРіРѕ С…Р°СЂР°РєС‚РёСЂРёСЃС‚РёРєРё РјР°РєСЃРёРјР°Р»СЊРЅС‹Рµ СЃ С‚РµРј С‡С‚Рѕ СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ Сѓ РёРіСЂРѕРєР°




	//}
}


//Anticheat
bool HkAddChatLogSpeed(uint iClientID, wstring wscMessage)
{
	/*time_t tNow = time(0);
	struct tm *stNow = localtime(&tNow);
	FILE *f = fopen((scAcctPath + "speedhack" + itos(stNow->tm_mon + 1) + "_" + itos(stNow->tm_mday) + "_" + itos(stNow->tm_year + 1900) + ".log").c_str(), "at");
	if (!f)
		return false;
	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	CAccount *acc = HkGetAccountByCharname(wscCharname);
	wstring wscAccountDir;
	HkGetAccountDirName(acc, wscAccountDir);
	fprintf(f, "%s[%s]%.2d:%.2d:%.2d[%s]\n", wstos(wscCharname).c_str(), wstos(wscMessage).c_str(), stNow->tm_hour, stNow->tm_min, stNow->tm_sec, wstos(wscAccountDir).c_str());
	fclose(f);*/
	return true;
}

/**
Р­С‚Р° С„СѓРЅРєС†РёСЏ РІС‹Р·С‹РІР°РµС‚СЃСЏ FLHook, РєРѕРіРґР° РїРѕР»СЊР·РѕРІР°С‚РµР»СЊ РІРІРѕРґРёС‚ СЃС‚СЂРѕРєСѓ С‡Р°С‚Р°. 
Р•СЃР»Рё СЌС‚Рѕ С‚Р°Рє, РјС‹ РїС‹С‚Р°РµРјСЃСЏ СЌС‚Рѕ РѕР±СЂР°Р±РѕС‚Р°С‚СЊ.
*/
bool UserCmd_Process(uint iClientID, const wstring& wscCmd)
{
	returncode = DEFAULT_RETURNCODE;

	pub::Audio::PlaySoundEffect(iClientID, CreateID("add_cmd"));

	return false;
}


bool HkAddChatLogProc(uint iClientID, wstring wscMessage)
{
	/*time_t tNow = time(0);
	struct tm *stNow = localtime(&tNow);
	FILE *f = fopen((scAcctPath + "process" + itos(stNow->tm_mon + 1) + "_" + itos(stNow->tm_mday) + "_" + itos(stNow->tm_year + 1900) + ".log").c_str(), "at");
	if (!f)
		return false;
	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	CAccount *acc = HkGetAccountByCharname(wscCharname);
	wstring wscAccountDir;
	HkGetAccountDirName(acc, wscAccountDir);
	fprintf(f, "%s[%s]%.2d:%.2d:%.2d[%s]\n", wstos(wscCharname).c_str(), wstos(wscMessage).c_str(), stNow->tm_hour, stNow->tm_min, stNow->tm_sec, wstos(wscAccountDir).c_str());
	fclose(f);*/
	return true;
}

bool HkAddChatLogATProc(uint iClientID, wstring wscMessage)
{
	/*time_t tNow = time(0);
	struct tm *stNow = localtime(&tNow);
	FILE *f = fopen((scAcctPath + "atprocess" + itos(stNow->tm_mon + 1) + "_" + itos(stNow->tm_mday) + "_" + itos(stNow->tm_year + 1900) + ".log").c_str(), "at");
	if (!f)
		return false;
	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	CAccount *acc = HkGetAccountByCharname(wscCharname);
	wstring wscAccountDir;
	HkGetAccountDirName(acc, wscAccountDir);
	fprintf(f, "%s[%s]%.2d:%.2d:%.2d[%s]\n", wstos(wscCharname).c_str(), wstos(wscMessage).c_str(), stNow->tm_hour, stNow->tm_min, stNow->tm_sec, wstos(wscAccountDir).c_str());
	fclose(f);*/
	return true;
}

bool HkAddChatLogCRC(uint iClientID, wstring wscMessage)
{
	/*time_t tNow = time(0);
	struct tm *stNow = localtime(&tNow);
	FILE *f = fopen((scAcctPath + "crccheats" + itos(stNow->tm_mon + 1) + "_" + itos(stNow->tm_mday) + "_" + itos(stNow->tm_year + 1900) + ".log").c_str(), "at");
	if (!f)
		return false;
	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	CAccount *acc = HkGetAccountByCharname(wscCharname);
	wstring wscAccountDir;
	HkGetAccountDirName(acc, wscAccountDir);
	fprintf(f, "%s[%s]%.2d:%.2d:%.2d[%s]\n", wstos(wscCharname).c_str(), wstos(wscMessage).c_str(), stNow->tm_hour, stNow->tm_min, stNow->tm_sec, wstos(wscAccountDir).c_str());
	fclose(f);*/
	return true;
}

void CmdFullLog(CCmds* classptr, const wstring wscToggle)
{
	//if (!(classptr->rights & RIGHT_SUPERADMIN)) { classptr->Print(L"ERR No permission\n"); return; }

	//if (ToLower(wscToggle) == L"on")
	//{
	//	FullLog = true;
	//}
	//else if (ToLower(wscToggle) == L"off")
	//{
	//	FullLog = false;
	//}
	//else if (!wscToggle.length())
	//{
	//	classptr->Print(L"ERR argument should be <on|off>\n");
	//	return;
	//}
	//else
	//{
	//	classptr->Print(L"ERR argument should be <on|off>\n");
	//	return;
	//}
	//classptr->Print(L"OK\n");
}

void CmdTest(CCmds* classptr)
{
	//if (!(classptr->rights & RIGHT_SUPERADMIN)) { classptr->Print(L"ERR No permission\n"); return; }
	//HkMsgU(L"test");
	//classptr->Print(L"OK\n");
}

void CmdATTest(CCmds* classptr)
{
	//if (!(classptr->rights & RIGHT_SUPERADMIN)) { classptr->Print(L"ERR No permission\n"); return; }
	//HkMsgU(L"atprocess");
	//classptr->Print(L"OK\n");
}

#define IS_CMD(a) !wscCmd.compare(L##a)

EXPORT bool ExecuteCommandString_Callback(CCmds* classptr, const wstring &wscCmd)
{
	returncode = NOFUNCTIONCALL;  // flhook needs to care about our return code

	//if (IS_CMD("prlog"))
	//{
	//	returncode = SKIPPLUGINS_NOFUNCTIONCALL; // do not let other plugins kick in since we now handle the command
	//	CmdFullLog(classptr, classptr->ArgStr(1));
	//	return true;
	//}


	//if (IS_CMD("actest"))
	//{
	//	returncode = SKIPPLUGINS_NOFUNCTIONCALL; // do not let other plugins kick in since we now handle the command
	//	CmdTest(classptr);
	//	return true;
	//}

	//if (IS_CMD("attest"))
	//{
	//	returncode = SKIPPLUGINS_NOFUNCTIONCALL; // do not let other plugins kick in since we now handle the command
	//	CmdATTest(classptr);
	//	return true;
	//}

	return false;
}

EXPORT void CmdHelp_Callback(CCmds* classptr)
{
	returncode = DEFAULT_RETURNCODE;

	//classptr->Print(L"prlog <on><off> <--- СѓСЃС‚Р°РЅР°РІР»РёРІР°РµС‚ Р¶СѓСЂРЅР°Р» РїСЂРѕС†РµСЃСЃРѕРІ РґР»СЏ С‚РµСЃС‚РёСЂРѕРІР°РЅРёСЏ\n");
	//classptr->Print(L"actest <--- РїСЂРѕРІРµСЂРєР° РїСЂРѕС†РµСЃСЃРѕРІ РёРіСЂРѕРєРѕРІ\n");
	//classptr->Print(L"attest <--- СЂРµРіРёСЃС‚СЂРёСЂСѓРµС‚ РІСЃРµ DLL, РїСЂРёРєСЂРµРїР»РµРЅРЅС‹Рµ Рє С„СЂРёР»Р°РЅСЃРµСЂСѓ\n");
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "anticheat plugin by kosacid";
	p_PI->sShortName = "anticheat";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CmdHelp_Callback, PLUGIN_CmdHelp_Callback, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ExecuteCommandString_Callback, PLUGIN_ExecuteCommandString_Callback, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Update, PLUGIN_HkIServerImpl_Update, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::CharacterSelect_AFTER, PLUGIN_HkIServerImpl_CharacterSelect_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::PlayerLaunch_AFTER, PLUGIN_HkIServerImpl_PlayerLaunch_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::SubmitChat_AFTER, PLUGIN_HkIServerImpl_SubmitChat_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	return p_PI;
}
