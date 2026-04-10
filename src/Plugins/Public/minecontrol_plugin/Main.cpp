/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 10 апреля 2026 12:33:16
 * Version: 1.0.25
 */

/**
 Mining Plugin for FLHook-Plugin
 by Cannon.

0.1:
 Initial release
0.2:
 Используйте собственные расчеты полей астероидов.
0.3:
 При использовании loot-Cheat корабль игроков взрывается и регистрируется на flhook cheaters.log
0.4:
 Исправлены проблемы расчета зон.
 Добавлен бонус за полем
0.5:
 Исправлено исправление проблем с расчетом зоны.
 Добавлена модификация товара для полей
1.0:
 Отказался от собственных расчетов зон и вернулся к использованию расчетов FL.
 Изменены бонусы, теперь они работают только в том случае, если присутствуют все предметы экипировки.
 Изменен формат файла конфигурации, чтобы ускорить установку.
1.1:
 Исправлена проблема с инициализацией бонусов игрока.
 Добавлены сообщения об ошибках раздела playerbonus и исправлено надоедливое предупреждение в flserver-errors.log.
1.2:
 Изменена добытая добыча, теперь она попадает прямо в грузовой отсек. 
 Кроме того, добыча полезных ископаемых работает только в том случае, если по плавающей добыче попадает шахтерская пушка. 
 Обычное оружие не работает.
 Теперь система ведет исторический учет добытой руды на месторождениях. 
 Поля со временем перезаряжаются и истощаются по мере добычи.
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

static float set_fGenericFactor = 1.0f;
static int set_iPluginDebug = 0;
static string set_scStatsPath;

extern void PrintZones();

struct PLAYER_BONUS
{
	PLAYER_BONUS() : iLootID(0), fBonus(0.0f), iRep(-1) {}

	// Идентификатор добычи, к которому применяется эта конфигурация.
	uint iLootID;

	// Множитель бонуса добычи.
	float fBonus;

	// Принадлежность/репутация игрока
	uint iRep;

	// Список кораблей, на которые распространяется данный бонус
	list<uint> lstShips;

	// Перечень предметов оборудования, которые должен иметь корабль
	list<uint> lstItems;

	// Список идентификаторов боеприпасов для шахтерских орудий
	list<uint> lstAmmo;
};
multimap<uint, PLAYER_BONUS> set_mmapPlayerBonus;

struct ZONE_BONUS
{
	ZONE_BONUS() : fBonus(0.0f), iReplacementLootID(0), fRechargeRate(0), fCurrReserve(100000), fMaxReserve(50000), fMined(0) {}

	string scZone;

	/// <summary>
	/// Множитель бонуса добычи.
	/// </summary>
	float fBonus;

	/// <summary>
	/// Хэш предмета на замену выпавшего
	/// </summary>
	uint iReplacementLootID;

	/// <summary>
	/// Скорость перезарядки зоны. 
	/// Это количество единиц руды, добавляемых в запас за минуту.
	/// </summary>
	float fRechargeRate;

	/// <summary>
	/// Текущее количество руды в зоне. 
	/// Когда этот показатель становится низким, добывать руду становится труднее.
	/// Когда он достигает 0, добывать руду невозможно.
	/// </summary>
	float fCurrReserve;

	/// <summary>
	/// Максимальный лимит на количество руды на месторождении
	/// </summary>
	float fMaxReserve;

	/// <summary>
	/// Количество добытой руды.
	/// </summary>
	float fMined;
};
map<uint, ZONE_BONUS> set_mapZoneBonus;


struct CLIENT_DATA
{
	CLIENT_DATA() : bSetup(false), iDebug(0),
		iPendingMineAsteroidEvents(0), iMineAsteroidEvents(0) {}

	bool bSetup;
	map<uint, float> mapLootBonus;
	map<uint, list<uint> > mapLootAmmoLst;
	map<uint, list<uint> > mapLootShipLst;
	int iDebug;

	int iPendingMineAsteroidEvents;
	int iMineAsteroidEvents;
	time_t tmMineAsteroidSampleStart;

	uint LastTimeMessageAboutBeingFull;
};
map<uint, CLIENT_DATA> mapClients;



/** A return code to indicate to FLHook if we want the hook processing to continue. */
PLUGIN_RETURNCODE returncode;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Верните строковый параметр в позиции iPos из строки ini. 
/// Разделителем является символ ','. Кусок кода взят из Motahs Flak.
static string GetTrimParam(const string &scLine, uint iPos)
{
	string scOut = "";
	for (uint i = 0, j = 0; (i <= iPos) && (j < scLine.length()); j++)
	{
		if (scLine[j] == ',')
			i++;
		else if (i == iPos)
			scOut += scLine[j];
		else if (i > iPos)
			break;
	}

	while (scOut.size() && (scOut[0] == L' ' || scOut[0] == L'\t' || scOut[0] == L'\n' || scOut[0] == L'\r'))
	{
		scOut = scOut.substr(1);
	}
	while (scOut.size() && (scOut[scOut.size() - 1] == L' ' || scOut[scOut.size() - 1] == L'\t'
		|| scOut[scOut.size() - 1] == L'\n' || scOut[scOut.size() - 1] == L'\r'))
	{
		scOut = scOut.substr(0, scOut.length() - 1);
	}
	return scOut;
}

