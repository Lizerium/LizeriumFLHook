/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 01 апреля 2026 13:06:06
 * Version: 1.0.13
 */

// AlleyPlugin for FLHookPlugin
// April 2015 by Alley
//
// 
// Это свободное программное обеспечение; вы можете распространять его и/или изменять
// по своему усмотрению без каких - либо ограничений.Если вы это сделаете, я был бы признателен
// за уведомление и/или упоминание где-либо.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <float.h>
#include <FLHook.h>
#include <plugin.h>
#include <list>
#include <set>

#include <PluginUtilities.h>
#include "PlayerRestrictions.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Сооружения и всякое такое дерьмо, йоу
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct scistruct
{
	list<uint> canmount;
	list<uint> nomount;
};

map <uint, wstring> shipclassnames;
map <uint, wstring> itemnames;
map <uint, scistruct> shipclassitems;
map <uint, wstring> owned;

map<uint, uint> mapIDs;

//мы храним их здесь, поскольку это более эффективно, 
//чем постоянно запрашивать идентификатор, с которым летит игрок.
struct pinfo
{
	uint playerid;
	float maxholdsize;
	uint shipclass;
};

map <uint, pinfo> clientplayerid;

struct iddockinfo
{
	int type;
	uint cargo;
	list<uint> systems;
	list<uint> shipclasses;
};

//первым uint будет хэш идентификатора
map <uint, iddockinfo> iddock;

map<uint, uint> player_last_base;

