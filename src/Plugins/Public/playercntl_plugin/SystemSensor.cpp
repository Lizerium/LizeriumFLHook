/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 07 апреля 2026 10:57:40
 * Version: 1.0.22
 */

// Player Control plugin for FLHookPlugin
// Feb 2010 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.
//
// This file includes code that was not written by me but I can't find
// the original author (I know they posted on the-starport.net about it).

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
#include <algorithm>

#include <PluginUtilities.h>
#include "Main.h"

#include "Shlwapi.h"

#include <FLCoreServer.h>
#include <FLCoreCommon.h>

namespace SystemSensor
{
	enum MODE
	{
		MODE_OFF = 0x00,
		MODE_JUMPGATE = 0x01,
		MODE_TRADELANE = 0x02,
	};

	struct SENSOR
	{
		uint iSystemID;
		uint iEquipID;
		uint iNetworkID;
	};

	/// Карта оборудования и систем, имеющих сенсорные сети
	static multimap<unsigned int, SENSOR> set_mmapSensorEquip;
	static multimap<unsigned int, SENSOR> set_mmapSensorSystem;

	// Ограничение по идентификатору хакера
	uint systemsensor_hackersid;

	struct INFO
	{
		INFO() : iAvailableNetworkID(0), iLastScanNetworkID(0), bInJumpGate(false), iMode(MODE_OFF) {}
		uint iAvailableNetworkID;

		list<CARGO_INFO> lstLastScan;
		uint iLastScanNetworkID;

		bool bInJumpGate;

		uint iMode;
		uint HasRestrictedAccess;
	};

	static map<UINT, INFO> mapInfo;

