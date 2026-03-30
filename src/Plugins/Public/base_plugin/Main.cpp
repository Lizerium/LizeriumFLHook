/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 30 марта 2026 12:21:08
 * Version: 1.0.11
 */

/**
 Base Plugin for FLHook-Plugin
 by Cannon.

0.1:
 Initial release
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
#include <PluginUtilities.h>
#include "Main.h"
#include <sstream>
#include <hookext_exports.h>

// Клиенты
map<uint, CLIENT_DATA> clients;

// Базы
map<uint, PlayerBase*> player_bases;

map<uint, bool> mapPOBShipPurchases;

/// 0 = HTML, 1 = JSON, 2 = Both
int ExportType = 0;

/// Режим отладки
int set_plugin_debug = 0;

/// Корабль, используемый для строительства и модернизации баз.
uint set_construction_shiparch = 0;

/// Карта количества товаров, необходимых для строительного корабля
map<uint, uint> construction_items;

/// список предметов и их количество, необходимое для ремонта 10000 единиц урона
list<REPAIR_ITEM> set_base_repair_items;

/// список предметов, используемых человеческим экипажем
map<uint, uint> set_base_crew_consumption_items;
map<uint, uint> set_base_crew_food_items;

/// Товар, используемый в качестве экипажа для базы.
uint set_base_crew_type;

/// Код возврата, указывающий FLHook, хотим ли мы продолжить обработку перехватчика.
PLUGIN_RETURNCODE returncode;

/// Сопоставление хеша псевдонима предмета с рецептами для создания предмета.
map<uint, RECIPE> recipes;

/// Карта хеша псевдонима предмета с рецептами для работы со щитом.
map<uint, uint> shield_power_items;

/// Сопоставление идентификаторов космических объектов с базовыми модулями для ускорения алгоритмов повреждения.
map<uint, Module*> spaceobj_modules;

/// Путь к HTML-странице статуса защиты
string set_status_path_html;

/// то же самое, но для JSON
string set_status_path_json;

/// Повреждения базы каждый тик
uint set_damage_per_tick = 600;

// Секунды на тик урона
uint set_damage_tick_time = 16;

// Секунды на тик
uint set_tick_time = 16;

// Сколько повреждений мы исцеляем за цикл ремонта?
uint repair_per_repair_cycle = 60000;

/// Если щит поднят, то урон по базе изменяется на этот множитель.
float set_shield_damage_multiplier = 0.03f;

// Если это правда, используйте новую защитную платформу на солнечной энергии.	 	
bool set_new_spawn = true;

/// True, если настройки должны быть перезагружены
bool load_settings_required = true;

/// режим отпуска (holiday)
bool set_holiday_mode = false;

//структура звуков pob
POBSOUNDS pbsounds;

//структура архетипа
map<string, ARCHTYPE_STRUCT> mapArchs;

//товары, за которыми стоит следить для регистрации
map<uint, wstring> listCommodities;

//активация враждебности и оружейной платформы от урона, нанесенного одним игроком
float damage_threshold = 400000;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlayerBase *GetPlayerBase(uint base)
{
	map<uint, PlayerBase*>::iterator i = player_bases.find(base);
	if (i != player_bases.end())
		return i->second;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlayerBase *GetPlayerBaseForClient(uint iClientID)
{
	map<uint, CLIENT_DATA>::iterator j = clients.find(iClientID);
	if (j == clients.end())
		return 0;

	map<uint, PlayerBase*>::iterator i = player_bases.find(j->second.player_base);
	if (i == player_bases.end())
		return 0;

	return i->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlayerBase *GetLastPlayerBaseForClient(uint iClientID)
{
	map<uint, CLIENT_DATA>::iterator j = clients.find(iClientID);
	if (j == clients.end())
		return 0;

	map<uint, PlayerBase*>::iterator i = player_bases.find(j->second.last_player_base);
	if (i == player_bases.end())
		return 0;

	return i->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Logging(const char *szString, ...)
{
	char szBufString[1024];
	va_list marker;
	va_start(marker, szString);
	_vsnprintf(szBufString, sizeof(szBufString) - 1, szString, marker);

	char szBuf[64];
	time_t tNow = time(0);
	struct tm *t = localtime(&tNow);
	strftime(szBuf, sizeof(szBuf), "%d/%m/%Y %H:%M:%S", t);

	FILE *Logfile = fopen(("./flhook_logs/flhook_cheaters.log"), "at");
	if (Logfile)
	{
		fprintf(Logfile, "%s %s\n", szBuf, szBufString);
		fflush(Logfile);
		fclose(Logfile);
	}
}

// These logging functions need consolidating.
void BaseLogging(const char *szString, ...)
{
	char szBufString[1024];
	va_list marker;
	va_start(marker, szString);
	_vsnprintf(szBufString, sizeof(szBufString) - 1, szString, marker);

	char szBuf[64];
	time_t tNow = time(0);
	struct tm *t = localtime(&tNow);
	strftime(szBuf, sizeof(szBuf), "%d/%m/%Y %H:%M:%S", t);

	FILE *BaseLogfile = fopen("./flhook_logs/playerbase_events.log", "at");
	if (BaseLogfile)
	{
		fprintf(BaseLogfile, "%s %s\n", szBuf, szBufString);
		fflush(BaseLogfile);
		fclose(BaseLogfile);
	}
}

FILE *LogfileEventCommodities = fopen("./flhook_logs/event_pobsales.log", "at");

void LoggingEventCommodity(const char *szString, ...)
{
	char szBufString[1024];
	va_list marker;
	va_start(marker, szString);
	_vsnprintf(szBufString, sizeof(szBufString) - 1, szString, marker);

	char szBuf[64];
	time_t tNow = time(0);
	struct tm *t = localtime(&tNow);
	strftime(szBuf, sizeof(szBuf), "%d/%m/%Y %H:%M:%S", t);
	fprintf(LogfileEventCommodities, "%s %s\n", szBuf, szBufString);
	fflush(LogfileEventCommodities);
	fclose(LogfileEventCommodities);
	LogfileEventCommodities = fopen("./flhook_logs/event_pobsales.log", "at");
}

void Notify_Event_Commodity_Sold(uint iClientID, string commodity, int count, string basename)
{
	//internal log
	wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	wstring wscMsgLog = L"<%player> has sold <%units> of the event commodity <%eventname> to the POB <%pob>";
	wscMsgLog = ReplaceStr(wscMsgLog, L"%player", wscCharname.c_str());
	wscMsgLog = ReplaceStr(wscMsgLog, L"%eventname", stows(commodity).c_str());
	wscMsgLog = ReplaceStr(wscMsgLog, L"%units", stows(itos(count)).c_str());
	wscMsgLog = ReplaceStr(wscMsgLog, L"%pob", stows(basename).c_str());
	string scText = wstos(wscMsgLog);
	LoggingEventCommodity("%s", scText.c_str());
}

void LogCheater(uint iClientID, const wstring &reason)
{
	CAccount *acc = Players.FindAccountFromClientID(iClientID);

	if (!HkIsValidClientID(iClientID) || !acc)
	{
		AddLog("ERROR: invalid parameter in log cheater, clientid=%u acc=%08x reason=%s", iClientID, acc, wstos(reason).c_str());
		return;
	}

	//internal log
	string scText = wstos(reason);
	Logging("%s", scText.c_str());

	/*
	// Set the kick timer to kick this player. We do this to break potential
	// stack corruption.
	HkDelayedKick(iClientID, 1);

	// Ban the account.
	flstr *flStr = CreateWString(acc->wszAccID);
	Players.BanAccount(*flStr, true);
	FreeWString(flStr);

	// Overwrite the ban file so that it contains the ban reason
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	string scBanPath = scAcctPath + wstos(wscDir) + "\\banned";
	FILE *file = fopen(scBanPath.c_str(), "wb");
	if (file)
	{
	fprintf(file, "Autobanned by BasePlugin\n");
	fclose(file);
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// For the specified iClientID setup the reputation to any bases in the
// iClientID's system.
void SyncReputationForClientShip(uint ship, uint iClientID)
{
	int player_rep;
	pub::SpaceObj::GetRep(ship, player_rep);

	uint system;
	pub::SpaceObj::GetSystem(ship, system);

	map<uint, PlayerBase*>::iterator base = player_bases.begin();
	for (; base != player_bases.end(); base++)
	{
		if (base->second->system == system)
		{
			float attitude = base->second->GetAttitudeTowardsClient(iClientID);
			if (set_plugin_debug > 1)
				ConPrint(L"SyncReputationForClientShip:: ship=%u attitude=%f base=%08x\n", ship, attitude, base->first);
			for (vector<Module*>::iterator module = base->second->modules.begin();
				module != base->second->modules.end(); ++module)
			{
				if (*module)
				{
					(*module)->SetReputation(player_rep, attitude);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// HTML-encodes a string and returns the encoded string.
wstring HtmlEncode(wstring text)
{
	wstring sb;
	int len = text.size();
	for (int i = 0; i < len; i++)
	{
		switch (text[i])
		{
		case L'<':
			sb.append(L"&lt;");
			break;
		case L'>':
			sb.append(L"&gt;");
			break;
		case L'"':
			sb.append(L"&quot;");
			break;
		case L'&':
			sb.append(L"&amp;");
			break;
		default:
			if (text[i] > 159)
			{
				sb.append(L"&#");
				sb.append(stows(itos((int)text[i])));
				sb.append(L";");
			}
			else
			{
				sb.append(1, text[i]);
			}
			break;
		}
	}
	return sb;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Clear iClientID info when a iClientID connects.
void ClearClientInfo(uint iClientID)
{
	clients.erase(iClientID);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;
	load_settings_required = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Load the configuration
void LoadSettingsActual()
{
	returncode = DEFAULT_RETURNCODE;

	// The path to the configuration file.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string cfg_file = string(szCurDir) + "\\flhook_plugins\\base.cfg";
	string cfg_fileitems = string(szCurDir) + "\\flhook_plugins\\base_recipe_items.cfg";
	string cfg_filemodules = string(szCurDir) + "\\flhook_plugins\\base_recipe_modules.cfg";
	string cfg_filearch = string(szCurDir) + "\\flhook_plugins\\base_archtypes.cfg";

	map<uint, PlayerBase*>::iterator base = player_bases.begin();
	for (; base != player_bases.end(); base++)
	{
		delete base->second;
	}

	recipes.clear();
	construction_items.clear();
	set_base_repair_items.clear();
	set_base_crew_consumption_items.clear();
	set_base_crew_food_items.clear();
	shield_power_items.clear();

	HookExt::ClearMiningObjData();

	INI_Reader ini;
	if (ini.open(cfg_file.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("general"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("debug"))
					{
						set_plugin_debug = ini.get_value_int(0);
					}
					else if (ini.is_value("status_path_html"))
					{
						set_status_path_html = ini.get_value_string();
					}
					else if (ini.is_value("status_path_json"))
					{
						set_status_path_json = ini.get_value_string();
					}
					else if (ini.is_value("damage_threshold"))
					{
						damage_threshold = ini.get_value_float(0);
					}
					else if (ini.is_value("status_export_type"))
					{
						ExportType = ini.get_value_int(0);
					}
					else if (ini.is_value("damage_per_tick"))
					{
						set_damage_per_tick = ini.get_value_int(0);
					}
					else if (ini.is_value("damage_tick_time"))
					{
						set_damage_tick_time = ini.get_value_int(0);
					}
					else if (ini.is_value("tick_time"))
					{
						set_tick_time = ini.get_value_int(0);
					}
					else if (ini.is_value("health_to_heal_per_cycle"))
					{
						repair_per_repair_cycle = ini.get_value_int(0);
					}
					else if (ini.is_value("shield_damage_multiplier"))
					{
						set_shield_damage_multiplier = ini.get_value_float(0);
					}
					else if (ini.is_value("construction_shiparch"))
					{
						set_construction_shiparch = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("construction_item"))
					{
						uint good = CreateID(ini.get_value_string(0));
						uint quantity = ini.get_value_int(1);
						construction_items[good] = quantity;
					}
					else if (ini.is_value("base_crew_item"))
					{
						set_base_crew_type = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("base_repair_item"))
					{
						REPAIR_ITEM item;
						item.good = CreateID(ini.get_value_string(0));
						item.quantity = ini.get_value_int(1);
						set_base_repair_items.push_back(item);
					}
					else if (ini.is_value("base_crew_consumption_item"))
					{
						uint good = CreateID(ini.get_value_string(0));
						uint quantity = ini.get_value_int(1);
						set_base_crew_consumption_items[good] = quantity;
					}
					else if (ini.is_value("base_crew_food_item"))
					{
						uint good = CreateID(ini.get_value_string(0));
						uint quantity = ini.get_value_int(1);
						set_base_crew_food_items[good] = quantity;
					}
					else if (ini.is_value("shield_power_item"))
					{
						uint good = CreateID(ini.get_value_string(0));
						uint quantity = ini.get_value_int(1);
						shield_power_items[good] = quantity;
					}
					else if (ini.is_value("set_new_spawn"))
					{
						set_new_spawn = true;
					}
					else if (ini.is_value("set_holiday_mode"))
					{
						set_holiday_mode = ini.get_value_bool(0);
						if (set_holiday_mode)
						{
							ConPrint(L"BASE: Attention, POB Holiday mode is enabled.\n");
						}
					}
					else if (ini.is_value("watch"))
					{
						uint c = CreateID(ini.get_value_string());
						listCommodities[c] = stows(ini.get_value_string());

					}
				}
			}
		}
		ini.close();
	}

	if (ini.open(cfg_fileitems.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("recipe"))
			{
				RECIPE recipe;
				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
					{
						recipe.nickname = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("produced_item"))
					{
						recipe.produced_item = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("infotext"))
					{
						recipe.infotext = stows(ini.get_value_string());
					}
					else if (ini.is_value("cooking_rate"))
					{
						recipe.cooking_rate = ini.get_value_int(0);
					}
					else if (ini.is_value("consumed"))
					{
						recipe.consumed_items[CreateID(ini.get_value_string(0))] = ini.get_value_int(1);
					}
					else if (ini.is_value("reqlevel"))
					{
						recipe.reqlevel = ini.get_value_int(0);
					}
				}
				recipes[recipe.nickname] = recipe;
			}
		}
		ini.close();
	}

	if (ini.open(cfg_filemodules.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("recipe"))
			{
				RECIPE recipe;
				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
					{
						recipe.nickname = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("produced_item"))
					{
						recipe.produced_item = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("infotext"))
					{
						recipe.infotext = stows(ini.get_value_string());
					}
					else if (ini.is_value("cooking_rate"))
					{
						recipe.cooking_rate = ini.get_value_int(0);
					}
					else if (ini.is_value("consumed"))
					{
						recipe.consumed_items[CreateID(ini.get_value_string(0))] = ini.get_value_int(1);
					}
					else if (ini.is_value("reqlevel"))
					{
						recipe.reqlevel = ini.get_value_int(0);
					}
				}
				recipes[recipe.nickname] = recipe;
			}
		}
		ini.close();
	}

	if (ini.open(cfg_filearch.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("arch"))
			{
				ARCHTYPE_STRUCT archstruct;
				string nickname = "default";
				int radius = 0;
				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
					{
						nickname = ini.get_value_string(0);
					}
					else if (ini.is_value("invulnerable"))
					{
						archstruct.invulnerable = ini.get_value_int(0);
					}
					else if (ini.is_value("logic"))
					{
						archstruct.logic = ini.get_value_int(0);
					}
					else if (ini.is_value("radius"))
					{
						archstruct.radius = ini.get_value_float(0);
					}
					else if (ini.is_value("idrestriction"))
					{
						archstruct.idrestriction = ini.get_value_int(0);
					}
					else if (ini.is_value("isjump"))
					{
						archstruct.isjump = ini.get_value_int(0);
					}
					else if (ini.is_value("shipclassrestriction"))
					{
						archstruct.shipclassrestriction = ini.get_value_int(0);
					}
					else if (ini.is_value("allowedshipclasses"))
					{
						archstruct.allowedshipclasses.push_back(ini.get_value_int(0));
					}
					else if (ini.is_value("allowedids"))
					{
						archstruct.allowedids.push_back(CreateID(ini.get_value_string(0)));
					}
					else if (ini.is_value("module"))
					{
						archstruct.modules.push_back(ini.get_value_string(0));
					}
					else if (ini.is_value("display"))
					{
						archstruct.display = ini.get_value_bool(0);
					}
					else if (ini.is_value("mining"))
					{
						archstruct.mining = ini.get_value_bool(0);
					}
					else if (ini.is_value("miningevent"))
					{
						archstruct.miningevent = ini.get_value_string(0);
					}
				}
				mapArchs[nickname] = archstruct;
			}
		}
		ini.close();
	}

	//Create the POB sound hashes
	pbsounds.destruction1 = CreateID("pob_evacuate2");
	pbsounds.destruction2 = CreateID("pob_firecontrol");
	pbsounds.heavydamage1 = CreateID("pob_breach");
	pbsounds.heavydamage2 = CreateID("pob_reactor");
	pbsounds.heavydamage3 = CreateID("pob_heavydamage");
	pbsounds.mediumdamage1 = CreateID("pob_hullbreach");
	pbsounds.mediumdamage2 = CreateID("pob_critical");
	pbsounds.lowdamage1 = CreateID("pob_fire");
	pbsounds.lowdamage2 = CreateID("pob_engineering");

	char datapath[MAX_PATH];
	GetUserDataPath(datapath);

	// Create base account dir if it doesn't exist
	string basedir = string(datapath) + "\\Accts\\MultiPlayer\\player_bases\\";
	CreateDirectoryA(basedir.c_str(), 0);

	// Load and spawn all bases
	string path = string(datapath) + "\\Accts\\MultiPlayer\\player_bases\\base_*.ini";

	WIN32_FIND_DATA findfile;
	HANDLE h = FindFirstFile(path.c_str(), &findfile);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			string filepath = string(datapath) + "\\Accts\\MultiPlayer\\player_bases\\" + findfile.cFileName;
			PlayerBase *base = new PlayerBase(filepath);
			player_bases[base->base] = base;
			base->Spawn();
		} while (FindNextFile(h, &findfile));
		FindClose(h);
	}

	// Load and sync player state
	struct PlayerData *pd = 0;
	while (pd = Players.traverse_active(pd))
	{
		uint iClientID = pd->iOnlineID;
		if (HkIsInCharSelectMenu(iClientID))
			continue;

		// If this player is in space, set the reputations.
		if (pd->iShipID)
			SyncReputationForClientShip(pd->iShipID, iClientID);

		// Get state if player is in player base and  reset the commodity list
		// and send a dummy entry if there are no commodities in the market
		LoadDockState(iClientID);
		if (clients[iClientID].player_base)
		{
			PlayerBase *base = GetPlayerBaseForClient(iClientID);
			if (base)
			{
				// Reset the commodity list	and send a dummy entry if there are no
				// commodities in the market
				SaveDockState(iClientID);
				SendMarketGoodSync(base, iClientID);
				SendBaseStatus(iClientID, base);
			}
			else
			{
				// Force the ship to launch to space as the base has been destroyed
				DeleteDockState(iClientID);
				SendResetMarketOverride(iClientID);
				ForceLaunch(iClientID);
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HkTimerCheckKick()
{
	returncode = DEFAULT_RETURNCODE;

	if (load_settings_required)
	{
		load_settings_required = false;
		LoadSettingsActual();
	}

	uint curr_time = (uint)time(0);
	map<uint, PlayerBase*>::iterator iter = player_bases.begin();
	while (iter != player_bases.end())
	{
		PlayerBase *base = iter->second;
		// Advance to next base in case base is deleted in timer dispatcher
		++iter;
		// Dispatch timer but we can safely ignore the return
		base->Timer(curr_time);
	}

	if (ExportType == 0 || ExportType == 2)
	{
		// Write status to an html formatted page every 60 seconds
		if ((curr_time % 60) == 0 && set_status_path_html.size() > 0)
		{
			ExportData::ToHTML();
		}
	}

	if (ExportType == 1 || ExportType == 2)
	{
		// Write status to a json formatted page every 60 seconds
		if ((curr_time % 60) == 0 && set_status_path_json.size() > 0)
		{
			ExportData::ToJSON();
		}
	}
}

bool __stdcall HkCb_IsDockableError(uint dock_with, uint base)
{
	if (GetPlayerBase(base))
		return false;
	ConPrint(L"ERROR: Base not found dock_with=%08x base=%08x\n", dock_with, base);
	return true;
}

__declspec(naked) void HkCb_IsDockableErrorNaked()
{
	__asm
	{
		test[esi + 0x1b4], eax
		jnz no_error
		push[edi + 0xB8]
		push[esi + 0x1b4]
		call HkCb_IsDockableError
		test al, al
		jz no_error
		push 0x62b76d3
		ret
		no_error :
		push 0x62b76fc
			ret
	}
}

bool __stdcall HkCb_Land(IObjInspectImpl *obj, uint base_dock_id, uint base)
{
	if (obj)
	{
		uint iClientID = HkGetClientIDByShip(obj->get_id());
		if (iClientID)
		{
			if (set_plugin_debug > 1)
				ConPrint(L"Land client=%u base_dock_id=%u base=%u\n", iClientID, base_dock_id, base);

			// If we're docking at a player base, do nothing.
			if (clients[iClientID].player_base)
				return true;

			// If we're not docking at a player base then clear 
			// the last base flag
			clients[iClientID].last_player_base = 0;
			clients[iClientID].player_base = 0;
			if (base == 0)
			{
				char szSystem[1024];
				pub::GetSystemNickname(szSystem, sizeof(szSystem), Players[iClientID].iSystemID);

				char szProxyBase[1024];
				sprintf(szProxyBase, "%s_proxy_base", szSystem);

				uint iProxyBaseID = CreateID(szProxyBase);

				clients[iClientID].player_base = base_dock_id;
				clients[iClientID].last_player_base = base_dock_id;
				if (set_plugin_debug > 1)
					ConPrint(L"Land[2] client=%u baseDockID=%u base=%u player_base=%u\n", iClientID, base_dock_id, base, clients[iClientID].player_base);
				pub::Player::ForceLand(iClientID, iProxyBaseID);
				return false;
			}
		}
	}
	return true;
}

__declspec(naked) void HkCb_LandNaked()
{
	__asm
	{
		mov al, [ebx + 0x1c]
		test al, al
		jz not_in_dock

		mov eax, [ebx + 0x18] // base id
		push eax
		mov eax, [esp + 0x14] // dock target
		push eax
		push edi // objinspect
		call HkCb_Land
		test al, al
		jz done

		not_in_dock :
		// Copied from moor.dll to support mooring.
		mov	al, [ebx + 0x1c]
			test	al, al
			jnz	done
			// It's false, so a safe bet that it's a moor.  Is it the player?
			mov	eax, [edi]
			mov	ecx, edi
			call[eax + 0xbc] // is_player
			test	al, al
			jnz done




			done :
		push 0x6D0C251
			ret
	}
}

static bool patched = false;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	srand((uint)time(0));
	// If we're being loaded from the command line while FLHook is running then
	// set_scCfgFile will not be empty so load the settings as FLHook only
	// calls load settings on FLHook startup and .rehash.
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (!patched)
		{
			patched = true;

			hModServer = GetModuleHandleA("server.dll");
			{
				// Call our function on landing
				byte patch[] = { 0xe9 }; // jmpr
				WriteProcMem((char*)hModServer + 0x2c24c, patch, sizeof(patch));
				PatchCallAddr((char*)hModServer, 0x2c24c, (char*)HkCb_LandNaked);
			}

			hModCommon = GetModuleHandleA("common.dll");
			{
				// Suppress "is dockable " error message
				byte patch[] = { 0xe9 }; // jmpr
				WriteProcMem((char*)hModCommon + 0x576cb, patch, sizeof(patch));
				PatchCallAddr((char*)hModCommon, 0x576cb, (char*)HkCb_IsDockableErrorNaked);
			}

			{
				// Suppress GetArch() error on max hit points call
				byte patch[] = { 0x90, 0x90 }; // nop nop
				WriteProcMem((char*)hModCommon + 0x995b6, patch, sizeof(patch));
				WriteProcMem((char*)hModCommon + 0x995fc, patch, sizeof(patch));
			}
		}

		HkLoadStringDLLs();
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (patched)
		{
			{
				// Unpatch the landing hook
				byte patch[] = { 0x8A, 0x43, 0x1C, 0x84, 0xC0 };
				WriteProcMem((char*)hModServer + 0x2c24c, patch, sizeof(patch));
			}

			{
				// Unpatch the Suppress "is dockable " error message
				byte patch[] = { 0x85, 0x86, 0xb4, 0x01, 0x00 };
				WriteProcMem((char*)hModCommon + 0x576cb, patch, sizeof(patch));
			}
		}

		map<uint, PlayerBase*>::iterator base = player_bases.begin();
		for (; base != player_bases.end(); base++)
		{
			delete base->second;
		}

		HkUnloadStringDLLs();
	}
	return true;
}

bool UserCmd_Process(uint iClientID, const wstring &args)
{
	returncode = DEFAULT_RETURNCODE;
	if (args.find(L"/base login") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseLogin(iClientID, args);
		return true;
	}
	else if (args.find(L"/base addpwd") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseAddPwd(iClientID, args);
		return true;
	}
	else if (args.find(L"/base rmpwd") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseRmPwd(iClientID, args);
		return true;
	}
	else if (args.find(L"/base lstpwd") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseLstPwd(iClientID, args);
		return true;
	}
	else if (args.find(L"/base setmasterpwd") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseSetMasterPwd(iClientID, args);
		return true;
	}
	else if (args.find(L"/base addtag") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0116"));
		PlayerCommands::BaseRmHostileTag(iClientID, args);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0117"));
		PlayerCommands::BaseAddAllyTag(iClientID, args);
		return true;
	}
	else if (args.find(L"/base rmtag") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseRmAllyTag(iClientID, args);
		return true;
	}
	else if (args.find(L"/base lsttag") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseLstAllyTag(iClientID, args);
		return true;
	}
	else if (args.find(L"/base addhostile") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0118"));
		PlayerCommands::BaseRmAllyTag(iClientID, args);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0117"));
		PlayerCommands::BaseAddHostileTag(iClientID, args);
		return true;
	}
	else if (args.find(L"/base rmhostile") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseRmHostileTag(iClientID, args);
		return true;
	}
	else if (args.find(L"/base lsthostile") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseLstHostileTag(iClientID, args);
		return true;
	}
	else if (args.find(L"/base rep") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseRep(iClientID, args);
		return true;
	}
	else if (args.find(L"/base defensemode") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseDefenseMode(iClientID, args);
		return true;
	}
	else if (args.find(L"/base deploy") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseDeploy(iClientID, args);
		return true;
	}
	else if (args.find(L"/shop") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::Shop(iClientID, args);
		return true;
	}
	else if (args.find(L"/basebank") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::Bank(iClientID, args);
		return true;
	}
	else if (args.find(L"/base info") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseInfo(iClientID, args);
		return true;
	}
	else if (args.find(L"/base facmod") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseFacMod(iClientID, args);
		return true;
	}
	else if (args.find(L"/base defmod") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseDefMod(iClientID, args);
		return true;
	}
	else if (args.find(L"/base shieldmod") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseShieldMod(iClientID, args);
		return true;
	}
	else if (args.find(L"/base buildmod") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseBuildMod(iClientID, args);
		return true;
	}
	else if (args.find(L"/base") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		PlayerCommands::BaseHelp(iClientID, args);
		return true;
	}
	return false;
}

static bool IsDockingAllowed(PlayerBase *base, uint iClientID)
{
	// Allies can always dock.
	wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
	for (list<wstring>::iterator i = base->ally_tags.begin(); i != base->ally_tags.end(); ++i)
	{
		if (charname.find(*i) == 0)
		{
			return true;
		}
	}

	// Base allows neutral ships to dock
	if (base->defense_mode == 2)
	{
		return true;
	}

	// Base allows neutral ships to dock
	if (base->defense_mode == 4)
	{
		return true;
	}

	return false;
}

// If this is a docking request at a player controlled based then send
// an update to set the base arrival text, base economy and change the
// infocards.

void __stdcall SystemSwitchOutComplete(unsigned int iShip, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	// Make player invincible to fix JHs/JGs near mine fields sometimes
	// exploding player while jumping (in jump tunnel)
	pub::SpaceObj::SetInvincible(iShip, true, true, 0);
	if (AP::SystemSwitchOutComplete(iShip, iClientID))
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
}

int __cdecl Dock_Call(unsigned int const &iShip, unsigned int const &base, int iCancel, enum DOCK_HOST_RESPONSE response)
{
	returncode = DEFAULT_RETURNCODE;

	uint iClientID = HkGetClientIDByShip(iShip);
	//AP::ClearClientInfo(iClientID);

	if (iClientID && (response == PROCEED_DOCK || response == DOCK) && iCancel != -1)
	{
		PlayerBase* pbase = GetPlayerBase(base);
		if (pbase)
		{
			if (mapArchs[pbase->basetype].isjump == 1)
			{
				//check if we have an ID restriction
				if (mapArchs[pbase->basetype].idrestriction == 1)
				{
					bool foundid = false;
					for (list<EquipDesc>::iterator item = Players[iClientID].equipDescList.equip.begin(); item != Players[iClientID].equipDescList.equip.end(); item++)
					{
						if (item->bMounted)
						{
							list<uint>::iterator iditer = mapArchs[pbase->basetype].allowedids.begin();
							while (iditer != mapArchs[pbase->basetype].allowedids.end())
							{
								if (*iditer == item->iArchID)
								{
									foundid = true;
									PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0120"));
									break;
								}
								iditer++;
							}
						}
					}
					if (foundid == false)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0121"));
						pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
						returncode = SKIPPLUGINS_NOFUNCTIONCALL;
						return 0;
					}
				}

				//check if we have a shipclass restriction
				if (mapArchs[pbase->basetype].shipclassrestriction == 1)
				{
					bool foundclass = false;
					// get the player ship class
					Archetype::Ship* TheShipArch = Archetype::GetShip(Players[iClientID].iShipArchetype);
					uint shipclass = TheShipArch->iShipClass;

					list<uint>::iterator iditer = mapArchs[pbase->basetype].allowedshipclasses.begin();
					while (iditer != mapArchs[pbase->basetype].allowedshipclasses.end())
					{
						if (*iditer == shipclass)
						{
							foundclass = true;
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0122"));
							break;
						}
						iditer++;
					}

					if (foundclass == false)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0123"));
						pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
						returncode = SKIPPLUGINS_NOFUNCTIONCALL;
						return 0;
					}
				}

				Vector pos;
				Matrix ornt;

				pub::SpaceObj::GetLocation(iShip, pos, ornt);

				pos.x = pbase->destposition.x;
				pos.y = pbase->destposition.y;
				pos.z = pbase->destposition.z;

				const Universe::ISystem *iSys = Universe::get_system(pbase->destsystem);
				wstring wscSysName = HkGetWStringFromIDS(iSys->strid_name);

				//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0124"), wscSysName.c_str(), pos.x, pos.y, pos.z);
				AP::SwitchSystem(iClientID, pbase->destsystem, pos, ornt);
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				return 1;
			}

			// Shield is up, docking is not possible.
			if (pbase->shield_active_time)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0125"));
				pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				return 0;
			}

			if (!IsDockingAllowed(pbase, iClientID))
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0126"));
				pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				return 0;
			}

			SendBaseStatus(iClientID, pbase);
		}
	}
	return 0;
}

void __stdcall CharacterSelect(struct CHARACTER_ID const &cId, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// Sync base names for the 
	map<uint, PlayerBase*>::iterator base = player_bases.begin();
	for (; base != player_bases.end(); base++)
	{
		HkChangeIDSString(iClientID, base->second->solar_ids, base->second->basename);
	}
}

void __stdcall CharacterSelect_AFTER(struct CHARACTER_ID const &cId, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	if (set_plugin_debug > 1)
		ConPrint(L"CharacterSelect_AFTER client=%u player_base=%u\n", iClientID, clients[iClientID].player_base);

	// If this ship is in a player base is then set then docking ID to emulate
	// a landing.
	LoadDockState(iClientID);
	if (clients[iClientID].player_base)
	{
		if (set_plugin_debug > 1)
			ConPrint(L"CharacterSelect_AFTER[2] client=%u player_base=%u\n", iClientID, clients[iClientID].player_base);

		// If this base does not exist, dump the ship into space
		PlayerBase *base = GetPlayerBase(clients[iClientID].player_base);
		if (!base)
		{
			DeleteDockState(iClientID);
			SendResetMarketOverride(iClientID);
			ForceLaunch(iClientID);
		}
		// If the player file indicates that the ship is in a base but this isn't this
		// base then dump the ship into space.
		else if (Players[iClientID].iBaseID != base->proxy_base)
		{
			DeleteDockState(iClientID);
			SendResetMarketOverride(iClientID);
			ForceLaunch(iClientID);
		}
	}
}

void __stdcall BaseEnter(uint base, uint iClientID)
{
	if (set_plugin_debug > 1)
		ConPrint(L"BaseEnter base=%u client=%u player_base=%u last_player_base=%u\n", base, iClientID,
			clients[iClientID].player_base, clients[iClientID].last_player_base);

	returncode = DEFAULT_RETURNCODE;

	clients[iClientID].admin = false;
	clients[iClientID].viewshop = false;

	// If the last player base is set then we have not docked at a non player base yet.
	if (clients[iClientID].last_player_base)
	{
		clients[iClientID].player_base = clients[iClientID].last_player_base;
	}

	// If the player is registered as being in a player controlled base then 
	// send the economy update, player system update and save a file to indicate
	// that we're in the base->
	if (clients[iClientID].player_base)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (base)
		{
			// Reset the commodity list	and send a dummy entry if there are no
			// commodities in the market
			SaveDockState(iClientID);
			SendMarketGoodSync(base, iClientID);
			SendBaseStatus(iClientID, base);
			return;
		}
		else
		{
			// Force the ship to launch to space as the base has been destroyed
			DeleteDockState(iClientID);
			SendResetMarketOverride(iClientID);
			ForceLaunch(iClientID);
			return;
		}
	}

	DeleteDockState(iClientID);
	SendResetMarketOverride(iClientID);
}

void __stdcall BaseExit(uint base, uint iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	if (set_plugin_debug > 1)
		ConPrint(L"BaseExit base=%u client=%u player_base=%u\n", base, iClientID, clients[iClientID].player_base);

	// Reset iClientID state and save it retaining the last player base ID to deal with respawn.
	clients[iClientID].admin = false;
	clients[iClientID].viewshop = false;
	if (clients[iClientID].player_base)
	{
		if (set_plugin_debug)
			ConPrint(L"BaseExit base=%u client=%u player_base=%u\n", base, iClientID, clients[iClientID].player_base);

		clients[iClientID].last_player_base = clients[iClientID].player_base;
		clients[iClientID].player_base = 0;
		SaveDockState(iClientID);
	}
	else
	{
		DeleteDockState(iClientID);
	}

	// Clear the base market and text
	SendResetMarketOverride(iClientID);
	SendSetBaseInfoText2(iClientID, L"");

	//wstring base_status = L"<RDL><PUSH/>";
	//base_status += L"<TEXT>" + XMLText(base->name) + L", " + HkGetWStringFromIDS(sys->strid_name) +  L"</TEXT><PARA/><PARA/>";
}

void __stdcall RequestEvent(int iIsFormationRequest, unsigned int iShip, unsigned int iDockTarget, unsigned int p4, unsigned long p5, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	if (iClientID)
	{
		if (!iIsFormationRequest)
		{
			PlayerBase *base = GetPlayerBase(iDockTarget);
			if (base)
			{
				// Shield is up, docking is not possible.
				if (base->shield_active_time)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0125"));
					pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
					returncode = SKIPPLUGINS_NOFUNCTIONCALL;
					return;
				}

				if (!IsDockingAllowed(base, iClientID))
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0126"));
					pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("info_access_denied"));
					returncode = SKIPPLUGINS_NOFUNCTIONCALL;
					return;
				}
			}
		}
	}
}

/// The base the player is launching from.
PlayerBase* player_launch_base = 0;

/// If the ship is launching from a player base record this so that
/// override the launch location.
bool __stdcall LaunchPosHook(uint space_obj, struct CEqObj &p1, Vector &pos, Matrix &rot, int dock_mode)
{
	returncode = DEFAULT_RETURNCODE;
	if (player_launch_base)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		pos = player_launch_base->position;
		rot = player_launch_base->rotation;
		TranslateX(pos, rot, -750);
		if (set_plugin_debug)
			ConPrint(L"LaunchPosHook[1] space_obj=%u pos=%0.0f %0.0f %0.0f dock_mode=%u\n",
				space_obj, pos.x, pos.y, pos.z, dock_mode);
		player_launch_base = 0;
	}
	return true;
}

/// If the ship is launching from a player base record this so that
/// we will override the launch location.
void __stdcall PlayerLaunch(unsigned int ship, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	if (set_plugin_debug > 1)
		ConPrint(L"PlayerLaunch ship=%u client=%u\n", ship, iClientID);
	player_launch_base = GetPlayerBase(clients[iClientID].last_player_base);
}


void __stdcall PlayerLaunch_AFTER(unsigned int ship, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	SyncReputationForClientShip(ship, iClientID);
}

void __stdcall JumpInComplete(unsigned int system, unsigned int ship)
{
	returncode = DEFAULT_RETURNCODE;

	if (set_plugin_debug > 1)
		ConPrint(L"JumpInComplete system=%u ship=%u\n");

	uint iClientID = HkGetClientIDByShip(ship);
	if (iClientID)
	{
		SyncReputationForClientShip(ship, iClientID);
	}
}

bool lastTransactionBase = false;
uint lastTransactionArchID = 0;
int lastTransactionCount = 0;
uint lastTransactionClientID = 0;
void __stdcall GFGoodSell(struct SGFGoodSellInfo const &gsi, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	lastTransactionBase = false;

	// If the iClientID is in a player controlled base
	PlayerBase *base = GetPlayerBaseForClient(iClientID);
	if (base)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		if (base->market_items.find(gsi.iArchID) == base->market_items.end()
			&& !clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0129"));
			clients[iClientID].reverse_sell = true;
			return;
		}

		MARKET_ITEM &item = base->market_items[gsi.iArchID];

		uint count = gsi.iCount;
		int price = (int)item.price * count;

		// base money check //
		if (count > ULONG_MAX / item.price)
		{
			clients[iClientID].reverse_sell = true;
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0130"));

			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("nnv_anomaly_detected"));
			wstring wscMsgU = L"KITTY ALERT: Possible type 3 POB cheating by %name (Count = %count, Price = %price)\n";
			wscMsgU = ReplaceStr(wscMsgU, L"%name", wscCharname.c_str());
			wscMsgU = ReplaceStr(wscMsgU, L"%count", stows(itos(count)).c_str());
			wscMsgU = ReplaceStr(wscMsgU, L"%price", stows(itos((int)item.price)).c_str());

			ConPrint(wscMsgU);
			LogCheater(iClientID, wscMsgU);

			return;
		}

		if (price < 0)
		{
			clients[iClientID].reverse_sell = true;
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0130"));

			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("nnv_anomaly_detected"));
			wstring wscMsgU = L"KITTY ALERT: Possible type 4 POB cheating by %name (Count = %count, Price = %price)\n";
			wscMsgU = ReplaceStr(wscMsgU, L"%name", wscCharname.c_str());
			wscMsgU = ReplaceStr(wscMsgU, L"%count", stows(itos(count)).c_str());
			wscMsgU = ReplaceStr(wscMsgU, L"%price", stows(itos((int)item.price)).c_str());

			ConPrint(wscMsgU);
			LogCheater(iClientID, wscMsgU);

			return;
		}

		// If the base doesn't have sufficient cash to support this purchase
		// reduce the amount purchased and shift the cargo back to the ship.
		if (base->money < price)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0132"));
			clients[iClientID].reverse_sell = true;
			return;
		}

		if ((item.quantity + count) > item.max_stock)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0133"));
			clients[iClientID].reverse_sell = true;
			return;
		}

		// Предотвратить получение игроком недействительного собственного капитала.
		float fValue;
		pub::Player::GetAssetValue(iClientID, fValue);

		int iCurrMoney;
		pub::Player::InspectCash(iClientID, iCurrMoney);

		long long lNewMoney = iCurrMoney;
		lNewMoney += price;

		if (fValue + price > 2100000000 || lNewMoney > 2100000000)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0134"));
			clients[iClientID].reverse_sell = true;
			return;
		}

		if (base->AddMarketGood(gsi.iArchID, gsi.iCount))
		{
			lastTransactionBase = true;
			lastTransactionArchID = gsi.iArchID;
			lastTransactionCount = gsi.iCount;
			lastTransactionClientID = iClientID;
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0129"));
			clients[iClientID].reverse_sell = true;
			return;
		}

		pub::Player::AdjustCash(iClientID, price);
		base->ChangeMoney(0 - price);
		base->Save();

		if (listCommodities.find(gsi.iArchID) != listCommodities.end())
		{
			string cname = wstos(listCommodities[gsi.iArchID]);
			string cbase = wstos(base->basename);

			Notify_Event_Commodity_Sold(iClientID, cname, gsi.iCount, cbase);
		}

		//build string and log the purchase
		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		const GoodInfo *gi = GoodList_get()->find_by_id(gsi.iArchID);
		string gname = wstos(HtmlEncode(HkGetWStringFromIDS(gi->iIDSName)));
		string msg = "Player " + wstos(charname) + " sold item " + gname + " x" + itos(count);
		Log::LogBaseAction(wstos(base->basename), msg.c_str());

		//Event plugin hooks
		if (HookExt::IniGetB(iClientID, "event.enabled") && (clients[iClientID].reverse_sell == false))
		{
			//HkMsgU(L"DEBUG: POB event enabled");
			if (base->basename == HookExt::IniGetWS(iClientID, "event.eventpob"))
			{
				//HkMsgU(L"DEBUG: event pob found");
				if (gsi.iArchID == HookExt::IniGetI(iClientID, "event.eventpobcommodity"))
				{
					//HkMsgU(L"DEBUG: POB event commodity found");
					//At this point, send the data to HookExt
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0136"));
					HookExt::AddPOBEventData(iClientID, wstos(HookExt::IniGetWS(iClientID, "event.eventid")), gsi.iCount);
				}
			}
		}


	}
}

void __stdcall ReqRemoveItem(unsigned short slot, int count, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	if (clients[iClientID].player_base && clients[iClientID].reverse_sell)
	{
		returncode = SKIPPLUGINS;
		int hold_size;
		HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(iClientID), clients[iClientID].cargo, hold_size);
	}
}

void __stdcall ReqRemoveItem_AFTER(unsigned short iID, int count, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	uint player_base = clients[iClientID].player_base;
	if (player_base)
	{
		if (clients[iClientID].reverse_sell)
		{
			returncode = SKIPPLUGINS;
			clients[iClientID].reverse_sell = false;

			foreach(clients[iClientID].cargo, CARGO_INFO, ci)
			{
				if (ci->iID == iID)
				{
					Server.ReqAddItem(ci->iArchID, ci->hardpoint.value, count, ci->fStatus, ci->bMounted, iClientID);
					return;
				}
			}
		}
		else
		{
			// Update the player CRC so that the player is not kicked for 'ship related' kick
			PlayerData *pd = &Players[iClientID];
			char *ACCalcCRC = (char*)hModServer + 0x6FAF0;
			__asm
			{
				pushad
				mov ecx, [pd]
				call[ACCalcCRC]
				mov ecx, [pd]
				mov[ecx + 320h], eax
				popad
			}
		}
	}
}

void __stdcall GFGoodBuy(struct SGFGoodBuyInfo const &gbi, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// If the iClientID is in a player controlled base
	PlayerBase *base = GetPlayerBaseForClient(iClientID);
	if (base)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		uint count = gbi.iCount;
		if (count > base->market_items[gbi.iGoodID].quantity)
			count = base->market_items[gbi.iGoodID].quantity;

		int price = (int)base->market_items[gbi.iGoodID].price * count;
		int curr_money;
		pub::Player::InspectCash(iClientID, curr_money);

		const wstring &charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		// In theory, these should never be called.
		if (count == 0 || ((base->market_items[gbi.iGoodID].min_stock > (base->market_items[gbi.iGoodID].quantity - count)) && !clients[iClientID].admin))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0137"));
			returncode = SKIPPLUGINS_NOFUNCTIONCALL;
			clients[iClientID].stop_buy = true;
			return;
		}
		else if (curr_money < price)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0138"));
			returncode = SKIPPLUGINS_NOFUNCTIONCALL;
			clients[iClientID].stop_buy = true;
			return;
		}

		if (((base->market_items[gbi.iGoodID].min_stock > (base->market_items[gbi.iGoodID].quantity - count)) && clients[iClientID].admin))
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0139"));

		clients[iClientID].stop_buy = false;
		base->RemoveMarketGood(gbi.iGoodID, count);
		pub::Player::AdjustCash(iClientID, 0 - price);
		base->ChangeMoney(price);
		base->Save();

		//build string and log the purchase
		const GoodInfo *gi = GoodList_get()->find_by_id(gbi.iGoodID);
		string gname = wstos(HtmlEncode(HkGetWStringFromIDS(gi->iIDSName)));
		string msg = "Player " + wstos(charname) + " purchased item " + gname + " x" + itos(count);
		Log::LogBaseAction(wstos(base->basename), msg.c_str());

		if (gi && gi->iType == GOODINFO_TYPE_SHIP)
		{
			returncode = SKIPPLUGINS;
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0140"));
		}
		else if (gi && gi->iType == GOODINFO_TYPE_HULL)
		{
			returncode = SKIPPLUGINS;
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0141"));
		}
	}
}

void __stdcall GFGoodBuy_AFTER(struct SGFGoodBuyInfo const &gbi, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// If the iClientID is in a player controlled base
	PlayerBase *base = GetPlayerBaseForClient(iClientID);
	if (base)
	{
		returncode = SKIPPLUGINS;
		// Update the player CRC so that the player is not kicked for 'ship related' kick
		PlayerData *pd = &Players[iClientID];
		char *ACCalcCRC = (char*)hModServer + 0x6FAF0;
		__asm
		{
			pushad
			mov ecx, [pd]
			call[ACCalcCRC]
			mov ecx, [pd]
			mov[ecx + 320h], eax
			popad
		}

		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0142"));
		//HkSaveChar((const wchar_t*)Players.GetActiveCharacterName(iClientID));
		//HkDelayedKick(iClientID, 10);

	}
}

void __stdcall ReqAddItem(unsigned int good, char const *hardpoint, int count, float fStatus, bool bMounted, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	PlayerBase *base = GetPlayerBaseForClient(iClientID);
	if (base)
	{
		returncode = SKIPPLUGINS;
		if (clients[iClientID].stop_buy)
		{
			if (clients[iClientID].stop_buy)
				clients[iClientID].stop_buy = false;
			returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		}
	}
}

void __stdcall ReqAddItem_AFTER(unsigned int good, char const *hardpoint, int count, float fStatus, bool bMounted, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// If the iClientID is in a player controlled base
	PlayerBase *base = GetPlayerBaseForClient(iClientID);
	if (base)
	{
		returncode = SKIPPLUGINS;
		PlayerData *pd = &Players[iClientID];

		// Add to check-list which is being compared to the users equip-list when saving
		// char to fix "Ship or Equipment not sold on base" kick
		EquipDesc ed;
		ed.sID = pd->LastEquipID;
		ed.iCount = 1;
		ed.iArchID = good;
		pd->lShadowEquipDescList.add_equipment_item(ed, false);

		// Update the player CRC so that the player is not kicked for 'ship related' kick
		char *ACCalcCRC = (char*)hModServer + 0x6FAF0;
		__asm
		{
			pushad
			mov ecx, [pd]
			call[ACCalcCRC]
			mov ecx, [pd]
			mov[ecx + 320h], eax
			popad
		}
	}
}

/// Ignore cash commands from the iClientID when we're in a player base.
void __stdcall ReqChangeCash(int cash, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	if (clients[iClientID].player_base)
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
}

/// Ignore cash commands from the iClientID when we're in a player base.
void __stdcall ReqSetCash(int cash, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	if (clients[iClientID].player_base)
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
}


void __stdcall ReqEquipment(class EquipDescList const &edl, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	if (clients[iClientID].player_base)
		returncode = SKIPPLUGINS;
}

void __stdcall CShip_destroy(CShip* ship)
{
	returncode = DEFAULT_RETURNCODE;

	// Dispatch the destroy event to the appropriate module.
	uint space_obj = ship->get_id();
	map<uint, Module*>::iterator i = spaceobj_modules.find(space_obj);
	if (i != spaceobj_modules.end())
	{
		returncode = SKIPPLUGINS;
		i->second->SpaceObjDestroyed(space_obj);
	}
}

void BaseDestroyed(uint space_obj, uint iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	map<uint, Module*>::iterator i = spaceobj_modules.find(space_obj);
	if (i != spaceobj_modules.end())
	{
		returncode = SKIPPLUGINS;
		i->second->SpaceObjDestroyed(space_obj);
	}
}

void __stdcall HkCb_AddDmgEntry(DamageList *dmg, unsigned short p1, float damage, enum DamageEntry::SubObjFate fate)
{
	returncode = DEFAULT_RETURNCODE;
	if (iDmgToSpaceID && dmg->get_inflictor_id())
	{
		float curr, max;
		pub::SpaceObj::GetHealth(iDmgToSpaceID, curr, max);

		map<uint, Module*>::iterator i = spaceobj_modules.find(iDmgToSpaceID);
		if ((i != spaceobj_modules.end()) && (i->second->mining == false))
		{
			if (set_plugin_debug)
				ConPrint(L"HkCb_AddDmgEntry iDmgToSpaceID=%u get_inflictor_id=%u curr=%0.2f max=%0.0f damage=%0.2f cause=%u is_player=%u player_id=%u fate=%u\n",
					iDmgToSpaceID, dmg->get_inflictor_id(), curr, max, damage, dmg->get_cause(), dmg->is_inflictor_a_player(), dmg->get_inflictor_owner_player(), fate);

			if (set_holiday_mode)
			{
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				iDmgToSpaceID = 0;
				return;
			}

			// A work around for an apparent bug where mines/missiles at the base
			// causes the base damage to jump down to 0 even if the base is
			// otherwise healthy.
			if (damage == 0.0f /*&& dmg->get_cause()==7*/ && curr > 200000)
			{
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				if (set_plugin_debug)
					ConPrint(L"HkCb_AddDmgEntry[1] - invalid damage?\n");
				return;
			}

			// If this is an NPC hit then suppress the call completely
			if (!dmg->is_inflictor_a_player())
			{
				if (set_plugin_debug)
					ConPrint(L"HkCb_AddDmgEntry[2] suppressed - npc\n");
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				iDmgToSpaceID = 0;
				return;
			}

			// Ask the combat magic plugin if we need to do anything differently
			COMBAT_DAMAGE_OVERRIDE_STRUCT info;
			info.iMunitionID = iDmgMunitionID;
			info.fDamageMultiplier = 0.0f;
			Plugin_Communication(COMBAT_DAMAGE_OVERRIDE, &info);

			if (info.fDamageMultiplier != 0.0f)
			{
				//ConPrint(L"base: Got a response back, info.fDamage = %0.0f\n", info.fDamage);
				//ConPrint(L"base: Got a response back, changing damage = %0.0f -> ", damage);
				damage = (curr - (curr - damage) * info.fDamageMultiplier);
				if (damage < 0.0f)
					damage = 0.0f;
				//ConPrint(L"%0.0f\n", damage);
			}

			// This call is for us, skip all plugins.		
			float new_damage = i->second->SpaceObjDamaged(iDmgToSpaceID, dmg->get_inflictor_id(), curr, damage);
			returncode = SKIPPLUGINS;

			if (new_damage == 0.0f)
			{
				new_damage = damage;
			}

			if (new_damage <= 0 && p1 == 1)
			{
				uint iType;
				pub::SpaceObj::GetType(iDmgToSpaceID, iType);
				uint iClientIDKiller = HkGetClientIDByShip(dmg->get_inflictor_id());
				if (set_plugin_debug)
					ConPrint(L"HkCb_AddDmgEntry[3]: iType is %u, iClientIDKiller is %u\n", iType, iClientIDKiller);
				if (iClientIDKiller && iType & (OBJ_DOCKING_RING | OBJ_STATION | OBJ_WEAPONS_PLATFORM))
					BaseDestroyed(iDmgToSpaceID, iClientIDKiller);
			}

			returncode = SKIPPLUGINS_NOFUNCTIONCALL;
			if (set_plugin_debug)
				ConPrint(L"HkCb_AddDmgEntry[4] suppressed - shield up - new_damage=%0.0f\n", new_damage);
			dmg->add_damage_entry(p1, new_damage, fate);
			iDmgToSpaceID = 0;
			return;
		}
	}
}

