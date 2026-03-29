/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 29 марта 2026 16:56:36
 * Version: 1.0.2
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
#include <ctime>

#include <PluginUtilities.h>
#include "Main.h"

#include <FLCoreServer.h>
#include <FLCoreCommon.h>

#include <map>
#include <sstream>
#include <iostream>
#include "../alley/PlayerRestrictions.h"


namespace GiveCash
{
	// Минимальная сумма перевода.
	static int set_iMinTransfer = 100;

	// Количество кораблей которые пользователь может хранить в файле своих настроек
	static int set_ShipsSaveCount = 100;

	// Переводы в / из символов в этой системе не разрешены.
	static list<uint> set_lBlockedSystems;

	// Включить обнаружение читов наличных в доке
	static bool set_bCheatDetection = false;

	// Запрещаем передачи, если персонаж не был в сети хотя бы это время
	static int set_iMinTime = 0;

	// Запрещаем передачи если у цели меньше 10 часов налёта
	static int set_iMinTimeTarget = 36000;

	/*
		Он проверяет историю денежных поступлений персонажа и распечатывает все полученные денежные сообщения.
		Также исправляет список исправлений денег, мы можем это сделать, потому что этот плагин называется
		до получения доступа к списку исправлений денег.
	*/
	static void CheckTransferLog(uint iClientID)
	{
		wstring wscCharname = ToLower((const wchar_t*)Players.GetActiveCharacterName(iClientID));

		string logFile;
		if (!GetUserFilePath(logFile, wscCharname, "-givecashlog.txt"))
			return;

		FILE *f = fopen(logFile.c_str(), "r");
		if (!f)
			return;

		// Буфер фиксированной длины может быть немного опасен, но длина имен символов фиксирована
		// примерно до 30 символов, так что это должно быть нормально, и в худшем случае 
		// Мы поймаем исключение.
		try {
			char buf[1000];
			while (fgets(buf, 1000, f) != NULL)
			{
				string scValue = buf;
				wstring msg = L"";
				uint lHiByte;
				uint lLoByte;
				while (scValue.length() > 3 && sscanf(scValue.c_str(), "%02X%02X", &lHiByte, &lLoByte) == 2)
				{
					scValue = scValue.substr(4);
					msg.append(1, (wchar_t)((lHiByte << 8) | lLoByte));
				}
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0809"), msg.c_str());
			}
		}
		catch (...) {}
		// Всегда закрывайте файл и удаляйте журнал givecash.
		fclose(f);
		remove(logFile.c_str());
	}

	/*
		Сохраните передачу на диск, чтобы мы могли сообщить принимающему персонажу
		когда они входят в систему.Журнал записывается в шестнадцатеричном формате ascii для поддержки широкого
		наборы символов.
	*/
	static void LogTransfer(wstring wscToCharname, wstring msg)
	{
		string logFile;
		if (!GetUserFilePath(logFile, wscToCharname, "-givecashlog.txt"))
			return;
		FILE *f = fopen(logFile.c_str(), "at");
		if (!f)
			return;

		try
		{
			for (uint i = 0; (i < msg.length()); i++)
			{
				char cHiByte = msg[i] >> 8;
				char cLoByte = msg[i] & 0xFF;
				fprintf(f, "%02X%02X", ((uint)cHiByte) & 0xFF, ((uint)cLoByte) & 0xFF);
			}
			fprintf(f, "\n");
		}
		catch (...) {}
		fclose(f);
		return;
	}

