/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 апреля 2026 06:54:28
 * Version: 1.0.471
 */

// FLHook Plugin to hold a miscellaneous collection of commands and 
// other such things that don't fit into other plugins
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include <vector>
#include <io.h>
#include <PluginUtilities.h>
#include "header.h"

#define ADDR_FLCONFIG 0x25410
string scUserStore;
wstring HkGetWStringFromIDS(uint iIDS);
void HkLoadDLLConf(const char *szFLConfigFile);
vector<HINSTANCE> vDLLss;
list<wstring> CargoNameList;
int set_errorstore_num;
int CountEmptyHoldPlayerCurrent = 0;//Количество слотов свободных при учёте загрузки склада в корабль
int GoodsToHoldPlayerOk = 0;		//Остаток в склад
int CountListCagoHoldPlayer = 0;	//Количество слотов под товары на складе
int iGoods = 0;						//Число товаров на складе
int iGoodsCargoNum = 0;				//Число товара на складе
int intcorrectvolume = 0;			//Вес больше 1 но число их меньше чем свободных слотов
int intcorrectvolumeostatok = 0;	//Вес больше 1 но число их меньше чем свободных слотов ОСТСТОК в склад
int CountGoodsCargo = 0;			//Число товара на складе
int iGoodsCargoOstatok = 0;			//Число оставшегося товара после сброса
uint nCount = 0;					//Количество наноботов в трюме
uint sCount = 0;					//Количество батарей в трюме
uint iNanobotsID = 2911012559;		//id 
uint iShieldBatID = 2596081674;		//id 
string colorKeyType0 = "";			//Цвет команды /sinfo под ячейки ключа с типом 0
string colorValueType0 = "";		//Цвет команды /sinfo под ячейки значения с типом 0
string colorKeyType1 = "";			//Цвет команды /sinfo под ячейки ключа с типом 1
string colorValueType1 = "";		//Цвет команды /sinfo под ячейки значения с типом 1
int countMaxLinesSinfo = 0;			//Количество строк /sinfo в одной странице

string utos(uint i)
{
	char szBuf[16];
	sprintf_s(szBuf, "%u", i);
	return szBuf;
}

wstring ftows(float f)
{
	wchar_t wszBuf[16];
	swprintf_s(wszBuf, L"%g", f);
	return wszBuf;
}

PLUGIN_RETURNCODE returncode;

EXPORT void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	char *szFLConfig = (char*)((char*)GetModuleHandle(0) + ADDR_FLCONFIG);
	HkLoadDLLConf(szFLConfig);

	// ErrorStore
	char szCurDirStore[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDirStore), szCurDirStore);
	string store_file = string(szCurDirStore) + "\\flhook_plugins\\ErrorStore.ini";
	colorKeyType0 = IniGetS(store_file, "General", "colorKeyType0", "0xAFAE0140");
	colorValueType0 = IniGetS(store_file, "General", "colorValueType0", "0x251CEE40");
	colorKeyType1 = IniGetS(store_file, "General", "colorKeyType1", "0xAFAE0140");
	colorValueType1 = IniGetS(store_file, "General", "colorValueType1", "0x251CEE40");
	countMaxLinesSinfo = IniGetI(store_file, "General", "MaxLinesSinfo", 10);
	set_errorstore_num = IniGetI(store_file, "General", "allequip", 0);
	CargoNameList.clear();
	for (uint i = 0;; i++)
	{
		char szBuf[64];
		sprintf(szBuf, "M%u", i);
		string cargonumname = IniGetS(store_file, "CargoName", szBuf, "");
		if (!cargonumname.length())
			break;

		CargoNameList.push_back(stows(cargonumname));
	}
}

/// Hook will call this function after calling a plugin function to see if we the
/// processing to continue
EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		LoadSettings();
	return true;
}

EXPORT void UserCmd_Help(uint iClientID, const wstring &wscParam)
{
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1399"), GetLocalized(iClientID, "MSG_1400"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1401"), GetLocalized(iClientID, "MSG_1402"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1403"), GetLocalized(iClientID, "MSG_1584"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1404"), GetLocalized(iClientID, "MSG_1405"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1406"), GetLocalized(iClientID, "MSG_1585"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1407"), GetLocalized(iClientID, "MSG_1408"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1409"), GetLocalized(iClientID, "MSG_1410"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1411"), GetLocalized(iClientID, "MSG_1412"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1413"), GetLocalized(iClientID, "MSG_1586"));
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1414"), GetLocalized(iClientID, "MSG_1586"));
}

