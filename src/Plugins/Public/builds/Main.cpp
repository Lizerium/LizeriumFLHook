/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 12 апреля 2026 14:16:58
 * Version: 1.0.27
 */

#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"
#include <vector>
#define ADDR_FLCONFIG 0x25410

wstring HkGetWStringFromIDS(uint iIDS);
void PrintListBuilds(std::list<INISECTIONVALUE>::iterator& builds, std::list<INISECTIONVALUE>& lstParts, std::list<CARGO_INFO>& lstCargo, int& counter, std::wstring& List, std::string file, uint iClientID);
void HkLoadDLLConf(const char *szFLConfigFile);
vector<HINSTANCE> vDLLs;
PLUGIN_RETURNCODE returncode;

//* WEAPON
string set_scBuildFile_Equip;
int countFirstListEl_Equ;
list<INISECTIONVALUE> lstBuilds_Equ_1;
list<INISECTIONVALUE> lstBuilds_Equ_2;
//
int counter_Equ_2;
//*

//* WEAPON
string set_scBuildFile_Weapon;
int countFirstListEl_Wea;
list<INISECTIONVALUE> lstBuilds_Wea_1;
list<INISECTIONVALUE> lstBuilds_Wea_2;
//
int counter_Wea_2;
//*

//* AMUNITION
string set_scBuildFile_Amun;
int countFirstListEl_Amu;
list<INISECTIONVALUE> lstBuilds_Amu_1;
list<INISECTIONVALUE> lstBuilds_Amu_2;
//
int counter_Amu_2;
//*


//* LICENCE
string set_scBuildFile_Lic;
int countFirstListEl_Lic;
list<INISECTIONVALUE> lstBuilds_Lic_1;
list<INISECTIONVALUE> lstBuilds_Lic_2;
//
int counter_Lic_2;
//*

EXPORT void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	char *szFLConfig = (char*)((char*)GetModuleHandle(0) + ADDR_FLCONFIG);
	HkLoadDLLConf(szFLConfig);

	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	
	lstBuilds_Wea_1.clear();
	lstBuilds_Wea_2.clear();
	lstBuilds_Amu_1.clear();
	lstBuilds_Amu_2.clear();
	lstBuilds_Lic_1.clear();
	lstBuilds_Lic_2.clear();
	lstBuilds_Equ_1.clear();
	lstBuilds_Equ_2.clear();

	// EQUIPMENT
	set_scBuildFile_Equip = string(szCurDir) + "\\flhook_plugins\\builde.ini";

	countFirstListEl_Equ = 50;
	IniGetSection(set_scBuildFile_Equip, "build", lstBuilds_Equ_1);
	lstBuilds_Equ_1.resize(countFirstListEl_Equ);

	counter_Equ_2 = 50;
	IniGetSection(set_scBuildFile_Equip, "build", lstBuilds_Equ_2);
	int count = 0;
	for (auto it = lstBuilds_Equ_2.begin(); it != lstBuilds_Equ_2.end(); ) {
		count++;
		if (count <= counter_Equ_2) {
			it = lstBuilds_Equ_2.erase(it);
		}
		else {
			++it;
		}
	}
	// END EQUIPMENT

	// WEAPON
	set_scBuildFile_Weapon = string(szCurDir) + "\\flhook_plugins\\buildw.ini";

	countFirstListEl_Wea = 50;
	IniGetSection(set_scBuildFile_Weapon, "build", lstBuilds_Wea_1);
	lstBuilds_Wea_1.resize(countFirstListEl_Wea);

	counter_Wea_2 = 50;
	IniGetSection(set_scBuildFile_Weapon, "build", lstBuilds_Wea_2);
	count = 0;
	for (auto it = lstBuilds_Wea_2.begin(); it != lstBuilds_Wea_2.end(); ) {
		count++;
		if (count <= counter_Wea_2) {
			it = lstBuilds_Wea_2.erase(it);
		}
		else {
			++it;
		}
	}
	// END WEAPON
	
	// AMUNITION
	set_scBuildFile_Amun = string(szCurDir) + "\\flhook_plugins\\builda.ini";

	countFirstListEl_Amu = 50;
	IniGetSection(set_scBuildFile_Amun, "build", lstBuilds_Amu_1);
	lstBuilds_Amu_1.resize(countFirstListEl_Amu);

	counter_Amu_2 = 50;
	IniGetSection(set_scBuildFile_Amun, "build", lstBuilds_Amu_2);
	count = 0;
	for (auto it = lstBuilds_Amu_2.begin(); it != lstBuilds_Amu_2.end(); ) {
		count++;
		if (count <= counter_Lic_2) {
			it = lstBuilds_Amu_2.erase(it);
		}
		else {
			++it;
		}
	}
	// END AMUNITION
	
	// LICENCE
	set_scBuildFile_Lic = string(szCurDir) + "\\flhook_plugins\\buildl.ini";

	countFirstListEl_Lic = 65;
	IniGetSection(set_scBuildFile_Lic, "build", lstBuilds_Lic_1);
	lstBuilds_Lic_1.resize(countFirstListEl_Lic);

	counter_Lic_2 = 65;
	IniGetSection(set_scBuildFile_Lic, "build", lstBuilds_Lic_2);
	count = 0;
	for (auto it = lstBuilds_Lic_2.begin(); it != lstBuilds_Lic_2.end(); ) {
		count++;
		if (count <= counter_Lic_2) {
			it = lstBuilds_Lic_2.erase(it);
		}
		else {
			++it;
		}
	}
	// END LICENCE
}

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
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0417"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0418"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0419"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0420"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0421"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0422"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0423"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0424"));
}

