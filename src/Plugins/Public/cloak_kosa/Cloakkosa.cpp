/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 апреля 2026 10:59:03
 * Version: 1.0.24
 */

#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include <vector>

#define ADDR_FLCONFIG 0x25410
wstring HkGetWStringFromIDS(uint iIDS);
void HkLoadDLLConf(const char *szFLConfigFile);
char HkUnMarkObject(uint iClientID, uint iObject);
struct CLOAK_INFO
{
	uint		iCloakingTime;
	uint		iCloakWarmup;
	uint		iCloakCooldown;
	uint		iCloakSlot;
	bool		bCanCloak;
	bool		bCloaked;
	bool		bWantsCloak;
	bool		bIsCloaking;
	mstime		tmCloakTime;
	bool		bMustSendUncloak;
	vector<uint> vMarkedObjs;
	vector<uint> vAutoMarkedObjs;
	vector<uint> vDelayedSystemMarkedObjs;
};
void ClearCloakInfo(uint iClientID);
vector<HINSTANCE> vDLLs;

CLOAK_INFO CloakInfo[201];
list<INISECTIONVALUE> set_lstCloakDevices;
bool set_bSendCooldownMsg;
bool set_bPreventFireCloak;
bool set_bDropShield;
bool return_Cloak_off = false;
wstring set_wscCooldownMsg;
wstring set_wscDockedError;
wstring set_wscNoCloakDeviceError;
wstring set_wscCloakPrepareMsg;
wstring set_wscCloakPrepareTime;
wstring set_wscCooldownTimeRemaining;
wstring set_wscCloakTimeRemaining;
CLIENT_CLOAK_COSA_STRUCT communicationInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//EXPORT void UserCmd_Help(uint iClientID, const wstring &wscParam)
//{
//	PrintUserCmdText(iClientID, GetLocalized("MSG_0527"));
//	PrintUserCmdText(iClientID, GetLocalized("MSG_0528"));
//	PrintUserCmdText(iClientID, GetLocalized("MSG_0529"));
//	PrintUserCmdText(iClientID, GetLocalized("MSG_0530"));
//}

HK_ERROR HkCloak(uint iClientID)
{
	CloakInfo[iClientID].bIsCloaking = true;
	CloakInfo[iClientID].bWantsCloak = false;
	CloakInfo[iClientID].bCloaked = false;
	CloakInfo[iClientID].tmCloakTime = timeInMS();

	//фиксируем в глобальную переменную активацию режима невидимости
	communicationInfo.iClientID = iClientID;
	communicationInfo.isChargingCloak = false;
	communicationInfo.isCloaked = true;
	Plugin_Communication(CLIENT_CLOAK_INFO, &communicationInfo);

	XActivateEquip ActivateEq;
	ActivateEq.bActivate = true;
	ActivateEq.iSpaceID = ClientInfo[iClientID].iShip;
	ActivateEq.sID = CloakInfo[iClientID].iCloakSlot;
	Server.ActivateEquip(iClientID, ActivateEq);
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0531"));
	if (set_bDropShield)
		pub::SpaceObj::DrainShields(ClientInfo[iClientID].iShip);
	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID2 = HkGetClientIdFromPD(pPD);
		pub::Player::MarkObj(iClientID2, ClientInfo[iClientID].iShip, 0);
		HkUnMarkObject(iClientID2, ClientInfo[iClientID].iShip);
	}

	return HKE_OK;
}

HK_ERROR HkUnCloak(uint iClientID)
{
	CloakInfo[iClientID].bIsCloaking = false;
	CloakInfo[iClientID].bWantsCloak = false;
	CloakInfo[iClientID].bCloaked = false;
	CloakInfo[iClientID].tmCloakTime = timeInMS();

	XActivateEquip ActivateEq;
	ActivateEq.bActivate = false;
	ActivateEq.iSpaceID = ClientInfo[iClientID].iShip;
	ActivateEq.sID = CloakInfo[iClientID].iCloakSlot;
	Server.ActivateEquip(iClientID, ActivateEq);

	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0532"));

	//фиксируем в глобальную переменную деактивацию режима невидимости
	communicationInfo.iClientID = iClientID;
	communicationInfo.isChargingCloak = false;
	communicationInfo.isCloaked = false;
	Plugin_Communication(CLIENT_CLOAK_INFO, &communicationInfo);

	return HKE_OK;
}