void UserCmd_Store(uint iClientID, const wstring &wscParam)
{
	CAccount *acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	scUserStore = scAcctPath + wstos(wscDir) + "\\store.ini";
	uint iShip;
	pub::Player::GetShip(iClientID, iShip);
	uint iTarget;
	pub::SpaceObj::GetTarget(iShip, iTarget);
	uint iType;
	pub::SpaceObj::GetType(iTarget, iType);
	if (iType == 8192)
	{
		uint wscGoods = ToInt(GetParam(wscParam, ' ', 0));
		int wscCount = ToInt(GetParam(wscParam, ' ', 1));
		bool print = true;
		if (wscParam.find(L"all") != -1)
		{
			list<CARGO_INFO> lstCargo;
			int iRem;
			bool ret = true;
			HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
			int countRemove = 0;
			foreach(lstCargo, CARGO_INFO, cargo)
			{
				const GoodInfo *gi = GoodList::find_by_id(cargo->iArchID);
				if (!gi)
					continue;
				Archetype::Equipment *eq = Archetype::GetEquipment(cargo->iArchID);
				EQ_TYPE eqType = HkGetEqType(eq);
				if (!cargo->bMounted && gi->iIDS && cargo->iArchID != iNanobotsID && cargo->iArchID != iShieldBatID && eqType != ET_MUNITION)
				{
					countRemove++;
					int iGoods = 0;
					print = false;
					iGoods = IniGetI(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(cargo->iArchID), 0);
					IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(cargo->iArchID), itos(iGoods + cargo->iCount));
					HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, cargo->iCount);
					if(countRemove <= 100)
						PrintUserCmdTextColorKV(iClientID, to_wstring(countRemove) + L": ", HkGetWStringFromIDS(eq->iIdsName) 
							+ GetLocalized(iClientID, "MSG_1587") + to_wstring(cargo->iCount) + GetLocalized(iClientID, "MSG_1588"));
				}
			}

			if (countRemove > 100)
			{
				int hide = countRemove - 100;
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1415") + to_wstring(hide) 
					+ GetLocalized(iClientID, "MSG_1589"), GetLocalized(iClientID, "MSG_1428"));
			}

			if (print)
			{
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1416"), GetLocalized(iClientID, "MSG_1417"));
			}
			else {
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1418"), GetLocalized(iClientID, "MSG_1419"));
				print = true;
			}
			return;
		}

		if (wscCount <= 0)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1420"), GetLocalized(iClientID, "MSG_1421"));
			return;
		}
		if (wscGoods == 0 || wscCount == 0)
		{
			//необходимо ввести правильные значения try / enumcargo id amount
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1422"),
				GetLocalized(iClientID, "MSG_1590"));
		}
		else
		{
			list<CARGO_INFO> lstCargo;
			int iRem;
			HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
			foreach(lstCargo, CARGO_INFO, cargo)
			{
				if (cargo->iID == wscGoods)
				{
					if (cargo->iCount - wscCount < 0)
					{
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1423"), GetLocalized(iClientID, "MSG_1424"));
					}
					else
					{
						Archetype::Equipment *eq = Archetype::GetEquipment(cargo->iArchID);
						EQ_TYPE eqType = HkGetEqType(eq);
						if (!cargo->bMounted && cargo->iArchID != iNanobotsID && cargo->iArchID != iShieldBatID && eqType != ET_MUNITION)//Было без условия
						{
							int iGoods = 0;
							iGoods = IniGetI(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(cargo->iArchID), 0);
							IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(cargo->iArchID), itos(iGoods + wscCount));
							HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, wscCount);
							Archetype::Gun *gunes = (Archetype::Gun *)eq;
							const GoodInfo *gizes = GoodList::find_by_id(cargo->iArchID);
							if (!gizes)
								continue;
							if (gunes->iArchID)
							{
								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1425"), HkGetWStringFromIDS(gunes->iIdsName) + L" = " + to_wstring(wscCount));
							}
							else
							{
								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1426"), HkGetWStringFromIDS(gizes->iIDSName) + L" = " + to_wstring(wscCount));
							}
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1230"));
						}

						foreach(CargoNameList, wstring, i)
						{
							if (to_wstring(cargo->iArchID) == (*i))
							{
								int iGoods = 0;
								iGoods = IniGetI(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(cargo->iArchID), 0);
								IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(cargo->iArchID), itos(iGoods + wscCount));
								HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, wscCount);
								const GoodInfo *gizes = GoodList::find_by_id(cargo->iArchID);

								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1427"), HkGetWStringFromIDS(gizes->iIDSName) + L" = " + to_wstring(wscCount));
								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1433"), GetLocalized(iClientID, "MSG_1429"));
							}
						}
					}
				}
			}
		}
	}
	else
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1430"), GetLocalized(iClientID, "MSG_1431"));
	}
}