static void ForcePlayerBaseDock(uint iClientID, PlayerBase *base)
{
	char system_nick[1024];
	pub::GetSystemNickname(system_nick, sizeof(system_nick), base->system);

	char proxy_base_nick[1024];
	sprintf(proxy_base_nick, "%s_proxy_base", system_nick);

	uint proxy_base_id = CreateID(proxy_base_nick);

	clients[iClientID].player_base = base->base;
	clients[iClientID].last_player_base = base->base;

	if (set_plugin_debug > 1)
		ConPrint(L"ForcePlayerBaseDock client=%u player_base=%u\n", iClientID, clients[iClientID].player_base);

	uint system;
	pub::Player::GetSystem(iClientID, system);

	pub::Player::ForceLand(iClientID, proxy_base_id);
	if (system != base->system)
	{
		Server.BaseEnter(proxy_base_id, iClientID);
		Server.BaseExit(proxy_base_id, iClientID);

		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring charfilename;
		HkGetCharFileName(charname, charfilename);
		charfilename += L".fl";
		CHARACTER_ID charid;
		strcpy(charid.szCharFilename, wstos(charname.substr(0, 14)).c_str());

		Server.CharacterSelect(charid, iClientID); \
	}
}

#define IS_CMD(a) !args.compare(L##a)