/// Возвращает true, если список грузов содержит указанный товар.
static bool ContainsEquipment(list<CARGO_INFO> &lstCargo, uint iArchID)
{
	foreach(lstCargo, CARGO_INFO, c)
		if (c->bMounted && c->iArchID == iArchID)
			return true;
	return false;
}

/// Верните фактор, чтобы изменить добычу добычи.
static float GetBonus(uint iRep, uint iShipID, list<CARGO_INFO> lstCargo, uint iLootID)
{
	if (!set_mmapPlayerBonus.size())
		return 0.0f;

	// Получите все бонусы игроков за этот товар.
	multimap<uint, PLAYER_BONUS>::iterator start = set_mmapPlayerBonus.lower_bound(iLootID);
	multimap<uint, PLAYER_BONUS>::iterator end = set_mmapPlayerBonus.upper_bound(iLootID);
	for (; start != end; start++)
	{
		// Проверьте соответствие репутации, если требуется репутация.
		if (start->second.iRep != -1 && iRep != start->second.iRep)
			continue;

		// Проверьте соответствие корабля.
		if (find(start->second.lstShips.begin(), start->second.lstShips.end(), iShipID) == start->second.lstShips.end())
			continue;

		// Убедитесь, что каждый простой элемент в списке оборудования присутствует и установлен.
		bool bEquipMatch = true;
		for (list<uint>::iterator item = start->second.lstItems.begin(); item != start->second.lstItems.end(); item++)
		{
			if (!ContainsEquipment(lstCargo, *item))
			{
				bEquipMatch = false;
				break;
			}
		}

		// Это матч.
		if (bEquipMatch)
			return start->second.fBonus;
	}

	return 0.0f;
}

