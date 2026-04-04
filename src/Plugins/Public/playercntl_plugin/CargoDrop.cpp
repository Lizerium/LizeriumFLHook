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
#include <random>

#include <PluginUtilities.h>
#include "Main.h"

namespace CargoDrop
{
	wstring set_wscDisconnectInSpaceMsg = L"%player пытается задействовать маскировочное устройство";

	/// Если правда, сообщите о отключении игроков.
	bool set_bReportDisconnectingPlayers = true;

	/// Если правда, убивайте игроков, которые отключаются во время взаимодействия с другими игроками.
	bool set_bKillDisconnectingPlayers = false;

	/// При истинном уроне и луте игроки, которые отключаются во время взаимодействия с другими игроками.
	bool set_bLootDisconnectingPlayers = true;

	/// Контейнер, в котором будет храниться добыча при сбросе с корабля.
	static int set_iLootCrateID = 0;

	/// Бросьте игроку биты корабля n штук x это количество.
	static float set_fHullFct = 0.1f;

	/// Элемент, представляющий биты корабля.
	static int set_iHullItem1ID = 0;

	/// Элемент, представляющий биты корабля.
	static int set_iHullItem2ID = 0;

	/// список предметов, которые не выпадают при разграблении корабля F1d.
	static map<uint, uint> set_mapNoLootItems;

	static float set_fDisconnectingPlayersRange = 5000.0f;

	class INFO
	{
	public:
		INFO() : bF1DisconnectProcessed(false), dLastTimestamp(0) {}
		bool bF1DisconnectProcessed;
		double dLastTimestamp;
		Vector vLastPosition;
		Quaternion vLastDir;
	};
	static map<uint, INFO> mapInfo;

	void CargoDrop::LoadSettings(const string &scPluginCfgFile)
	{
		set_bReportDisconnectingPlayers = IniGetB(scPluginCfgFile, "General", "ReportDisconnectingPlayers", true);
		set_bKillDisconnectingPlayers = IniGetB(scPluginCfgFile, "General", "KillDisconnectingPlayers", false);
		set_bLootDisconnectingPlayers = IniGetB(scPluginCfgFile, "General", "LootDisconnectingPlayers", false);
		set_fDisconnectingPlayersRange = IniGetF(scPluginCfgFile, "General", "DisconnectingPlayersRange", 5000.0f);

		set_fHullFct = ToFloat(stows(IniGetS(scPluginCfgFile, "General", "HullDropFactor", "0.1")));
		set_iLootCrateID = CreateID(IniGetS(scPluginCfgFile, "General", "CargoDropContainer", "lootcrate_ast_loot_metal").c_str());
		set_iHullItem1ID = CreateID(IniGetS(scPluginCfgFile, "General", "HullDrop1NickName", "commodity_super_alloys").c_str());
		set_iHullItem2ID = CreateID(IniGetS(scPluginCfgFile, "General", "HullDrop2NickName", "commodity_engine_components").c_str());
		set_wscDisconnectInSpaceMsg = stows(IniGetS(scPluginCfgFile, "General", "DisconnectMsg", "%player пытается задействовать маскировочное устройство"));

		// Прочтите список предметов без добычи (item-nick)
		list<INISECTIONVALUE> lstItems;
		IniGetSection(scPluginCfgFile, "NoLootItems", lstItems);
		set_mapNoLootItems.clear();
		foreach(lstItems, INISECTIONVALUE, iter)
		{
			uint itemID = CreateID(iter->scKey.c_str());
			set_mapNoLootItems[itemID] = itemID;
		}
	}