bool ExecuteCommandString_Callback(CCmds* cmd, const wstring &args)
{
	returncode = DEFAULT_RETURNCODE;
	/*if (args.find(L"dumpbases")==0)
	{
		Universe::ISystem *sys = Universe::GetFirstSystem();
		FILE* f = fopen("bases.txt", "w");
		while (sys)
		{
			fprintf(f, "[Base]\n");
			fprintf(f, "nickname = %s_proxy_base\n", sys->nickname);
			fprintf(f, "system = %s\n", sys->nickname);
			fprintf(f, "strid_name = 0\n");
			fprintf(f, "file=Universe\\Systems\\proxy_base->ini\n");
			fprintf(f, "BGCS_base_run_by=W02bF35\n\n");

			sys = Universe::GetNextSystem();
		}
		fclose(f);
	}
	if (args.find(L"makebases")==0)
	{
		struct Universe::ISystem *sys = Universe::GetFirstSystem();
		while (sys)
		{
			string path = string("..\\DATA\\UNIVERSE\\SYSTEMS\\") + string(sys->nickname) + "\\" + string(sys->nickname) + ".ini";
			FILE *file = fopen(path.c_str(), "a+");
			if (file)
			{
				ConPrint(L"doing path %s\n", stows(path).c_str());
				fprintf(file, "\n\n[Object]\n");
				fprintf(file, "nickname = %s_proxy_base\n", sys->nickname);
				fprintf(file, "dock_with = %s_proxy_base\n", sys->nickname);
				fprintf(file, "base = %s_proxy_base\n", sys->nickname);
				fprintf(file, "pos = 0, -100000, 0\n");
				fprintf(file, "archetype = invisible_base\n");
				fprintf(file, "behavior = NOTHING\n");
				fprintf(file, "visit = 128\n");
				fclose(file);
			}
			sys = Universe::GetNextSystem();
		}
		return true;
	}*/
	if (args.find(L"testrecipe") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		uint iClientID = HkGetClientIdFromCharname(cmd->GetAdminName());
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			cmd->Print(L"ERR Not in player base");
			return true;
		}

		uint recipe_name = CreateID(wstos(cmd->ArgStr(1)).c_str());

		RECIPE recipe = recipes[recipe_name];
		for (map<uint, uint>::iterator i = recipe.consumed_items.begin(); i != recipe.consumed_items.end(); ++i)
		{
			base->market_items[i->first].quantity += i->second;
			SendMarketGoodUpdated(base, i->first, base->market_items[i->first]);
			cmd->Print(L"Added %ux %08x", i->second, i->first);
		}
		base->Save();
		cmd->Print(L"OK");
		return true;
	}
	else if (args.find(L"testdeploy") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		uint iClientID = HkGetClientIdFromCharname(cmd->GetAdminName());
		if (!iClientID)
		{
			cmd->Print(L"ERR Not in game");
			return true;
		}

		for (map<uint, uint>::iterator i = construction_items.begin(); i != construction_items.end(); ++i)
		{
			uint good = i->first;
			uint quantity = i->second;
			pub::Player::AddCargo(iClientID, good, quantity, 1.0, false);
		}

		cmd->Print(L"OK");
		return true;
	}
	else if (args.compare(L"beam") == 0)
	{
		returncode = DEFAULT_RETURNCODE;
		wstring charname = cmd->ArgCharname(1);
		wstring basename = cmd->ArgStrToEnd(2);

		// Fall back to default behaviour.
		if (cmd->rights != RIGHT_SUPERADMIN)
		{
			return false;
		}

		HKPLAYERINFO info;
		if (HkGetPlayerInfo(charname, info, false) != HKE_OK)
		{
			return false;
		}

		if (info.iShip == 0)
		{
			return false;
		}

		// Search for an match at the start of the name
		for (map<uint, PlayerBase*>::iterator i = player_bases.begin(); i != player_bases.end(); ++i)
		{
			if (ToLower(i->second->basename).find(ToLower(basename)) == 0)
			{
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				ForcePlayerBaseDock(info.iClientID, i->second);
				cmd->Print(L"OK");
				return true;
			}
		}

		// Exact match failed, try a for an partial match
		for (map<uint, PlayerBase*>::iterator i = player_bases.begin(); i != player_bases.end(); ++i)
		{
			if (ToLower(i->second->basename).find(ToLower(basename)) != -1)
			{
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
				ForcePlayerBaseDock(info.iClientID, i->second);
				cmd->Print(L"OK");
				return true;
			}
		}

		// Fall back to default flhook .beam command
		return false;
	}
	else if (args.find(L"basedestroy") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		uint iClientID = HkGetClientIdFromCharname(cmd->GetAdminName());

		//return SpaceObjDestroyed(space_obj);
		//alleynote1
		int billythecat = 0;
		PlayerBase *base;
		for (map<uint, PlayerBase*>::iterator i = player_bases.begin(); i != player_bases.end(); ++i)
		{
			if (i->second->basename == cmd->ArgStrToEnd(1))
			{
				base = i->second;
				billythecat = 1;
			}
		}


		if (billythecat == 0)
		{
			cmd->Print(L"ERR Base doesn't exist lmao");
			return true;
		}

		base->base_health = 0;
		if (base->base_health < 1)
		{
			return CoreModule(base).SpaceObjDestroyed(CoreModule(base).space_obj);
		}

		//cmd->Print(L"OK Base is gone are you proud of yourself.");
		return true;
	}
	else if (args.find(L"basetogglegod") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		uint iClientID = HkGetClientIdFromCharname(cmd->GetAdminName());
		bool optype = cmd->ArgInt(1);

		//return SpaceObjDestroyed(space_obj);
		//alleynote1
		int billythecat = 0;
		PlayerBase *base;
		for (map<uint, PlayerBase*>::iterator i = player_bases.begin(); i != player_bases.end(); ++i)
		{
			if (i->second->basename == cmd->ArgStrToEnd(2))
			{
				base = i->second;
				billythecat = 1;
				break;
			}
		}


		if (billythecat == 0)
		{
			cmd->Print(L"ERR Base doesn't exist lmao");
			return true;
		}


		if (optype == true)
		{
			base->invulnerable = true;
			cmd->Print(L"OK Base made invulnerable.");
		}
		else if (optype == false)
		{
			base->invulnerable = false;
			cmd->Print(L"OK Base made vulnerable.");
		}

		//cmd->Print(L"OK Base is gone are you proud of yourself.");
		return true;
	}
	else if (args.find(L"testbase") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		uint iClientID = HkGetClientIdFromCharname(cmd->GetAdminName());

		uint ship;
		pub::Player::GetShip(iClientID, ship);
		if (!ship)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0143"));
			return true;
		}

		int min = 100;
		int max = 5000;
		int randomsiegeint = min + (rand() % (int)(max - min + 1));

		string randomname = "TB";

		stringstream ss;
		ss << randomsiegeint;
		string str = ss.str();

		randomname.append(str);

		// Check for conflicting base name
		if (GetPlayerBase(CreateID(PlayerBase::CreateBaseNickname(randomname).c_str())))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0144"));
			return true;
		}

		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		AddLog("NOTICE: Base created %s by %s (%s)",
			randomname.c_str(),
			wstos(charname).c_str(),
			wstos(HkGetAccountID(HkGetAccountByCharname(charname))).c_str());

		wstring password = L"hastesucks";
		wstring basename = stows(randomname);

		PlayerBase *newbase = new PlayerBase(iClientID, password, basename);
		player_bases[newbase->base] = newbase;
		newbase->basetype = "legacy";
		newbase->basesolar = "legacy";
		newbase->baseloadout = "legacy";
		newbase->defense_mode = 1;

		for (map<string, ARCHTYPE_STRUCT>::iterator iter = mapArchs.begin(); iter != mapArchs.end(); iter++)
		{

			ARCHTYPE_STRUCT &thearch = iter->second;
			if (iter->first == newbase->basetype)
			{
				newbase->invulnerable = thearch.invulnerable;
				newbase->logic = thearch.logic;
			}
		}

		newbase->Spawn();
		newbase->Save();

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0145"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0146"), password.c_str());

		return true;
	}
	else if (args.find(L"jumpcreate") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		uint iClientID = HkGetClientIdFromCharname(cmd->GetAdminName());
		PlayerBase *base = GetPlayerBaseForClient(iClientID);

		uint ship;
		pub::Player::GetShip(iClientID, ship);
		if (!ship)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0143"));
			return true;
		}

		// If the ship is moving, abort the processing.
		Vector dir1;
		Vector dir2;
		pub::SpaceObj::GetMotion(ship, dir1, dir2);
		if (dir1.x > 5 || dir1.y > 5 || dir1.z > 5)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0148"));
			return true;
		}

		wstring archtype = cmd->ArgStr(1);
		if (!archtype.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0149"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0150"));
			return true;
		}
		wstring loadout = cmd->ArgStr(2);
		if (!loadout.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0151"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0150"));
			return true;
		}
		wstring type = cmd->ArgStr(3);
		if (!type.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0153"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0150"));
			return true;
		}
		wstring destsystem = cmd->ArgStr(4);
		if (!destsystem.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0155"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0150"));
			return true;
		}

		Vector destpos;
		destpos.x = cmd->ArgFloat(5);
		destpos.y = cmd->ArgFloat(6);
		destpos.z = cmd->ArgFloat(7);

		wstring theaffiliation = cmd->ArgStr(8);
		if (!theaffiliation.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0157"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0150"));
			return true;
		}


		wstring basename = cmd->ArgStrToEnd(9);
		if (!basename.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0159"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0150"));
			return true;
		}



		// Check for conflicting base name
		if (GetPlayerBase(CreateID(PlayerBase::CreateBaseNickname(wstos(basename)).c_str())))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0161"));
			return true;
		}

		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		AddLog("NOTICE: Base created %s by %s (%s)",
			wstos(basename).c_str(),
			wstos(charname).c_str(),
			wstos(HkGetAccountID(HkGetAccountByCharname(charname))).c_str());

		wstring password = L"nopassword";

		PlayerBase *newbase = new PlayerBase(iClientID, password, basename);
		player_bases[newbase->base] = newbase;
		newbase->affiliation = CreateID(wstos(theaffiliation).c_str());
		newbase->basetype = wstos(type);
		newbase->basesolar = wstos(archtype);
		newbase->baseloadout = wstos(loadout);
		newbase->defense_mode = 4;
		newbase->base_health = 10000000000;

		newbase->destsystem = CreateID(wstos(destsystem).c_str());
		newbase->destposition = destpos;

		for (map<string, ARCHTYPE_STRUCT>::iterator iter = mapArchs.begin(); iter != mapArchs.end(); iter++)
		{

			ARCHTYPE_STRUCT &thearch = iter->second;
			if (iter->first == newbase->basetype)
			{
				newbase->invulnerable = thearch.invulnerable;
				newbase->logic = thearch.logic;
				newbase->radius = thearch.radius;
			}
		}

		newbase->Spawn();
		newbase->Save();

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0162"));
		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0146"), password.c_str());
		return true;
	}
	else if (args.find(L"basecreate") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		uint iClientID = HkGetClientIdFromCharname(cmd->GetAdminName());
		PlayerBase *base = GetPlayerBaseForClient(iClientID);

		uint ship;
		pub::Player::GetShip(iClientID, ship);
		if (!ship)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0143"));
			return true;
		}

		// If the ship is moving, abort the processing.
		Vector dir1;
		Vector dir2;
		pub::SpaceObj::GetMotion(ship, dir1, dir2);
		if (dir1.x > 5 || dir1.y > 5 || dir1.z > 5)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0148"));
			return true;
		}

		wstring password = cmd->ArgStr(1);
		if (!password.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0166"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0167"));
			return true;
		}
		wstring archtype = cmd->ArgStr(2);
		if (!archtype.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0149"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0167"));
			return true;
		}
		wstring loadout = cmd->ArgStr(3);
		if (!loadout.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0151"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0167"));
			return true;
		}
		wstring type = cmd->ArgStr(4);
		if (!type.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0153"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0167"));
			return true;
		}
		uint theaffiliation = cmd->ArgInt(5);

		wstring basename = cmd->ArgStrToEnd(6);
		if (!basename.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0174"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0167"));
			return true;
		}



		// Check for conflicting base name
		if (GetPlayerBase(CreateID(PlayerBase::CreateBaseNickname(wstos(basename)).c_str())))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0161"));
			return true;
		}

		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		AddLog("NOTICE: Base created %s by %s (%s)",
			wstos(basename).c_str(),
			wstos(charname).c_str(),
			wstos(HkGetAccountID(HkGetAccountByCharname(charname))).c_str());

		PlayerBase *newbase = new PlayerBase(iClientID, password, basename);
		player_bases[newbase->base] = newbase;
		newbase->affiliation = theaffiliation;
		newbase->basetype = wstos(type);
		newbase->basesolar = wstos(archtype);
		newbase->baseloadout = wstos(loadout);
		newbase->defense_mode = 2;
		newbase->base_health = 10000000000;

		for (map<string, ARCHTYPE_STRUCT>::iterator iter = mapArchs.begin(); iter != mapArchs.end(); iter++)
		{

			ARCHTYPE_STRUCT &thearch = iter->second;
			if (iter->first == newbase->basetype)
			{
				newbase->invulnerable = thearch.invulnerable;
				newbase->logic = thearch.logic;
			}
		}

		newbase->Spawn();
		newbase->Save();

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0177"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0146"), password.c_str());
		return true;
	}
	else if (args.find(L"basedebugon") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		set_plugin_debug = 1;
		cmd->Print(L"OK base debug is on, sure hope you know what you're doing here.\n");
		return true;
	}
	else if (args.find(L"basedebugoff") == 0)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		set_plugin_debug = 0;
		cmd->Print(L"OK base debug is off.\n");
		return true;
	}

	return false;
}