void CheckClientSetup(uint iClientID)
{
	if (!mapClients[iClientID].bSetup)
	{
		if (set_iPluginDebug > 1)
			ConPrint(L"NOTICE: iClientID=%d setup bonuses\n", iClientID);
		mapClients[iClientID].bSetup = true;
		mapClients[iClientID].LastTimeMessageAboutBeingFull = 0;

		// Получить принадлежность игрока
		uint iRepGroupID = -1;
		IObjInspectImpl* oship = HkGetInspect(iClientID);
		if (oship)
			oship->get_affiliation(iRepGroupID);

		// Получить тип корабля
		uint iShipID;
		pub::Player::GetShipID(iClientID, iShipID);

		// Получите груз корабля, чтобы мы могли проверить документы, оружие и т. д.
		list<CARGO_INFO> lstCargo;
		int remainingHoldSize = 0;
		HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(iClientID), lstCargo, remainingHoldSize);
		if (set_iPluginDebug > 1)
		{
			ConPrint(L"NOTICE: iClientID=%d iRepGroupID=%u iShipID=%u lstCargo=", iClientID, iRepGroupID, iShipID);
			foreach(lstCargo, CARGO_INFO, ci)
			{
				ConPrint(L"%u ", ci->iArchID);
			}
			ConPrint(L"\n");
		}

		// Проверьте список бонусов игрока и проверьте, есть ли у этого игрока подходящий корабль и оборудование.
		// затем запишите бонус и типы оружия, которые можно использовать для сбора руды.
		mapClients[iClientID].mapLootBonus.clear();
		mapClients[iClientID].mapLootAmmoLst.clear();
		mapClients[iClientID].mapLootShipLst.clear();
		for (multimap<uint, PLAYER_BONUS>::iterator i = set_mmapPlayerBonus.begin(); i != set_mmapPlayerBonus.end(); i++)
		{
			uint iLootID = i->first;
			float fBonus = GetBonus(iRepGroupID, iShipID, lstCargo, iLootID);
			if (fBonus > 0.0f)
			{
				mapClients[iClientID].mapLootBonus[iLootID] = fBonus;
				mapClients[iClientID].mapLootAmmoLst[iLootID] = i->second.lstAmmo;
				mapClients[iClientID].mapLootShipLst[iLootID] = i->second.lstShips;
				if (set_iPluginDebug > 1)
				{
					ConPrint(L"NOTICE: iClientID=%d iLootID=%08x fBonus=%2.2f\n", iClientID, iLootID, fBonus);
				}
			}
		}

		wstring wscRights;
		HkGetAdmin((const wchar_t*)Players.GetActiveCharacterName(iClientID), wscRights);
		if (wscRights.size())
			mapClients[iClientID].iDebug = set_iPluginDebug;
	}
}

EXPORT void HkTimerCheckKick()
{
	returncode = DEFAULT_RETURNCODE;

	// Выполняйте 60-секундные задания.
	if ((time(0) % 60) == 0)
	{
		// Перезарядите поля
		for (map<uint, ZONE_BONUS>::iterator i = set_mapZoneBonus.begin(); i != set_mapZoneBonus.end(); i++)
		{
			i->second.fCurrReserve += i->second.fRechargeRate;
			if (i->second.fCurrReserve > i->second.fMaxReserve)
				i->second.fCurrReserve = i->second.fMaxReserve;
		}

		// Сохранение статуса зоны на диск
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scStatsPath = string(szDataPath) + "\\Accts\\MultiPlayer\\mining_stats.txt";
		FILE *file = fopen(scStatsPath.c_str(), "w");
		if (file)
		{
			fprintf(file, "[Zones]\n");
			for (map<uint, ZONE_BONUS>::iterator i = set_mapZoneBonus.begin(); i != set_mapZoneBonus.end(); i++)
			{
				if (i->second.scZone.size())
				{
					fprintf(file, "%s, %0.0f, %0.0f\n", i->second.scZone.c_str(), i->second.fCurrReserve, i->second.fMined);
				}
			}
			fclose(file);
		}
	}
}

/// Очистить информацию о клиенте при подключении клиента.
EXPORT void ClearClientInfo(uint iClientID)
{
	mapClients[iClientID].bSetup = false;
	mapClients[iClientID].mapLootBonus.clear();
	mapClients[iClientID].mapLootAmmoLst.clear();
	mapClients[iClientID].iDebug = 0;
	mapClients[iClientID].iPendingMineAsteroidEvents = 0;
	mapClients[iClientID].iMineAsteroidEvents = 0;
	mapClients[iClientID].tmMineAsteroidSampleStart = 0;
}