void SCI::LoadSettings()
{
	// Путь к файлу конфигурации.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\alley_shipclassitems.cfg";
	string idfile = "..\\data\\equipment\\misc_equip.ini";

	INI_Reader ini;
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("shipclasses"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("class"))
					{
						shipclassnames[ini.get_value_int(0)] = stows(ini.get_value_string(1));
					}
				}
			}
			else if (ini.is_header("itemnames"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("item"))
					{
						itemnames[CreateID(ini.get_value_string(0))] = stows(ini.get_value_string(1));
					}
				}
			}
			else if (ini.is_header("itemrestrict"))
			{
				scistruct sci;
				uint itemarchid;
				while (ini.read_value())
				{
					if (ini.is_value("item"))
					{
						itemarchid = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("canmount"))
					{
						sci.canmount.push_back(ini.get_value_int(0));
					}
					else if (ini.is_value("nomount"))
					{
						sci.nomount.push_back(CreateID(ini.get_value_string(0)));
					}
				}
				shipclassitems[itemarchid] = sci;
			}
			else if (ini.is_header("idrestrict"))
			{
				iddockinfo info;
				uint id;
				while (ini.read_value())
				{
					if (ini.is_value("name"))
					{
						id = CreateID(ini.get_value_string(0));
					}
					else if (ini.is_value("type"))
					{
						info.type = ini.get_value_int(0);
					}
					else if (ini.is_value("cargo"))
					{
						info.cargo = ini.get_value_int(0);
					}
					else if (ini.is_value("shipclass"))
					{
						info.shipclasses.push_back(ini.get_value_int(0));
					}
					else if (ini.is_value("system"))
					{
						info.systems.push_back(CreateID(ini.get_value_string(0)));
					}
				}
				iddock[id] = info;
			}
		}
		ini.close();
	}
	if (ini.open(idfile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("Tractor"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
					{
						mapIDs[CreateID(ini.get_value_string(0))] = 0;
					}
				}
			}
		}
		ini.close();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Зависимости
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SCI::CheckItems(unsigned int iClientID)
{
	//bool foundamountedid = false;
	//// Проверьте все предметы на корабле, посмотрите, не предназначены ли они для этого класса кораблей.
	//for (list<EquipDesc>::iterator item = Players[iClientID].equipDescList.equip.begin(); item != Players[iClientID].equipDescList.equip.end(); item++)
	//{
	//	//мы вставляем небольшой чек, чтобы проверить, установлен ли идентификатор
	//	if (mapIDs.find(item->iArchID) != mapIDs.end())
	//	{
	//		if (item->bMounted)
	//		{
	//			foundamountedid = true;
	//		}
	//	}

	//	if (shipclassitems.find(item->iArchID) != shipclassitems.end())
	//	{
	//		if (item->bMounted)
	//		{
	//			//более эффективно сначала выяснить, установлен ли нужный нам элемент, 
	//			//а затем, при необходимости, получить данные для сообщения об ошибке.
	//			for (map<uint, scistruct>::iterator iter = shipclassitems.begin(); iter != shipclassitems.end(); iter++)
	//			{
	//				if (iter->first == item->iArchID)
	//				{
	//					Archetype::Ship* TheShipArch = Archetype::GetShip(Players[iClientID].iShipArchetype);
	//					wstring classname = shipclassnames.find(TheShipArch->iShipClass)->second;
	//					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0076"), classname.c_str());
	//					bool foundclass = false;

	//					// выясните, соответствует ли класс корабля
	//					list<uint>::iterator iterclass = iter->second.canmount.begin();
	//					while (iterclass != iter->second.canmount.end())
	//					{
	//						if (*iterclass == TheShipArch->iShipClass)
	//						{
	//							foundclass = true;
	//							//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0077"), itemnames.find(iter->first)->second.c_str());
	//							break;
	//						}
	//						iterclass++;
	//					}

	//					if (foundclass == false)
	//					{
	//						//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0078"));
	//						wstring wscMsg = L"ОШИБКА: вы не можете отстыковаться с установленным % элементом. Этот элемент не может быть установлен на %shipclass.";
	//						wscMsg = ReplaceStr(wscMsg, L"%item", itemnames.find(iter->first)->second.c_str());
	//						wscMsg = ReplaceStr(wscMsg, L"%shipclass", classname.c_str());
	//						owned[iClientID] = wscMsg;
	//						StoreReturnPointForClient(iClientID);
	//						return;
	//					}
	//					// проверьте, нет ли предметов, которые нельзя монтировать
	//					else
	//					{
	//						for (list<EquipDesc>::iterator itemstack = Players[iClientID].equipDescList.equip.begin(); itemstack != Players[iClientID].equipDescList.equip.end(); itemstack++)
	//						{
	//							if (itemstack->bMounted)
	//							{
	//								bool founditernostack = false;
	//								// выясните, соответствует ли подключенный элемент списку, не относящемуся к стеку
	//								list<uint>::iterator iternostack = iter->second.nomount.begin();
	//								while (iternostack != iter->second.nomount.end())
	//								{
	//									if (*iternostack == itemstack->iArchID)
	//									{
	//										founditernostack = true;
	//										//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0079"), itemnames.find(itemstack->equip.iArchID)->second.c_str());
	//										break;
	//									}
	//									iternostack++;
	//								}

	//								if (founditernostack == true)
	//								{
	//									wstring wscMsg = L"ОШИБКА: вам не разрешено устанавливать %item и %second одновременно.";
	//									wscMsg = ReplaceStr(wscMsg, L"%item", itemnames.find(iter->first)->second.c_str());
	//									wscMsg = ReplaceStr(wscMsg, L"%second", itemnames.find(itemstack->iArchID)->second.c_str());
	//									owned[iClientID] = wscMsg;
	//									StoreReturnPointForClient(iClientID);
	//									return;
	//								}
	//							}
	//						}
	//					}
	//					//PrintUserCmdText(iClientID, wscMsg);
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}

	//if (foundamountedid == false)
	//{
	//	wstring wscMsg = L"ОШИБКА: у вас нет удостоверения личности на вашем корабле. Оно должно быть у вас.";
	//	owned[iClientID] = wscMsg;
	//	StoreReturnPointForClient(iClientID);
	//}
	return;
}

// на основе плагина conn
void SCI::StoreReturnPointForClient(unsigned int client)
{
	// Используем *ПОСЛЕДНЮЮ* базу игроков, а не текущую, потому что обработчик BaseExit базы
	// мог быть запущен раньше нас и уничтожил запись о привязке к POB раньше
	// нам пришлось запускать CheckItems и StoreReturnPointForClient
	uint base = GetCustomLastBaseForClient(client);
	// Он не привязан к пользовательской базе, проверьте, есть ли обычная база
	if (!base) {
		pub::Player::GetBase(client, base);
	}
	if (!base) {
		return;
	}

	player_last_base[client] = base;
}