	void LoadSettings(const string &scPluginCfgFile)
	{
		INI_Reader ini;
		if (ini.open(scPluginCfgFile.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header("SystemSensor"))
				{
					while (ini.read_value())
					{
						SENSOR sensor;
						sensor.iSystemID = CreateID(ini.get_name_ptr());
						sensor.iEquipID = CreateID(ini.get_value_string(0));
						sensor.iNetworkID = ini.get_value_int(1);
						set_mmapSensorEquip.insert(multimap<uint, SENSOR>::value_type(sensor.iEquipID, sensor));
						set_mmapSensorSystem.insert(multimap<uint, SENSOR>::value_type(sensor.iSystemID, sensor));
					}
				}
				if (ini.is_header("SystemSensorRestrictions"))
				{
					while (ini.read_value())
					{
						if (ini.is_value("Hackers"))
						{
							systemsensor_hackersid = CreateID(ini.get_value_string(0));
						}
					}
				}
			}
			ini.close();
		}
	}

	bool UserCmd_Net(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscMode = ToLower(GetParam(wscParam, ' ', 0));
		if (wscMode.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1192"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		if (!mapInfo[iClientID].iAvailableNetworkID)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1193"));
			mapInfo[iClientID].iMode = MODE_OFF;
			return true;
		}

		if (wscMode == L"all")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1194"));
			mapInfo[iClientID].iMode = MODE_JUMPGATE | MODE_TRADELANE;
		}
		else if (wscMode == L"jumponly")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1195"));
			mapInfo[iClientID].iMode = MODE_JUMPGATE;
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1196"));
			mapInfo[iClientID].iMode = MODE_OFF;
		}
		return true;
	}

	bool UserCmd_ShowScan(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		uint HasRestrictedAccess = 0;
		HasRestrictedAccess = mapInfo[iClientID].HasRestrictedAccess;

		if (HasRestrictedAccess)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1197"));
			return false;
		}
		else
		{
			wstring wscTargetCharname = GetParam(wscParam, ' ', 0);
			if (wscCmd.find(L"/showscan$ ") == 0 || wscCmd.find(L"/scanid ") == 0)
			{
				wchar_t *wszTargetCharname = (wchar_t*)Players.GetActiveCharacterName(ToInt(wscTargetCharname));
				if (wszTargetCharname)
					wscTargetCharname = wszTargetCharname;
			}

			if (wscTargetCharname.size() == 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1192"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}

			uint iTargetClientID = HkGetClientIDFromArg(wscTargetCharname);
			if (iTargetClientID == -1)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1199"));
				return true;
			}

			map<uint, INFO>::iterator iterTargetClientID = mapInfo.find(iTargetClientID);
			if (iterTargetClientID == mapInfo.end()
				|| !mapInfo[iClientID].iAvailableNetworkID
				|| !iterTargetClientID->second.iLastScanNetworkID
				|| mapInfo[iClientID].iAvailableNetworkID != iterTargetClientID->second.iLastScanNetworkID)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1200"));
				return true;
			}

			wstring wscEqList;
			foreach(iterTargetClientID->second.lstLastScan, CARGO_INFO, ci)
			{
				string scHardpoint = ci->hardpoint.value;
				if (scHardpoint.length())
				{
					Archetype::Equipment *eq = Archetype::GetEquipment(ci->iArchID);
					if (eq && eq->iIdsName)
					{
						wstring wscResult;
						switch (HkGetEqType(eq))
						{
							case ET_GUN:
							case ET_MISSILE:
							case ET_CD:
							case ET_CM:
							case ET_TORPEDO:
							case ET_OTHER:
								if (wscEqList.length())
									wscEqList += L",";
								wscResult = HkGetWStringFromIDS(eq->iIdsName);
								wscEqList += wscResult;
								break;
							default:
								break;
						}
					}
				}
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1201"), wscEqList.c_str());
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1202"));
			return true;
		}
	}

	void ClearClientInfo(uint iClientID)
	{
		mapInfo.erase(iClientID);
	}

	static void EnableSensorAccess(uint iClientID)
	{
		// Получение местоположения и списка грузов.
		int iHoldSize;
		list<CARGO_INFO> lstCargo;
		HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(iClientID), lstCargo, iHoldSize);

		unsigned int iSystemID;
		pub::Player::GetSystem(iClientID, iSystemID);

		// Если это судно имеет нужное оборудование и находится в нужной системе, то разрешите доступ.
		uint iAvailableNetworkID = 0;
		uint HasRestrictedAccess = 0;
		foreach(lstCargo, CARGO_INFO, i)
		{
			if (i->bMounted)
			{
				multimap<UINT, SENSOR>::iterator start = set_mmapSensorEquip.lower_bound(i->iArchID);
				multimap<UINT, SENSOR>::iterator end = set_mmapSensorEquip.upper_bound(i->iArchID);
				while (start != end)
				{
					if (start->second.iEquipID == systemsensor_hackersid)
					{
						HasRestrictedAccess = 2;
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1203"));
					}
					if (start->second.iSystemID == iSystemID)
					{
						iAvailableNetworkID = start->second.iNetworkID;
						break;
					}
					++start;
				}
			}
		}

		if (iAvailableNetworkID != mapInfo[iClientID].iAvailableNetworkID)
		{
			mapInfo[iClientID].iAvailableNetworkID = iAvailableNetworkID;
			mapInfo[iClientID].HasRestrictedAccess = HasRestrictedAccess;
			if (HasRestrictedAccess) {
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1204"));
			}
			else if (iAvailableNetworkID) {
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1205"));
			}
			else {
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1206"));
			}
		}
	}

	void PlayerLaunch(unsigned int iShip, unsigned int iClientID)
	{
		EnableSensorAccess(iClientID);
	}

	static void DumpSensorAccess(uint iClientID, const wstring &wscType, uint iType)
	{
		unsigned int iSystemID;
		pub::Player::GetSystem(iClientID, iSystemID);

		// Найдите сеть датчиков для этой системы.
		multimap<UINT, SENSOR>::iterator siter = set_mmapSensorSystem.lower_bound(iSystemID);
		multimap<UINT, SENSOR>::iterator send = set_mmapSensorSystem.upper_bound(iSystemID);
		if (siter == send)
			return;

		if (mapInfo.find(iClientID) == mapInfo.end())
		{
			SystemSensor::ClearClientInfo(iClientID);
		}

		// Фиксируйте груз судна.
		int iHoldSize;
		HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(iClientID), mapInfo[iClientID].lstLastScan, iHoldSize);
		mapInfo[iClientID].iLastScanNetworkID = siter->second.iNetworkID;

		// Оповещите всех игроков, подключенных к сенсорной сети, о том, что этот корабль находится
		map<UINT, INFO>::iterator piter = mapInfo.begin();
		map<UINT, INFO>::iterator pend = mapInfo.end();
		while (piter != pend)
		{
			if (piter->second.iAvailableNetworkID == siter->second.iNetworkID)
			{
				const Universe::ISystem *iSys = Universe::get_system(iSystemID);
				if (iSys)
				{
					if (piter->second.iMode & iType)
					{
						if (piter->second.HasRestrictedAccess)
						{
							wstring wscSysName = HkGetWStringFromIDS(iSys->strid_name);
							PrintUserCmdText(piter->first, GetLocalized(iClientID, "MSG_1207"),
								Players.GetActiveCharacterName(iClientID), iClientID,
								wscType.c_str(), wscSysName.c_str(), GetLocation(iClientID).c_str());
						}
						else
						{
							wstring wscSysName = HkGetWStringFromIDS(iSys->strid_name);
							PrintUserCmdText(piter->first, GetLocalized(iClientID, "MSG_1208"),
								Players.GetActiveCharacterName(iClientID), iClientID,
								wscType.c_str(), wscSysName.c_str(), GetLocation(iClientID).c_str());
						}
					}
				}
			}
			piter++;
		}
	}

	// Тип прыжка записи.
	void Dock_Call(unsigned int const &iShip, unsigned int const &iDockTarget, int iCancel, enum DOCK_HOST_RESPONSE response)
	{
		uint iClientID = HkGetClientIDByShip(iShip);
		if (iClientID && (response == PROCEED_DOCK || response == DOCK) && !iCancel)
		{
			uint iTypeID;
			pub::SpaceObj::GetType(iDockTarget, iTypeID);
			if (iTypeID == OBJ_JUMP_GATE)
			{
				mapInfo[iClientID].bInJumpGate = true;
			}
			else
			{
				mapInfo[iClientID].bInJumpGate = false;
			}
		}
	}


	void JumpInComplete(unsigned int iSystem, unsigned int iShip, unsigned int iClientID)
	{
		EnableSensorAccess(iClientID);
		if (mapInfo[iClientID].bInJumpGate)
		{
			mapInfo[iClientID].bInJumpGate = false;
			DumpSensorAccess(iClientID, L"exited jumpgate", MODE_JUMPGATE);
		}
	}

	void GoTradelane(unsigned int iClientID, struct XGoTradelane const &xgt)
	{
		DumpSensorAccess(iClientID, L"entered tradelane", MODE_TRADELANE);
	}

	void StopTradelane(unsigned int iClientID, unsigned int p1, unsigned int p2, unsigned int p3)
	{
		DumpSensorAccess(iClientID, L"exited tradelane", MODE_TRADELANE);
	}
}