/// Load the configuration
EXPORT void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	// The path to the configuration file.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\minecontrol.cfg";

	// Load generic settings
	set_fGenericFactor = IniGetF(scPluginCfgFile, "MiningGeneral", "GenericFactor", 1.0);
	set_iPluginDebug = IniGetI(scPluginCfgFile, "MiningGeneral", "Debug", 0);
	set_scStatsPath = IniGetS(scPluginCfgFile, "MiningGeneral", "StatsPath", "");

	if (set_iPluginDebug)
		ConPrint(L"NOTICE: generic_factor=%0.0f debug=%d\n", set_fGenericFactor, set_iPluginDebug);

	// Patch Archetype::GetEquipment & Archetype::GetShip 
	// to suppress annoying warnings flserver-errors.log
	unsigned char patch1[] = { 0x90, 0x90 };
	WriteProcMem((char*)0x62F327E, &patch1, 2);
	WriteProcMem((char*)0x62F944E, &patch1, 2);
	WriteProcMem((char*)0x62F123E, &patch1, 2);

	// Загрузите список бонусов игрока и список бонусов поля.
	// Чтобы получить бонус за конкретный товар, игроку необходимо иметь
	// принадлежность (если это поле не пусто), один из кораблей и
	// все элементы оборудования.
	// [PlayerBonus] раздел имеет следующий формат:
	// Commodity, Bonus, Affiliation, List of ships, equipment or cargo separated by commas.
	// [FieldBonus] раздел имеет следующий формат:
	// Field, Bonus, Replacement Commodity
	set_mapZoneBonus.clear();
	set_mmapPlayerBonus.clear();
	INI_Reader ini;
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("PlayerBonus"))
			{
				while (ini.read_value())
				{
					string scLine = ini.get_name_ptr();

					PLAYER_BONUS pb;
					pb.iLootID = CreateID(GetTrimParam(scLine, 0).c_str());
					if (!Archetype::GetEquipment(pb.iLootID) && !Archetype::GetSimple(pb.iLootID))
					{
						ConPrint(L"ERROR: %s:%d: item '%s' not valid\n", stows(ini.get_file_name()).c_str(), ini.get_line_num(),
							stows(GetTrimParam(scLine, 0)).c_str());
						continue;
					}

					pb.fBonus = (float)atof(GetTrimParam(scLine, 1).c_str());
					if (pb.fBonus <= 0.0f)
					{
						ConPrint(L"ERROR: %s:%d: bonus not valid\n", stows(ini.get_file_name()).c_str(), ini.get_line_num());
						continue;
					}

					pb.iRep = -1;
					if (GetTrimParam(scLine, 2) != "*")
					{
						pub::Reputation::GetReputationGroup(pb.iRep, GetTrimParam(scLine, 2).c_str());
						if (pb.iRep == -1)
						{
							ConPrint(L"ERROR: %s:%d: reputation not valid\n", stows(ini.get_file_name()).c_str(), ini.get_line_num());
							continue;
						}
					}

					int i = 3;
					string scShipOrEquip = GetTrimParam(scLine, i++);
					while (scShipOrEquip.size())
					{
						uint iItemID = CreateID(scShipOrEquip.c_str());
						if (Archetype::GetShip(iItemID))
						{
							pb.lstShips.push_back(iItemID);
						}
						else if (Archetype::GetEquipment(iItemID))
						{
							Archetype::Equipment *eq = Archetype::GetEquipment(iItemID);
							if (eq->get_class_type() == Archetype::GUN)
							{
								Archetype::Gun* gun = (Archetype::Gun*) eq;
								if (gun->iProjectileArchID && gun->iProjectileArchID != 0xBAADF00D && gun->iProjectileArchID != 0x3E07E70)
								{
									pb.lstAmmo.push_back(gun->iProjectileArchID);
								}
							}
							else
							{
								pb.lstItems.push_back(iItemID);
							}
						}
						else if (Archetype::GetSimple(iItemID))
						{
							pb.lstItems.push_back(iItemID);
						}
						else
						{
							ConPrint(L"ERROR: %s:%d: item '%s' not valid\n", stows(ini.get_file_name()).c_str(), ini.get_line_num(), stows(scShipOrEquip).c_str());
						}

						scShipOrEquip = GetTrimParam(scLine, i++);
					}

					set_mmapPlayerBonus.insert(multimap<uint, PLAYER_BONUS>::value_type(pb.iLootID, pb));
					if (set_iPluginDebug)
					{
						ConPrint(L"NOTICE: mining player bonus %s(%u) %2.2f %s(%u)\n",
							stows(GetTrimParam(scLine, 0)).c_str(), pb.iLootID, pb.fBonus, stows(GetTrimParam(scLine, 2)).c_str(), pb.iRep);
					}
				}
			}
			else if (ini.is_header("ZoneBonus"))
			{
				while (ini.read_value())
				{
					string scLine = ini.get_name_ptr();

					string scZone = GetTrimParam(scLine, 0);
					if (!scZone.size())
					{
						ConPrint(L"ERROR: %s:%d: zone not valid\n", stows(ini.get_file_name()).c_str(), ini.get_line_num());
						continue;
					}

					float fBonus = (float)atof(GetTrimParam(scLine, 1).c_str());
					if (fBonus <= 0.0f)
					{
						ConPrint(L"ERROR: %s:%d: bonus not valid\n", stows(ini.get_file_name()).c_str(), ini.get_line_num());
						continue;
					}

					uint iReplacementLootID = 0;
					string scReplacementLoot = GetTrimParam(scLine, 2);
					if (scReplacementLoot.size())
					{
						iReplacementLootID = CreateID(scReplacementLoot.c_str());
					}

					//
					float fRechargeRate = 1000;
					//float fRechargeRate = (float)atof(GetTrimParam(scLine, 3).c_str());
					//if (fRechargeRate <= 0.0f)
					//{
					//	fRechargeRate = 50000;					
					//}

					float fMaxReserve = 100000;
					//float fMaxReserve = (float)atof(GetTrimParam(scLine, 4).c_str());
					//if (fMaxReserve <= 0.0f)
					//{
					//	fMaxReserve = 100000;					
					//}

					uint iZoneID = CreateID(scZone.c_str());
					set_mapZoneBonus[iZoneID].scZone = scZone;
					set_mapZoneBonus[iZoneID].fBonus = fBonus;
					set_mapZoneBonus[iZoneID].iReplacementLootID = iReplacementLootID;
					set_mapZoneBonus[iZoneID].fRechargeRate = fRechargeRate;
					set_mapZoneBonus[iZoneID].fMaxReserve = fMaxReserve;
					if (set_iPluginDebug)
					{
						ConPrint(L"NOTICE: zone bonus %s fBonus=%2.2f iReplacementLootID=%s(%u) fRechargeRate=%0.0f fMaxReserve=%0.0f\n",
							stows(scZone).c_str(), fBonus, stows(scReplacementLoot).c_str(), iReplacementLootID, fRechargeRate, fMaxReserve);
					}
				}
			}
		}
		ini.close();
	}

	// Read the last saved zone reserve.
	char szDataPath[MAX_PATH];
	GetUserDataPath(szDataPath);
	string scStatsPath = string(szDataPath) + "\\Accts\\MultiPlayer\\mining_stats.txt";
	if (ini.open(scStatsPath.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("Zones"))
			{
				while (ini.read_value())
				{
					string scLine = ini.get_name_ptr();
					string scZone = GetTrimParam(scLine, 0);
					float fCurrReserve = (float)atof(GetTrimParam(scLine, 1).c_str());
					float fMined = (float)atof(GetTrimParam(scLine, 2).c_str());
					uint iZoneID = CreateID(scZone.c_str());
					if (set_mapZoneBonus.find(iZoneID) != set_mapZoneBonus.end())
					{
						set_mapZoneBonus[iZoneID].fCurrReserve = fCurrReserve;
						set_mapZoneBonus[iZoneID].fMined = fMined;
					}
				}
			}
		}
		ini.close();
	}

	// Удалите патч теперь, когда мы закончили загрузку.
	unsigned char patch2[] = { 0xFF, 0x12 };
	WriteProcMem((char*)0x62F327E, &patch2, 2);
	WriteProcMem((char*)0x62F944E, &patch2, 2);
	WriteProcMem((char*)0x62F123E, &patch2, 2);

	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);
		ClearClientInfo(iClientID);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	srand((uint)time(0));
	// If we're being loaded from the command line while FLHook is running then
	// set_scCfgFile will not be empty so load the settings as FLHook only
	// calls load settings on FLHook startup and .rehash.
	if (fdwReason == DLL_PROCESS_ATTACH && set_scCfgFile.length() > 0)
		LoadSettings();
	return true;
}