void UserCmd_eBuild_1(uint iClientID, const wstring &wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}
	int counter = 0;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Equ_1, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Equip, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;
					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo *gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = 0;
			bool create = false;
			foreach(lstBuilds_Equ_1, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment *eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo *id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume*wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship *ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > ship->iMaxNanobots)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = 0;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume*wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Equip, builds->scKey.c_str(), lstParts);
						foreach(lstParts, INISECTIONVALUE, parts)
						{
							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));
								if (PartsID == cargo->iArchID)
								{
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if (BuildNumber < 0 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if (!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	foreach(lstBuilds_Equ_1, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Equip, iClientID);
	}
	HkFMsg(iClientID, List);
}

void UserCmd_eBuild_2(uint iClientID, const wstring &wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}
	int counter = counter_Equ_2;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Equ_2, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Equip, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;
					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo *gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = counter_Equ_2;
			bool create = false;
			foreach(lstBuilds_Equ_2, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment *eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo *id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume*wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship *ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > ship->iMaxNanobots)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = 0;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume*wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Equip, builds->scKey.c_str(), lstParts);
						foreach(lstParts, INISECTIONVALUE, parts)
						{
							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));
								if (PartsID == cargo->iArchID)
								{
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if(BuildNumber < counter_Equ_2 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if(!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	foreach(lstBuilds_Equ_2, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Equip, iClientID);
	}
	HkFMsg(iClientID, List);
}

void UserCmd_wBuild_1(uint iClientID, const wstring &wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}
	int counter = 0;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Wea_1, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Weapon, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;

					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo *gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = 0;
			bool create = false;
			foreach(lstBuilds_Wea_1, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment *eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo *id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume*wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship *ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > 0)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = ship->iMaxNanobots - nCount;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume*wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Weapon, builds->scKey.c_str(), lstParts);
						foreach(lstParts, INISECTIONVALUE, parts)
						{
							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));
								if (PartsID == cargo->iArchID)
								{
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if (BuildNumber < 0 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if (!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	foreach(lstBuilds_Wea_1, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Weapon, iClientID);
	}
	HkFMsg(iClientID, List);
}

void UserCmd_wBuild_2(uint iClientID, const wstring &wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}
	int counter = counter_Wea_2;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Wea_2, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Weapon, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;
					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo *gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = counter_Wea_2;
			bool create = false;
			foreach(lstBuilds_Wea_2, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment *eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo *id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume*wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship *ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > 0)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = ship->iMaxNanobots - nCount;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume*wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Weapon, builds->scKey.c_str(), lstParts);
						foreach(lstParts, INISECTIONVALUE, parts)
						{
							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));
								if (PartsID == cargo->iArchID)
								{
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if(BuildNumber < counter_Wea_2 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if (!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	foreach(lstBuilds_Wea_2, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Weapon, iClientID);
	}
	HkFMsg(iClientID, List);
}

void UserCmd_aBuild_1(uint iClientID, const wstring &wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}
	int counter = 0;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Amu_1, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Amun, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;
					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo *gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = 0;
			bool create = false;
			foreach(lstBuilds_Amu_1, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment *eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo *id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume*wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship *ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > ship->iMaxNanobots)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = 0;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume*wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Amun, builds->scKey.c_str(), lstParts);
						foreach(lstParts, INISECTIONVALUE, parts)
						{
							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));
								if (PartsID == cargo->iArchID)
								{
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if (BuildNumber < 0 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if (!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	foreach(lstBuilds_Amu_1, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Amun, iClientID);
	}
	HkFMsg(iClientID, List);
}

void UserCmd_aBuild_2(uint iClientID, const wstring& wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}
	int counter = counter_Amu_2;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Amu_2, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Amun, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;
					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo* gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = counter_Amu_2;
			bool create = false;
			foreach(lstBuilds_Amu_2, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment* eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo* id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume * wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship* ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > ship->iMaxNanobots)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = 0;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume * wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Amun, builds->scKey.c_str(), lstParts);
						foreach(lstParts, INISECTIONVALUE, parts)
						{
							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));
								if (PartsID == cargo->iArchID)
								{
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if (BuildNumber < counter_Amu_2 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if (!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	foreach(lstBuilds_Amu_2, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Amun, iClientID);
	}
	HkFMsg(iClientID, List);
}

void UserCmd_lBuild_1(uint iClientID, const wstring& wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}

	int counter = 0;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Lic_1, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Lic, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;
					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo* gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = 0;
			bool create = false;
			foreach(lstBuilds_Lic_1, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment* eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo* id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume * wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship* ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > ship->iMaxNanobots)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = 0;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume * wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Lic, builds->scKey.c_str(), lstParts);
						//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0504") + to_wstring((lstParts.size())));

						foreach(lstParts, INISECTIONVALUE, parts)
						{
							//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0505") + to_wstring((lstCargo.size())));

							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));

								if (PartsID == cargo->iArchID)
								{
									//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0506") + to_wstring((PartsID)));
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if (BuildNumber < 0 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if (!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	foreach(lstBuilds_Lic_1, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Lic, iClientID);
	}
	HkFMsg(iClientID, List);
}

void UserCmd_lBuild_2(uint iClientID, const wstring& wscParam)
{
	uint iShip = 0;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0425"));
		return;
	}

	int counter = counter_Lic_2;
	int countgoods = 0;
	int countgoodsT = 0;
	int BuildNumber = ToInt(GetParam(wscParam, ' ', 0));
	list<INISECTIONVALUE> lstParts;
	list<CARGO_INFO> lstCargo;
	int iRem;
	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);

	if (wscParam.length() > 0)
	{
		foreach(lstBuilds_Lic_2, INISECTIONVALUE, builds)
		{
			if (counter == BuildNumber)
			{
				IniGetSection(set_scBuildFile_Lic, builds->scKey.c_str(), lstParts);
				foreach(lstParts, INISECTIONVALUE, parts)
				{
					uint PartsID = CreateID(parts->scKey.c_str());
					countgoodsT++;
					uint iNum = 0;
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						const GoodInfo* gi = GoodList::find_by_id(cargo->iArchID);
						if (!gi)
							continue;
						if (!cargo->bMounted && gi->iIDS)
						{
							iNum++;
							if (cargo->iArchID == PartsID && cargo->iCount >= ToInt(stows(parts->scValue)))
							{
								countgoods++;
							}
						}
					}
					if (!iNum)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0426"));
						return;
					}
				}
			}
			counter++;
		}
		if (countgoods == countgoodsT)
		{
			counter = counter_Lic_2;
			bool create = false;
			foreach(lstBuilds_Lic_2, INISECTIONVALUE, builds)
			{
				uint wscGoods = CreateID(builds->scKey.c_str());
				if (counter == BuildNumber)
				{
					int wscCount = 1;
					Archetype::Equipment* eq = Archetype::GetEquipment(wscGoods);
					const GoodInfo* id = GoodList::find_by_archetype(wscGoods);
					if (!id)
						continue;
					float fRemainingHold;
					pub::Player::GetRemainingHoldSize(iClientID, fRemainingHold);
					if (id->iType == 0)
					{
						if (eq->fVolume * wscCount > fRemainingHold)
						{
							PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
							wscCount = 0;
						}
					}
					int nCount = 0;
					int sCount = 0;
					uint iNanobotsID = 2911012559;
					uint iShieldBatID = 2596081674;
					Archetype::Ship* ship = Archetype::GetShip(Players[iClientID].iShipArchetype);
					foreach(lstCargo, CARGO_INFO, cargo)
					{
						if (cargo->iArchID == iNanobotsID) { nCount = cargo->iCount; }
						if (cargo->iArchID == iShieldBatID) { sCount = cargo->iCount; }
						if (wscGoods == iNanobotsID)
						{
							uint amount = nCount + wscCount;
							if (amount > ship->iMaxNanobots)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0428"));
								wscCount = 0;
							}
						}
						if (wscGoods == iShieldBatID)
						{
							uint amount = sCount + wscCount;
							if (amount > ship->iMaxShieldBats)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0429"));
								wscCount = 0;
							}
						}
					}
					if (id->iType == 1)
					{
						int uCount = 0;
						foreach(lstCargo, CARGO_INFO, cargo)
						{
							if (cargo->iArchID == wscGoods) { uCount = cargo->iCount; }
							if (wscCount + uCount > MAX_PLAYER_AMMO)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0430"));
								wscCount = 0;
							}
							if (eq->fVolume * wscCount > fRemainingHold)
							{
								PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0427"));
								wscCount = 0;
							}
						}
					}
					if (wscCount == 1)
					{
						IniGetSection(set_scBuildFile_Lic, builds->scKey.c_str(), lstParts);
						foreach(lstParts, INISECTIONVALUE, parts)
						{
							foreach(lstCargo, CARGO_INFO, cargo)
							{
								uint PartsID = CreateID(parts->scKey.c_str());
								int iCargo = ToInt(stows(parts->scValue));
								if (PartsID == cargo->iArchID)
								{
									HkRemoveCargo(ARG_CLIENTID(iClientID), cargo->iID, iCargo);
								}
							}
						}
						HkAddCargo(ARG_CLIENTID(iClientID), wscGoods, wscCount, false);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0432"));
						create = true;
					}
					//Archetype::Gun *gun = (Archetype::Gun *)eq;
					//if (gun->iArchID)//if not here items will stack and not show the amount
					//{
					//	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0433"));
					//}
				}
				counter++;
			}

			if (BuildNumber < counter_Lic_2 || BuildNumber >= counter)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0434"));
			if (!create)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0435"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0436"));
		}
		return;
	}
	wstring List = L"";
	
	foreach(lstBuilds_Lic_2, INISECTIONVALUE, builds)
	{
		PrintListBuilds(builds, lstParts, lstCargo, counter, List, set_scBuildFile_Lic, iClientID);
	}
	HkFMsg(iClientID, List);
}