void HkInitCloakSettings(uint iClientID)
{
	CloakInfo[iClientID].bCanCloak = false;

	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

	list<CARGO_INFO> lstEquipment;
	int iRemaining;
	HkEnumCargo(wscCharname, lstEquipment, iRemaining);
	foreach(lstEquipment, CARGO_INFO, it)
	{
		if (it->bMounted)
		{
			// проверить установленное маскировочное устройство
			foreach(set_lstCloakDevices, INISECTIONVALUE, it2)
			{
				uint iArchIDCloak = CreateID(it2->scKey.c_str());
				if (it->iArchID == iArchIDCloak)
				{
					CloakInfo[iClientID].iCloakingTime = ToInt(GetParam(stows(it2->scValue), ',', 0));
					CloakInfo[iClientID].iCloakCooldown = ToInt(GetParam(stows(it2->scValue), ',', 1));
					CloakInfo[iClientID].iCloakWarmup = ToInt(GetParam(stows(it2->scValue), ',', 2));
					CloakInfo[iClientID].bCanCloak = true;
					CloakInfo[iClientID].bIsCloaking = false;
					CloakInfo[iClientID].bWantsCloak = false;
					CloakInfo[iClientID].bCloaked = false;
					CloakInfo[iClientID].iCloakSlot = it->iID;
					CloakInfo[iClientID].tmCloakTime = timeInMS();
					break;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HkTimerCloakHandler()
{
	// для всех игроков
	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);

		if (CloakInfo[iClientID].bCanCloak)
		{
			// отправлять состояние маскировки для незамаскированных игроков, способных к маскировке (только для них в космосе)
			// это код, исправляющий ошибку, из-за которой игроки не всегда видели незамаскированных игроков.

			uint iShip = 0;
			pub::Player::GetShip(iClientID, iShip);
			if (iShip)
			{
				if (!CloakInfo[iClientID].bCloaked && !CloakInfo[iClientID].bIsCloaking)
				{
					XActivateEquip ActivateEq;
					ActivateEq.bActivate = false;
					ActivateEq.iSpaceID = iShip;
					ActivateEq.sID = CloakInfo[iClientID].iCloakSlot;
					Server.ActivateEquip(iClientID, ActivateEq);
					// запрет выпуска дрона
					communicationInfo.iClientID = iClientID;
					communicationInfo.isChargingCloak = false;
					communicationInfo.isCloaked = false;
					Plugin_Communication(CLIENT_CLOAK_INFO, &communicationInfo);
				}
				else
				{
					communicationInfo.iClientID = iClientID;
					communicationInfo.isChargingCloak = false;
					communicationInfo.isCloaked = true;
					Plugin_Communication(CLIENT_CLOAK_INFO, &communicationInfo);
				}

				// проверить тайминги плаща
				mstime tmTimeNow = timeInMS();

				if (CloakInfo[iClientID].bWantsCloak && (tmTimeNow - CloakInfo[iClientID].tmCloakTime) > CloakInfo[iClientID].iCloakWarmup)
				{
					HkCloak(iClientID);
				}

				if (CloakInfo[iClientID].bIsCloaking && (tmTimeNow - CloakInfo[iClientID].tmCloakTime) > CloakInfo[iClientID].iCloakingTime)
				{
					CloakInfo[iClientID].bIsCloaking = false;
					CloakInfo[iClientID].bCloaked = true;
					CloakInfo[iClientID].tmCloakTime = tmTimeNow;
				}

				mstime tmCloakRemaining = tmTimeNow - CloakInfo[iClientID].tmCloakTime;

				if (CloakInfo[iClientID].bCloaked && CloakInfo[iClientID].iCloakingTime && tmCloakRemaining > 0)
				{
					HkUnCloak(iClientID);
				}

				if (set_bSendCooldownMsg 
					&& !CloakInfo[iClientID].bCloaked 
					&& tmCloakRemaining < (CloakInfo[iClientID].iCloakCooldown + 500) 
					&& tmCloakRemaining >= CloakInfo[iClientID].iCloakCooldown)
				{
					PrintUserCmdText(iClientID, set_wscCooldownMsg);
					return_Cloak_off = true;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_Cloak(uint iClientID, const wstring &wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, set_wscDockedError);
		return;
	}

	if (CloakInfo[iClientID].bCanCloak)
	{
		mstime tmTimeNow = timeInMS();
		if (!CloakInfo[iClientID].bCloaked && !CloakInfo[iClientID].bIsCloaking)
		{
			if (CloakInfo[iClientID].bWantsCloak)
			{
				PrintUserCmdText(iClientID, set_wscCloakPrepareTime, (int)((CloakInfo[iClientID].iCloakWarmup - (tmTimeNow - CloakInfo[iClientID].tmCloakTime)) / 1000.0f + 0.5f));
			}
			else if ((tmTimeNow - CloakInfo[iClientID].tmCloakTime) > CloakInfo[iClientID].iCloakCooldown && return_Cloak_off)
			{
				CloakInfo[iClientID].tmCloakTime = tmTimeNow;
				CloakInfo[iClientID].bWantsCloak = true;
				PrintUserCmdText(iClientID, set_wscCloakPrepareMsg);
				return_Cloak_off = false;
			}
			else
			{
				PrintUserCmdText(iClientID, set_wscCooldownTimeRemaining, (int)((CloakInfo[iClientID].iCloakCooldown - (tmTimeNow - CloakInfo[iClientID].tmCloakTime)) / 1000.0f + 0.5f));
			}
		}
		else
			PrintUserCmdText(iClientID, set_wscCloakTimeRemaining, CloakInfo[iClientID].iCloakingTime ? (int)((CloakInfo[iClientID].iCloakingTime - (tmTimeNow - CloakInfo[iClientID].tmCloakTime)) / 1000.0f + 0.5f) : 0);
	}
	else
		PrintUserCmdText(iClientID, set_wscNoCloakDeviceError);
}

void UserCmd_UnCloak(uint iClientID, const wstring &wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip) {
		PrintUserCmdText(iClientID, set_wscDockedError);
		return;
	}

	if (!return_Cloak_off)
	{
		return;
	}

	if (CloakInfo[iClientID].bCanCloak) {
		if (!HKHKSUCCESS(HkUnCloak(iClientID)))
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0533"));
	}
	else
		PrintUserCmdText(iClientID, set_wscNoCloakDeviceError);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGIN_RETURNCODE returncode;
EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;
	char *szFLConfig = (char*)((char*)GetModuleHandle(0) + ADDR_FLCONFIG);
	HkLoadDLLConf(szFLConfig);

	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\cloak.ini";

	set_lstCloakDevices.clear();
	IniGetSection(scPluginCfgFile, "CloakDevices", set_lstCloakDevices);
	set_bSendCooldownMsg = IniGetB(scPluginCfgFile, "General", "CooldownMsg", false);
	set_bPreventFireCloak = IniGetB(scPluginCfgFile, "General", "PreventFireCloak", false);
	set_bDropShield = IniGetB(scPluginCfgFile, "General", "DropShieldsCloak", false);
	uint iIDS;
	iIDS = IniGetI(scPluginCfgFile, "General", "CooldownMsgIDS", 0);
	if (iIDS)
		set_wscCooldownMsg = HkGetWStringFromIDS(iIDS);
	else
		set_wscCooldownMsg = L"Устройство невидимости готово к использованию!";
	iIDS = IniGetI(scPluginCfgFile, "General", "DockedErrorIDS", 0);
	if (iIDS)
		set_wscDockedError = HkGetWStringFromIDS(iIDS);
	else
		set_wscDockedError = L"Ошибка: вы состыкованы.";
	iIDS = IniGetI(scPluginCfgFile, "General", "NoCloakDeviceErrorIDS", 0);
	if (iIDS)
		set_wscNoCloakDeviceError = HkGetWStringFromIDS(iIDS);
	else
		set_wscNoCloakDeviceError = L"Ошибка: на вашем корабле нет маскирующего устройства.";
	iIDS = IniGetI(scPluginCfgFile, "General", "CloakPrepareMsgIDS", 0);
	if (iIDS)
		set_wscCloakPrepareMsg = HkGetWStringFromIDS(iIDS);
	else
		set_wscCloakPrepareMsg = L"Подготовка к маскировке...";
	iIDS = IniGetI(scPluginCfgFile, "General", "CloakPrepareTimeIDS", 0);
	if (iIDS)
		set_wscCloakPrepareTime = HkGetWStringFromIDS(iIDS);
	else
		set_wscCloakPrepareTime = L"Время до включения: %i сек.";
	iIDS = IniGetI(scPluginCfgFile, "General", "CooldownTimeRemainingIDS", 0);
	if (iIDS)
		set_wscCooldownTimeRemaining = HkGetWStringFromIDS(iIDS);
	else
		set_wscCooldownTimeRemaining = L"Устройство невидимости ещё не готово! Оставшееся время: %i сек.";
	iIDS = IniGetI(scPluginCfgFile, "General", "CloakTimeRemainingIDS", 0);
	if (iIDS)
		set_wscCloakTimeRemaining = HkGetWStringFromIDS(iIDS);
	else
		set_wscCloakTimeRemaining = L"Вы уже замаскированы. Оставшееся время: %i сек.";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		LoadSettings();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void(*_UserCmdProc)(uint, const wstring &);

struct USERCMD
{
	wchar_t *wszCmd;
	_UserCmdProc proc;
};

USERCMD UserCmds[] =
{
	{ L"/cloak",				UserCmd_Cloak},
	{ L"/c",					UserCmd_Cloak},
	{ L"/uncloak",				UserCmd_UnCloak},
	{ L"/uc",					UserCmd_UnCloak},
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

void ClearCloakInfo(uint iClientID)
{
	CloakInfo[iClientID].bMustSendUncloak = false;
	CloakInfo[iClientID].iCloakingTime = 0;
	CloakInfo[iClientID].iCloakWarmup = 0;
	CloakInfo[iClientID].iCloakCooldown = 0;
	CloakInfo[iClientID].iCloakSlot = 0;
	CloakInfo[iClientID].bCanCloak = false;
	CloakInfo[iClientID].bCloaked = false;
	CloakInfo[iClientID].bIsCloaking = false;
	CloakInfo[iClientID].bWantsCloak = false;
	CloakInfo[iClientID].tmCloakTime = 0;
}

namespace HkIServerImpl
{
	// add timers here
	typedef void(*_TimerFunc)();

	struct TIMER
	{
		_TimerFunc	proc;
		mstime		tmIntervallMS;
		mstime		tmLastCall;
	};

	TIMER Timers[] =
	{
		{HkTimerCloakHandler,			500,				0}
	};

	EXPORT int __stdcall Update(void)
	{
		returncode = DEFAULT_RETURNCODE;

		static bool bFirstTime = true;
		if (bFirstTime)
		{
			bFirstTime = false;
			// проверить наличие залогиненных игроков и сбросить данные их подключения
			struct PlayerData *pPD = 0;
			while (pPD = Players.traverse_active(pPD))
				ClearCloakInfo(HkGetClientIdFromPD(pPD));
		}

		// таймеры вызова
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

	EXPORT void __stdcall PlayerLaunch(unsigned int iShip, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;

		HkInitCloakSettings(iClientID);
		if (CloakInfo[iClientID].bCanCloak)
		{
			CloakInfo[iClientID].bMustSendUncloak = true;
		}
	}

	EXPORT void __stdcall SPObjUpdate(struct SSPObjUpdateInfo const &ui, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;

		if (CloakInfo[iClientID].bCanCloak && CloakInfo[iClientID].bMustSendUncloak && !CloakInfo[iClientID].bIsCloaking) {
			HkUnCloak(iClientID);
			CloakInfo[iClientID].bMustSendUncloak = false;
		}
	}

	EXPORT void __stdcall JumpInComplete(unsigned int iSystemID, unsigned int iShip)
	{
		returncode = DEFAULT_RETURNCODE;

		int iRep;
		pub::SpaceObj::GetRep(iShip, iRep);
		uint iClientID = Reputation::Vibe::GetClientID(iRep);

		if (iClientID && CloakInfo[iClientID].bCanCloak && (CloakInfo[iClientID].bCloaked || CloakInfo[iClientID].bIsCloaking))
			CloakInfo[iClientID].bMustSendUncloak = true;
	}

	EXPORT void __stdcall FireWeapon(unsigned int iClientID, struct XFireWeaponInfo const &wpn)
	{
		if (set_bPreventFireCloak && CloakInfo[iClientID].bCanCloak && (CloakInfo[iClientID].bIsCloaking || CloakInfo[iClientID].bCloaked))
		{
			returncode = NOFUNCTIONCALL; // Заботится о ракетах/минах
		}
		else
		{
			returncode = DEFAULT_RETURNCODE;
		}
	}

	EXPORT void __stdcall SPMunitionCollision(struct SSPMunitionCollisionInfo const & ci, unsigned int iClientID)
	{
		if (set_bPreventFireCloak && CloakInfo[iClientID].bCanCloak && (CloakInfo[iClientID].bIsCloaking || CloakInfo[iClientID].bCloaked))
		{
			returncode = NOFUNCTIONCALL; // Бережно относится к оружию
		}
		else
		{
			returncode = DEFAULT_RETURNCODE;
		}
	}
}

wstring HkGetWStringFromIDS(uint iIDS) // Работает только для имен
{
	if (!iIDS)
		return L"";

	uint iDLL = iIDS / 0x10000;
	iIDS -= iDLL * 0x10000;

	wchar_t wszBuf[512];
	if (LoadStringW(vDLLs[iDLL], iIDS, wszBuf, 512))
		return wszBuf;
	return L"";
}

void HkLoadDLLConf(const char *szFLConfigFile)
{
	for (uint i = 0; i < vDLLs.size(); i++)
	{
		FreeLibrary(vDLLs[i]);
	}
	vDLLs.clear();
	HINSTANCE hDLL = LoadLibraryEx((char*)((char*)GetModuleHandle(0) + 0x256C4), NULL, LOAD_LIBRARY_AS_DATAFILE); //typically resources.dll
	if (hDLL)
		vDLLs.push_back(hDLL);
	INI_Reader ini;
	if (ini.open(szFLConfigFile, false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("Resources"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("DLL"))
					{
						hDLL = LoadLibraryEx(ini.get_value_string(0), NULL, LOAD_LIBRARY_AS_DATAFILE);
						if (hDLL)
							vDLLs.push_back(hDLL);
					}
				}
			}
		}
		ini.close();
	}
}

char HkUnMarkObject(uint iClientID, uint iObject)
{
	if (!iObject)
		return 1;

	for (uint i = 0; i < CloakInfo[iClientID].vMarkedObjs.size(); i++)
	{
		if (CloakInfo[iClientID].vMarkedObjs[i] == iObject)
		{
			if (i != CloakInfo[iClientID].vMarkedObjs.size() - 1)
			{
				CloakInfo[iClientID].vMarkedObjs[i] = CloakInfo[iClientID].vMarkedObjs[CloakInfo[iClientID].vMarkedObjs.size() - 1];
			}
			CloakInfo[iClientID].vMarkedObjs.pop_back();
			pub::Player::MarkObj(iClientID, iObject, 0);
			pub::Audio::PlaySoundEffect(iClientID, 2939827141); //CreateID("ui_select_remove")
			return 0;
		}
	}

	for (uint j = 0; j < CloakInfo[iClientID].vAutoMarkedObjs.size(); j++)
	{
		if (CloakInfo[iClientID].vAutoMarkedObjs[j] == iObject)
		{
			if (j != CloakInfo[iClientID].vAutoMarkedObjs.size() - 1)
			{
				CloakInfo[iClientID].vAutoMarkedObjs[j] = CloakInfo[iClientID].vAutoMarkedObjs[CloakInfo[iClientID].vAutoMarkedObjs.size() - 1];
			}
			CloakInfo[iClientID].vAutoMarkedObjs.pop_back();
			pub::Player::MarkObj(iClientID, iObject, 0);
			pub::Audio::PlaySoundEffect(iClientID, 2939827141); //CreateID("ui_select_remove")
			return 0;
		}
	}

	for (uint k = 0; k < CloakInfo[iClientID].vDelayedSystemMarkedObjs.size(); k++)
	{
		if (CloakInfo[iClientID].vDelayedSystemMarkedObjs[k] == iObject)
		{
			if (k != CloakInfo[iClientID].vDelayedSystemMarkedObjs.size() - 1)
			{
				CloakInfo[iClientID].vDelayedSystemMarkedObjs[k] = CloakInfo[iClientID].vDelayedSystemMarkedObjs[CloakInfo[iClientID].vDelayedSystemMarkedObjs.size() - 1];
			}
			CloakInfo[iClientID].vDelayedSystemMarkedObjs.pop_back();
			pub::Audio::PlaySoundEffect(iClientID, 2939827141); //CreateID("ui_select_remove")
			return 0;
		}
	}
	return 2;
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "Cloak plugin by M0tah|Based on code by w0dk4";
	p_PI->sShortName = "Cloak";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	//	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Help, PLUGIN_UserCmd_Help,0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Update, PLUGIN_HkIServerImpl_Update, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::PlayerLaunch, PLUGIN_HkIServerImpl_PlayerLaunch, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::SPObjUpdate, PLUGIN_HkIServerImpl_SPObjUpdate, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::JumpInComplete, PLUGIN_HkIServerImpl_JumpInComplete, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::FireWeapon, PLUGIN_HkIServerImpl_FireWeapon, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::SPMunitionCollision, PLUGIN_HkIServerImpl_SPMunitionCollision, 0));
	return p_PI;
}