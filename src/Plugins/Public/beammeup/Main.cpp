/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 30 марта 2026 12:21:08
 * Version: 1.0.11
 */

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
#include <FLHook.h>
#include <math.h>

#define CLIENT_STATE_NONE		0
#define CLIENT_STATE_TRANSFER	1
#define CLIENT_STATE_RETURN		2
#define ADDR_FLCONFIG 0x25410

int transferFlags[MAX_CLIENT_ID + 1];

void TeleportPointFunc(uint& iClientID, uint& ItemID, uint& BaseID);
void HkLoadDLLConf(const char* szFLConfigFile);
wstring HkGetWStringFromIDS(uint iIDS);
vector<HINSTANCE> vDLLs;

// Плагин загружен: банкир **(banker.dll)**
int set_iPluginDebug = 0;

// Целевая система, не может выпрыгнуть отсюда.
uint set_iTargetSystemID = 0;

// База для телепортации.
uint set_iTargetBaseID = 0;

// Ограниченная система, отсюда невозможно выпрыгнуть.
uint set_iRestrictedSystemID = 0;

// База, которую можно использовать, если игрок оказался в ловушке "beammeup" системы.
uint set_iDefaultBaseID = 0;

// Предмет в грузовом отсеке для корректного телепорта в Альдрин.
uint set_iDefaultAldrinTeleportItemID = 0;

// База в Альдрин.
uint set_iDefaultAldrinTeleportBaseID = 0;

// Предмет в грузовом отсеке для корректного телепорта в Секретную локацию.
uint set_iDefaultSecretMapTeleportItemID = 0;

// Предмет в грузовом отсеке для корректного телепорта в Секретную локацию.
uint set_iDefaultSecretMapTeleportBaseID = 0;

/// Код возврата, указывающий FLHook, хотим ли мы продолжить обработку перехватчика.
PLUGIN_RETURNCODE returncode;

/// Очистить информацию о клиенте при подключении клиента.
void ClearClientInfo(uint iClientID)
{
	transferFlags[iClientID] = CLIENT_STATE_NONE;
}

/// Загрузите конфигурацию
void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	memset(transferFlags, 0, sizeof(int) * (MAX_CLIENT_ID + 1));

	// Путь к файлу конфигурации.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\beammeup.ini";

	// Загрузить общие настройки
	set_iPluginDebug = IniGetI(scPluginCfgFile, "General", "Debug", 0);
	set_iTargetSystemID = CreateID(IniGetS(scPluginCfgFile, "General", "TargetSystem", "pvp01").c_str());
	set_iTargetBaseID = CreateID(IniGetS(scPluginCfgFile, "General", "TargetBase", "pvp01_02_base").c_str());
	set_iRestrictedSystemID = CreateID(IniGetS(scPluginCfgFile, "General", "RestrictedSystem", "bw11").c_str());
	set_iDefaultBaseID = CreateID(IniGetS(scPluginCfgFile, "General", "DefaultBase", "Li01_mobile_proxy_base").c_str());

	set_iDefaultAldrinTeleportItemID = CreateID(IniGetS(scPluginCfgFile, "Teleports", "AldrinTeleportItem", "commod_aldrin01").c_str());
	set_iDefaultAldrinTeleportBaseID = CreateID(IniGetS(scPluginCfgFile, "Teleports", "AldrinTeleportBaseId", "ALDRIN01_01_Base").c_str());
	set_iDefaultSecretMapTeleportItemID = CreateID(IniGetS(scPluginCfgFile, "Teleports", "SecretMapTeleportItem", "commod_unk01").c_str());
	set_iDefaultSecretMapTeleportBaseID = CreateID(IniGetS(scPluginCfgFile, "Teleports", "SecretMapTeleportBaseId", "UB01_01_Base").c_str());

	char* szFLConfig = (char*)((char*)GetModuleHandle(0) + ADDR_FLCONFIG);
	HkLoadDLLConf(szFLConfig);
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
	std::wstring playerName = (wchar_t*)Players.GetActiveCharacterName(iClientID);
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
		{
			return false;
		}	
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
	// Не пристыкован к кастомной базе, проверьте наличие штатной базы
	uint base = GetCustomBaseForClient(iClientID);
	if (!base)
		pub::Player::GetBase(iClientID, base);
	if (!base)
		return;

	HookExt::IniSetI(iClientID, "beammeup.retbase", base);
}

