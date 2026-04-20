/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 16:23:13
 * Version: 1.0.468
 */

#pragma warning(disable: 4146 4996)
#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

#define PRINT_ERROR_NORETURN() { for(uint i = 0; (i < sizeof(wscError)/sizeof(wstring)); i++) PrintUserCmdText(iClientID, wscError[i]); }
#define PRINT_DISABLED() PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0404"));

struct BOUNTY_HUNT
{
	static int sBhCounter;
	BOUNTY_HUNT() {
		iCounter = sBhCounter++;
	};
	int		iCounter;
	uint	uiTargetID;
	uint	uiInitiatorID;
	wstring wscTarget;
	wstring wscInitiator;
	uint	uiCredits;
	mstime  msEnd;
	bool operator==(struct BOUNTY_HUNT const & sT);
	bool operator==(const BOUNTY_HUNT& other) const {
		return iCounter == other.iCounter &&
			uiTargetID == other.uiTargetID &&
			uiInitiatorID == other.uiInitiatorID &&
			wscTarget == other.wscTarget &&
			wscInitiator == other.wscInitiator &&
			uiCredits == other.uiCredits &&
			msEnd == other.msEnd;
	}
};

void PrintUniverseText(wstring wscText, ...);
void BhKillCheck(uint uiClientID, uint uiKillerID);
void CancelBh(uint uiClientID);
void BhTimeOutCheck();

PLUGIN_RETURNCODE returncode;
bool set_bBhEnabled;
uint set_iBhAuto;
int  set_iLowLevelProtect;
bool set_bEnableList = true;
list<BOUNTY_HUNT> lstBountyHunt;

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

EXPORT void UserCmd_Help(uint iClientID, const wstring &wscParam)
{
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0405"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0406"));
}

EXPORT void LoadSettings()
{
	string set_scCfgGeneralFile;
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	set_scCfgGeneralFile = string(szCurDir) + "\\flhook_plugins\\bountyhunt.ini";
	set_iBhAuto = IniGetI(set_scCfgGeneralFile, "General", "AutoBountyHunt", 0);
	set_bBhEnabled = IniGetB(set_scCfgGeneralFile, "General", "EnableBountyHunt", false);
	set_iLowLevelProtect = IniGetI(set_scCfgGeneralFile, "General", "lvlprotect", 0);
	set_bEnableList = IniGetB(set_scCfgGeneralFile, "General", "EnableList", true);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		LoadSettings();
	return true;
}

EXPORT void SendDeathMsg(const wstring &wscMsg, uint iSystemID, uint iClientIDVictim, uint iClientIDKiller)
{
	returncode = DEFAULT_RETURNCODE;
	if (set_bBhEnabled)
	{
		BhKillCheck(iClientIDVictim, iClientIDKiller);
	}
}

void UserCmd_BountyHunt(uint iClientID, const wstring &wscParam)
{
	if (!set_bBhEnabled)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_1532"),
		GetLocalized(iClientID, "MSG_1533"),
		GetLocalized(iClientID, "MSG_1534"),
	};

	mstime nowtime = timeInMS();
	wstring wscTarget = GetParam(wscParam, ' ', 0);
	wstring wscCredits = GetParam(wscParam, ' ', 1);
	wstring wscTime = GetParam(wscParam, ' ', 2);
	if (!wscTarget.length() || !wscCredits.length())
	{
		PRINT_ERROR_NORETURN();
		if (set_bEnableList && lstBountyHunt.begin() != lstBountyHunt.end()) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0407"));
			foreach(lstBountyHunt, BOUNTY_HUNT, it)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0408"), it->wscTarget.c_str(), it->uiCredits, (uint)((it->msEnd - nowtime) / 60000));
			}
		}
		return;
	}

	uint uiTargetID = 0, uiPrize = 0, uiTime = 30;
	uiPrize = wcstol(wscCredits.c_str(), NULL, 10);
	uiTime = wcstol(wscTime.c_str(), NULL, 10);
	uiTargetID = HkGetClientIdFromCharname(wscTarget);

	int iRankTarget;
	pub::Player::GetRank(uiTargetID, iRankTarget);

	if (uiTargetID == -1 || HkIsInCharSelectMenu(uiTargetID))
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0409"), wscTarget.c_str());
		return;
	}

	if (iRankTarget < set_iLowLevelProtect)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0410"));//Lowlevelchars may not be hunted.
		return;
	}
	if (uiPrize > 10000000 || uiPrize == 0)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0411"), uiPrize);//prize %u not allowed (min:1 max:100000000)
		return;
	}
	if (uiTime < 1 || uiTime > 240)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0412"));
		uiTime = 30;
	}
	wstring wscInitiatior = L"Player";
	if (iClientID)
	{
		uint uiClientCash;
		pub::Player::InspectCash(iClientID, (int &)uiClientCash);
		if (uiClientCash < uiPrize)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0413"));//you do not possess enough credits
			return;
		}
		pub::Player::AdjustCash(iClientID, -uiPrize);
		wscInitiatior = (wchar_t*)Players.GetActiveCharacterName(iClientID);
	}

	BOUNTY_HUNT bh;
	bh.uiInitiatorID = iClientID;
	bh.msEnd = nowtime + (mstime)(uiTime * 60000);
	bh.wscInitiator = wscInitiatior;
	bh.uiCredits = uiPrize;
	bh.wscTarget = wscTarget;
	bh.uiTargetID = uiTargetID;
	lstBountyHunt.push_back(bh);

	PrintUniverseText(GetLocalized(iClientID, "MSG_1535"), bh.wscInitiator.c_str(), bh.uiCredits, bh.wscTarget.c_str(), uiTime);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_BountyHuntId(uint iClientID, const wstring &wscParam)
{
	if (!set_bBhEnabled)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_1532"),
		GetLocalized(iClientID, "MSG_1533"),
		GetLocalized(iClientID, "MSG_1534"),
	};

	mstime nowtime = timeInMS();
	wstring wscTarget = GetParam(wscParam, ' ', 0);
	wstring wscCredits = GetParam(wscParam, ' ', 1);
	wstring wscTime = GetParam(wscParam, ' ', 2);
	if (!wscTarget.length() || !wscCredits.length())
	{
		PRINT_ERROR_NORETURN();
		if (set_bEnableList && lstBountyHunt.begin() != lstBountyHunt.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0407"));
			foreach(lstBountyHunt, BOUNTY_HUNT, it)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0415"), it->wscTarget.c_str(), it->uiCredits, (uint)((it->msEnd - nowtime) / 60000));
			}
		}
		return;
	}

	uint iClientIDTarget = ToInt(wscTarget);
	if (!HkIsValidClientID(iClientIDTarget) || HkIsInCharSelectMenu(iClientIDTarget))
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0416"));
		return;
	}

	wstring wscParamNew;
	wstring wscCharName;
	wscCharName = ((wchar_t*)Players.GetActiveCharacterName(iClientIDTarget));
	wscParamNew = wstring(wscCharName + L" " + wscCredits + L" " + wscTime);
	UserCmd_BountyHunt(iClientID, wscParamNew);
}

