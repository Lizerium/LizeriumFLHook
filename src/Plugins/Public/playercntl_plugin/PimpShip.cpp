/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 04 апреля 2026 13:00:07
 * Version: 1.0.19
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


IMPORT unsigned int  MakeLocationID(unsigned int, char const *);

namespace PimpShip
{
	// Стоимость за измененный товар.
	static int set_iCost = 0;

	// Список дилерских помещений
	static map<uint, wstring> set_mapDealers;

	// Единица оборудования для одного клиента.
	struct EQ_HARDPOINT
	{
		EQ_HARDPOINT() : sID(0), iArchID(0) {}

		uint sID;
		uint iArchID;
		wstring wscHardPoint;
	};

	// Список подключенных клиентов.
	struct INFO
	{
		INFO() : bInPimpDealer(false) {}

		// Карта ID опорной точки для оснащения.
		map<uint, EQ_HARDPOINT> mapCurrEquip;

		bool bInPimpDealer;
	};
	static map<uint, INFO> mapInfo;

	// Сопоставление идентификатора элемента с ИНФОРМАЦИЕЙ об объекте
	struct ITEM_INFO
	{
		ITEM_INFO() : iArchID(0) {}

		uint iArchID;
		wstring wscNickname;
		wstring wscDescription;
	};
	map<uint, ITEM_INFO> mapAvailableItems;

	bool IsItemArchIDAvailable(uint iArchID)
	{
		for (map<uint, ITEM_INFO>::iterator iter = mapAvailableItems.begin();
			iter != mapAvailableItems.end();
			iter++)
		{
			if (iter->second.iArchID == iArchID)
				return true;
		}
		return false;
	}

	wstring GetItemDescription(uint iArchID)
	{
		for (map<uint, ITEM_INFO>::iterator iter = mapAvailableItems.begin();
			iter != mapAvailableItems.end();
			iter++)
		{
			if (iter->second.iArchID == iArchID)
				return iter->second.wscDescription;
		}
		return L"";
	}

	void PimpShip::LoadSettings(const string &scPluginCfgFile)
	{
		set_iCost = 0;
		mapAvailableItems.clear();
		set_mapDealers.clear();

		// Patch BaseDataList::get_room_data to suppress annoying warnings flserver-errors.log
		unsigned char patch1[] = { 0x90, 0x90 };
		WriteProcMem((char*)0x62660F2, &patch1, 2);

		int iItemID = 1;
		INI_Reader ini;
		if (ini.open(scPluginCfgFile.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header("ShipPimper"))
				{
					while (ini.read_value())
					{
						if (ini.is_value("cost"))
						{
							set_iCost = ini.get_value_int(0);
						}
						else if (ini.is_value("equip"))
						{
							string nickname = ini.get_value_string(0);
							string description = ini.get_value_string(1);
							uint iArchID = CreateID(nickname.c_str());
							mapAvailableItems[iItemID].iArchID = iArchID;
							mapAvailableItems[iItemID].wscNickname = stows(nickname);
							mapAvailableItems[iItemID].wscDescription = stows(description);
							if (mapAvailableItems[iItemID].wscDescription.length() == 0)
								mapAvailableItems[iItemID].wscDescription = mapAvailableItems[iItemID].wscNickname;
							iItemID++;
						}
						else if (ini.is_value("room"))
						{
							string nickname = ini.get_value_string(0);
							uint iLocationID = CreateID(nickname.c_str());
							if (!BaseDataList_get()->get_room_data(iLocationID))
							{
								if (set_iPluginDebug > 0)
								{
									ConPrint(L"NOTICE: Room %s does not exist\n", stows(nickname).c_str());
								}
							}
							else
							{
								set_mapDealers[iLocationID] = stows(nickname);
							}
						}
					}
				}
			}
			ini.close();
		}