void UserCmd_Ustore(uint iClientID, const wstring &wscParam)
{
	int maxSlotsPlayer = 160;
	CAccount *acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	scUserStore = scAcctPath + wstos(wscDir) + "\\store.ini";
	list<INISECTIONVALUE> lstGoods;
	IniGetSection(scUserStore, wstos(HkGetPlayerSystem(iClientID)), lstGoods);
	int count = 0;
	uint NAME_GOOD = 0;

	//УДАЛЯЮ ВСЕ ПУСТЫЕ БЛЯТЬ
	foreach(lstGoods, INISECTIONVALUE, it3)
	{
		// СКОЛЬКО ЕСТЬ НА СКЛАДЕ
		int COUNT_GOOD = ToInt(stows(it3->scValue).c_str());
		if (COUNT_GOOD <= 0)
		{
			lstGoods.erase(it3);
		}
	}

	foreach(lstGoods, INISECTIONVALUE, it3)
	{
		// СКОЛЬКО ЕСТЬ НА СКЛАДЕ
		count = count + 1;
		if (ToInt(GetParam(wscParam, ' ', 0).c_str()) == count)
		{
			// ИМЯ ПРЕДМЕТА
			NAME_GOOD = ToInt(stows(it3->scKey).c_str());
			int COUNT_GOOD = ToInt(stows(it3->scValue).c_str());
			//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1231")  +  to_wstring(count) + L" | " + to_wstring(NAME_GOOD) + L"|" + to_wstring(COUNT_GOOD));
		}
	}

	// СКОЛЬКО СНЯТЬ СО СКЛАДА
	int COUNT_REMOVE_GOOD = ToInt(GetParam(wscParam, ' ', 1));
	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1232") + to_wstring(NAME_GOOD) + L"|" + to_wstring(COUNT_REMOVE_GOOD));

	uint iShip;
	pub::Player::GetShip(iClientID, iShip);
	uint iTarget;
	pub::SpaceObj::GetTarget(iShip, iTarget);
	uint iType;
	pub::SpaceObj::GetType(iTarget, iType);

	if (iType == 8192 && wscParam.find(L"all") != -1)
	{
		float fRemainingHoldAll;
		list<INISECTIONVALUE> lstGoodsFFF;
		IniGetSection(scUserStore, wstos(HkGetPlayerSystem(iClientID)), lstGoodsFFF);
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1432"), GetLocalized(iClientID, "MSG_1460"));
		foreach(lstGoodsFFF, INISECTIONVALUE, it3)
		{
			pub::Player::GetRemainingHoldSize(iClientID, fRemainingHoldAll);
			uint wscGoodsAll = ToInt(stows(it3->scKey).c_str()); //Айди товара
			iGoodsCargoNum = IniGetI(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(wscGoodsAll), 0); //Количество товара
			CountGoodsCargo = iGoodsCargoNum; //Количество товара

			if (iGoodsCargoNum > 0)
			{
				Archetype::Equipment *eqd = Archetype::GetEquipment(wscGoodsAll);
				const GoodInfo *gis = GoodList::find_by_id(wscGoodsAll);
				if (!gis)
					continue;
				Archetype::Gun *guns = (Archetype::Gun *)eqd;

				float VolumeSizeGood = eqd->fVolume * iGoodsCargoNum;

				//PrintUserCmdTextColorKV(iClientID, to_wstring(eqd->fVolume) 
				//	+ L"<|>" + to_wstring(iGoodsCargoNum) 
				//	+ L"<|>" + to_wstring(VolumeSizeGood) + L"<|>", 
				//	to_wstring(fRemainingHoldAll));

				iGoodsCargoOstatok = 0;
				GoodsToHoldPlayerOk = 0;
				if (gis->iType == 0)
				{
					if (VolumeSizeGood > fRemainingHoldAll && ToInt(ftows(fRemainingHoldAll)) != 0)
					{
						iGoodsCargoNum = ToInt(ftows(fRemainingHoldAll)) / (int)eqd->fVolume; //введённое в консоли равно свободному размеру трюма корабля

						iGoodsCargoOstatok = CountGoodsCargo - iGoodsCargoNum; //Остаток после отправки

						//PrintUserCmdTextColorKV(iClientID, to_wstring(iGoodsCargoNum) + L"|", to_wstring(fRemainingHoldAll) + L"|" + to_wstring(iGoodsCargoOstatok));
					}
				}

				EQ_TYPE eqType = HkGetEqType(eqd);

				if (guns->iArchID && eqType != ET_MUNITION)
				{
					CountListCagoHoldPlayer = 0;
					list<CARGO_INFO> lstCargoFF;
					int iRem;
					HkEnumCargo(ARG_CLIENTID(iClientID), lstCargoFF, iRem);
					foreach(lstCargoFF, CARGO_INFO, it)
					{
						Archetype::Equipment *eqvd = Archetype::GetEquipment(it->iArchID);
						const GoodInfo *gisd = GoodList::find_by_id(it->iArchID);
						Archetype::Gun *gundd = (Archetype::Gun *)eqvd;
						CountListCagoHoldPlayer++;
					}

					if (CountListCagoHoldPlayer <= maxSlotsPlayer)
					{
						float SizeGoodSend = eqd->fVolume;
						float VolumeSize = SizeGoodSend * CountGoodsCargo;

						if (VolumeSize > fRemainingHoldAll && ToInt(ftows(fRemainingHoldAll)) != 0)
						{
							PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1434"), 
								GetLocalized(iClientID, "MSG_1435") 
								+ HkGetWStringFromIDS(guns->iIdsName) 
								+ GetLocalized(iClientID, "MSG_1591") + to_wstring(CountGoodsCargo) + L"!");
							continue;
						}

						/*PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1233")
							+ to_wstring(VolumeSize)
							+ L"|CountGoodsCargo:" + to_wstring(CountGoodsCargo)
							+ L"|SizeGun:" + to_wstring(guns->fVolume)
							+ L"|SizeEqd:" + to_wstring(eqd->fVolume)
							+ L"|CountEmptyHoldPlayerCurrent:" + to_wstring(CountEmptyHoldPlayerCurrent));*/

						if (iGoodsCargoNum > 0 && fRemainingHoldAll != NULL && eqType != ET_MUNITION)
						{
							HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, CountGoodsCargo, false);
							IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(0));
							PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
								to_wstring(CountGoodsCargo));
						}
						////Если оружие имеет вес >= 1
						//if (guns->fVolume >= 1.0)
						//{
						//	//если количество предметов на [складе]
						//	//[меньше] чем свободных слотов у [игрока]
						//	if (CountGoodsCargo < CountEmptyHoldPlayerCurrent)
						//	{
						//		intcorrectvolume = (ToInt(ftows(fRemainingHoldAll)) / (int)eqd->fVolume);
						//		intcorrectvolumeostatok = CountGoodsCargo - intcorrectvolume; //Остаток после отправки
						//		if (intcorrectvolume != 0 && intcorrectvolume > CountGoodsCargo)
						//		{
						//			HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, CountGoodsCargo, false);
						//			IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(0));
						//			PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//				 to_wstring(CountGoodsCargo));
						//		}
						//		if (intcorrectvolume != 0 && intcorrectvolume == CountGoodsCargo)
						//		{
						//			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1234")
						//				+ to_wstring(intcorrectvolume));

						//			HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, intcorrectvolume, false);
						//			IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(0));
						//			PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//				 to_wstring(intcorrectvolume));
						//		}
						//		if (intcorrectvolume != 0 && intcorrectvolume < CountGoodsCargo)
						//		{
						//			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1235")
						//				+ to_wstring(intcorrectvolume));
						//			HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, intcorrectvolume, false);
						//			IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(intcorrectvolumeostatok));
						//			PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//				 to_wstring(intcorrectvolume));
						//		}
						//	}

						//	//если количество предметов на [складе]
						//	//[больше] чем свободных слотов у [игрока]
						//	if (CountGoodsCargo > CountEmptyHoldPlayerCurrent)
						//	{
						//		// количество вещей в трюме - остаток
						//		GoodsToHoldPlayerOk = CountGoodsCargo - CountEmptyHoldPlayerCurrent;
						//		// вычисляем сколько товара влезет в свободную часть корабля например 10000 влезет Воды в трюм на 10000
						//		intcorrectvolume = ToInt(ftows(fRemainingHoldAll)) / (int)eqd->fVolume;

						//		intcorrectvolumeostatok = GoodsToHoldPlayerOk + (CountEmptyHoldPlayerCurrent - intcorrectvolume); //Остаток после отправки

						//		// если кол-во влезаемого не равно нулю и не больше свободного места в корабле
						//		if (intcorrectvolume != 0 && intcorrectvolume < fRemainingHoldAll)
						//		{
						//			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1236")
						//				+ to_wstring(intcorrectvolume));

						//			HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, intcorrectvolume, false);
						//			IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(intcorrectvolumeostatok));
						//			PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//				 to_wstring(intcorrectvolume));
						//		}
						//		if (intcorrectvolume != 0 && intcorrectvolume > CountEmptyHoldPlayerCurrent)
						//		{
						//			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1237")
						//				+ to_wstring(CountEmptyHoldPlayerCurrent));

						//			HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, CountEmptyHoldPlayerCurrent, false);
						//			IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(CountGoodsCargo - CountEmptyHoldPlayerCurrent));
						//			PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//				 to_wstring(CountEmptyHoldPlayerCurrent));
						//		}
						//	}
						//}

						////если вес меньше еденицы  = 0
						//if (guns->fVolume < 1.0) {
						//	//если количество предметов на [складе]
						//	//[меньше] чем свободных слотов у [игрока]
						//	if (CountGoodsCargo < CountEmptyHoldPlayerCurrent)
						//	{
						//		HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, CountGoodsCargo, false);
						//		IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(0));
						//		PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//			to_wstring(CountGoodsCargo));
						//	}

						//	//если количество предметов на [складе]
						//	//[больше] чем свободных слотов у [игрока]
						//	if (CountGoodsCargo > CountEmptyHoldPlayerCurrent)
						//	{
						//		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1238")
						//			+ to_wstring(CountEmptyHoldPlayerCurrent));

						//		HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, CountEmptyHoldPlayerCurrent, false);
						//		IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(GoodsToHoldPlayerOk));
						//		PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//			 to_wstring(CountEmptyHoldPlayerCurrent));
						//	}

						//	//если количество предметов на [складе]
						//	//[равно] количеству свободных слотов у [игрока]
						//	if (CountGoodsCargo == CountEmptyHoldPlayerCurrent)
						//	{
						//		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1239")
						//			+ to_wstring(CountEmptyHoldPlayerCurrent));

						//		HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, CountEmptyHoldPlayerCurrent, false);
						//		IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(0));
						//		PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(guns->iIdsName) + L" = ",
						//			 to_wstring(CountEmptyHoldPlayerCurrent));
						//	}
						//}
					}
					else
					{	//Предел вещей достигнут
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1436"), 
							GetLocalized(iClientID, "MSG_1437") + to_wstring(maxSlotsPlayer) 
							+ GetLocalized(iClientID, "MSG_1592"));
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1438"), GetLocalized(iClientID, "MSG_1439"));
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1440"), GetLocalized(iClientID, "MSG_1441") 
							+ to_wstring(CountListCagoHoldPlayer));
						return;
					}
				}
				else
				{
					CountListCagoHoldPlayer = 0;
					list<CARGO_INFO> lstCargo;
					int iRem;
					HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
					foreach(lstCargo, CARGO_INFO, it)
					{
						Archetype::Equipment *eqvd = Archetype::GetEquipment(it->iArchID);
						const GoodInfo *gisd = GoodList::find_by_id(it->iArchID);
						Archetype::Gun *gundd = (Archetype::Gun *)eqvd;
						CountListCagoHoldPlayer++;
					}

					if (CountListCagoHoldPlayer <= maxSlotsPlayer)
					{
						float SizeGoodSend = eqd->fVolume;
						float VolumeSize = SizeGoodSend * CountGoodsCargo;

						if (VolumeSize > fRemainingHoldAll && ToInt(ftows(fRemainingHoldAll)) != 0)
						{
							PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1442"), GetLocalized(iClientID, "MSG_1443") + HkGetWStringFromIDS(gis->iIDSName) 
								+ GetLocalized(iClientID, "MSG_1591") + to_wstring(CountGoodsCargo) + L"!");
							continue;
						}
						if (iGoodsCargoNum > 0 && fRemainingHoldAll != NULL && eqType != ET_MUNITION)
						{
							HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, CountGoodsCargo, false);
							IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(guns->iArchID), itos(0));
							PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(gis->iIDSName) + L" = ",
								to_wstring(CountGoodsCargo));
						}
					}
					else //Предел вещей достигнут
					{
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1444"), GetLocalized(iClientID, "MSG_1445") 
							+ to_wstring(maxSlotsPlayer) + GetLocalized(iClientID, "MSG_1592"));
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1446"), GetLocalized(iClientID, "MSG_1447"));
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1448"), GetLocalized(iClientID, "MSG_1449") 
							+ to_wstring(CountListCagoHoldPlayer));
						return;
					}

					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1240") + to_wstring(iGoodsCargoNum));
					//если товар
					//if (fRemainingHoldAll >= NULL && iGoodsCargoNum > 0)
					//{
					//	CountListCagoHoldPlayer = 0;
					//	list<CARGO_INFO> lstCargo;
					//	int iRem;
					//	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
					//	foreach(lstCargo, CARGO_INFO, it)
					//	{
					//		Archetype::Equipment *eqvd = Archetype::GetEquipment(it->iArchID);
					//		const GoodInfo *gisd = GoodList::find_by_id(it->iArchID);
					//		Archetype::Gun *gundd = (Archetype::Gun *)eqvd;
					//		CountListCagoHoldPlayer++;
					//	}
					//	CountEmptyHoldPlayerCurrent = 150 - (int)CountListCagoHoldPlayer;
					//	if (CountEmptyHoldPlayerCurrent > 0 && iGoodsCargoNum > 0 && fRemainingHoldAll != NULL && eqType != ET_MUNITION)
					//	{
					//		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1241")
					//			+ to_wstring(iGoodsCargoNum));

					//		HkAddCargo(ARG_CLIENTID(iClientID), wscGoodsAll, iGoodsCargoNum, false);
					//		IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(wscGoodsAll), itos(iGoodsCargoOstatok));
					//		PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(gis->iIDSName) + L" = ",
					//			 to_wstring(iGoodsCargoNum));
					//	}
					//	if (CountEmptyHoldPlayerCurrent == 0)
					//	{
					//		//Предел вещей достигнут
					//		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1450"), GetLocalized(iClientID, "MSG_1451"));
					//		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1452"), GetLocalized(iClientID, "MSG_1453"));
					//		return;
					//	}
					//}
					//else {
					//	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1454"), GetLocalized(iClientID, "MSG_1455"));
					//	return;
					//}
				}
			}
		}
		return;
	}

	if (iType == 8192)
	{
		if (COUNT_REMOVE_GOOD <= 0)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1456"), GetLocalized(iClientID, "MSG_1457"));
			//ОШИБКА: нельзя перевести отрицательные или нулевые суммы
			return;
		}
		if (NAME_GOOD == 0 && COUNT_REMOVE_GOOD == 0)
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1458"), 
				GetLocalized(iClientID, "MSG_1593"));
			//необходимо ввести количество товара/ ввести айди товара
		}
		else
		{
			iGoods = IniGetI(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(NAME_GOOD), 0);
			//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1242") + to_wstring(iGoods));
			Archetype::Equipment *eq = Archetype::GetEquipment(NAME_GOOD);
			EQ_TYPE eqType = HkGetEqType(eq);
			//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1243") + to_wstring(iGoods));

			if (iGoods == 0 || eqType == ET_MUNITION)
			{

				bool ket = false;
				foreach(CargoNameList, wstring, i)
				{
					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1244") + to_wstring(NAME_GOOD) + L"|" + (*i));
					if (to_wstring(NAME_GOOD) == (*i))
					{
						HkAddCargo(ARG_CLIENTID(iClientID), NAME_GOOD, COUNT_REMOVE_GOOD, false);
						IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(NAME_GOOD), itos(iGoods - COUNT_REMOVE_GOOD));
						const GoodInfo *gir = GoodList::find_by_id(NAME_GOOD);
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1459"), HkGetWStringFromIDS(gir->iIDSName) 
							+ L" = " + to_wstring(COUNT_REMOVE_GOOD));
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1479"), GetLocalized(iClientID, "MSG_1461"));
						ket = true;
					}
					if (!ket)
					{	//Товаров не найдено
						PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1462"), 
							GetLocalized(iClientID, "MSG_1594"));
					}
				}
			}
			else
			{
				//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1245") + to_wstring(iGoods));


				if (iGoods - COUNT_REMOVE_GOOD < 0)//Число товаров минус введённое в консоли
				{
					PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1463"), GetLocalized(iClientID, "MSG_1464"));
				}
				else
				{
					const GoodInfo* id = GoodList::find_by_archetype(NAME_GOOD);

					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1246") + to_wstring(NAME_GOOD));

					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);

					if (id->iType == 0)
					{
						if (eq->fVolume * COUNT_REMOVE_GOOD > fRemainingHold && ToInt(ftows(fRemainingHold)) != 0)
						{
							PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1465"), GetLocalized(iClientID, "MSG_1466"));
							COUNT_REMOVE_GOOD = ToInt(ftows(fRemainingHold)) / (int)eq->fVolume; //введённое в консоли равно размеру трюма корабля
						}
					}
					Archetype::Ship* ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					list<CARGO_INFO> lstCargo;
					int iRem;
					HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1247") + to_wstring(NAME_GOOD));

					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; } //cargo это число товара на складе
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (NAME_GOOD == iNanobotsID)
						{
							uint amount = nCount + COUNT_REMOVE_GOOD;////количесвто наноботов указанное в консоли плюс число наноботов в трюме
							if (amount > ship->iMaxNanobots)
							{
								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1467"), GetLocalized(iClientID, "MSG_1468"));
								if (nCount <= ship->iMaxNanobots)
								{
									COUNT_REMOVE_GOOD = ship->iMaxNanobots - nCount;//wscCount = ship->iMaxNanobots-nCount;
								}
							}
						}
						if (NAME_GOOD == iShieldBatID)
						{
							uint amount = sCount + COUNT_REMOVE_GOOD;//количесвто аккумов указанное в консоли плюс число наноботов в трюме
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1469"), GetLocalized(iClientID, "MSG_1470"));
								if (sCount <= ship->iMaxShieldBats)
								{
									COUNT_REMOVE_GOOD = ship->iMaxShieldBats - sCount;//wscCount = ship->iMaxShieldBats-sCount
								}
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == NAME_GOOD) { uCount = cargo->iCount; }
							if (COUNT_REMOVE_GOOD + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1471"), GetLocalized(iClientID, "MSG_1472"));
								COUNT_REMOVE_GOOD = MAX_PLAYER_AMMO - uCount;
							}
							if (eq->fVolume * COUNT_REMOVE_GOOD > fRemainingHold)
							{
								PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1473"), GetLocalized(iClientID, "MSG_1474"));
								COUNT_REMOVE_GOOD = 0;
							}
						}
					}
					Archetype::Gun* gun = (Archetype::Gun*)eq;
					if (gun->iArchID)// если не здесь, предметы будут складываться и не показывать количество
					{
						//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1248") + to_wstring(NAME_GOOD));

						if (fRemainingHold != NULL && COUNT_REMOVE_GOOD != 0)
						{
							HkAddCargo(ARG_CLIENTID(iClientID), NAME_GOOD, COUNT_REMOVE_GOOD, false);
							IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(NAME_GOOD), itos(iGoods - COUNT_REMOVE_GOOD));
							PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1475"),
								HkGetWStringFromIDS(gun->iIdsName) + L" = " + to_wstring(COUNT_REMOVE_GOOD));
						}
						else
						{
							PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1476"), GetLocalized(iClientID, "MSG_1477"));
						}
					}
					else
					{
						//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1249") + to_wstring(NAME_GOOD));

						if (fRemainingHold != NULL && COUNT_REMOVE_GOOD != 0)
						{
							HkAddCargo(ARG_CLIENTID(iClientID), NAME_GOOD, COUNT_REMOVE_GOOD, false);
							IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), utos(NAME_GOOD), itos(iGoods - COUNT_REMOVE_GOOD));
							const GoodInfo* gir = GoodList::find_by_id(NAME_GOOD);
							PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1478"), HkGetWStringFromIDS(gir->iIDSName) 
								+ L" = " + to_wstring(COUNT_REMOVE_GOOD));
							PrintUserCmdTextColorKV(iClientID, L"...", GetLocalized(iClientID, "MSG_1480"));
						}
						else
						{
							PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1481"), GetLocalized(iClientID, "MSG_1482"));
						}
					}
					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1250") + to_wstring(NAME_GOOD));

				}
			}
		}
	}
	else
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1483"), GetLocalized(iClientID, "MSG_1484"));
	}
}

