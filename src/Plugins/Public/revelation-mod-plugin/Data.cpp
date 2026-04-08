/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 08 апреля 2026 14:28:37
 * Version: 1.0.23
 */

#include "Data.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{

			map <wstring, DataItem::BaseData> Data::baseData;
			map <wstring, DataItem::SystemData> Data::systemData;
			map <wstring, DataItem::ShipData> Data::shipData;
			map <wstring, DataItem::FactionData> Data::factionData;
			map <wstring, DataItem::GoodsData> Data::goodsData;

			map <uint, DataItem::BaseData*> Data::baseDataIDRef;
			map <uint, DataItem::SystemData*> Data::systemDataIDRef;
			map <uint, DataItem::ShipData*> Data::shipDataIDRef;
			map <uint, DataItem::FactionData*> Data::factionDataIDRef;
			map <uint, DataItem::GoodsData*> Data::goodsDataIDRef;

			Data::Data()
			{
			}

			Data::~Data()
			{
			}

			void Data::PrintInfoOrErrorLog(string message)
			{
				FILE* Logfile = fopen("./flhook_logs/revellation.log", "at");
				string infoEx = message;
				fprintf(Logfile, infoEx.c_str());
				fclose(Logfile);
			}

			// Base
			void Data::addBase(DataItem::BaseData item)
			{
				wstring nick = item.getNickname();

				if (baseData.find(nick) != baseData.end()) 
				{
					Exception::RedeclaredNickname e;

					e.nickname(nick);

					PrintInfoOrErrorLog(wstos(nick) + " addBase\n");

					throw e;
				}

				baseData.insert(make_pair(nick, item));

				baseDataIDRef[item.getID()] = &baseData[nick];
			}

			DataItem::BaseData* Data::getBase(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (baseData.find(nickname) == baseData.end()) 
				{
					Exception::NickNameNotFound e;

					PrintInfoOrErrorLog(wstos(nickname) + " getBase\n");

					e.nickname(nickname);

					throw e;
				}

				return &baseData[nickname];
			}

			DataItem::BaseData* Data::getBase(uint id)
			{
				if (baseDataIDRef.find(id) == baseDataIDRef.end())
				{
					Exception::IDNotFound e;

					PrintInfoOrErrorLog(wstos(to_wstring(id)) + " id getBase\n");

					e.nickname(id);

					throw e;
				}

				return baseDataIDRef[id];
			}

			map <wstring, DataItem::BaseData> Data::getAllBases()
			{
				return baseData;
			}

			// System
			void Data::addSystem(DataItem::SystemData item)
			{
				wstring nick = item.getNickname();

				if (systemData.find(nick) != systemData.end()) 
				{
					Exception::RedeclaredNickname e;

					PrintInfoOrErrorLog(to_string(systemData.size()) + " - " + wstos(nick) + " addSystem\n");

					e.nickname(nick);

					throw e;
				}

				systemData.insert(make_pair(nick, item));

				systemDataIDRef[item.getID()] = &systemData[nick];
			}

			DataItem::SystemData* Data::getSystem(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (systemData.find(nickname) == systemData.end()) 
				{
					Exception::NickNameNotFound e;

					PrintInfoOrErrorLog(to_string(systemData.size()) + " - " + wstos(nickname) + " addSystem\n");

					e.nickname(nickname);

					throw e;
				}

				return &systemData[nickname];
			}

			DataItem::SystemData* Data::getSystem(uint id)
			{
				if (systemDataIDRef.find(id) == systemDataIDRef.end())
				{
					Exception::IDNotFound e;

					PrintInfoOrErrorLog(wstos(to_wstring(id)) + " getSystem\n");

					e.nickname(id);

					throw e;
				}

				return systemDataIDRef[id];
			}

			// Ship
			void Data::addShip(DataItem::ShipData item)
			{
				wstring nick = item.getNickname();

				if (shipData.find(nick) != shipData.end()) 
				{
					Exception::RedeclaredNickname e;

					PrintInfoOrErrorLog(wstos(nick) + " addShip\n");

					e.nickname(nick);

					throw e;
				}

				shipData.insert(make_pair(nick, item));

				shipDataIDRef[item.getID()] = &shipData[nick];
			}

			DataItem::ShipData* Data::getShip(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (shipData.find(nickname) == shipData.end()) {
					Exception::NickNameNotFound e;

					PrintInfoOrErrorLog(wstos(nickname) + " getShip\n");

					e.nickname(nickname);

					throw e;
				}

				return &shipData[nickname];
			}

			DataItem::ShipData* Data::getShip(uint id)
			{
				if (shipDataIDRef.find(id) == shipDataIDRef.end())
				{
					Exception::IDNotFound e;

					PrintInfoOrErrorLog(wstos(to_wstring(id)) + " id getShip\n");

					e.nickname(id);

					throw e;
				}

				return shipDataIDRef[id];
			}

			// Faction
			void Data::addFaction(DataItem::FactionData item)
			{
				wstring nick = item.getNickname();

				if (factionData.find(nick) != factionData.end()) 
				{
					Exception::RedeclaredNickname e;

					PrintInfoOrErrorLog(wstos(nick) + " addFaction\n");

					e.nickname(nick);

					throw e;
				}

				factionData.insert(make_pair(nick, item));

				factionDataIDRef[item.getID()] = &factionData[nick];
			}

			DataItem::FactionData* Data::getFaction(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (factionData.find(nickname) == factionData.end()) {
					Exception::NickNameNotFound e;

					PrintInfoOrErrorLog(to_string(factionData.size()) + " - " + wstos(nickname) + " getFaction\n");

					e.nickname(nickname);

					throw e;
				}

				return &factionData[nickname];
			}

			DataItem::FactionData* Data::getFaction(uint id)
			{
				if (factionDataIDRef.find(id) == factionDataIDRef.end())
				{
					Exception::IDNotFound e;

					PrintInfoOrErrorLog(to_string(factionDataIDRef.size()) + " - " + wstos(to_wstring(id)) + " id getFaction\n");

					e.nickname(id);

					throw e;
				}

				return factionDataIDRef[id];
			}

			// Goods
			void Data::addGoods(DataItem::GoodsData item)
			{
				wstring nick = item.getNickname();

				if (goodsData.find(nick) != goodsData.end()) 
				{
					Exception::RedeclaredNickname e;

					PrintInfoOrErrorLog(to_string(goodsData.size()) + " - " + wstos(nick) + " addGoods\n");

					e.nickname(nick);

					throw e;
				}

				goodsData.insert(make_pair(nick, item));

				goodsDataIDRef[item.getID()] = &goodsData[nick];
			}

			DataItem::GoodsData* Data::getGoods(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (goodsData.find(nickname) == goodsData.end()) 
				{
					Exception::NickNameNotFound e;

					PrintInfoOrErrorLog(to_string(goodsData.size()) + " - " + wstos(nickname) + " getGoods\n");

					e.nickname(nickname);

					throw e;
				}

				return &goodsData[nickname];
			}

			DataItem::GoodsData* Data::getGoods(uint id)
			{
				if (goodsDataIDRef.find(id) == goodsDataIDRef.end())
				{
					Exception::IDNotFound e;

					PrintInfoOrErrorLog(to_string(goodsDataIDRef.size()) + " - " + wstos(to_wstring(id)) + " id getGoods\n");

					e.nickname(id);

					throw e;
				}

				return goodsDataIDRef[id];
			}

		}
	}
}