/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 06 апреля 2026 12:49:28
 * Version: 1.0.21
 */

// DroneBays for FLHookPlugin
// April 2018 by Conrad
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
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

static int set_iPluginDebug = 0;

/// A return code to indicate to FLHook if we want the hook processing to continue.
PLUGIN_RETURNCODE returncode;

void LoadSettings();

map<uint, DroneBuildTimerWrapper> buildTimerMap;
map<uint, DroneDespawnWrapper> droneDespawnMap;
map<uint, ClientDroneInfo> clientDroneInfo;

map<uint, BayArch> availableDroneBays;
map<string, DroneArch> availableDroneArch;
map<uint, ChatDebounceStruct> droneAlertDebounceMap;

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Loading Settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	string File_FLHook = R"(..\exe\flhook_plugins\dronebays.cfg)";
	int loadedBays = 0;
	int loadedDrones = 0;

	INI_Reader ini;
	if (ini.open(File_FLHook.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("DroneArch"))
			{
				DroneArch droneArch;
				string aliasName;
				while (ini.read_value())
				{
					if (ini.is_value("aliasName"))
					{
						aliasName = ini.get_value_string(0);
					}
					else if (ini.is_value("archetype"))
					{
						droneArch.archetype = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("loadout"))
					{
						droneArch.loadout = CreateID(ini.get_value_string(0));
					}
				}
				if (aliasName.empty())
				{
					ConPrint(L"DRONEBAY: Configuration ERROR: Found DroneArch section missing aliasName.\n");
				}
				else
				{
					availableDroneArch[aliasName] = droneArch;
					loadedDrones++;
				}
			}
		}
		ini.close();
	}
	if (ini.open(File_FLHook.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("BayArch"))
			{
				BayArch bayArch;
				uint bayEquipId = 0;

				while (ini.read_value())
				{
					if (ini.is_value("equipmentId"))
					{
						bayEquipId = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("launchtime"))
					{
						bayArch.iDroneBuildTime = ini.get_value_int(0);
					}
					else if (ini.is_value("availabledrone"))
					{
						string droneArchName = ini.get_value_string(0);
						if (availableDroneArch.count(droneArchName) == 0)
						{
							const wchar_t* wszDroneArchName = stows(droneArchName).c_str();
							ConPrint(L"DRONEBAY: Ignoring BayArch availabledrone = %s line as there is no DroneArch section with aliasName = %s!\n", wszDroneArchName, wszDroneArchName);
						}
						else
						{
							bayArch.availableDrones.emplace_back(droneArchName);
						}
					}
					else if (ini.is_value("operationrange"))
					{
						bayArch.droneRange = ini.get_value_int(0);
					}
					else if (ini.is_value("validtarget"))
					{
						bayArch.validShipclassTargets.push_back(ini.get_value_int(0));
					}
				}
				if (bayEquipId == 0)
				{
					ConPrint(L"DRONEBAY: Configuration ERROR: Found BayArch without valid equipmentId.\n");
				}
				else
				{
					availableDroneBays[bayEquipId] = bayArch;
					loadedBays++;
				}
			}
		}
		ini.close();
	}

	ConPrint(L"DRONEBAY: %i bayarches loaded.\n", loadedBays);
	ConPrint(L"DRONEBAY: %i dronearches loaded.\n", loadedDrones);
}

/** Очистка при отключении клиента */
void ClearClientInfo(uint iClientID)
{
	// Если у пользователя существует дрон, уничтожьте его.
	if (clientDroneInfo[iClientID].deployedInfo.deployedDroneObj != 0)
	{
		pub::SpaceObj::Destroy(clientDroneInfo[iClientID].deployedInfo.deployedDroneObj, DestroyType::FUSE);
	}

	// Сотрите все записи структуры
	buildTimerMap.erase(iClientID);
	droneDespawnMap.erase(iClientID);
	clientDroneInfo.erase(iClientID);
	droneAlertDebounceMap.erase(iClientID);
}