void UserCmd_Istore(uint iClientID, const wstring& wscParam)
{
	CAccount* acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	scUserStore = scAcctPath + wstos(wscDir) + "\\store.ini";
	list<INISECTIONVALUE> lstGoods;
	IniGetSection(scUserStore, wstos(HkGetPlayerSystem(iClientID)), lstGoods);

	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1251") + to_wstring(lstGoods.size()));

	// УДАЛЯЮ ВСЕ ПУСТЫЕ
	foreach(lstGoods, INISECTIONVALUE, it)
	{
		// СКОЛЬКО ЕСТЬ НА СКЛАДЕ
		int COUNT_GOOD = ToInt(stows(it->scValue).c_str());
		if (COUNT_GOOD <= 0)
		{
			//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1252") + stows(it->scKey));
			lstGoods.erase(it);
		}
	}

	wstring numberParam = ToLower(GetParam(wscParam, ' ', 0));
	int maxElemenets = countMaxLinesSinfo;
	int skipElements = maxElemenets * ToInt(numberParam);
	int size = lstGoods.size() / countMaxLinesSinfo;
	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1253") + to_wstring(lstGoods.size()));

	int countErase = 0;
	for (auto it = lstGoods.begin(); it != lstGoods.end(); )
	{
		countErase++;

		if (countErase <= skipElements) {
			it = lstGoods.erase(it);
			int iGoods = ToInt(stows(it->scValue).c_str());
			//PrintUserCmdText(iClientID, stows(it->scKey) + L"$| maxElemenets::" + to_wstring(maxElemenets)
				//+ L"| numberParam::" + numberParam
				//+ L"| numberParam::" + to_wstring(skipElements)
				//+ L"|" + to_wstring(iGoods));
		}
		else {
			++it;
		}
	}

	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1254") + to_wstring(lstGoods.size()));

	if (lstGoods.size() == 0)
	{
		PrintUserCmdTextColorKV(iClientID, to_wstring(ToInt(numberParam)) + L" | " + to_wstring(size),
			GetLocalized(iClientID, "MSG_1499") + to_wstring(size) + L")");
		return;
	}

	int count = 0;
	wstring keyGColol = stows(colorKeyType0);
	wstring keyGValue = stows(colorValueType0);
	wstring keyCColol = stows(colorKeyType1);
	wstring keyCValue = stows(colorValueType1);

	foreach(lstGoods, INISECTIONVALUE, it3)
	{
		int iGoods = ToInt(stows(it3->scValue).c_str());
		if (iGoods > 0)
		{
			//IniWrite(scUserStore, wstos(HkGetPlayerSystem(iClientID)), it3->scKey, it3->scValue);
			count = count + 1;
			if (count > maxElemenets)
				break;
			int index = count + skipElements;

			Archetype::Equipment* eq = Archetype::GetEquipment(ToInt(stows(it3->scKey).c_str()));
			const GoodInfo* gi = GoodList::find_by_id(ToInt(stows(it3->scKey).c_str()));
			if (!gi)
				continue;
			Archetype::Gun* gun = (Archetype::Gun*)eq;
			if (gun->iArchID)
			{
				if (gi->iType == 0)
				{
					PrintUserCmdTextColorKVCustom(iClientID, to_wstring(index) + L".  ", keyCColol, 
						HkGetWStringFromIDS(gun->iIdsName) + L" = " + stows(it3->scValue), keyCValue);
				}
				else PrintUserCmdTextColorKVCustom(iClientID, to_wstring(index) + L".  ", keyGColol,
					HkGetWStringFromIDS(gun->iIdsName) + L" = " + stows(it3->scValue), keyGValue);
			}
			else
			{
				PrintUserCmdTextColorKV(iClientID, to_wstring(index) + L": [?] ",
					HkGetWStringFromIDS(gi->iIDSName) + L" = " + stows(it3->scValue));
			}
		}
	}

	PrintUserCmdTextColorKV(iClientID, to_wstring(ToInt(numberParam)) + L" | " + to_wstring(size), 
		GetLocalized(iClientID, "MSG_1499") + to_wstring(size) + L")");
}

