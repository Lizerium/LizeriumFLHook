/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 08 апреля 2026 14:28:37
 * Version: 1.0.23
 */

/**
Connecticut Plugin by MadHunter
*/

// includes 

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
#include <hookext_exports.h>
#include <math.h>

#define CLIENT_STATE_NONE		0
#define CLIENT_STATE_TRANSFER	1
#define CLIENT_STATE_RETURN		2

int transferFlags[MAX_CLIENT_ID + 1];

int set_iPluginDebug = 0;
// База для телепортации.
uint set_iTargetBaseID = 0;

// Ограниченные системы, отсюда невозможно выпрыгнуть.
list<uint> set_lRestrictedSystemIDs;

// Целевая система, не может выпрыгнуть отсюда.
uint set_iTargetSystemID = 0;

// База, которую можно использовать, если игрок застрял в системе связи.
uint set_iDefaultBaseID = 0;

/// Код возврата, указывающий FLHook, хотим ли мы продолжить обработку перехватчика.
PLUGIN_RETURNCODE returncode;

/// Очистить информацию о клиенте при подключении клиента.
void ClearClientInfo(uint iClientID)
{
	transferFlags[iClientID] = CLIENT_STATE_NONE;
}

/// Load the configuration
void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	memset(transferFlags, 0, sizeof(int) * (MAX_CLIENT_ID + 1));

	// The path to the configuration file.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\conn.cfg";

	// Load generic settings
	set_iPluginDebug = IniGetI(scPluginCfgFile, "General", "Debug", 0);
	set_iTargetBaseID = CreateID(IniGetS(scPluginCfgFile, "General", "TargetBase", "li06_05_base").c_str());
	set_iTargetSystemID = CreateID(IniGetS(scPluginCfgFile, "General", "TargetSystem", "li06").c_str());
	set_iDefaultBaseID = CreateID(IniGetS(scPluginCfgFile, "General", "DefaultBase", "li01_proxy_base").c_str());

	INI_Reader ini;
	set_lRestrictedSystemIDs.clear();
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("General"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("RestrictedSystem"))
					{
						string blockedSystem = ini.get_value_string();
						set_lRestrictedSystemIDs.push_back(CreateID(blockedSystem.c_str()));
						if (set_iPluginDebug)
							ConPrint(L"УВЕДОМЛЕНИЕ: Добавление системы с ограниченным подключением %s\n", stows(blockedSystem).c_str());
					}
				}
			}
		}
		ini.close();
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	static bool patched = false;
	srand((uint)time(0));

	// If we're being loaded from the command line while FLHook is running then
	// set_scCfgFile will not be empty so load the settings as FLHook only
	// calls load settings on FLHook startup and .rehash.
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (set_scCfgFile.length() > 0)
			LoadSettings();
	}
	return true;
}

bool IsDockedClient(unsigned int iClientID)
{
	unsigned int base = 0;
	pub::Player::GetBase(iClientID, base);
	if (base)
		return true;

	return false;
}

bool ValidateCargo(unsigned int iClientID)
{
	std::wstring playerName = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	std::list<CARGO_INFO> cargo;
	int holdSize = 0;

	HkEnumCargo(playerName, cargo, holdSize);

	for (std::list<CARGO_INFO>::const_iterator it = cargo.begin(); it != cargo.end(); ++it)
	{
		const CARGO_INFO & item = *it;

		bool flag = false;
		pub::IsCommodity(item.iArchID, flag);

		// Присутствует какой-то товар.
		if (flag)
			return false;
	}

	return true;
}

uint GetCustomBaseForClient(unsigned int iClientID)
{
	// Перейдите к плагинам, если этот корабль пристыкован к специальной базе.
	CUSTOM_BASE_IS_DOCKED_STRUCT info;
	info.iClientID = iClientID;
	info.iDockedBaseID = 0;
	Plugin_Communication(CUSTOM_BASE_IS_DOCKED, &info);
	return info.iDockedBaseID;
}

void StoreReturnPointForClient(unsigned int iClientID)
{
	// Он не пристыкован к пользовательской базе, проверьте наличие обычной базы.
	uint base = GetCustomBaseForClient(iClientID);
	if (!base)
		pub::Player::GetBase(iClientID, base);
	if (!base)
		return;

	HookExt::IniSetI(iClientID, "conn.retbase", base);
}

