/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 03:08:00
 * Version: 1.0.467
 */

// Player Control plugin for FLHookPlugin
// Feb 2010 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <float.h>
#include <FLHook.h>
#include <plugin.h>
#include <math.h>
#include <list>
#include <set>

#include <PluginUtilities.h>
#include "Main.h"

#include <FLCoreServer.h>
#include <FLCoreCommon.h>

namespace RepFixer
{

	struct FactionRep
	{
		// Репутационный никнейм группы для корректировки.
		string scRepGroup;

		// Режим регулировки. Если репутация игрока для scRepGroup выше, 
		// чем fRep, то сделайте репутацию равной fRep
		static const int MODE_REP_LESSTHAN = 0;

		// Режим регулировки. Если репутация игрока для scRepGroup меньше, 
		// чем fRep, то сделайте репутацию равной fRep
		static const int MODE_REP_GREATERTHAN = 1;

		// Ничего не меняйте/игнорируйте эту репутационную группу.
		static const int MODE_REP_NO_CHANGE = 2;

		// Закрепите группу представителей до этого уровня.
		static const int MODE_REP_STATIC = 3;

		// Режим регулировки.
		int iMode;

		// Репутационный лимит.
		float fRep;
	};

	struct TagHack
	{
		string scRepGroup;
		float fRep;
	};

	/// Сопоставление ID снаряжения фракций со списком репутации.
	static map<unsigned int, list<FactionRep>> set_mapFactionReps;

	/// Теги rephacks, (regex, связанные с rephacks)
	static map<wstring, list<TagHack>> set_mapTagHacks;

	/// Если истинные обновления регистрируются в flhook.log
	static bool set_bLogUpdates = false;

	/// Если true, то элемент ID должен быть смонтирован
	static bool set_bItemMustBeMounted = true;

	/// Если true, то делайте обновления.
	static bool set_bEnableRepFixUpdates = true;

	/// Загрузите репутацию для указанного никнейма идентификатора фракции снаряжения.
	static void LoadFactionReps(const string &scPluginCfgFile, const string &scIDNick)
	{
		uint archID = CreateID(scIDNick.c_str());

		list<FactionRep> lstFactionReps;

		list<INISECTIONVALUE> lstValues;
		IniGetSection(scPluginCfgFile, scIDNick, lstValues);
		foreach(lstValues, INISECTIONVALUE, var)
		{
			if (var->scValue.size() > 0)
			{
				FactionRep factionRep;
				factionRep.scRepGroup = var->scKey;

				factionRep.fRep = ToFloat(GetParam(stows(var->scValue), ',', 0));
				if (factionRep.fRep > 1.0f)
					factionRep.fRep = 1.0f;
				else if (factionRep.fRep < -1.0f)
					factionRep.fRep = -1.0f;

				factionRep.iMode = ToInt(GetParam(stows(var->scValue), ',', 1));
				if (factionRep.iMode == FactionRep::MODE_REP_LESSTHAN
					|| factionRep.iMode == FactionRep::MODE_REP_GREATERTHAN
					|| factionRep.iMode == FactionRep::MODE_REP_STATIC)
				{
					if (set_iPluginDebug > 0)
					{
						ConPrint(L"NOTICE: Add reputation %s/%s rep=%0.2f mode=%d\n",
							stows(scIDNick).c_str(), stows(var->scKey).c_str(), factionRep.fRep, factionRep.iMode);
					}
					lstFactionReps.push_back(factionRep);
				}
			}
		}

		set_mapFactionReps[archID] = lstFactionReps;
	}

	void LoadTagRephacks()
	{
		// The path to the configuration file.
		char szCurDir[MAX_PATH];
		GetCurrentDirectory(sizeof(szCurDir), szCurDir);
		string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\playercntl_tagrephacks.cfg";

		int iLoaded = 0;

		INI_Reader ini;
		if (ini.open(scPluginCfgFile.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header("tag"))
				{
					wstring tagname;
					list<TagHack> replist;

					while (ini.read_value())
					{
						if (ini.is_value("name"))
						{
							tagname = stows(ini.get_value_string());
						}
						else if (ini.is_value("rep"))
						{
							TagHack th;
							th.scRepGroup = ini.get_value_string(0);
							th.fRep = ini.get_value_float(1);
							replist.push_back(th);
						}
					}
					set_mapTagHacks[tagname] = replist;
					++iLoaded;
				}
			}
			ini.close();
		}