std::wstring WStringToCP1251(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(1251, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(1251, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return stows(strTo);
}

void UserCmd_CargoSystems(uint iClientID, const wstring &wscParam)
{
	CAccount *acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	string scUserStore = scAcctPath + wstos(wscDir) + "\\store.ini";

	INI_Reader ini;
	list<INISECTIONVALUE> lstGoods;
	if (!(ini.open(scUserStore.c_str(), false)))
		return;

	while (ini.read_header())
	{
		char buffer[256] = "";
		uint iSystemID = 0;
		string header = ini.get_header_ptr();
		pub::GetSystemID(iSystemID, header.c_str()); //получаю айди системы

		IniGetSection(scUserStore, header, lstGoods); //получаю список груза в системе
		int count = lstGoods.size(); //получаю число предметов

		string id = itos((int)iSystemID);
		const Universe::ISystem *iSystem = Universe::get_system(iSystemID);

		sprintf(buffer, "<TRA data=\"0xFF920540\" mask=\"-1\"/><TEXT> %s</TEXT><TRA data=\"0x00CC0000\" mask=\"-1\"/><TEXT>(%i) </TEXT>",
			wstos(HkGetWStringFromIDS(iSystem->strid_name)).c_str(), count);
		HkFMsg(iClientID, stows(buffer));

		lstGoods.clear();
	}

	ini.close();
}

void UserCmd_SearchGood(uint iClientID, const wstring& wscParam)
{
	wstring wscShipParam = ToLower(GetParam(wscParam, ' ', 0));

	if (!wscShipParam.length()) // Если параметр не был указан, выполните итерацию по списку.
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1485"), GetLocalized(iClientID, "MSG_1486"));
		return;
	}

	CAccount* acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	string scUserStore = scAcctPath + wstos(wscDir) + "\\store.ini";

	INI_Reader ini;
	list<INISECTIONVALUE> lstGoods;
	if (!(ini.open(scUserStore.c_str(), false)))
		return;

	int countSearch = 0;

	while (ini.read_header())
	{
		char buffer[256] = "";
		uint iSystemID = 0;
		string header = ini.get_header_ptr();
		pub::GetSystemID(iSystemID, header.c_str()); //получаю айди системы

		IniGetSection(scUserStore, header, lstGoods); //получаю список груза в системе
		int count = lstGoods.size(); //получаю число предметов
		
		string id = itos((int)iSystemID);
		const Universe::ISystem* iSystem = Universe::get_system(iSystemID);

		foreach(lstGoods, INISECTIONVALUE, good)
		{
			uint goodId = ToUInt(stows(good->scKey));
			const GoodInfo* gir = GoodList::find_by_id(goodId);
			wstring nameGood = HkGetWStringFromIDS(gir->iIDSName);

			if (nameGood.find(wscShipParam) < 1000)
			{
				countSearch++;
				PrintUserCmdTextColorKV(iClientID, to_wstring(countSearch) + L": " 
					+ nameGood + GetLocalized(iClientID, "MSG_1595"), HkGetWStringFromIDS(iSystem->strid_name)
					+ GetLocalized(iClientID, "MSG_1591") + stows(good->scValue));
				if (countSearch > 100)
					return;
			}
		}

		lstGoods.clear();
	}

	if (countSearch == 0)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1487"), GetLocalized(iClientID, "MSG_1488"));
	}

	ini.close();
}