	void CargoDrop::Timer()
	{
		// Отключение при взаимодействии проверок.
		for (map<uint, INFO>::iterator iter = mapInfo.begin(); iter != mapInfo.end(); iter++)
		{
			int iClientID = iter->first;

			// Если выбран символ или недопустимый символ, ничего не делайте.
			if (!HkIsValidClientID(iClientID) || HkIsInCharSelectMenu(iClientID))
				continue;

			// Если не в космосе, ничего не делай
			uint iShip;
			pub::Player::GetShip(iClientID, iShip);
			if (!iShip)
				continue;

			if (ClientInfo[iClientID].tmF1Time || ClientInfo[iClientID].tmF1TimeDisconnect)
			{
				wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

				// Опустошите щиты кораблей.
				pub::SpaceObj::DrainShields(iShip);

				// Смоделируйте обновление obj, чтобы остановить корабль в космосе.
				SSPObjUpdateInfo ui;
				iter->second.dLastTimestamp += 1.0;
				ui.fTimestamp = iter->second.dLastTimestamp;
				ui.cState = 0;
				ui.throttle = 0;
				ui.vPos = iter->second.vLastPosition;
				ui.vDir = iter->second.vLastDir;
				Server.SPObjUpdate(ui, iClientID);

				if (!iter->second.bF1DisconnectProcessed)
				{
					iter->second.bF1DisconnectProcessed = true;

					// Отправляйте отчет об отключениях на все корабли в радиусе действия сканера.
					if (set_bReportDisconnectingPlayers)
					{
						wstring wscMsg = set_wscDisconnectInSpaceMsg;
						wscMsg = ReplaceStr(wscMsg, L"%time", GetTimeString(set_bLocalTime));
						wscMsg = ReplaceStr(wscMsg, L"%player", wscCharname);
						PrintLocalUserCmdText(iClientID, wscMsg, set_fDisconnectingPlayersRange);
					}

					// Сбросьте груз игроков.
					if (set_bLootDisconnectingPlayers && IsInRange(iClientID, set_fDisconnectingPlayersRange))
					{
						uint iSystem = 0;
						pub::Player::GetSystem(iClientID, iSystem);
						uint iShip = 0;
						pub::Player::GetShip(iClientID, iShip);
						Vector vLoc = { 0.0f, 0.0f, 0.0f };
						Matrix mRot = { 0.0f, 0.0f, 0.0f };
						pub::SpaceObj::GetLocation(iShip, vLoc, mRot);
						vLoc.x += 30.0;

						list<CARGO_INFO> lstCargo;
						int iRemainingHoldSize = 0;
						if (HkEnumCargo(wscCharname, lstCargo, iRemainingHoldSize) == HKE_OK)
						{
							foreach(lstCargo, CARGO_INFO, item)
							{
								if (!item->bMounted && set_mapNoLootItems.find(item->iArchID) == set_mapNoLootItems.end())
								{
									HkRemoveCargo(wscCharname, item->iID, item->iCount);
									Server.MineAsteroid(iSystem, vLoc, set_iLootCrateID, item->iArchID, item->iCount, iClientID);
								}
							}
						}
						HkSaveChar(wscCharname);
					}

					// Убейте, если другие корабли находятся в пределах досягаемости сканера.
					if (set_bKillDisconnectingPlayers && IsInRange(iClientID, set_fDisconnectingPlayersRange))
					{
						uint iShip = 0;
						pub::Player::GetShip(iClientID, iShip);
						pub::SpaceObj::SetRelativeHealth(iShip, 0.0f);
					}
				}
			}
		}
	}


	/// Крюк для разрушения корабля. Его легче зацепить, чем PlayerDeath.
	/// Сброс процента от груза + Немного лута, представляющего собой обломки корабля.
	void CargoDrop::SendDeathMsg(const wstring &wscMsg, uint iSystem, uint iClientIDVictim, uint iClientIDKiller)
	{
		// Если выпадение добычи с корабля игрока включено, проверьте наличие добычи.
		if (set_fHullFct == 0.0f)
		{
			//PrintUserCmdText(iClientIDVictim, GetLocalized(iClientID, "MSG_0804") + to_wstring(set_fHullFct));
			return;
		}

		list<CARGO_INFO> lstCargo;
		int iRemainingHoldSize;
		if (HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(iClientIDVictim), lstCargo, iRemainingHoldSize) != HKE_OK)
			return;

		int iShipSizeEst = iRemainingHoldSize;
		list<CARGO_INFO> mList; //mount
		list<CARGO_INFO> umList; //unmount