void __stdcall ShipDestroyed(DamageList *_dmg, DWORD *ecx, uint iKill)
{
	returncode = DEFAULT_RETURNCODE;
	if (iKill)
	{
		CShip *cship = reinterpret_cast<CShip*>(ecx[4]);

		// Проверьте, уничтожается ли это дрон или авианосец
		for (auto drone = clientDroneInfo.begin(); drone != clientDroneInfo.end(); ++drone)
		{
			if (cship->get_id() == drone->second.deployedInfo.deployedDroneObj)
			{
				// Если да, очистите карту операторов связи и предупредите их.
				clientDroneInfo.erase(drone->first);

				PrintUserCmdText(drone->first, GetLocalized(drone->first, "MSG_0572"));
			}

			// Если носитель уничтожается, уничтожьте и дрон.
			else if (cship->get_id() == drone->second.carrierShipobj)
			{
				if (!drone->second.deployedInfo.deployedDroneObj != 0)
				{
					pub::SpaceObj::Destroy(drone->second.deployedInfo.deployedDroneObj, DestroyType::FUSE);
					clientDroneInfo.erase(drone->first);

					// Удалите все дроны в очереди на развертывание.
					buildTimerMap.erase(drone->first);

					PrintUserCmdText(drone->first, GetLocalized(drone->first, "MSG_0572"));

				}
			}
		}
	}
}

void __stdcall SystemSwitchOutComplete(unsigned int iShip, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// Если авианосец меняет системы, уничтожьте все старые дроны.
	if (clientDroneInfo[iClientID].deployedInfo.deployedDroneObj != 0)
	{
		pub::SpaceObj::Destroy(clientDroneInfo[iClientID].deployedInfo.deployedDroneObj, DestroyType::FUSE);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0574"));

		clientDroneInfo.erase(iClientID);
	}
}

void __stdcall BaseEnter_AFTER(unsigned int iBaseID, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// Если авианосец пристыкуется, уничтожьте всех дронов.
	if (clientDroneInfo[iClientID].deployedInfo.deployedDroneObj != 0)
	{
		pub::SpaceObj::Destroy(clientDroneInfo[iClientID].deployedInfo.deployedDroneObj, DestroyType::FUSE);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0575"));

		clientDroneInfo.erase(iClientID);
	}

	// Удалите все дроны в очереди на развертывание.
	buildTimerMap.erase(iClientID);
}

void __stdcall SetTarget_AFTER(uint iClientID, const XSetTarget& target)
{
	returncode = DEFAULT_RETURNCODE;

	// Если в течение нескольких секунд нам уже будет отправлено сообщение об этой операции таргетинга, проигнорируйте его.
	if (droneAlertDebounceMap[iClientID].debounceToggle)
	{
		return;
	}

	// Проверьте, является ли цель одним из наших дронов
	for (const auto& drone : clientDroneInfo)
	{
		if (drone.second.deployedInfo.deployedDroneObj == target.iSpaceID)
		{
			// Была ли последняя тревога по этому дрону? Если да, игнорируй это
			if (drone.second.deployedInfo.deployedDroneObj == droneAlertDebounceMap[iClientID].lastDroneObjSelected)
				return;

			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0576"), reinterpret_cast<const wchar_t*>(Players.GetActiveCharacterName(drone.first)));
			droneAlertDebounceMap[iClientID].debounceToggle = true;
			droneAlertDebounceMap[iClientID].lastDroneObjSelected = drone.second.deployedInfo.deployedDroneObj;
			return;
		}
	}
}

void HkTimerCheckKick()
{
	returncode = DEFAULT_RETURNCODE;

	// Обработать любые запуски дронов в очереди, если таковые имеются.
	if (!buildTimerMap.empty())
		Timers::processDroneBuildRequests(buildTimerMap);

	// Обработать любой запрос на стыковку в очереди, если таковой существует.
	if (!droneDespawnMap.empty())
		Timers::processDroneDockRequests(droneDespawnMap);

	// Каждые 2 секунды очищайте карту устранения дребезга, позволяя пользователю снова видеть предупреждение о дроне.
	static int timerVal = 0;
	if (timerVal % 2 == 0)
	{
		for (auto& userDebounce : droneAlertDebounceMap)
		{
			userDebounce.second.debounceToggle = false;
		}
		timerVal = 0;
	}
	timerVal++;
}