void __stdcall PlayerLaunch(unsigned int iShip, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;
	ClearClientInfo(iClientID);
}

/// Called when a gun hits something
void __stdcall SPMunitionCollision(struct SSPMunitionCollisionInfo const & ci, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// Если это не камень, который можно добыть, не делайте никакой другой обработки.
	if (ci.dwTargetShip != 0)
		return;

	returncode = SKIPPLUGINS_NOFUNCTIONCALL;

	// Инициализируйте настройку майнинга для этого клиента, если это еще не было сделано.
	CheckClientSetup(iClientID);

	uint iShip;
	pub::Player::GetShip(iClientID, iShip);

	Vector vPos;
	Matrix mRot;
	pub::SpaceObj::GetLocation(iShip, vPos, mRot);

	uint iClientSystemID;
	pub::Player::GetSystem(iClientID, iClientSystemID);
	CmnAsteroid::CAsteroidSystem* csys = CmnAsteroid::Find(iClientSystemID);
	if (csys)
	{
		// Найдите поле астероидов, которое подходит лучше всего.
		for (CmnAsteroid::CAsteroidField* cfield = csys->FindFirst(); cfield; cfield = csys->FindNext())
		{
			try
			{
				const Universe::IZone *zone = cfield->get_lootable_zone(vPos);
				if (cfield->near_field(vPos) && zone && zone->lootableZone)
				{
					CLIENT_DATA &cd = mapClients[iClientID];

					// Если снимается не-камень, 
					// у нас не будет связанного с ним события по добыче полезных ископаемых, 
					// поэтому игнорируйте это.
					cd.iPendingMineAsteroidEvents--;
					if (cd.iPendingMineAsteroidEvents < 0)
					{
						cd.iPendingMineAsteroidEvents = 0;
						return;
					}

					// Настройте бонус в зависимости от зоны.
					float fZoneBonus = 0.25f;
					if (set_mapZoneBonus[zone->iZoneID].fBonus)
						fZoneBonus = set_mapZoneBonus[zone->iZoneID].fBonus;

					// Если месторождение заминировано, уменьшите бонус.
					//fZoneBonus *= set_mapZoneBonus[zone->iZoneID].fCurrReserve 
					//				/ set_mapZoneBonus[zone->iZoneID].fMaxReserve;
					uint iLootID = zone->lootableZone->dynamic_loot_commodity;
					uint iCrateID = zone->lootableZone->dynamic_loot_container;

					// При необходимости измените товар.
					if (set_mapZoneBonus[zone->iZoneID].iReplacementLootID)
						iLootID = set_mapZoneBonus[zone->iZoneID].iReplacementLootID;

					// Если запись о бонусе за майнинг для этого товара не найдена, отметьте отсутствие бонуса.
					map<uint, list<uint> >::iterator ammolst = cd.mapLootAmmoLst.find(iLootID);
					bool bNoMiningCombo = false;
					if (ammolst == cd.mapLootAmmoLst.end())
					{
						bNoMiningCombo = true;
						if (cd.iDebug)
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0768"));
					}
					// Если этот добываемый товар не был поражен оружием нужного типа, пометьте его как отсутствие бонуса.
					else if (find(ammolst->second.begin(), ammolst->second.end(), ci.iProjectileArchID) == ammolst->second.end())
					{
						bNoMiningCombo = true;
						if (cd.iDebug)
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0769"));
					}

					// Если при выстреле не использовалось шахтерское ружье, 
					// или персонаж не использует действительную комбинацию для добычи этого товара, 
					// установите бонус на *0,5. 
					float fPlayerBonus = 0.5f;
					if (bNoMiningCombo)
						fPlayerBonus = 0.5f;
					else
						fPlayerBonus = cd.mapLootBonus[iLootID];

					// Если на этот корабль нацелен другой корабль, отправьте руду в грузовой отсек другого корабля.
					uint iSendToClientID = iClientID;
					if (!bNoMiningCombo)
					{
						uint iTargetShip;
						pub::SpaceObj::GetTarget(iShip, iTargetShip);
						if (iTargetShip)
						{
							uint iTargetClientID = HkGetClientIDByShip(iTargetShip);
							if (iTargetClientID)
							{
								if (HkDistance3DByShip(iShip, iTargetShip) < 1000.0f)
								{
									iSendToClientID = iTargetClientID;
								}
							}
						}
					}

					// Посчитать количество выпавшей добычи
					float fRand = (float)rand() / (float)RAND_MAX;

					// Рассчитайте выпадение добычи и бросьте ее.
					int iLootCount = (int)(fRand * set_fGenericFactor * fZoneBonus * fPlayerBonus * zone->lootableZone->dynamic_loot_count2);

					// Удалите этот lootCount из поля.
					set_mapZoneBonus[zone->iZoneID].fCurrReserve -= iLootCount;
					set_mapZoneBonus[zone->iZoneID].fMined += iLootCount;
					if (set_mapZoneBonus[zone->iZoneID].fCurrReserve <= 0)
					{
						set_mapZoneBonus[zone->iZoneID].fCurrReserve = 0;
						//iLootCount = 0;
					}

					if (mapClients[iClientID].iDebug)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0770"),
							fRand, set_fGenericFactor, fPlayerBonus, fZoneBonus, iLootCount, iLootID, iCrateID, set_mapZoneBonus[zone->iZoneID].fCurrReserve);
					}

					mapClients[iClientID].iMineAsteroidEvents++;
					if (mapClients[iClientID].tmMineAsteroidSampleStart < time(0))
					{
						float average = mapClients[iClientID].iMineAsteroidEvents / 30.0f;
						if (average > 2.0f)
						{
							AddLog("NOTICE: high mining rate charname=%s rate=%0.1f/sec location=%0.0f,%0.0f,%0.0f system=%08x zone=%08x",
								wstos((const wchar_t*)Players.GetActiveCharacterName(iClientID)).c_str(),
								average, vPos.x, vPos.y, vPos.z, zone->iSystemID, zone->iZoneID);
						}

						mapClients[iClientID].tmMineAsteroidSampleStart = time(0) + 30;
						mapClients[iClientID].iMineAsteroidEvents = 0;
					}

					if (iLootCount)
					{
						float fHoldRemaining;
						pub::Player::GetRemainingHoldSize(iSendToClientID, fHoldRemaining);
						if (fHoldRemaining < iLootCount)
						{
							iLootCount = (int)fHoldRemaining;
						}
						if (iLootCount == 0)
						{
							if (((uint)time(0) - mapClients[iClientID].LastTimeMessageAboutBeingFull) > 1)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0771"), reinterpret_cast<const wchar_t*>(Players.GetActiveCharacterName(iSendToClientID)));
								pub::Player::SendNNMessage(iClientID, CreateID("insufficient_cargo_space"));
								if (iClientID != iSendToClientID)
								{
									PrintUserCmdText(iSendToClientID, GetLocalized(iClientID, "MSG_0772"));
									pub::Player::SendNNMessage(iSendToClientID, CreateID("insufficient_cargo_space"));
								}
								mapClients[iClientID].LastTimeMessageAboutBeingFull = (uint)time(0);
							}
							return;
						}
						pub::Player::AddCargo(iSendToClientID, iLootID, iLootCount, 1.0, false);
					}
					return;
				}
			}
			catch (...) {}
		}
	}
}