	/* Возврат возврата, если этот аккаунт забанен */
	static bool IsBannedAccount(CAccount *acc)
	{
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);

		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);

		string path = string(szDataPath) + "\\Accts\\MultiPlayer\\" + wstos(wscDir) + "\\banned";

		FILE *file = fopen(path.c_str(), "r");
		if (file) {
			fclose(file);
			return true;
		}
		return false;
	}


	/* Возврат return, если этот символ находится в заблокированной системе */
	static bool InBlockedSystem(const wstring &wscCharname)
	{
		// Оптимизация, если у нас нет заблокированных систем.
		if (set_lBlockedSystems.size() == 0)
			return false;

		// Если символ вошел в систему, мы можем проверить память.
		uint iClientID = HkGetClientIdFromCharname(wscCharname);
		if (iClientID != -1)
		{
			uint iSystem = 0;
			pub::Player::GetSystem(iClientID, iSystem);
			if (find(set_lBlockedSystems.begin(), set_lBlockedSystems.end(), iSystem) != set_lBlockedSystems.end())
				return true;
			return false;
		}

		// Необходимо проверить файл charfile.
		wstring wscSystemNick;
		if (HkFLIniGet(wscCharname, L"system", wscSystemNick) != HKE_OK)
			return false;

		uint iSystem = 0;
		pub::GetSystemID(iSystem, wstos(wscSystemNick).c_str());
		if (find(set_lBlockedSystems.begin(), set_lBlockedSystems.end(), iSystem) != set_lBlockedSystems.end())
			return true;
		return false;
	}


	void GiveCash::LoadSettings(const string &scPluginCfgFile)
	{
		set_iMinTransfer = IniGetI(scPluginCfgFile, "GiveCash", "MinTransfer", 1);
		set_ShipsSaveCount = IniGetI(scPluginCfgFile, "GiveCash", "ShipSaveCount", 10);
		set_iMinTime = IniGetI(scPluginCfgFile, "GiveCash", "MinTime", 0);
		set_bCheatDetection = IniGetB(scPluginCfgFile, "GiveCash", "CheatDetection", true);

		INI_Reader ini;
		set_lBlockedSystems.clear();
		if (ini.open(scPluginCfgFile.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header("GiveCash"))
				{
					while (ini.read_value())
					{
						if (ini.is_value("BlockedSystem"))
						{
							string blockedSystem = ini.get_value_string();
							set_lBlockedSystems.push_back(CreateID(blockedSystem.c_str()));
							if (set_iPluginDebug)
								ConPrint(L"NOTICE: Adding givecash blocked system %s\n", stows(blockedSystem).c_str());
						}
					}
				}
			}
			ini.close();
		}
	}

	/// Проверять денежный перевод, пока этот символ был офлайн всякий раз, когда они
	/// войти или покинуть базу.
	void GiveCash::PlayerLaunch(uint iShip, unsigned int iClientID)
	{
		CheckTransferLog(iClientID);
	}

	/// Проверять денежный перевод, пока этот символ был офлайн всякий раз, когда они
	/// войти или покинуть базу. * /
	void GiveCash::BaseEnter(uint iBaseID, uint iClientID)
	{
		CheckTransferLog(iClientID);
	}

	bool GiveCash::UserCmd_GiveCashTarget(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		uint iShip, iTargetShip;
		pub::Player::GetShip(iClientID, iShip);
		pub::SpaceObj::GetTarget(iShip, iTargetShip);
		if (!iTargetShip)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0810"));
			return true;
		}
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		uint iClientIDTarget = HkGetClientIDByShip(iTargetShip);
		if (!iClientIDTarget)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0811"));
			return true;
		}
		wstring wscTargetCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientIDTarget);

		wstring wscCash = GetParam(wscParam, L' ', 0);
		wstring wscAnon = GetParam(wscParam, L' ', 1);
		wstring wscComment = GetParamToEnd(wscParam, L' ', 2);
		wscCash = ReplaceStr(wscCash, L".", L"");
		wscCash = ReplaceStr(wscCash, L",", L"");
		wscCash = ReplaceStr(wscCash, L"$", L"");
		int cash = ToInt(wscCash);
		if (!wscTargetCharname.length() || cash <= 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0812"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		bool bAnon = false;
		if (wscAnon == L"anon")
			bAnon = true;
		else if (wscAnon.size())
			wscComment = wscAnon + L" " + wscComment;

		GiveCash::GiveCashCombined(iClientID, cash, wscTargetCharname, wscCharname, bAnon, wscComment);
		return true;
	}


	bool GiveCash::SaveAndResetShip(uint iClientID, const wstring& wscCharname, const string& scUserFileShips)
	{
		uint ShipArchId = Players[iClientID].iShipArchetype;

		list<INISECTIONVALUE> shipList;
		//проверяю количество уже существующих кораблей
		IniGetSection(scUserFileShips, "Ships", shipList);
		if (shipList.size() >= set_ShipsSaveCount)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1354"), GetLocalized(iClientID, "MSG_1355") 
				+ to_wstring(set_ShipsSaveCount) 
				+ GetLocalized(iClientID, "MSG_1549"));
			int countShipIndex = 0;
			foreach(shipList, INISECTIONVALUE, iterShip)
			{
				countShipIndex++;
				uint compareArchId = ToUInt(stows(iterShip->scKey));

				Archetype::Ship* ship = Archetype::GetShip(ToUInt(stows(iterShip->scKey)));

				wstring msg = HkGetWStringFromIDS(ship->iIdsName).c_str();
				if (compareArchId == ShipArchId)
				{
					msg += GetLocalized(iClientID, "MSG_1550");
				}

				PrintUserCmdTextColorKV(iClientID, to_wstring(countShipIndex) + L": ", msg);
			}
			return false;
		}

		// удаляю старые данные для невелирования дублирования строк
		IniDelSection(scUserFileShips, to_string(ShipArchId));

		// помечаю головной Header новым кораблём для дальнейшего мониторинга числа сохранённых кораблей
		IniWrite(scUserFileShips, "Ships", to_string(ShipArchId), "");

		// перезаписываю секцию с кораблём или он создаст новую запись о нём
		EquipDescList equips = Players[iClientID].equipDescList;
		for (list<EquipDesc>::iterator item = equips.equip.begin(); item != equips.equip.end(); item++)
		{
			string line = to_string(item->iArchID) + ", "
				+ to_string(item->iDunno) + ", "
				+ to_string(item->iOwner) + ", "
				+ to_string(item->iCount) + ", "
				+ string(item->szHardPoint.value) + ", "
				+ to_string(item->bMounted);
			IniWrite(scUserFileShips, to_string(ShipArchId), to_string(item->sID), line);
			//PrintUserCmdText(iClientID, to_wstring(item->sID) + L" : " + to_wstring(item->get_count()));
		}

		Archetype::Ship* ship = Archetype::GetShip(ShipArchId);
		wstring msg = HkGetWStringFromIDS(ship->iIdsName).c_str();

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0813") + msg);
		return true;
	}

	bool GiveCash::ChangeShip(uint iClientID, const wstring& wscCharname, const string& scUserFileShips, int idChangeShip)
	{
		if (idChangeShip <= 0)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1356"), GetLocalized(iClientID, "MSG_1357"));
			return false;
		}

		list<INISECTIONVALUE> shipsList;
		IniGetSection(scUserFileShips, "Ships", shipsList);

		int index = 0;
		uint ShipArchId = 0;
		foreach(shipsList, INISECTIONVALUE, ship)
		{
			index++;
			if (index == idChangeShip)
			{
				ShipArchId = ToUInt(stows(ship->scKey));
				break;
			}
		}

		if (ShipArchId <= 0)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1358"), GetLocalized(iClientID, "MSG_1359"));
			return false;
		}

		uint ShipArchIdCurrent = Players[iClientID].iShipArchetype;

		if (ShipArchId == ShipArchIdCurrent)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1360"), GetLocalized(iClientID, "MSG_1361"));
			return false;
		}

		// Преобразование uint в строку
		string ShipStr = std::to_string(ShipArchId);
		// Преобразование строки в const char*
		const char* ShipArchIdChar = ShipStr.c_str();

		INI_Reader ini;
		EquipDescList listEQUIP;
		EquipDesc desc;
		EquipDescVector myVector(listEQUIP);

		Archetype::Ship* ship = Archetype::GetShip(ShipArchId);
		wstring nameShip = HkGetWStringFromIDS(ship->iIdsName);

		if (ini.open(scUserFileShips.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header(ShipArchIdChar))
				{
					while (ini.read_value())
					{
						desc.iArchID = ToUInt(stows(ini.get_value_string(0)));
						desc.sID = static_cast<USHORT>(ToInt(ini.get_name_ptr()));
						desc.iDunno = 0;
						desc.iOwner = 0;
						desc.iCount = static_cast<UINT>(ini.get_value_int(3));
						string hp = ini.get_value_string(4);
						if (hp.length() > 0) {
							char* hpA = new char[hp.length() + 1]; // Выделяем память
							strcpy(hpA, hp.c_str()); // Копируем строку
							hpA[hp.length()] = '\0'; // Добавляем нулевой символ в конец
							CacheString stringHP;
							stringHP.value = hpA;
							desc.szHardPoint = stringHP;
						}
						desc.bMounted = ini.get_value_bool(5);
						myVector.add_equipment_item(desc, false);
						
						desc.szHardPoint.clear(); // Очищаем CacheString перед следующей итерацией
					}
				}
			}
			ini.close();
		}

		pub::Player::SetShipAndLoadout(iClientID, ShipArchId, (const EquipDescVector&)myVector);


		uint last_base = HKGetLastBasePlayer(wscCharname);
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1362"), nameShip);
		GiveCash::MoveClient(iClientID, last_base, wscCharname);
	}

	void GiveCash::MoveClient(unsigned int iClientID, unsigned int targetBase, const wstring& wscCharname)
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
		//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1363"), to_wstring(targetBase));

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

			HkKickReason(wscCharname, GetLocalized(iClientID, "MSG_1551"));
		}
		else
		{
			HkKickReason(wscCharname, GetLocalized(iClientID, "MSG_1551"));
		}
	}

	uint GiveCash::HKGetLastBasePlayer(const wstring& wscCharname)
	{
		uint iBaseID = 0;

		list<wstring> lstCharFile;
		HK_ERROR err = HkReadCharFile(wscCharname, lstCharFile);
		if (err != HKE_OK)
			return err;

		foreach(lstCharFile, wstring, line)
		{
			wstring wscKey = Trim(line->substr(0, line->find(L"=")));
			if (wscKey == L"base" || wscKey == L"last_base")
			{
				int iFindEqual = line->find(L"=");
				if (iFindEqual == -1)
				{
					continue;
				}

				if ((iFindEqual + 1) >= (int)line->size())
				{
					continue;
				}

				iBaseID = CreateID(wstos(Trim(line->substr(iFindEqual + 1))).c_str());
				break;
			}
		}
		return iBaseID;
	}

	/// <summary>
	/// Меняет корабль у игрока и выводит список доступных из файла пользователя ships.ini
	/// </summary>
	bool GiveCash::UserCmd_ChangeShip(uint iClientID, const wstring& wscCmd, const wstring& wscParam, const wchar_t* usage)
	{
		uint iShip, iTargetShip, iBaseID;;
		pub::Player::GetShip(iClientID, iShip);
		pub::Player::GetBase(iClientID, iBaseID);
		pub::SpaceObj::GetTarget(iShip, iTargetShip);
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		uint yourArchShipId = Players[iClientID].iShipArchetype;

		CAccount* acc = Players.FindAccountFromClientID(iClientID);
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);
		string scUserFileShips = scAcctPath + wstos(wscDir) + "\\ships.ini";

		// Сохранение персонажей заставляет античит проверять и исправлять множество других проблем. 
		HkSaveChar(iClientID);
		// сохранение текущего корабля
		bool saveResult = SaveAndResetShip(iClientID, wscCharname, scUserFileShips);

		if (saveResult == false) return true;

		bool isExistShip = false;
		list<INISECTIONVALUE> shipList;

		IniGetSection(scUserFileShips, "Ships", shipList);

		wstring wscShipParam = ToLower(GetParam(wscParam, ' ', 0)); // Какой корабль мы пытаемся получить?
		int countShipIndex = 0;
		if (!wscShipParam.length()) // Если параметр не был указан, выполните итерацию по списку.
		{
			foreach(shipList, INISECTIONVALUE, iterShip)
			{
				countShipIndex++;
				uint compareArchId = ToUInt(stows(iterShip->scKey));

				Archetype::Ship* ship = Archetype::GetShip(ToUInt(stows(iterShip->scKey)));

				wstring msg = HkGetWStringFromIDS(ship->iIdsName).c_str();
				if (compareArchId == yourArchShipId)
				{
					msg += L" (ваш текущий)";
				}

				PrintUserCmdTextColorKV(iClientID, to_wstring(countShipIndex) + L": ", msg);
			}
			return false;
		}
		else
		{
			countShipIndex = ToInt(wscShipParam);
		}

		if (iBaseID)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0814"));
			return true;
		}

		// Проверяем есть ли груз у человека на корабле, если есть блокируем смену корабля
		list<CARGO_INFO> lstCargo;
		int iRem;
		HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
		uint iNum = 0;
		uint iNanobotsID = 2911012559;		//id 
		uint iShieldBatID = 2596081674;		//id 
		foreach(lstCargo, CARGO_INFO, it)
		{
			Archetype::Equipment* eq = Archetype::GetEquipment(it->iArchID);
			const GoodInfo* gi = GoodList::find_by_id(it->iArchID);
			if (!gi)
				continue;
			Archetype::Gun* gun = (Archetype::Gun*)eq;
			if (!it->bMounted)
			{
				if (gun->iArchID && it->iCount != 0 && gun->iArchID != iNanobotsID
					&& gun->iArchID != iShieldBatID)
				{
					PrintUserCmdTextColorKV(iClientID, to_wstring(it->iID) + L": ", HkGetWStringFromIDS(gun->iIdsName) + L" = " + to_wstring(it->iCount));
				}
				else
				{
					if (it->iCount != 0 && gun->iArchID != iNanobotsID
						&& gun->iArchID != iShieldBatID)
					{
						PrintUserCmdTextColorKV(iClientID, to_wstring(it->iID) + L": ", HkGetWStringFromIDS(gi->iIDSName) + L" = " + to_wstring(it->iCount));
					}
				}
				if (gun->iArchID != iNanobotsID
					&& gun->iArchID != iShieldBatID)
				{
					iNum++;
				}
			}
		}

		if (iNum > 0)
		{
			PrintUserCmdTextColorKV(iClientID, L"[" + to_wstring(iNum) + L"]: ", GetLocalized(iClientID, "MSG_1624"));
			return true;
		}

		// получить список оборудования корабля на который мы хотим поменять старый
		ChangeShip(iClientID, wscCharname, scUserFileShips, countShipIndex);
		
		return true;
	}

	/// <summary>
	/// Удалает выбранный корабль из списка и выводит список доступных из файла пользователя ships.ini
	/// </summary>
	bool GiveCash::UserCmd_DeleteShip(uint iClientID, const wstring& wscCmd, const wstring& wscParam, const wchar_t* usage)
	{
		CAccount* acc = Players.FindAccountFromClientID(iClientID);
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);
		string scUserFileShips = scAcctPath + wstos(wscDir) + "\\ships.ini";
		list<INISECTIONVALUE> shipList;
		uint yourArchShipId = Players[iClientID].iShipArchetype;
		IniGetSection(scUserFileShips, "Ships", shipList);

		wstring wscShipParam = ToLower(GetParam(wscParam, ' ', 0)); // Какой корабль мы пытаемся получить?
		int countShipIndex = 0;
		if (!wscShipParam.length()) // Если параметр не был указан, выполните итерацию по списку.
		{
			foreach(shipList, INISECTIONVALUE, iterShip)
			{
				countShipIndex++;
				uint compareArchId = ToUInt(stows(iterShip->scKey));

				Archetype::Ship* ship = Archetype::GetShip(ToUInt(stows(iterShip->scKey)));

				wstring msg = HkGetWStringFromIDS(ship->iIdsName).c_str();
				if (compareArchId == yourArchShipId)
				{
					msg += GetLocalized(iClientID, "MSG_1550");
				}

				PrintUserCmdTextColorKV(iClientID, to_wstring(countShipIndex) + L": ", msg);
			}
			return false;
		}
		else
		{
			countShipIndex = ToInt(wscShipParam);
		}

		int index = 0;
		uint ShipDeleteArchId = 0;
		foreach(shipList, INISECTIONVALUE, ship)
		{
			index++;
			if (index == countShipIndex)
			{
				ShipDeleteArchId = ToUInt(stows(ship->scKey));
				break;
			}
		}

		if (ShipDeleteArchId == yourArchShipId)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1364"), GetLocalized(iClientID, "MSG_1365"));
			return true;
		}

		IniDelete(scUserFileShips, "Ships", to_string(ShipDeleteArchId));
		IniDelSection(scUserFileShips, to_string(ShipDeleteArchId));
		
		Archetype::Ship* ship = Archetype::GetShip(ShipDeleteArchId);
		wstring nameShip = HkGetWStringFromIDS(ship->iIdsName);
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1366"), nameShip);
		return true;
	}

	/** Process a give cash command */
	bool GiveCash::UserCmd_GiveCash(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		// Получить имя текущего персонажа
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		// Получаем параметры из пользовательской команды.
		wstring wscTargetCharname = GetParam(wscParam, L' ', 0);	//12.08.2021 - Имя цели 
		wstring wscCash = GetParam(wscParam, L' ', 1);				//12.08.2021 - Бабки
		wstring wscAnon = GetParam(wscParam, L' ', 2);				//12.08.2021 - Анонимно или нет
		wstring wscComment = GetParamToEnd(wscParam, L' ', 3);		//12.08.2021 - Комментарий
		wscCash = ReplaceStr(wscCash, L".", L"");
		wscCash = ReplaceStr(wscCash, L",", L"");
		wscCash = ReplaceStr(wscCash, L"$", L"");
		int cash = ToInt(wscCash);
		//12.08.2021 - Имя цели не введено или денег меньше или равно нулю
		if (!wscTargetCharname.length() || cash <= 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0812"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		bool bAnon = false;
		if (wscAnon == L"anon")
			bAnon = true;
		else if (wscAnon.size())
			wscComment = wscAnon + L" " + wscComment;

		GiveCash::GiveCashCombined(iClientID, cash, wscTargetCharname, wscCharname, bAnon, wscComment);
		return true;
	}


	HK_ERROR err;
	HK_ERROR err_old;
	int secs = 0;
	int secstarget = 0;

	bool GiveCash::GiveCashCombined(uint iClientID, const int &cash, const wstring &wscTargetCharname, const wstring &wscCharname, const bool &bAnon, const wstring &wscComment)
	{
		bool error = (err == HkGetOnLineTime(wscCharname, secs));
		bool error_olds = err_old = HkGetOnLineTime(wscTargetCharname, secstarget);
		wstring error_string = HkErrGetText(err);
		wstring error_old_string = HkErrGetText(err_old);

		//12.08.2021 - Имя аккаунта цели не равно нулю
		if (HkGetAccountByCharname(wscTargetCharname) == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0816"));
			return true;
		}

		//12.08.2021 - онлайн цели не равен нулю
		if (error != HKE_OK) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0817") + error_string);
			return true;
		}

		//12.08.2021 - онлайн наигранный у цели больше чем лимит
		if (error_olds != HKE_OK) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0818") + error_old_string);
			return true;
		}

		if (secs < set_iMinTime)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0819"));
			return true;
		}
		//------------------------
		if (secstarget < set_iMinTimeTarget)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0820"));
			return true;
		}

		//12.08.2021 - Получаем имя цели
		CAccount *iTargetAcc = HkGetAccountByCharname(wscTargetCharname);
		if (iTargetAcc == 0)//12.08.2021 - проверяем его существование
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0821"));
			return true;
		}

		//12.08.2021 -  цель и игрок не в бане
		if (InBlockedSystem(wscCharname) || InBlockedSystem(wscTargetCharname) || IsBannedAccount(iTargetAcc))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0822"));
			return true;
		}

		// 12.08.2021 - Считываем текущее количество кредитов для игрока
		// и проверяем, что у персонажа достаточно денег.
		int iCash = 0;
		if ((err = HkGetCash(wscCharname, iCash)) != HKE_OK) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		if (cash < set_iMinTransfer || cash < 0) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0824") + ToMoneyStr(set_iMinTransfer) + GetLocalized(iClientID, "MSG_1552"));
			return true;
		}

		if (iCash < cash)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0825"), iCash, cash);
			return true;
		}

		// 12.08.2021 - Предотвращаем повреждение целевого корабля.
		float fTargetValue = 0.0f;
		if ((err = HKGetShipValue(wscTargetCharname, fTargetValue)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		int iCashTarget = 0;
		if ((err = HkGetCash(wscTargetCharname, iCashTarget)) != HKE_OK) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		// 18.08.2024 - проверяем деньги цели
		long newCashAnalyze = iCashTarget + cash;
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0828"), newCashAnalyze);

		if (newCashAnalyze >= 1900000000 || newCashAnalyze <= 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0829"));
			return true;
		}

		// Рассчитать новые денежные средства
		int iExpectedCash = 0;
		if ((err = HkGetCash(wscTargetCharname, iExpectedCash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0830") + HkErrGetText(err));
			return true;
		}

		// 12.08.2021 - увеличиваем число денег цели
		iExpectedCash += cash;

		// Сначала выполняем античит-проверку принимающего персонажа.
		uint targetClientId = HkGetClientIdFromCharname(wscTargetCharname);
		if (targetClientId != -1 && !HkIsInCharSelectMenu(targetClientId))
		{
			if (HkAntiCheat(targetClientId) != HKE_OK)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0831"));
				AddLog("NOTICE: Possible cheating when sending %s credits from %s (%s) to %s (%s)",
					wstos(ToMoneyStr(cash)).c_str(),
					wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
					wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str());
				return true;
			}
			HkSaveChar(targetClientId);
		}

		if (targetClientId != -1)
		{
			if (ClientInfo[iClientID].iTradePartner || ClientInfo[targetClientId].iTradePartner)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0832"));
				AddLog("NOTICE: Trade window open when sending %s credits from %s (%s) to %s (%s) %u %u",
					wstos(ToMoneyStr(cash)).c_str(),
					wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
					wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
					iClientID, targetClientId);
				return true;
			}
		}

		// 18.08.2024 - проверяем деньги цели
		newCashAnalyze = iCashTarget + cash;

		// Удаляем деньги из текущего символа и сохраняем его, проверяя, что
		// сохранение завершается перед добавлением денег на целевой корабль.
		if (newCashAnalyze < 2000000000.0f)
		{
			if ((err = HkAddCash(wscCharname, 0 - cash)) != HKE_OK)//0 - 
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0833") + HkErrGetText(err));
				return true;
			}
		}

		if (HkAntiCheat(iClientID) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0831"));
			AddLog("NOTICE: Possible cheating when sending %s credits from %s (%s) to %s (%s)",
				wstos(ToMoneyStr(cash)).c_str(),
				wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
				wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str());
			return true;
		}
		HkSaveChar(iClientID);

		// 18.08.2024 - проверяем деньги цели
		newCashAnalyze = iCashTarget + cash;

		// Добавляем деньги целевому персонажу
		if (newCashAnalyze < 2000000000.0f)
		{
			if ((err = HkAddCash(wscTargetCharname, cash)) != HKE_OK)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0833") + HkErrGetText(err));
				return true;
			}
		}


		targetClientId = HkGetClientIdFromCharname(wscTargetCharname);
		if (targetClientId != -1 && !HkIsInCharSelectMenu(targetClientId))
		{
			if (HkAntiCheat(targetClientId) != HKE_OK)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0831"));
				AddLog("NOTICE: Possible cheating when sending %s credits from %s (%s) to %s (%s)",
					wstos(ToMoneyStr(cash)).c_str(),
					wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
					wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str());
				return true;
			}
			HkSaveChar(targetClientId);
		}


		// Проверяем, что у принимающего персонажа правильное количество денег.
		int iCurrCash;
		if ((err = HkGetCash(wscTargetCharname, iCurrCash)) != HKE_OK
			|| iCurrCash != iExpectedCash)
		{
			AddLog("ERROR: Cash transfer error when sending %s credits from %s (%s) to %s (%s) current %s credits expected %s credits ",
				wstos(ToMoneyStr(cash)).c_str(),
				wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
				wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
				wstos(ToMoneyStr(iCurrCash)).c_str(), wstos(ToMoneyStr(iExpectedCash)).c_str());
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0831"));
			return true;
		}

		// Get current time
		time_t rawtime;
		tm* timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		char buffer[22];
		strftime(buffer, 22, "[%Y-%m-%d] %H:%M:%S", timeinfo);
		wstring time = stows((string)buffer);

		// Если целевой игрок в сети, отправьте ему сообщение с сообщением
		// сообщая им, что они получили деньги.
		wstring msg = time + GetLocalized(iClientID, "MSG_1554") + ToMoneyStr(cash) 
			+ GetLocalized(iClientID, "MSG_1553") 
			+ ((bAnon) ? GetLocalized(iClientID, "MSG_1555") : wscCharname) + (wscComment.size() ? L" | " + wscComment : L"");
		if (targetClientId != -1 && !HkIsInCharSelectMenu(targetClientId))
		{
			PrintUserCmdText(targetClientId, GetLocalized(iClientID, "MSG_0809"), msg.c_str());
		}
		// В противном случае мы предполагаем, что персонаж не в сети, поэтому мы записываем запись
		// в файле givecash.ini персонажа. Когда они выходят в Интернет, мы сообщаем им
		// передачи. Ini очищается, когда персонаж входит в систему.
		else
		{
			LogTransfer(wscTargetCharname, msg);
		}

		AddLog("NOTICE: Send %s credits from %s (%s) to %s (%s) %s %s",
			wstos(ToMoneyStr(cash)).c_str(),
			wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
			wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
			wscComment.size() ? "| With comment:" : "", wstos(wscComment).c_str());

		// A friendly message explaining the transfer.
		msg = time + GetLocalized(iClientID, "MSG_1556") + ToMoneyStr(cash) + GetLocalized(iClientID, "MSG_1557") + wscTargetCharname;
		if (bAnon)
			msg += GetLocalized(iClientID, "MSG_1558");
		if (wscComment.size())
			msg += GetLocalized(iClientID, "MSG_1559");;

		// Grammar nazi.
		msg += L".";

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0809"), msg.c_str());
		return true;
	}

	/** Обработка команды set cash code */
	bool GiveCash::UserCmd_SetCashCode(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		string scFile;
		if (!GetUserFilePath(scFile, wscCharname, "-givecash.ini"))
			return true;

		wstring wscCode = GetParam(wscParam, L' ', 0);

		if (!wscCode.size())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0812"));
			PrintUserCmdText(iClientID, usage);
		}
		else if (wscCode == L"none")
		{
			IniWriteW(scFile, "Settings", "Code", L"");
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0841"));
		}
		else
		{
			IniWriteW(scFile, "Settings", "Code", wscCode);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0842") + wscCode);
		}
		return true;
	}

	/** Обработка команды show cash **/
	bool GiveCash::UserCmd_ShowCash(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		// Последняя ошибка.
		HK_ERROR err;

		// Получение текущего имени персонажа
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		// Получите параметры из команды пользователя.
		wstring wscTargetCharname = GetParam(wscParam, L' ', 0);
		wstring wscCode = GetParam(wscParam, L' ', 1);

		if (!wscTargetCharname.length() || !wscCode.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0812"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		CAccount *acc = HkGetAccountByCharname(wscTargetCharname);
		if (acc == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0816"));
			return true;
		}

		string scFile;
		if (!GetUserFilePath(scFile, wscTargetCharname, "-givecash.ini"))
			return true;

		wstring wscTargetCode = IniGetWS(scFile, "Settings", "Code", L"");
		if (!wscTargetCode.length() || wscTargetCode != wscCode)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0845"));
			return true;
		}

		int iCash = 0;
		if ((err = HkGetCash(wscTargetCharname, iCash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0847") + wscTargetCharname 
			+ GetLocalized(iClientID, "MSG_1560") + ToMoneyStr(iCash) + GetLocalized(iClientID, "MSG_1561"));
		return true;
	}


	/** Обработка команды розыгрыша кассы **/
	bool GiveCash::UserCmd_DrawCash(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		// Последняя ошибка.
		HK_ERROR err;
		HK_ERROR err_old;

		// Получение текущего имени персонажа
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		// Получите параметры из команды пользователя.
		wstring wscTargetCharname = GetParam(wscParam, L' ', 0);
		wstring wscCode = GetParam(wscParam, L' ', 1);
		wstring wscCash = GetParam(wscParam, L' ', 2);
		wscCash = ReplaceStr(wscCash, L".", L"");
		wscCash = ReplaceStr(wscCash, L",", L"");
		wscCash = ReplaceStr(wscCash, L"$", L"");
		int cash = ToInt(wscCash);
		//12.08.2021 - Если имя игрока не введено, или денег меньше нуля или какой-то код не имеет длинну
		if (!wscTargetCharname.length() || !wscCode.length() || cash <= 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0812"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		//12.08.2021 - Получаем имя цели
		CAccount *iTargetAcc = HkGetAccountByCharname(wscTargetCharname);
		if (iTargetAcc == 0)//12.08.2021 - проверяем его существование
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0821"));
			return true;
		}

		int secs = 0;
		int secstarget = 0;
		//12.08.2021 - проверяем наличие игрока в сети
		if ((err = HkGetOnLineTime(wscTargetCharname, secs)) != HKE_OK) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		//12.08.2021 - снова проверяем онлайн
		if ((err_old = HkGetOnLineTime(wscTargetCharname, secstarget)) != HKE_OK) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err_old));
			return true;
		}

		if (secs < set_iMinTime)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0819"));
			return true;
		}

		if (secstarget < set_iMinTimeTarget)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0820"));
			return true;
		}

		//12.08.2021 - проверяем аккаунты на статус забаненный
		if (InBlockedSystem(wscCharname) || InBlockedSystem(wscTargetCharname) || IsBannedAccount(iTargetAcc))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0822"));
			return true;
		}

		string scFile;
		//12.08.2021 - Если у пользователя нету файла \\Accts\\MultiPlayer\\ givecash.ini
		if (!GetUserFilePath(scFile, wscTargetCharname, "-givecash.ini"))
			return true;

		wstring wscTargetCode = IniGetWS(scFile, "Settings", "Code", L"");
		if (!wscTargetCode.length() || wscTargetCode != wscCode)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0845"));
			return true;
		}

		if (cash < set_iMinTransfer || cash < 0) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0824") + ToMoneyStr(set_iMinTransfer) + L" кредитов.");
			return true;
		}

		int tCash = 0;
		//12.08.2021 - Получаем числа бабла у цели
		if ((err = HkGetCash(wscTargetCharname, tCash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		//12.08.2021 - Если у цели число денег меньше чем число отправляемых
		if (tCash < cash)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0858"));
			return true;
		}

		// Отметьте добавление этих денег этому игроку не должно превышать максимальную стоимость корабля.
		float fTargetValue = 0.0f;
		if ((err = HKGetShipValue(wscCharname, fTargetValue)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		// 18.08.2024 - проверяем деньги цели
		long newCashAnalyze = fTargetValue + cash;

		if (newCashAnalyze > 2000000000.0f || newCashAnalyze <= 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0829"));
			return true;
		}

		// Рассчитать новые денежные средства
		int iExpectedCash = 0;
		if ((err = HkGetCash(wscCharname, iExpectedCash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}
		iExpectedCash += cash;

		// Сначала проведите античит-проверку на принимающем корабле.
		if (HkAntiCheat(iClientID) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0862"));
			AddLog("NOTICE: Possible cheating when drawing %s credits from %s (%s) to %s (%s)",
				wstos(ToMoneyStr(cash)).c_str(),
				wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
				wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str());
			return true;
		}
		HkSaveChar(iClientID);


		uint targetClientId = HkGetClientIdFromCharname(wscTargetCharname);
		if (targetClientId != -1)
		{
			if (ClientInfo[iClientID].iTradePartner || ClientInfo[targetClientId].iTradePartner)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0832"));
				AddLog("NOTICE: Trade window open when drawing %s credits from %s (%s) to %s (%s) %u %u",
					wstos(ToMoneyStr(cash)).c_str(),
					wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
					wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
					iClientID, targetClientId);
				return true;
			}
		}

		// Изъятие денег у целевого персонажа
		if ((err = HkAddCash(wscTargetCharname, 0 - cash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		if (targetClientId != -1 && !HkIsInCharSelectMenu(targetClientId))
		{
			if (HkAntiCheat(targetClientId) != HKE_OK)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0862"));
				AddLog("NOTICE: Possible cheating when drawing %s credits from %s (%s) to %s (%s)",
					wstos(ToMoneyStr(cash)).c_str(),
					wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
					wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str());
				return true;
			}
			HkSaveChar(targetClientId);
		}

		// Добавьте деньги этому игроку
		if ((err = HkAddCash(wscCharname, cash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0823") + HkErrGetText(err));
			return true;
		}

		if (HkAntiCheat(iClientID) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0862"));
			AddLog("NOTICE: Possible cheating when drawing %s credits from %s (%s) to %s (%s)",
				wstos(ToMoneyStr(cash)).c_str(),
				wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
				wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str());
			return true;
		}
		HkSaveChar(iClientID);

		// Убедитесь, что у принимающего игрока есть правильная сумма наличных.
		int iCurrCash;
		if ((err = HkGetCash(wscCharname, iCurrCash)) != HKE_OK
			|| iCurrCash != iExpectedCash)
		{
			AddLog("ERROR: Cash transfer error when drawing %s credits from %s (%s) to %s (%s) current %s credits expected %s credits ",
				wstos(ToMoneyStr(cash)).c_str(),
				wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
				wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
				wstos(ToMoneyStr(iCurrCash)).c_str(), wstos(ToMoneyStr(iExpectedCash)).c_str());
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0862"));
		}

		// Если целевой игрок находится в сети, отправьте ему сообщение о том, 
		// что он получил переведенные деньги.
		wstring msg = GetLocalized(iClientID, "MSG_1562") + ToMoneyStr(cash) + GetLocalized(iClientID, "MSG_1563") + wscCharname;
		if (targetClientId != -1 && !HkIsInCharSelectMenu(targetClientId))
		{
			PrintUserCmdText(targetClientId, GetLocalized(iClientID, "MSG_0809"), msg.c_str());
		}
		// В противном случае мы предполагаем, что персонаж находится в автономном режиме, 
		// поэтому записываем запись в givecash.ini символов.
		// Когда они подключаются к сети, мы информируем их о переводе. 
		// Ini очищается, когда персонаж входит в систему.
		else
		{
			LogTransfer(wscTargetCharname, msg);
		}

		AddLog("NOTICE: Draw %s credits from %s (%s) to %s (%s)",
			wstos(ToMoneyStr(cash)).c_str(),
			wstos(wscTargetCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscTargetCharname))).c_str(),
			wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str());

		// Дружеское сообщение с объяснением трансфера.
		msg = GetTimeString(set_bLocalTime) + GetLocalized(iClientID, "MSG_1564") + ToMoneyStr(cash) 
			+ GetLocalized(iClientID, "MSG_1565") + wscTargetCharname;
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0809"), msg.c_str());
		return true;
	}
}