unsigned int ReadReturnPointForClient(unsigned int iClientID)
{
	return HookExt::IniGetI(iClientID, "beammeup.retbase");
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

	pub::Player::ForceLand(iClientID, targetBase); // луч

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

bool UserCmd_Process(uint iClientID, const wstring &wscParam)
{
	returncode = DEFAULT_RETURNCODE;

	if (!wscParam.compare(L"/beammeup"))
	{
		// CUSTOM_BASE_BEAM_STRUCT info;
		// Запретить переход, если вы находитесь в системе с ограниченным доступом или в целевой системе.
		uint system = 0;
		uint base = 0;
		pub::Player::GetSystem(iClientID, system);
		pub::Player::GetBase(iClientID, base);
		if (system == set_iRestrictedSystemID
			|| system == set_iTargetSystemID
			|| base != (uint)set_iDefaultBaseID)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0399"));
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
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0400"));
		transferFlags[iClientID] = CLIENT_STATE_TRANSFER;

		return true;
	}
	else if (!wscParam.compare(L"/return"))
	{
		if (!ReadReturnPointForClient(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0401"));
			return true;
		}

		if (!IsDockedClient(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1526"));
			return true;
		}

		if (!CheckReturnDock(iClientID, set_iTargetBaseID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0402"));
			return true;
		}

		if (!ValidateCargo(iClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1527"));
			return true;
		}

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0403"));
		transferFlags[iClientID] = CLIENT_STATE_RETURN;

		return true;
	}

	if (wscParam.rfind(L"/beammeup", 0) == 0) // строка начинается с "/beammeup"
	{
		wstring wscTarget = GetParam(wscParam, ' ', 1);
		uint base = 0;

		if (wscTarget.length() > 0)
		{
			if (wscTarget == L"aldrin")
			{
				pub::Player::GetBase(iClientID, base);
				if (base > 0)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1498"));
					return true;
				}
				Archetype::Ship* TheShipArch = Archetype::GetShip(Players[iClientID].iShipArchetype);
				if (TheShipArch->iShipClass == 11)
					TeleportPointFunc(iClientID, set_iDefaultAldrinTeleportItemID, set_iDefaultAldrinTeleportBaseID);
				else
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1497"));
				return true;
			}

			if (wscTarget == L"secret")
			{
				pub::Player::GetBase(iClientID, base);
				if (base > 0)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1498"));
					return true;
				}
				Archetype::Ship* TheShipArch = Archetype::GetShip(Players[iClientID].iShipArchetype);
				if (TheShipArch->iShipClass == 11)
					TeleportPointFunc(iClientID, set_iDefaultSecretMapTeleportItemID, set_iDefaultSecretMapTeleportBaseID);
				else
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1497"));
				return true;
			}

			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1492"));
			return true;
		}
	}

	return false;
}

void TeleportPointFunc(uint& iClientID, uint& ItemID, uint& BaseID)
{
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1493"));

	list<CARGO_INFO> lstCargo;
	int iRem;
	bool ret = true;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
	int countRemove = 0;
	bool existItem = 0;
	foreach(lstCargo, CARGO_INFO, cargo)
	{
		const GoodInfo* gi = GoodList::find_by_id(cargo->iArchID);
		if (!gi)
			continue;

		// ищу необходимый предмет
		if (cargo->iArchID == ItemID)
		{
			wstring name = HkGetWStringFromIDS(gi->iIDSName);

			//чищу грузовой отсек
			HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, cargo->iCount);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1494") + name);

			//отправляю пассажира на базу
			MoveClient(iClientID, BaseID);
			PrintUserCmdText(iClientID, name + GetLocalized(iClientID, "MSG_1496"));
			existItem++;
			break;
		}
	}

	if (existItem == 0)
	{
		const GoodInfo* gi = GoodList::find_by_id(ItemID);
		wstring nameNeedItem = HkGetWStringFromIDS(gi->iIDSName);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1495") + nameNeedItem);
	}
}

wstring HkGetWStringFromIDS(uint iIDS) //Only works for names
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

void HkLoadDLLConf(const char* szFLConfigFile)
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
		HookExt::IniSetI(iClientID, "beammeup.retbase", 0);
		return;
	}
}


/** Функции для перехвата */
EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "BeamMeUp Plugin based on: Connecticut Plugin by MadHunter";
	p_PI->sShortName = "beammeup";
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