void Plugin_Communication_CallBack(PLUGIN_MESSAGE msg, void* data)
{
	returncode = DEFAULT_RETURNCODE;

	if (msg == CLIENT_CLOAK_INFO)
	{
		CLIENT_CLOAK_COSA_STRUCT* info = reinterpret_cast<CLIENT_CLOAK_COSA_STRUCT*>(data);
		UserCommands::IsCloakPlugin(false);

		// Проверьте, актуальна ли предоставленная нам информация. (Мы заботимся только о пользователях в несущей структуре.)
		if (clientDroneInfo.find(info->iClientID) != clientDroneInfo.end())
		{
			if (info->isChargingCloak || info->isCloaked)
			{
				UserCommands::IsCloakPlugin(true);

				// Если клиент скрыт или маскируется, не позволяйте ему использовать дрон.
				if (buildTimerMap.find(info->iClientID) != buildTimerMap.end())
				{
					PrintUserCmdText(info->iClientID, GetLocalized(info->iClientID, "MSG_0577"));
					clientDroneInfo[info->iClientID].buildState = STATE_DRONE_OFF;
					buildTimerMap.erase(info->iClientID);
					return;
				}
			}
		}
	}
	return;
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
	{ L"/dronedeploy", UserCommands::UserCmd_Deploy, L"Usage: /deploydrone [DroneType]" },
	{ L"/dd", UserCommands::UserCmd_Deploy , L"Usage: /dd [DroneType]" },
	{ L"/dronetarget", UserCommands::UserCmd_AttackTarget, L"Usage: Нацельтесь на судно и запустите эту команду с дроном в космосе." },
	{ L"/dta", UserCommands::UserCmd_AttackTarget, L"Usage: Нацельтесь на судно и запустите эту команду с дроном в космосе." },
	{ L"/dronedebug", UserCommands::UserCmd_Debug, L"Usage: Git gud" },
	{ L"/dronestop", UserCommands::UserCmd_DroneStop, L"Usage: /dronestop -- Это заставляет дрон останавливаться, что бы он ни делал, и сидеть на месте."},
	{ L"/ds", UserCommands::UserCmd_DroneStop, L"Usage: /ds -- Это заставляет дрон останавливаться, что бы он ни делал, и сидеть на месте."},
	{ L"/dronerecall", UserCommands::UserCmd_RecallDrone, L"Usage: /dronerecall"},
	{ L"/dr", UserCommands::UserCmd_RecallDrone, L"Usage: /dr"},
	{ L"/dronehelp", UserCommands::UserCmd_DroneHelp, L"Usage: /dronehelp"},
	{ L"/dh", UserCommands::UserCmd_DroneHelp, L"Usage: /dh"},
	{ L"/dronetypes", UserCommands::UserCmd_DroneBayAvailability, L"Usage: /dronetypes"},
	{ L"/dt", UserCommands::UserCmd_DroneBayAvailability, L"Usage: /dt"},
	{ L"/dronecome", UserCommands::UserCmd_DroneCome, L"Usage: /dronecome"},
	{ L"/dc", UserCommands::UserCmd_DroneCome, L"Usage: /dc"}
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions to hook
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "Drone Bays by Remnant";
	p_PI->sShortName = "dronebay";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkTimerCheckKick, PLUGIN_HkTimerCheckKick, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ClearClientInfo, PLUGIN_ClearClientInfo, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ShipDestroyed, PLUGIN_ShipDestroyed, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseEnter_AFTER, PLUGIN_HkIServerImpl_BaseEnter_AFTER, 0));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&SystemSwitchOutComplete, PLUGIN_HkIServerImpl_SystemSwitchOutComplete, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&SetTarget_AFTER, PLUGIN_HkIServerImpl_SetTarget_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Plugin_Communication_CallBack, PLUGIN_Plugin_Communication, 0));

	return p_PI;
}