unsigned int ReadReturnPointForClient(unsigned int iClientID)
{
	return HookExt::IniGetI(iClientID, "conn.retbase");
}

void MoveClient(unsigned int iClientID, unsigned int targetBase)
{
	// Попросите другой плагин обрабатывать луч.
	CUSTOM_BASE_BEAM_STRUCT info;
	info.iClientID = iClientID;
	info.iTargetBaseID = targetBase;
	info.bBeamed = false;
	Plugin_Communication(CUSTOM_BASE_BEAM, &info);
	if (info.bBeamed)
		return;

	// Ни один плагин не справился с этим, сделайте это сами.
	unsigned int system;
	pub::Player::GetSystem(iClientID, system);
	Universe::IBase* base = Universe::get_base(targetBase);

	pub::Player::ForceLand(iClientID, targetBase); // beam

	// если не в той же системе, эмулируйте загрузку F1
	if (base->iSystemID != system)
	{
		Server.BaseEnter(targetBase, iClientID);
		Server.BaseExit(targetBase, iClientID);
		wstring wscCharFileName;
		HkGetCharFileName(ARG_CLIENTID(iClientID), wscCharFileName);
		wscCharFileName += L".fl";
		CHARACTER_ID cID;
		strcpy(cID.szCharFilename, wstos(wscCharFileName.substr(0, 14)).c_str());
		Server.CharacterSelect(cID, iClientID);
	}
}

bool CheckReturnDock(unsigned int iClientID, unsigned int target)
{
	unsigned int base = 0;
	pub::Player::GetBase(iClientID, base);

	if (base == target)
		return true;

	return false;
}

bool UserCmd_Process(uint iClientID, const wstring &cmd)
{
	returncode = DEFAULT_RETURNCODE;

	if (!cmd.compare(L"/conn"))
	{
		// Запретить переход, если вы находитесь в системе с ограниченным доступом или в целевой системе.
		uint system = 0;
		pub::Player::GetSystem(iClientID, system);
		if (find(set_lRestrictedSystemIDs.begin(), set_lRestrictedSystemIDs.end(), system) != set_lRestrictedSystemIDs.end()
			|| system == set_iTargetSystemID
			|| GetCustomBaseForClient(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0567"));
			return true;
		}

		if (!IsDockedClient(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1526"));
			return true;
		}

		if (!ValidateCargo(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1527"));
			return true;
		}

		StoreReturnPointForClient(iClientID);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0568"));
		transferFlags[iClientID] = CLIENT_STATE_TRANSFER;

		return true;
	}
	else if (!cmd.compare(L"/return"))
	{
		if (!ReadReturnPointForClient(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0569"));
			return true;
		}

		if (!IsDockedClient(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1541"));
			return true;
		}

		if (!CheckReturnDock(iClientID, set_iTargetBaseID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0570"));
			return true;
		}

		if (!ValidateCargo(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1527"));
			return true;
		}

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0571"));
		transferFlags[iClientID] = CLIENT_STATE_RETURN;

		return true;
	}

	return false;
}

void __stdcall CharacterSelect(struct CHARACTER_ID const &charid, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	transferFlags[iClientID] = CLIENT_STATE_NONE;
}

void __stdcall PlayerLaunch_AFTER(unsigned int ship, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	if (transferFlags[iClientID] == CLIENT_STATE_TRANSFER)
	{
		if (!ValidateCargo(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1527"));
			return;
		}

		transferFlags[iClientID] = CLIENT_STATE_NONE;
		MoveClient(iClientID, set_iTargetBaseID);
		return;
	}

	if (transferFlags[iClientID] == CLIENT_STATE_RETURN)
	{
		if (!ValidateCargo(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1527"));
			return;
		}

		transferFlags[iClientID] = CLIENT_STATE_NONE;
		unsigned int returnPoint = ReadReturnPointForClient(iClientID);

		if (!returnPoint)
			return;

		MoveClient(iClientID, returnPoint);
		HookExt::IniSetI(iClientID, "conn.retbase", 0);
		return;
	}
}


/** Functions to hook */
EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "Conn Plugin by MadHunter";
	p_PI->sShortName = "conn";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ClearClientInfo, PLUGIN_ClearClientInfo, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CharacterSelect, PLUGIN_HkIServerImpl_CharacterSelect, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&PlayerLaunch_AFTER, PLUGIN_HkIServerImpl_PlayerLaunch_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	return p_PI;
}