		foreach(lstCargo, CARGO_INFO, iter)
		{
			if (!(iter->bMounted))
			{
				umList.push_back(*iter);
				iShipSizeEst += iter->iCount;
			} 
			else
			{
				mList.push_back(*iter);
			}
		}

		int iHullDrop = (int)(set_fHullFct*(float)iShipSizeEst);
		if (iHullDrop > 0)
		{
			uint iShip;
			pub::Player::GetShip(iClientIDVictim, iShip);
			Vector myLocation;
			Matrix myRot;
			pub::SpaceObj::GetLocation(iShip, myLocation, myRot);
			myLocation.x += 30.0;

			// Создаем генератор случайных чисел
			std::random_device rd;
			std::mt19937 generator(rd());

			// Создаем распределение для случайных чисел от 1 до 3
			std::uniform_int_distribution<> distribution(1, 2);
			// Генерируем случайное число 
			int random_number = distribution(generator);
			std::uniform_int_distribution<> distributionMount(0, mList.size());
			std::uniform_int_distribution<> distributionUnMount(0, umList.size());
			std::uniform_int_distribution<> distributionOther(1, 4);
			int random_item_other_index = distributionOther(generator);

			if (set_iPluginDebug)
				ConPrint(L"NOTICE: player control cargo drop in system %08x at %f,%f,%f for ship size of iShipSizeEst=%d iHullDrop=%d\n", iSystem, myLocation.x, myLocation.y, myLocation.z, iShipSizeEst, iHullDrop);
			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientIDVictim);

			for (int i = 1; i <= random_number; i++)
			{
				int random_item_mount_index = distributionMount(generator);
				int random_item_unmount_index = distributionUnMount(generator);
				//PrintUserCmdText(iClientIDVictim, GetLocalized(iClientID, "MSG_0805") + to_wstring(random_item_mount_index) + L"|" + to_wstring(random_item_unmount_index));

				int m = 0;
				foreach(mList, CARGO_INFO, iter)
				{
					m++;
					if (m == random_item_mount_index)
					{
						//PrintUserCmdText(iClientIDVictim, GetLocalized(iClientID, "MSG_0806") + to_wstring(set_iLootCrateID) + L"|" + to_wstring(iter->iArchID) + L"|" + to_wstring(random_item_mount_index));
						Server.MineAsteroid(iSystem, myLocation, set_iLootCrateID, iter->iArchID, 1, iClientIDVictim);
						HkRemoveCargo(wscCharname, iter->iArchID, iter->iCount);
						break;
					}
				}

				m = 0;
				foreach(umList, CARGO_INFO, iter)
				{
					m++;
					if (m == random_item_mount_index)
					{
						//PrintUserCmdText(iClientIDVictim, GetLocalized(iClientID, "MSG_0806") + to_wstring(set_iLootCrateID) + L"|" + to_wstring(iter->iArchID) + L"|" + to_wstring(random_item_unmount_index));
						Server.MineAsteroid(iSystem, myLocation, set_iLootCrateID, iter->iArchID, iter->iCount, iClientIDVictim);
						HkRemoveCargo(wscCharname, iter->iArchID, iter->iCount);
						break;
					}
				}
				//PrintUserCmdText(iClientIDVictim, GetLocalized(iClientID, "MSG_0808") + to_wstring(i));
			}

			Server.MineAsteroid(iSystem, myLocation, set_iLootCrateID, set_iHullItem1ID, random_item_other_index, iClientIDVictim);
			Server.MineAsteroid(iSystem, myLocation, set_iLootCrateID, set_iHullItem2ID, random_item_other_index, iClientIDVictim);
		}
	}

	void CargoDrop::ClearClientInfo(unsigned int iClientID)
	{
		mapInfo.erase(iClientID);
	}

	void CargoDrop::SPObjUpdate(struct SSPObjUpdateInfo const &ui, unsigned int iClientID)
	{
		mapInfo[iClientID].dLastTimestamp = ui.fTimestamp;
		mapInfo[iClientID].vLastPosition = ui.vPos;
		mapInfo[iClientID].vLastDir = ui.vDir;
	}
}