// на основе плагина conn
uint SCI::GetCustomLastBaseForClient(unsigned int client)
{
	// Переходите к плагинам, если этот корабль пришвартован к пользовательской базе.
	CUSTOM_BASE_LAST_DOCKED_STRUCT info;
	info.iClientID = client;
	info.iLastDockedBaseID = 0;
	Plugin_Communication(CUSTOM_BASE_LAST_DOCKED, &info);
	return info.iLastDockedBaseID;
}

// на основе плагина conn
void SCI::MoveClient(unsigned int client, unsigned int targetBase)
{
	// Попросите другой плагин обработать этот луч.
	CUSTOM_BASE_BEAM_STRUCT info;
	info.iClientID = client;
	info.iTargetBaseID = targetBase;
	info.bBeamed = false;
	Plugin_Communication(CUSTOM_BASE_BEAM, &info);
	if (info.bBeamed)
		return;

	// Ни один плагин с этим не справился, сделайте это сами.
	pub::Player::ForceLand(client, targetBase); // луч
}

void SCI::CheckOwned(unsigned int iClientID)
{
	//end = not found
	if (owned.find(iClientID) == owned.end())
	{
		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0080"));
	}
	//else = found
	else
	{
		PrintUserCmdText(iClientID, owned.find(iClientID)->second.c_str());
		MoveClient(iClientID, player_last_base[iClientID]);
		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0081"));
		owned.erase(iClientID);
		player_last_base.erase(iClientID);
	}
}

void SCI::ClearClientInfo(uint iClientID)
{
	owned.erase(iClientID);
	clientplayerid.erase(iClientID);
}

void SCI::UpdatePlayerID(unsigned int iClientID)
{
	// Получите информацию о местоположении и списке груза.
	int iHoldSize;
	list<CARGO_INFO> lstCargo;
	HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(iClientID), lstCargo, iHoldSize);

	foreach(lstCargo, CARGO_INFO, i)
	{
		if (i->bMounted)
		{
			// это хорошее удостоверение личности
			for (map<uint, iddockinfo>::iterator iter = iddock.begin(); iter != iddock.end(); iter++)
			{
				if (iter->first == i->iArchID)
				{
					pinfo info;
					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0082"));
					info.playerid = i->iArchID;

					Archetype::Ship *ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					info.maxholdsize = ship->fHoldSize;
					info.shipclass = ship->iShipClass;

					clientplayerid[iClientID] = info;

					return;
				}
			}
		}
	}

	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0083"));
	return;
}

bool SCI::CanDock(uint iDockTarget, uint iClientID)
{
	//Сначала мы проверяем, есть ли игрок в нашем списке наблюдения.
	if (clientplayerid.find(iClientID) != clientplayerid.end())
	{
		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0084"));
		//временно скопируйте идентификатор, чтобы мы не отображали его на карте.
		uint id = clientplayerid[iClientID].playerid;
		uint currsystem = Players[iClientID].iSystemID;
		bool arewe = false;

		//Находимся ли мы в системе, о которой заботимся
		for (list<uint>::iterator iter = iddock[id].systems.begin(); iter != iddock[id].systems.end(); iter++)
		{
			if (*iter == currsystem)
			{
				arewe = true;
				//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0085"));
				break;
			}
		}

		if (arewe == true)
		{
			uint iTypeID;
			pub::SpaceObj::GetType(iDockTarget, iTypeID);

			if (iTypeID == OBJ_DOCKING_RING || iTypeID == OBJ_STATION)
			{
				//сначала мы проверяем ограничения на перевозку грузов, так как это должно устранить значительную их часть
				if (clientplayerid[iClientID].maxholdsize > iddock[id].cargo)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0086"));
					return false;
				}

				uint currshipclass = clientplayerid[iClientID].shipclass;
				for (list<uint>::iterator iter = iddock[id].shipclasses.begin(); iter != iddock[id].shipclasses.end(); iter++)
				{
					if (*iter == currshipclass)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0087"));
						return false;
					}
				}
			}
			else
			{
				return true;
			}
		}

		//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0088"));
	}

	return true;
}