		ConPrint(L"Playercntl: Loaded %u tag rephacks\n", iLoaded);
	}

	/// Load the plugin settings.
	void RepFixer::LoadSettings(const string &scPluginCfgFile)
	{
		set_bEnableRepFixUpdates = IniGetB(scPluginCfgFile, "RepFixer", "EnableRepFixUpdates", false);
		set_bLogUpdates = IniGetB(scPluginCfgFile, "RepFixer", "LogRepFixUpdates", false);
		set_bItemMustBeMounted = IniGetB(scPluginCfgFile, "RepFixer", "ItemMustBeMounted", true);

		// Для каждого предмета снаряжения ID/License загрузите список репутации фракции.
		set_mapFactionReps.clear();
		list<INISECTIONVALUE> lstValues;
		IniGetSection(scPluginCfgFile, "RepFixerItems", lstValues);
		foreach(lstValues, INISECTIONVALUE, var)
			LoadFactionReps(scPluginCfgFile, var->scKey);

		LoadTagRephacks();
	}

	/// Для указанного идентификатора клиента проверьте 
	/// и сбросьте все фракции, репутация которых превышает допустимое значение.
	static void CheckReps(unsigned int iClientID)
	{
		wstring wscCharName = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		list<CARGO_INFO> lstCargo;
		int remainingHoldSize = 0;
		HkEnumCargo(wscCharName, lstCargo, remainingHoldSize);

		foreach(lstCargo, CARGO_INFO, cargo)
		{
			// Если элемент не смонтирован и мы проверяем только смонтированные элементы, то переходите к следующему.
			if (!cargo->bMounted && set_bItemMustBeMounted)
				continue;

			// Если элемент не является идентификатором, то перейдите к следующему.
			map<unsigned int, list<FactionRep> >::iterator iterIDs = set_mapFactionReps.find(cargo->iArchID);
			if (iterIDs == set_mapFactionReps.end())
				continue;

			// Элемент является идентификатором; Проверьте и скорректируйте репутацию игроков, если это необходимо.
			for (list<FactionRep>::iterator iterReps = iterIDs->second.begin(); iterReps != iterIDs->second.end(); iterReps++)
			{
				const FactionRep &rep = *iterReps;

				float fRep = 0.0f;
				HkGetRep(wscCharName, stows(rep.scRepGroup), fRep);
				if (((fRep > rep.fRep) && (rep.iMode == FactionRep::MODE_REP_LESSTHAN))
					|| ((fRep < rep.fRep) && (rep.iMode == FactionRep::MODE_REP_GREATERTHAN)))
				{
					if (set_bLogUpdates)
					{
						AddLog("NOTICE: Updating reputation %s from %0.2f to %0.2f on %s (%s)",
							rep.scRepGroup.c_str(), fRep, rep.fRep,
							wstos(wscCharName).c_str(),
							wstos(HkGetAccountID(HkGetAccountByCharname(wscCharName))).c_str());
					}
					HkSetRep(wscCharName, stows(rep.scRepGroup), rep.fRep);
				}
				else if ((fRep != rep.fRep) && (rep.iMode == FactionRep::MODE_REP_STATIC))
				{
					if (set_bLogUpdates)
					{
						AddLog("NOTICE: Updating reputation %s from %0.2f to %0.2f on %s (%s)",
							rep.scRepGroup.c_str(), fRep, rep.fRep,
							wstos(wscCharName).c_str(),
							wstos(HkGetAccountID(HkGetAccountByCharname(wscCharName))).c_str());
					}
					HkSetRep(wscCharName, stows(rep.scRepGroup), rep.fRep);
				}
			}

			// Мы скорректировали представителей, прекратили поиск по списку грузов.
			break;
		}

		//Репхаки на основе тегов
		for (map<wstring, list<TagHack>>::iterator tagReps = set_mapTagHacks.begin(); tagReps != set_mapTagHacks.end(); tagReps++)
		{
			if (wscCharName.find(tagReps->first) != string::npos)
			{
				//У нас матч, применяем повторения
				for each (TagHack tag in tagReps->second)
				{
					HkSetRep(wscCharName, stows(tag.scRepGroup), tag.fRep);
				}
				//HkMsgU(L"Примененные рехаки тегов");
				break;
			}
		}

		return;
	}

	void RepFixer::PlayerLaunch(unsigned int iShip, unsigned int iClientID)
	{
		if (set_bEnableRepFixUpdates)
			CheckReps(iClientID);
	}

	void RepFixer::BaseEnter(unsigned int iBaseID, unsigned int iClientID)
	{
		if (set_bEnableRepFixUpdates)
			CheckReps(iClientID);
	}
}