void PrintListBuilds(std::list<INISECTIONVALUE>::iterator& builds, 
	std::list<INISECTIONVALUE>& lstParts, 
	std::list<CARGO_INFO>& lstCargo, 
	int& counter, 
	std::wstring& List,
	std::string file,
	uint iClientID)
{
	char buffer[256] = "";
	uint BuildsID = CreateID(builds->scKey.c_str());
	IniGetSection(file, builds->scKey.c_str(), lstParts);
	const GoodInfo* gi = GoodList::find_by_id(BuildsID);

	list<bool> globalOks;
	foreach(lstParts, INISECTIONVALUE, parts)
	{
		foreach(lstCargo, CARGO_INFO, cargo)
		{
			const GoodInfo* gi = GoodList::find_by_id(cargo->iArchID);
			int countLoot = ToInt(stows(parts->scValue));
			if (!gi)
				continue;
			if (!cargo->bMounted && gi->iIDS)
			{
				uint id_Cargo = CreateID(parts->scKey.c_str());
				if (id_Cargo == cargo->iArchID)
				{
					int count_User_Loot = cargo->iCount;
					if (count_User_Loot >= countLoot)
						globalOks.push_back(true);
					break;
				}
			}
		}
	}
	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0524") + to_wstring(globalOks.size()) + L"|" + to_wstring(lstParts.size()));
	string defaultKeyStartColor = "0x000FFF01";
	string defaultValueStartColor = "0xBEBEBE90";

	if (globalOks.size() == lstParts.size())
	{
		defaultValueStartColor = "0xEBD05790";
		defaultKeyStartColor = "0x7311C101";
	}

	string magStart = "<TRA data=\""
		+ defaultKeyStartColor + "\" mask=\"-1\"/><TEXT> ("
		+ to_string(counter) + ")</TEXT><TRA data=\""
		+ defaultValueStartColor + "\" mask=\"-1\"/><TEXT>"
		+ wstos(HkGetWStringFromIDS(gi->iIDSName)).c_str() + "--></TEXT>";

	snprintf(buffer, sizeof(buffer), magStart.c_str());

	List += stows(buffer);
	counter++;
	char buffer1[256];

	foreach(lstParts, INISECTIONVALUE, parts)
	{
		char buffer1[256] = "";
		uint PartsID = CreateID(parts->scKey.c_str());
		const GoodInfo* gp = GoodList::find_by_id(PartsID);

		string nameLoot = wstos(HkGetWStringFromIDS(gp->iIDSName)).c_str();
		int countLoot = ToInt(stows(parts->scValue));
		string countLootString = parts->scValue;
		string defaulteKeyColor = "0xFF920540";
		string defaulteValueColor = "0x00CC0000";
		int needCount = 0;

		foreach(lstCargo, CARGO_INFO, cargo)
		{
			const GoodInfo* gi = GoodList::find_by_id(cargo->iArchID);
			if (!gi)
				continue;
			if (!cargo->bMounted && gi->iIDS)
			{
				uint id_Cargo = CreateID(parts->scKey.c_str());
				//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0525") + to_wstring(id_Cargo) + L"|" + to_wstring(cargo->iArchID));
				if (id_Cargo == cargo->iArchID)
				{
					defaulteKeyColor = "0x7D480240";
					int count_User_Loot = cargo->iCount;
					//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0526") + to_wstring(count_User_Loot) + L"|" + to_wstring(countLoot));
					if (count_User_Loot >= countLoot)
						defaulteValueColor = "0x006B0000";
					if (count_User_Loot < countLoot)
						needCount = countLoot - count_User_Loot;
				}
			}
		}

		if (needCount > 0)
			countLootString = countLootString + "(РЅСѓР¶РЅРѕ РµС‰С‘ " + to_string(needCount) + ")";

		string msg = "<TRA data=\"" + defaulteKeyColor + "\" mask=\"-1\"/><TEXT> "
			+ nameLoot + "</TEXT><TRA data=\""
			+ defaulteValueColor + "\" mask=\"-1\"/><TEXT>="
			+ countLootString + "; </TEXT>";

		snprintf(buffer1, sizeof(buffer1), msg.c_str());
		List += stows(buffer1);
	}

	HkFMsg(iClientID, List);
	List = L"";
}