/// Вызывается при добыче астериода. Мы игнорируем все параметры клиента.
void __stdcall MineAsteroid(uint iClientSystemID, class Vector const &vPos, uint iCrateID, uint iLootID, uint iCount, uint iClientID)
{
	mapClients[iClientID].iPendingMineAsteroidEvents += 4;
	//	ConPrint(L"mine_asteroid %d %d %d\n", iCrateID, iLootID, iCount);
	returncode = SKIPPLUGINS_NOFUNCTIONCALL;
	return;
}

#define IS_CMD(a) !wscCmd.compare(L##a)

bool ExecuteCommandString_Callback(CCmds* cmd, const wstring &wscCmd)
{
	returncode = DEFAULT_RETURNCODE;

	if (IS_CMD("printminezones"))
	{
		returncode = NOFUNCTIONCALL;
		PrintZones();
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Functions to hook */
EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "Mine Control Plugin by cannon";
	p_PI->sShortName = "minecontrol";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ClearClientInfo, PLUGIN_ClearClientInfo, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&PlayerLaunch, PLUGIN_HkIServerImpl_PlayerLaunch, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&MineAsteroid, PLUGIN_HkIServerImpl_MineAsteroid, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&SPMunitionCollision, PLUGIN_HkIServerImpl_SPMunitionCollision, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkTimerCheckKick, PLUGIN_HkTimerCheckKick, 0));
	return p_PI;
}