void UserCmd_EnumCargo(uint iClientID, const wstring &wscParam)
{
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
	uint iNum = 0;
	foreach(lstCargo, CARGO_INFO, it)
	{
		Archetype::Equipment *eq = Archetype::GetEquipment(it->iArchID);
		const GoodInfo *gi = GoodList::find_by_id(it->iArchID);
		if (!gi)
			continue;
		Archetype::Gun *gun = (Archetype::Gun *)eq;
		if (!it->bMounted)
		{
			if (gun->iArchID && it->iCount != 0)
			{
				PrintUserCmdTextColorKV(iClientID, to_wstring(it->iID) + L": ", HkGetWStringFromIDS(gun->iIdsName) + L" = " + to_wstring(it->iCount));
			}
			else
			{
				if (it->iCount != 0)
				{
					PrintUserCmdTextColorKV(iClientID, to_wstring(it->iID) + L": ", HkGetWStringFromIDS(gi->iIDSName) + L" = " + to_wstring(it->iCount));
				}
			}
			iNum++;
		}
	}
	if (!iNum)
	{
		PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1489"), GetLocalized(iClientID, "MSG_1490"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void(*_UserCmdProc)(uint, const wstring &);

struct USERCMD
{
	wchar_t *wszCmd;
	_UserCmdProc proc;
};

USERCMD UserCmds[] =
{
	{ L"/shelp",				UserCmd_Help },
	{ L"/store",				UserCmd_Store },
	{ L"/unstore",			    UserCmd_Ustore},
	{ L"/sinfo",			    UserCmd_Istore},
	{ L"/enumcargo",            UserCmd_EnumCargo},
	{ L"/cargosystems",         UserCmd_CargoSystems},
	{ L"/carsys",				UserCmd_CargoSystems},
	{ L"/searchgood",			UserCmd_SearchGood},
	{ L"/sh",					UserCmd_SearchGood},
};

EXPORT bool UserCmd_Process(uint iClientID, const wstring &wscCmd)
{
	wstring wscCmdLower = ToLower(wscCmd);

	for (uint i = 0; (i < sizeof(UserCmds) / sizeof(USERCMD)); i++)
	{
		if (wscCmdLower.find(ToLower(UserCmds[i].wszCmd)) == 0)
		{
			wstring wscParam = L"";
			if (wscCmd.length() > wcslen(UserCmds[i].wszCmd))
			{
				if (wscCmd[wcslen(UserCmds[i].wszCmd)] != ' ')
					continue;
				wscParam = wscCmd.substr(wcslen(UserCmds[i].wszCmd) + 1);
			}
			UserCmds[i].proc(iClientID, wscParam);

			returncode = SKIPPLUGINS_NOFUNCTIONCALL; // мы обработали команду, немедленно возвращаемся
			return true;
		}
	}

	returncode = DEFAULT_RETURNCODE; // мы не обрабатывали команду, поэтому позвольте другим плагинам или FLHook включиться
	return false;
}

wstring HkGetWStringFromIDS(uint iIDS) //Only works for names
{
	if (!iIDS)
		return L"";

	uint iDLL = iIDS / 0x10000;
	iIDS -= iDLL * 0x10000;

	wchar_t wszBuf[512];
	if (LoadStringW(vDLLss[iDLL], iIDS, wszBuf, 512))
		return wszBuf;
	return L"";
}

void HkLoadDLLConf(const char *szFLConfigFile)
{
	for (uint i = 0; i < vDLLss.size(); i++)
	{
		FreeLibrary(vDLLss[i]);
	}
	vDLLss.clear();
	HINSTANCE hDLL = LoadLibraryEx((char*)((char*)GetModuleHandle(0) + 0x256C4), NULL, LOAD_LIBRARY_AS_DATAFILE); //typically resources.dll
	if (hDLL)
		vDLLss.push_back(hDLL);
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
							vDLLss.push_back(hDLL);
					}
				}
			}
		}
		ini.close();
	}
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "Shared store plugin by Dvurechensky";
	p_PI->sShortName = "store";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Help, PLUGIN_UserCmd_Help, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	return p_PI;
}