void UserCmd_aBuildHelp(uint iClientID, const wstring& wscParam)
{
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1338"), GetLocalized(iClientID, "MSG_1339"));
}

void UserCmd_wBuildHelp(uint iClientID, const wstring& wscParam)
{
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1340"), GetLocalized(iClientID, "MSG_1341"));
}

void UserCmd_eBuildHelp(uint iClientID, const wstring& wscParam)
{
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1342"), GetLocalized(iClientID, "MSG_1343"));
}

void UserCmd_lBuildHelp(uint iClientID, const wstring& wscParam)
{
	PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1344"), GetLocalized(iClientID, "MSG_1345"));
}

wstring HkGetWStringFromIDS(uint iIDS) //Only works for names
{
	if (!iIDS)
		return L"";

	uint iDLL = iIDS / 0x10000;
	iIDS -= iDLL * 0x10000;

	wchar_t wszBuf[512];
	if (LoadStringW(vDLLs[iDLL], iIDS, wszBuf, 512))
		return wszBuf;
	return L"";
}

void HkLoadDLLConf(const char *szFLConfigFile)
{
	for (uint i = 0; i < vDLLs.size(); i++)
	{
		FreeLibrary(vDLLs[i]);
	}
	vDLLs.clear();
	HINSTANCE hDLL = LoadLibraryEx((char*)((char*)GetModuleHandle(0) + 0x256C4), NULL, LOAD_LIBRARY_AS_DATAFILE); //typically resources.dll
	if (hDLL)
		vDLLs.push_back(hDLL);
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
							vDLLs.push_back(hDLL);
					}
				}
			}
		}
		ini.close();
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
	{ L"/bequip-1",				UserCmd_eBuild_1},
	{ L"/bequip-2",				UserCmd_eBuild_2},
	{ L"/bequip",				UserCmd_eBuildHelp},
	{ L"/bwep-1",			    UserCmd_wBuild_1},
	{ L"/bwep-2",			    UserCmd_wBuild_2},
	{ L"/bwep",					UserCmd_wBuildHelp},
	{ L"/bammo-1",			    UserCmd_aBuild_1},
	{ L"/bammo-2",			    UserCmd_aBuild_2},
	{ L"/bammo",			    UserCmd_aBuildHelp},
	{ L"/blic",					UserCmd_lBuildHelp},
	{ L"/blic-1",			    UserCmd_lBuild_1},
	{ L"/blic-2",			    UserCmd_lBuild_2},
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

			returncode = SKIPPLUGINS_NOFUNCTIONCALL; // we handled the command, return immediatly
			return true;
		}
	}

	returncode = DEFAULT_RETURNCODE; // we did not handle the command, so let other plugins or FLHook kick in
	return false;
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "build goods plugin by kosacid";
	p_PI->sShortName = "build";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Help, PLUGIN_UserCmd_Help, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	return p_PI;
}