void Plugin_Communication_CallBack(PLUGIN_MESSAGE msg, void* data)
{
	returncode = DEFAULT_RETURNCODE;

	if (msg == CUSTOM_BASE_BEAM)
	{
		CUSTOM_BASE_BEAM_STRUCT* info = reinterpret_cast<CUSTOM_BASE_BEAM_STRUCT*>(data); \
			PlayerBase *base = GetPlayerBase(info->iTargetBaseID);
		if (base)
		{
			returncode = SKIPPLUGINS;
			ForcePlayerBaseDock(info->iClientID, base);
			info->bBeamed = true;
		}
	}
	if (msg == CUSTOM_IS_IT_POB)
	{
		CUSTOM_BASE_IS_IT_POB_STRUCT* info = reinterpret_cast<CUSTOM_BASE_IS_IT_POB_STRUCT*>(data);
		PlayerBase *base = GetPlayerBase(info->iBase);
		returncode = SKIPPLUGINS;
		if (base)
		{
			info->bAnswer = true;
		}
	}
	else if (msg == CUSTOM_BASE_IS_DOCKED)
	{
		CUSTOM_BASE_IS_DOCKED_STRUCT* info = reinterpret_cast<CUSTOM_BASE_IS_DOCKED_STRUCT*>(data);
		PlayerBase *base = GetPlayerBaseForClient(info->iClientID);
		if (base)
		{
			returncode = SKIPPLUGINS;
			info->iDockedBaseID = base->base;
		}
	}
	else if (msg == CUSTOM_BASE_LAST_DOCKED)
	{
		CUSTOM_BASE_LAST_DOCKED_STRUCT* info = reinterpret_cast<CUSTOM_BASE_LAST_DOCKED_STRUCT*>(data);
		PlayerBase *base = GetLastPlayerBaseForClient(info->iClientID);
		if (base)
		{
			returncode = SKIPPLUGINS;
			info->iLastDockedBaseID = base->base;
		}
	}
	else if (msg == CUSTOM_JUMP)
	{
		CUSTOM_JUMP_STRUCT* info = reinterpret_cast<CUSTOM_JUMP_STRUCT*>(data);
		uint iClientID = HkGetClientIDByShip(info->iShipID);
		SyncReputationForClientShip(info->iShipID, iClientID);
	}
	else if (msg == CUSTOM_REVERSE_TRANSACTION)
	{
		if (lastTransactionBase)
		{
			CUSTOM_REVERSE_TRANSACTION_STRUCT* info = reinterpret_cast<CUSTOM_REVERSE_TRANSACTION_STRUCT*>(data);
			if (info->iClientID != lastTransactionClientID) {
				ConPrint(L"base: CUSTOM_REVERSE_TRANSACTION: Something is very wrong! Expected client ID %d but got %d\n", lastTransactionClientID, info->iClientID);
				return;
			}
			PlayerBase *base = GetPlayerBaseForClient(info->iClientID);

			MARKET_ITEM &item = base->market_items[lastTransactionArchID];
			int price = (int)item.price * lastTransactionCount;

			base->RemoveMarketGood(lastTransactionArchID, lastTransactionCount);

			pub::Player::AdjustCash(info->iClientID, -price);
			base->ChangeMoney(price);
			base->Save();
		}
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Functions to hook */
EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "Base Plugin by cannon";
	p_PI->sShortName = "base";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ClearClientInfo, PLUGIN_ClearClientInfo, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CharacterSelect, PLUGIN_HkIServerImpl_CharacterSelect, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&RequestEvent, PLUGIN_HkIServerImpl_RequestEvent, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LaunchPosHook, PLUGIN_LaunchPosHook, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&PlayerLaunch, PLUGIN_HkIServerImpl_PlayerLaunch, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&PlayerLaunch_AFTER, PLUGIN_HkIServerImpl_PlayerLaunch_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CharacterSelect_AFTER, PLUGIN_HkIServerImpl_CharacterSelect_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&JumpInComplete, PLUGIN_HkIServerImpl_JumpInComplete, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseEnter, PLUGIN_HkIServerImpl_BaseEnter, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseExit, PLUGIN_HkIServerImpl_BaseExit, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Dock_Call, PLUGIN_HkCb_Dock_Call, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&SystemSwitchOutComplete, PLUGIN_HkIServerImpl_SystemSwitchOutComplete, 0));


	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&GFGoodSell, PLUGIN_HkIServerImpl_GFGoodSell, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqRemoveItem, PLUGIN_HkIServerImpl_ReqRemoveItem, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqRemoveItem_AFTER, PLUGIN_HkIServerImpl_ReqRemoveItem_AFTER, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&GFGoodBuy, PLUGIN_HkIServerImpl_GFGoodBuy, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&GFGoodBuy_AFTER, PLUGIN_HkIServerImpl_GFGoodBuy_AFTER, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqAddItem, PLUGIN_HkIServerImpl_ReqAddItem, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqAddItem_AFTER, PLUGIN_HkIServerImpl_ReqAddItem_AFTER, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqChangeCash, PLUGIN_HkIServerImpl_ReqChangeCash, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqSetCash, PLUGIN_HkIServerImpl_ReqSetCash, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ReqEquipment, PLUGIN_HkIServerImpl_ReqEquipment, 11));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkTimerCheckKick, PLUGIN_HkTimerCheckKick, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ExecuteCommandString_Callback, PLUGIN_ExecuteCommandString_Callback, 0));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CShip_destroy, PLUGIN_HkIEngine_CShip_destroy, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseDestroyed, PLUGIN_BaseDestroyed, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkCb_AddDmgEntry, PLUGIN_HkCb_AddDmgEntry, 15));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Plugin_Communication_CallBack, PLUGIN_Plugin_Communication, 11));
	return p_PI;
}