int BOUNTY_HUNT::sBhCounter = 0;
bool BOUNTY_HUNT::operator ==(struct BOUNTY_HUNT const & sT)
{
	if (iCounter == sT.iCounter)
	{
		return true;
	}
	return false;
}

void BhTimeOutCheck()
{
	mstime nowtime = timeInMS();
	foreach(lstBountyHunt, BOUNTY_HUNT, it)
	{
		if (it->msEnd < nowtime)
		{
			HkAddCash(it->wscTarget, it->uiCredits);
			PrintUniverseText(GetLocalized(it->uiInitiatorID, "MSG_1536"), it->wscTarget.c_str(), it->uiCredits);
			lstBountyHunt.remove(*it);
			BhTimeOutCheck();
			break;
		}
	}
}

void BhKillCheck(uint uiClientID, uint uiKillerID)
{
	mstime nowtime = timeInMS();
	foreach(lstBountyHunt, BOUNTY_HUNT, it)
	{
		if (it->uiTargetID == uiClientID)
		{
			if (uiKillerID == 0 || uiClientID == uiKillerID)
			{
				PrintUniverseText(GetLocalized(uiClientID, "MSG_1537"), it->wscTarget.c_str());
			}
			else
			{
				wstring wscWinnerCharname;
				wscWinnerCharname = (wchar_t*)Players.GetActiveCharacterName(uiKillerID);
				if (wscWinnerCharname.size() > 0)
				{
					HkAddCash(wscWinnerCharname, it->uiCredits);
					PrintUniverseText(GetLocalized(uiKillerID, "MSG_1538"), wscWinnerCharname.c_str(), it->wscTarget.c_str(), it->uiCredits);
				}
				else
				{
					HkAddCash(it->wscInitiator, it->uiCredits);
				}

				lstBountyHunt.remove(*it);
				BhKillCheck(uiClientID, uiKillerID);
				break;
			}
		}
	}
}

void CancelBh(uint uiClientID)
{
	mstime nowtime = timeInMS();
	foreach(lstBountyHunt, BOUNTY_HUNT, it) {
		if (it->uiTargetID == uiClientID)
		{
			if (it->uiInitiatorID != 0)
			{
				HkAddCash(it->wscInitiator, it->uiCredits);
			}
			PrintUniverseText(GetLocalized(uiClientID, "MSG_1539"), it->wscInitiator.c_str());
			lstBountyHunt.remove(*it);
			CancelBh(uiClientID);
			break;
		}
	}
}

void PrintUniverseText(wstring wscText, ...)
{
	wchar_t wszBuf[1024 * 8] = L"";
	va_list marker;
	va_start(marker, wscText);
	_vsnwprintf(wszBuf, sizeof(wszBuf) - 1, wscText.c_str(), marker);
	wstring wscXML = L"<TRA data=\"" + set_wscAdminCmdStyle + L"\" mask=\"-1\"/><TEXT>" + XMLText(wszBuf) + L"</TEXT>";
	HkFMsgU(wscXML);
}

namespace HkIServerImpl
{
	typedef void(*_TimerFunc)();
	struct TIMER
	{
		_TimerFunc	proc;
		mstime		tmIntervallMS;
		mstime		tmLastCall;
	};

	TIMER Timers[] =
	{
		{BhTimeOutCheck,		2017,		0},
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
	{ L"/bountyhunt",		UserCmd_BountyHunt},
	{ L"/bountyhuntid",     UserCmd_BountyHuntId},
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

			returncode = SKIPPLUGINS_NOFUNCTIONCALL;
			return true;
		}
	}

	returncode = DEFAULT_RETURNCODE;
	return false;
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "BountyHunt plugin";
	p_PI->sShortName = "bountyhunt";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Update, PLUGIN_HkIServerImpl_Update, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	//	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Help, PLUGIN_UserCmd_Help,0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&SendDeathMsg, PLUGIN_SendDeathMsg, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	return p_PI;
}