		// Unpatch BaseDataList::get_room_data to suppress annoying warnings flserver-errors.log
		unsigned char unpatch1[] = { 0xFF, 0x12 };
		WriteProcMem((char*)0x62660F2, &patch1, 2);
	}

	void BuildEquipmentList(uint iClientID)
	{
		// Составьте список оборудования.
		int iSlotID = 1;

		mapInfo[iClientID].mapCurrEquip.clear();
		list<EquipDesc> &eqLst = Players[iClientID].equipDescList.equip;
		for (list<EquipDesc>::iterator eq = eqLst.begin(); eq != eqLst.end(); eq++)
		{
			if (IsItemArchIDAvailable(eq->iArchID))
			{
				mapInfo[iClientID].mapCurrEquip[iSlotID].sID = eq->sID;
				mapInfo[iClientID].mapCurrEquip[iSlotID].iArchID = eq->iArchID;
				mapInfo[iClientID].mapCurrEquip[iSlotID].wscHardPoint = stows(eq->szHardPoint.value);
				iSlotID++;
			}
		}
	}

	// При входе в помещение проверьте, находитесь ли вы в действительном помещении дилера судна 
	// (или base, если ShipDealer не определен)
	// Если мы это делаем, то выводим вступительный текст, в противном случае ничего не делайте.
	void PimpShip::LocationEnter(unsigned int iLocationID, unsigned int iClientID)
	{
		if (!set_bEnablePimpShip)
			return;

		if (set_mapDealers.find(iLocationID) == set_mapDealers.end())
		{
			uint iBaseID = 0;
			pub::Player::GetBase(iClientID, iBaseID);
			if (set_mapDealers.find(iBaseID) == set_mapDealers.end())
			{
				mapInfo[iClientID].bInPimpDealer = false;
				mapInfo[iClientID].mapCurrEquip.clear();
				return;
			}
		}

		mapInfo[iClientID].bInPimpDealer = true;
		BuildEquipmentList(iClientID);

		// Вступительные сообщения при входе в комнату.
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1085"), GetLocalized(iClientID, "MSG_1580"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1086"), GetLocalized(iClientID, "MSG_1581"));
	}

	// Восстановите список оборудования, если клиент покупает новый корабль.
	void ReqShipArch_AFTER(unsigned int iArchID, unsigned int iClientID)
	{
		if (!set_bEnablePimpShip || !mapInfo[iClientID].bInPimpDealer)
			return;

		BuildEquipmentList(iClientID);
	}

	bool PimpShip::UserCmd_PimpShip(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bEnablePimpShip)
			return false;

		if (!mapInfo[iClientID].bInPimpDealer)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1087"));
			return true;
		}

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1088"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1089") + ToMoneyStr(set_iCost) + GetLocalized(iClientID, "MSG_1582"));

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1090"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1091"));

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1092"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1093"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1094"));

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1095"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1096"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1097"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1098"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1099"));

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1100"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1101"));

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1102"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1103"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1104"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1105"));

		return true;
	}

	/// Показать настройку корабля игроков.
	bool PimpShip::UserCmd_ShowSetup(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!mapInfo[iClientID].bInPimpDealer || !set_bEnablePimpShip)
			return false;

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1106"), mapInfo[iClientID].mapCurrEquip.size());
		for (map<uint, EQ_HARDPOINT>::iterator iter = mapInfo[iClientID].mapCurrEquip.begin();
			iter != mapInfo[iClientID].mapCurrEquip.end();
			iter++)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1107"),
				iter->first, iter->second.wscHardPoint.c_str(), GetItemDescription(iter->second.iArchID).c_str());
		}
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1108"));
		return true;
	}

	/// Отображение элементов, которые могут быть изменены.
	bool PimpShip::UserCmd_ShowItems(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!mapInfo[iClientID].bInPimpDealer || !set_bEnablePimpShip)
			return false;

		uint beginFrom = 1;
		uint endAt = mapAvailableItems.size();

		ushort index = 0;
		for (wstring::const_iterator it = wscParam.begin(); it != wscParam.end(); it++)
		{
			index++;

			if (*it == '-')
			{
				if (it == wscParam.begin() && it == wscParam.end() - 1)
				{
					beginFrom = 1;
					endAt = mapInfo[iClientID].mapCurrEquip.size();
				}
				else if (it == wscParam.end() - 1)
				{
					beginFrom = ToUInt(wscParam.substr(0, index));
					endAt = mapInfo[iClientID].mapCurrEquip.size();
				}
				else if (it == wscParam.begin())
				{
					beginFrom = 1;
					endAt = ToUInt(wscParam.substr(index));

					if (endAt < beginFrom)
						swap(endAt, beginFrom);
				}
				else
				{
					beginFrom = ToUInt(wscParam.substr(0, index));
					endAt = ToUInt(wscParam.substr(index));

					if (endAt < beginFrom)
						swap(endAt, beginFrom);
				}

				break;
			}
		}

		if (endAt > mapAvailableItems.size())
			endAt = mapAvailableItems.size();

		if (beginFrom == 0)
			beginFrom = 1;

		if (beginFrom > endAt)
			beginFrom = endAt + 1;

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1109"), endAt - beginFrom + 1, mapAvailableItems.size());
		for (int i = beginFrom; i != endAt + 1; i++)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1110"), i, mapAvailableItems[i].wscDescription.c_str());
		}
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1108"));

		return true;
	}

	/// Измените элемент в идентификаторе слота на указанный элемент.
	bool PimpShip::UserCmd_ChangeItem(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!mapInfo[iClientID].bInPimpDealer || !set_bEnablePimpShip)
			return false;

		if (mapInfo[iClientID].mapCurrEquip.empty())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1112"));
			return true;
		}

		uint beginFrom = 0;
		uint endAt = 0;
		uint everyN = 1;

		wstring firstArg = GetParam(wscParam, ' ', 0);
		wstring secondArg = GetParam(wscParam, ' ', 1);

		uint iSelectedItemID = ToUInt(secondArg);

		if (!iSelectedItemID)
		{
			for (map<uint, ITEM_INFO>::iterator it = mapAvailableItems.begin(); it != mapAvailableItems.end(); it++)
			{
				if (it->second.wscNickname == secondArg)
				{
					iSelectedItemID = it->first;
					break;
				}
			}
		}

		if (mapAvailableItems.find(iSelectedItemID) == mapAvailableItems.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1113"));
			return true;
		}

		ushort index = 0;
		for (wstring::iterator it = firstArg.begin(); it != firstArg.end(); it++)
		{
			index++;

			if (*it >= '0' && *it <= '9') {}
			else if (*it == '-')
			{
				if (it == firstArg.begin() && it == firstArg.end() - 1)
				{
					beginFrom = 1;
					endAt = mapInfo[iClientID].mapCurrEquip.size();
				}
				else if (it == firstArg.end() - 1)
				{
					beginFrom = ToUInt(firstArg.substr(0, index));
					endAt = mapInfo[iClientID].mapCurrEquip.size();
				}
				else if (it == firstArg.begin())
				{
					beginFrom = 1;
					endAt = ToUInt(firstArg.substr(index));

					if (endAt < beginFrom)
						swap(endAt, beginFrom);
				}
				else
				{
					beginFrom = ToUInt(firstArg.substr(0, index));
					endAt = ToUInt(firstArg.substr(index));

					if (endAt < beginFrom)
						swap(endAt, beginFrom);
				}

				break;
			}
			else if (*it == '*')
			{
				if (beginFrom == 0)
				{
					endAt = mapInfo[iClientID].mapCurrEquip.size();

					if (it == firstArg.begin())
					{
						beginFrom = 1;
						everyN = ToUInt(firstArg.substr(index));
					}
					else if (it == firstArg.end() - 1)
					{
						beginFrom = 1;
						everyN = ToUInt(firstArg.substr(0, index));
					}
					else
					{
						beginFrom = ToUInt(firstArg.substr(0, index));
						everyN = ToUInt(firstArg.substr(index));

						if (beginFrom == 0)
							break;
					}
				}
				else
				{
					if (it == firstArg.end() - 1)
						endAt = mapInfo[iClientID].mapCurrEquip.size();
					else
					{
						endAt = ToUInt(firstArg.substr(index));

						if (endAt < beginFrom)
							swap(endAt, beginFrom);
					}

					break;
				}
			}
			else
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1114"));
				return true;
			}
		}

		if (everyN == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1115"));
			return true;
		}

		if ((beginFrom == 0) && endAt == 0)
		{
			beginFrom = endAt = ToUInt(firstArg);
			if (beginFrom == 0 || beginFrom > mapInfo[iClientID].mapCurrEquip.size())
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1116"));
				return true;
			}
		}

		if (beginFrom == 0 || beginFrom > mapInfo[iClientID].mapCurrEquip.size())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1117"));
			if (everyN == 1)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1118"), endAt);
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1119"), endAt, iSelectedItemID);
			}
			else
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1120"), everyN, endAt);
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1121"), everyN, endAt, iSelectedItemID);
			}

			return true;
		}

		if (endAt > mapInfo[iClientID].mapCurrEquip.size())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1122"));
			if (everyN == 1)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1123"), beginFrom);
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1124"), beginFrom, iSelectedItemID);
			}
			else
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1125"), everyN, beginFrom);
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1126"), beginFrom, everyN, iSelectedItemID);
			}
			return true;
		}

		int count = 0;
		map<uint, EQ_HARDPOINT>& info = mapInfo[iClientID].mapCurrEquip;
		uint newItem = mapAvailableItems[iSelectedItemID].iArchID;
		for (uint i = beginFrom; i < endAt + 1; i += everyN)
		{
			if (info[i].iArchID != newItem)
				count++;
		}

		if (count == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1127"));
			return true;
		}

		int iCash = 0;
		wstring wscCharName = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		HkGetCash(wscCharName, iCash);

		if (iCash < count * set_iCost)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1128"));
			return true;
		}

		HkAddCash(wscCharName, -(count * set_iCost));

		list<EquipDesc> &equip = Players[iClientID].equipDescList.equip;
		for (list<EquipDesc>::iterator it = equip.begin(); it != equip.end(); it++)
		{
			for (uint i = beginFrom; i < endAt + 1; i += everyN)
			{
				if (it->sID == info[i].sID)
				{
					it->iArchID = newItem;
					info[i].iArchID = newItem;
					break;
				}
			}
		}

		HkSetEquip(iClientID, equip);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1129"), count, endAt == beginFrom ? L"" : L"s");

		if (beginFrom == 1 && endAt == mapInfo[iClientID].mapCurrEquip.size() && everyN == 1 && firstArg != L"-")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1130"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1131"), iSelectedItemID);
		}

		if (beginFrom == 1 && endAt == mapInfo[iClientID].mapCurrEquip.size() && everyN > 1 && *firstArg.begin() != '*' && *(firstArg.end() - 1) != '*')
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1132"), everyN);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1133"), everyN, iSelectedItemID);
		}

		pub::Audio::PlaySoundEffect(iClientID, CreateID("ui_execute_transaction"));

		return UserCmd_ShowSetup(iClientID, wscCmd, wscParam, usage);
	}
}
