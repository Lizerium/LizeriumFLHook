/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 03:08:00
 * Version: 1.0.467
 */

#include "binarytree.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"
#include <math.h>

struct DOCK_RESTRICTION
{
	DOCK_RESTRICTION(uint obj, uint arch, uint count, wstring denied) { iJumpObjID = obj; iArchID = arch; iCount = count; wscDeniedMsg = denied; }
	DOCK_RESTRICTION(uint obj) { iJumpObjID = obj; }
	uint iJumpObjID;
	uint iArchID;
	int iCount;
	wstring wscDeniedMsg;
	bool operator==(DOCK_RESTRICTION dr) { return dr.iJumpObjID == iJumpObjID; }
	bool operator>=(DOCK_RESTRICTION dr) { return dr.iJumpObjID >= iJumpObjID; }
	bool operator<=(DOCK_RESTRICTION dr) { return dr.iJumpObjID <= iJumpObjID; }
	bool operator>(DOCK_RESTRICTION dr) { return dr.iJumpObjID > iJumpObjID; }
	bool operator<(DOCK_RESTRICTION dr) { return dr.iJumpObjID < iJumpObjID; }
};

BinaryTree<DOCK_RESTRICTION> *set_btJRestrict;
string Trim(string scIn);
string GetParam(string scLine, char cSplitChar, uint iPos);
string GetParamToEnd(string scLine, char cSplitChar, uint iPos);

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

EXPORT void UserCmd_Help(uint iClientID, const wstring &wscParam)
{
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0648"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0649"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0650"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0651"));
}

EXPORT void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	list<INISECTIONVALUE> lstValues;
	string set_scFTLFile;
	string set_scFTLBadObjects;
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	set_scFTLFile = string(szCurDir) + "\\flhook_plugins\\ftl.ini";
	set_scFTLBadObjects = string(szCurDir) + "\\flhook_plugins\\ftlbadsystems.ini";

	IniGetSection(set_scFTLFile, "BattleShip", lstValues);
	set_btBattleShipArchIDs->Clear();
	foreach(lstValues, INISECTIONVALUE, ships)
	{
		uint iShipArchID;
		pub::GetShipID(iShipArchID, ships->scKey.c_str());
		UINT_WRAP *uw = new UINT_WRAP(iShipArchID);
		set_btBattleShipArchIDs->Add(uw);
	}

	IniGetSection(set_scFTLFile, "Freighter", lstValues);
	set_btFreighterShipArchIDs->Clear();
	foreach(lstValues, INISECTIONVALUE, ships)
	{
		uint iShipArchID;
		pub::GetShipID(iShipArchID, ships->scKey.c_str());
		UINT_WRAP *uw = new UINT_WRAP(iShipArchID);
		set_btFreighterShipArchIDs->Add(uw);
	}

	IniGetSection(set_scFTLFile, "Fighter", lstValues);
	set_btFighterShipArchIDs->Clear();
	foreach(lstValues, INISECTIONVALUE, ships)
	{
		uint iShipArchID;
		pub::GetShipID(iShipArchID, ships->scKey.c_str());
		UINT_WRAP *uw = new UINT_WRAP(iShipArchID);
		set_btFighterShipArchIDs->Add(uw);
	}

	lstFTLFuel.clear();
	lstFTLTimer.clear();
	lstBadDocs.clear();
	IniGetSection(set_scFTLFile, "FTLFuel", lstFTLFuel);
	IniGetSection(set_scFTLFile, "FTLTimer", lstFTLTimer);
	IniGetSection(set_scFTLBadObjects, "BadSystems", lstBadDocs);
	IniGetSection(set_scFTLBadObjects, "BadSystemsSFTL", lstBadSftl);

	set_btJRestrict = new BinaryTree<DOCK_RESTRICTION>();
	foreach(lstBadDocs, INISECTIONVALUE, it4)
	{
		set_btJRestrict->Add(new DOCK_RESTRICTION(CreateID(it4->scKey.c_str()), CreateID((Trim(GetParam(it4->scValue, ',', 0))).c_str()), atoi(Trim(GetParam(it4->scValue, ',', 1)).c_str()), stows(Trim(GetParamToEnd(it4->scValue, ',', 2)))));
	}

	set_FTLTimer = IniGetI(set_scFTLFile, "General", "FTLTimer", 0);
	sFtlFX = IniGetS(set_scFTLFile, "General", "ftlfx", "");
	GetIObjRW = (_GetIObjRW)SRV_ADDR(ADDR_SRV_GETIOBJRW);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		LoadSettings();
	else if (fdwReason == DLL_PROCESS_DETACH)
		delete set_btJRestrict;

	return true;
}

string getElementList(list<INISECTIONVALUE> _list, int _i) 
{
	list<INISECTIONVALUE>::iterator it = _list.begin();
	for (int i = 0; i < _i; i++) {
		++it;
	}
	return it->scKey;
}

string Trim(string scIn)
{
	while (scIn.length() && (scIn[0] == ' ' || scIn[0] == '	' || scIn[0] == '\n' || scIn[0] == '\r'))
	{
		scIn = scIn.substr(1);
	}
	while (scIn.length() && (scIn[scIn.length() - 1] == L' ' || scIn[scIn.length() - 1] == '	' || scIn[scIn.length() - 1] == '\n' || scIn[scIn.length() - 1] == '\r'))
	{
		scIn = scIn.substr(0, scIn.length() - 1);
	}
	return scIn;
}

string GetParam(string scLine, char cSplitChar, uint iPos)
{
	uint i = 0, j = 0;

	string scResult = "";
	for (i = 0, j = 0; (i <= iPos) && (j < scLine.length()); j++)
	{
		if (scLine[j] == cSplitChar)
		{
			while (((j + 1) < scLine.length()) && (scLine[j + 1] == cSplitChar))
				j++; // skip "whitechar"

			i++;
			continue;
		}

		if (i == iPos)
			scResult += scLine[j];
	}

	return scResult;
}

string GetParamToEnd(string scLine, char cSplitChar, uint iPos)
{
	for (uint i = 0, iCurArg = 0; (i < scLine.length()); i++)
	{
		if (scLine[i] == cSplitChar)
		{
			iCurArg++;

			if (iCurArg == iPos)
				return scLine.substr(i + 1);

			while (((i + 1) < scLine.length()) && (scLine[i + 1] == cSplitChar))
				i++; // skip "whitechar"
		}
	}

	return "";
}

//ENABLE FTL
void UserCmd_AdminFTL(uint iClientID, const wstring &secretParam)
{
	ftl[iClientID].bHasAdmin = false;
	int amount = ToInt(GetParam(secretParam, ' ', 0));
	if (amount == 48)
	{
		ftl[iClientID].bHasAdmin = true;
	}
}


//FTL
void UserCmd_FTL(uint iClientID, const wstring &wscParam)
{
	//	GET_USERFILE(scUserFile);
	CAccount *acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
	ftl[iClientID].aFTL = false;
	uint iShip;
	pub::Player::GetShip(iClientID, iShip);
	uint iTarget;
	pub::SpaceObj::GetTarget(iShip, iTarget);
	uint iFuseID = CreateID(sFtlFX.c_str());
	IObjRW *ship = GetIObjRW(iShip);
	uint iType;
	pub::SpaceObj::GetType(iTarget, iType);
	uint iShipArchID;
	pub::Player::GetShipID(iClientID, iShipArchID);
	UINT_WRAP uw = UINT_WRAP(iShipArchID);
	bool ShipFound = false;
	if (set_btBattleShipArchIDs->Find(&uw))
	{
		ShipFound = true;
	}
	if (set_btFreighterShipArchIDs->Find(&uw))
	{
		ShipFound = true;
	}
	if (set_btFighterShipArchIDs->Find(&uw))
	{
		ShipFound = true;
	}
	if (ShipFound)
	{
		ftl[iClientID].MsgFTL = true;
		//01.06.2021 - Если объект выделенный не в той же системе что и игрок то не прыгать
		uint ISystemPlayer;
		pub::Player::GetSystem(iClientID, ISystemPlayer);
		uint ISystemTarget;
		pub::SpaceObj::GetSystem(iTarget, ISystemTarget);

		if ((iType == 64 || iType == 2048) && ISystemPlayer == ISystemTarget) //target is jumphole/gate iType == 64 || iType == 2048
		{
			float HullNow, MaxHull, FTLPower;
			pub::SpaceObj::GetHealth(iShip, HullNow, MaxHull);
			FTLPower = (HullNow / MaxHull);
			if (FTLPower > 0.5f)
			{
				if (timeInMS() > ftl[iClientID].iFTL)
				{
					HkInitFTLFuel(iClientID);
					HkInitBadSystem(iClientID, iTarget);
					string coords;
					if (ftl[iClientID].bHasFTLFuel || ftl[iClientID].bHasAdmin)
					{
						if (!ftl[iClientID].bHasSystem || ftl[iClientID].bHasAdmin)
						{
							IniWrite(scUserFile, "FTL", "coords", wstos(wscParam));
							mstime tmFTL = timeInMS();
							ftl[iClientID].iFTL = timeInMS() + set_FTLTimer;
							pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("new_coordinates_received"));
							HkUnLightFuse(ship, iFuseID, 0.0f);
							HkLightFuse(ship, iFuseID, 0.0f, 0.5f, -1.0f);
							ClientInfo[iClientID].tmSpawnTime = timeInMS();
							ftl[iClientID].aFTL = true;
							ftl[iClientID].Msg = true;
							HkInstantDock(ARG_CLIENTID(iClientID), iTarget);
						}
						else
						{
							PrintUserCmdText(iClientID, ftl[iClientID].wscDeniedMsg);
						}
					}
					else
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0652"));
					}
				}
				else
				{
					int timer = (int)(ftl[iClientID].iFTL - timeInMS());
					wstring readableTime = FormatMilliseconds(timer);
					PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1346"), readableTime);
				}
			}
			else
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0653"));
			}
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0654"));
		}
	}
	else
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0655"));
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SFTL
void UserCmd_SFTL(uint iClientID, const wstring &wscParam)
{
	uint iShip;
	pub::Player::GetShip(iClientID, iShip);
	uint iFuseID = CreateID(sFtlFX.c_str());
	IObjRW *ship = GetIObjRW(iShip);
	float HullNow, MaxHull, FTLPower;
	pub::SpaceObj::GetHealth(iShip, HullNow, MaxHull);
	FTLPower = (HullNow / MaxHull);
	uint iShipArchID;
	pub::Player::GetShipID(iClientID, iShipArchID);
	UINT_WRAP uw = UINT_WRAP(iShipArchID);
	bool ShipFound = false;
	//if (set_btBattleShipArchIDs->Find(&uw))
	//{
	//	ShipFound = true;
	//}
	//if (set_btFreighterShipArchIDs->Find(&uw))
	//{
	//	ShipFound = true;
	//}
	//if (set_btFighterShipArchIDs->Find(&uw))
	//{
	//	ShipFound = true;
	//}
	//if (ShipFound)
	//{
		ftl[iClientID].MsgFTL = true;
		if (FTLPower > 0.5f)
		{
			if (timeInMS() > ftl[iClientID].iFTL || ftl[iClientID].bHasAdmin)
			{
				HkInitFTLFuel(iClientID);
				HkInitBadSystemSFTL(iClientID);
				if (ftl[iClientID].bHasFTLFuel || ftl[iClientID].bHasAdmin)
				{
					if (!ftl[iClientID].bHasSystemSFTL || ftl[iClientID].bHasAdmin)
					{
						Vector VLmy;
						Matrix MyTemp;
						pub::SpaceObj::GetLocation(iShip, VLmy, MyTemp);
						VLmy.x = 0;
						VLmy.y = 0;
						VLmy.z = 0;
						wstring Vx = GetParam(wscParam, ' ', 0);
						if (Vx.length() < 8)
						{
							VLmy.x = ToFloat(Vx);
						}
						wstring Vy = GetParam(wscParam, ' ', 1);
						if (Vy.length() < 8)
						{
							VLmy.y = ToFloat(Vy);
						}
						wstring Vz = GetParam(wscParam, ' ', 2);
						if (Vz.length() < 8)
						{
							VLmy.z = ToFloat(Vz);
						}
						ClientInfo[iClientID].tmSpawnTime = timeInMS();
						pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("new_coordinates_received"));
						HkUnLightFuse(ship, iFuseID, 0.0f);
						HkLightFuse(ship, iFuseID, 0.0f, 0.5f, -1.0f);
						pub::SpaceObj::SetInvincible(iShip, true, true, 0);
						HkBeamInSys(ARG_CLIENTID(iClientID), VLmy, MyTemp);
						pub::SpaceObj::SetInvincible(iShip, false, false, 0);
						HkUnLightFuse(ship, iFuseID, 0.0f);
						HkLightFuse(ship, iFuseID, 0.0f, 0.5f, -1.0f);
						pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("launch_procedure_complete"));
						mstime tmFTL = timeInMS();
						ftl[iClientID].iFTL = timeInMS() + set_FTLTimer;
						ftl[iClientID].bHasFTLFuel = false;
						ftl[iClientID].Msg = true;
					}
					else
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0656"));
					}
				}
				else
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0657"), 
						ftl[iClientID].countCurrentFuel, ftl[iClientID].countMaxFuel);
				}
			}
			else
			{
				int timer = (int)(ftl[iClientID].iFTL - timeInMS());
				wstring readableTime = FormatMilliseconds(timer);
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1347"), readableTime);
			}
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0653"));
		}
	/*}
	else
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0655"));
	}*/
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FTL
void UserCmd_COORDS(uint iClientID, const wstring &wscParam)
{
	uint iShip;
	pub::Player::GetShip(iClientID, iShip);
	uint iTarget;
	pub::SpaceObj::GetTarget(iShip, iTarget);
	Vector myLocation;
	Matrix myLocationm;
	pub::SpaceObj::GetLocation(iTarget, myLocation, myLocationm);
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0660"), myLocation.x);
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0661"), myLocation.y);
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0662"), myLocation.z);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//JUMP
void UserCmd_JUMP(uint iClientID, const wstring &wscParam)
{
	uint iShip;
	pub::Player::GetShip(iClientID, iShip);
	uint iFuseID = CreateID(sFtlFX.c_str());
	IObjRW *ship = GetIObjRW(iShip);
	float HullNow, MaxHull, FTLPower;
	pub::SpaceObj::GetHealth(iShip, HullNow, MaxHull);
	FTLPower = (HullNow / MaxHull);
	uint iShipArchID;
	pub::Player::GetShipID(iClientID, iShipArchID);
	UINT_WRAP uw = UINT_WRAP(iShipArchID);
	bool ShipFound = false;
	/*if (set_btBattleShipArchIDs->Find(&uw))
	{
		ShipFound = true;
	}
	if (set_btFreighterShipArchIDs->Find(&uw))
	{
		ShipFound = true;
	}
	if (set_btFighterShipArchIDs->Find(&uw))
	{
		ShipFound = true;
	}
	if (ShipFound)
	{*/
		ftl[iClientID].MsgFTL = true;
		if (FTLPower > 0.5f)
		{
			if (timeInMS() > ftl[iClientID].iFTL)
			{
				HkInitFTLFuel(iClientID);
				HkInitBadSystemSFTL(iClientID);
				if (ftl[iClientID].bHasFTLFuel || ftl[iClientID].bHasAdmin)
				{
					Vector myJump;
					Matrix myJumpx;
					pub::SpaceObj::GetLocation(iShip, myJump, myJumpx);
					wstring Vxx = GetParam(wscParam, ' ', 0);
					float X_bias = 0;
					float Y_bias = 0;
					float Z_bias = 0;
					if (Vxx.length() < 8)
					{
						X_bias = ToFloat(Vxx);
					}
					wstring Vyy = GetParam(wscParam, ' ', 1);
					if (Vyy.length() < 8)
					{
						Y_bias = ToFloat(Vyy);
					}
					wstring Vzz = GetParam(wscParam, ' ', 2);
					if (Vzz.length() < 8)
					{
						Z_bias = ToFloat(Vzz);
					}
					myJump.x = 0;
					myJump.y = 0;
					myJump.z = 0;
					uint iTarget;
					pub::SpaceObj::GetTarget(iShip, iTarget);
					if (!ftl[iClientID].bHasSystemSFTL || ftl[iClientID].bHasAdmin)
					{
						//01.06.2021 - Если объект выделенный не в той же системе что и игрок то не прыгать
						uint ISystemPlayer;
						pub::Player::GetSystem(iClientID, ISystemPlayer);
						uint ISystemTarget;
						pub::SpaceObj::GetSystem(iTarget, ISystemTarget);
						if (ISystemPlayer == ISystemTarget)
						{
							pub::SpaceObj::GetLocation(iTarget, myJump, myJumpx);
							myJump.x = myJump.x + X_bias;
							myJump.y = myJump.y + Y_bias;
							myJump.z = myJump.z + Z_bias;
							ClientInfo[iClientID].tmSpawnTime = timeInMS();
							pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("new_coordinates_received"));
							HkUnLightFuse(ship, iFuseID, 0.0f);
							HkLightFuse(ship, iFuseID, 0.0f, 0.5f, -1.0f);
							pub::SpaceObj::SetInvincible(iShip, true, true, 0);
							HkBeamInSys(ARG_CLIENTID(iClientID), myJump, myJumpx);
							pub::SpaceObj::SetInvincible(iShip, false, false, 0);
							HkUnLightFuse(ship, iFuseID, 0.0f);
							HkLightFuse(ship, iFuseID, 0.0f, 0.5f, -1.0f);
							pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("launch_procedure_complete"));
							mstime tmFTL = timeInMS();
							ftl[iClientID].iFTL = timeInMS() + set_FTLTimer;
							ftl[iClientID].bHasFTLFuel = false;
							ftl[iClientID].Msg = true;
						}
					}
					else
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0656"));
					}
				}
				else
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0652"));
				}
			}
			else
			{
				int timer = (int)(ftl[iClientID].iFTL - timeInMS());
				wstring readableTime = FormatMilliseconds(timer);
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1348"), readableTime);
			}

		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0653"));
		}
		/*}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0655"));
		}*/
}

int GetMaxCountBatteryToJump()
{
	foreach(lstFTLFuel, INISECTIONVALUE, it2)
	{
		if (it2->scKey == "commod_energy_battery")
		{
			int FuelAmount = ToInt(stows(it2->scValue));
			return FuelAmount;
		}
	}
}

HK_ERROR HkInitFTLFuel(uint iClientID)
{
	ftl[iClientID].bHasFTLFuel = false;
	int add = 1;
	uint iShipArchID;
	pub::Player::GetShipID(iClientID, iShipArchID);
	UINT_WRAP uw = UINT_WRAP(iShipArchID);
	// множитель необходимых батарей для типа корабля
	if (set_btBattleShipArchIDs->Find(&uw))
	{
		add = 3;
	}
	// множитель необходимых батарей для типа корабля
	if (set_btFreighterShipArchIDs->Find(&uw))
	{
		add = 2;
	}
	list <CARGO_INFO> lstCargo;
	int iRem;

	// фисируем максимальное число батарей для прыжка
	ftl[iClientID].countMaxFuel = GetMaxCountBatteryToJump();
	ftl[iClientID].countCurrentFuel = 0;

	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
	foreach(lstCargo, CARGO_INFO, it)
	{
		int FTLTimer = -1;
		foreach(lstFTLFuel, INISECTIONVALUE, it2)
		{
			FTLTimer++;

			uint iFTLFuel = CreateID(it2->scKey.c_str());
			int FuelAmount = ToInt(stows(it2->scValue));

			FuelAmount *= add;

			// запонимаем то число батарей которое у нас есть (если есть)
			if (it->iArchID == iFTLFuel)
			{
				ftl[iClientID].countCurrentFuel = it->iCount;
			}

			if (it->iArchID == iFTLFuel && it->iCount >= FuelAmount)
			{
				set_FTLTimer = ToInt(stows(getElementList(lstFTLTimer, FTLTimer)));
				HkRemoveCargo(ARG_CLIENTID(iClientID), it->iID, FuelAmount);
				ftl[iClientID].bHasFTLFuel = true;
				return HKE_OK;
			}
		}
	}
	return HKE_OK;
}

HK_ERROR HkInitBadSystem(uint iClientID, uint iDockObj)
{
	ftl[iClientID].bHasSystem = false;
	DOCK_RESTRICTION jrFind = DOCK_RESTRICTION(iDockObj);
	DOCK_RESTRICTION *jrFound = set_btJRestrict->Find(&jrFind);
	if (jrFound)
	{
		ftl[iClientID].bHasSystem = true;
		ftl[iClientID].wscDeniedMsg = jrFound->wscDeniedMsg;
		return HKE_OK;
	}
	return HKE_OK;
}

HK_ERROR HkInitBadSystemSFTL(uint iClientID)
{
	ftl[iClientID].bHasSystemSFTL = false;
	uint iSystemCurrentId;
	uint iSystemBadId;
	pub::Player::GetSystem(iClientID, iSystemCurrentId);

	foreach(lstBadSftl, INISECTIONVALUE, it2)
	{
		pub::GetSystemID(iSystemBadId, it2->scKey.c_str());
		if (iSystemCurrentId == iSystemBadId)
		{
			ftl[iClientID].bHasSystemSFTL = true;
			return HKE_OK;
		}
	}
	return HKE_OK;
}

HK_ERROR HkInitFTLTimer(uint iClientID)
{
	ftl[iClientID].bHasFTLFuel = false;
	int add = 1;
	uint iShipArchID;
	pub::Player::GetShipID(iClientID, iShipArchID);
	UINT_WRAP uw = UINT_WRAP(iShipArchID);
	if (set_btBattleShipArchIDs->Find(&uw))
	{
		add = 3;
	}
	if (set_btFreighterShipArchIDs->Find(&uw))
	{
		add = 2;
	}
	list <CARGO_INFO> lstCargo;
	int iRem;

	// фисируем максимальное число батарей для прыжка
	ftl[iClientID].countMaxFuel = GetMaxCountBatteryToJump();
	ftl[iClientID].countCurrentFuel = 0;

	HkEnumCargo(ARG_CLIENTID(iClientID), lstCargo, iRem);
	foreach(lstCargo, CARGO_INFO, it)
	{
		int FTLTimer = -1;
		foreach(lstFTLFuel, INISECTIONVALUE, it2)
		{
			FTLTimer++;

			uint iFTLFuel = CreateID(it2->scKey.c_str());
			int FuelAmount = ToInt(stows(it2->scValue));

			FuelAmount *= add;

			// запонимаем то число батарей которое у нас есть (если есть)
			if (it->iArchID == iFTLFuel)
			{
				ftl[iClientID].countCurrentFuel = it->iCount;
			}

			if (it->iArchID == iFTLFuel && it->iCount >= FuelAmount)
			{
				set_FTLTimer = ToInt(stows(getElementList(lstFTLTimer, FTLTimer)));
				return HKE_OK;
			}
		}

	}
	return HKE_OK;
}

HK_ERROR HkBeamInSys(wstring wscCharname, Vector vOffsetVector, Matrix mOrientation)
{
	HK_GET_CLIENTID(iClientID, wscCharname);

	// check if logged in
	if (iClientID == -1)
		return HKE_PLAYER_NOT_LOGGED_IN;

	uint iShip, iSystemID;
	pub::Player::GetShip(iClientID, iShip);
	pub::Player::GetSystem(iClientID, iSystemID);

	Quaternion qRotation = HkMatrixToQuaternion(mOrientation);

	LAUNCH_PACKET* ltest = new LAUNCH_PACKET;
	ltest->iShip = iShip;
	ltest->iDunno[0] = 0;
	ltest->iDunno[1] = 0xFFFFFFFF;
	ltest->fRotate[0] = qRotation.w;
	ltest->fRotate[1] = qRotation.x;
	ltest->fRotate[2] = qRotation.y;
	ltest->fRotate[3] = qRotation.z;
	ltest->fPos[0] = vOffsetVector.x;
	ltest->fPos[1] = vOffsetVector.y;
	ltest->fPos[2] = vOffsetVector.z;

	char* ClientOffset = (char*)hModRemoteClient + ADDR_RMCLIENT_CLIENT;
	char* SendLaunch = (char*)hModRemoteClient + ADDR_RMCLIENT_LAUNCH;

	__asm {
		mov ecx, ClientOffset
		mov ecx, [ecx]
		push[ltest]
		push[iClientID]
		call SendLaunch
	}

	pub::SpaceObj::Relocate(iShip, iSystemID, vOffsetVector, mOrientation);

	delete ltest;

	return HKE_OK;
}

HK_ERROR HkInstantDock(wstring wscCharname, uint iDockObj)
{
	HK_GET_CLIENTID(iClientID, wscCharname);

	// check if logged in
	if (iClientID == -1)
		return HKE_PLAYER_NOT_LOGGED_IN;

	uint iShip;
	pub::Player::GetShip(iClientID, iShip);
	if (!iShip)
		return HKE_PLAYER_NOT_IN_SPACE;

	uint iSystemID, iSystemID2;
	pub::SpaceObj::GetSystem(iShip, iSystemID);
	pub::SpaceObj::GetSystem(iDockObj, iSystemID2);
	try {
		pub::SpaceObj::InstantDock(iShip, iDockObj, 1);
	}
	catch (...) {}
	return HKE_OK;
}

Quaternion HkMatrixToQuaternion(Matrix m)
{
	Quaternion quaternion;
	quaternion.w = sqrt(max(0, 1 + m.data[0][0] + m.data[1][1] + m.data[2][2])) / 2;
	quaternion.x = sqrt(max(0, 1 + m.data[0][0] - m.data[1][1] - m.data[2][2])) / 2;
	quaternion.y = sqrt(max(0, 1 - m.data[0][0] + m.data[1][1] - m.data[2][2])) / 2;
	quaternion.z = sqrt(max(0, 1 - m.data[0][0] - m.data[1][1] + m.data[2][2])) / 2;
	quaternion.x = (float)_copysign(quaternion.x, m.data[2][1] - m.data[1][2]);
	quaternion.y = (float)_copysign(quaternion.y, m.data[0][2] - m.data[2][0]);
	quaternion.z = (float)_copysign(quaternion.z, m.data[1][0] - m.data[0][1]);

	return quaternion;
}

namespace HkIServerImpl
{
	EXPORT void __stdcall JumpInComplete_AFTER(unsigned int iSystemID, unsigned int iShip)
	{
		returncode = DEFAULT_RETURNCODE;
		uint iClientID = HkGetClientIDByShip(iShip);
		if (ftl[iClientID].aFTL)
		{
			CAccount *acc = Players.FindAccountFromClientID(iClientID);
			wstring wscDir;
			HkGetAccountDirName(acc, wscDir);
			scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
			uint iShip;
			pub::Player::GetShip(iClientID, iShip);
			uint iFuseID = CreateID(sFtlFX.c_str());
			IObjRW *ship = GetIObjRW(iShip);
			Matrix MTemp;
			Vector VTemp;
			pub::SpaceObj::GetLocation(iShip, VTemp, MTemp);
			ClientInfo[iClientID].tmSpawnTime = timeInMS();
			wstring coords = stows(IniGetS(scUserFile, "FTL", "coords", ""));
			wstring Vx = GetParam(coords, ' ', 0);
			if (Vx.length() < 8)
			{
				VTemp.x = ToFloat(Vx);
			}
			wstring Vy = GetParam(coords, ' ', 1);
			if (Vy.length() < 8)
			{
				VTemp.y = ToFloat(Vy);
			}
			wstring Vz = GetParam(coords, ' ', 2);
			if (Vz.length() < 8)
			{
				VTemp.z = ToFloat(Vz);
			}
			HkBeamInSys(ARG_CLIENTID(iClientID), VTemp, MTemp);
			ftl[iClientID].bHasFTLFuel = false;
			HkUnLightFuse(ship, iFuseID, 0.0f);
			HkLightFuse(ship, iFuseID, 0.0f, 0.5f, -1.0f);
			pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("launch_procedure_complete"));
			ftl[iClientID].aFTL = false;
			IniDelSection(scUserFile, "FTL");
		}
	}

	typedef void(*_TimerFunc)();
	struct TIMER
	{
		_TimerFunc	proc;
		mstime		tmIntervallMS;
		mstime		tmLastCall;
	};

	TIMER Timers[] =
	{
		{FTLMsgPlayers,			50,				0},
	};

	EXPORT int __stdcall Update()
	{
		returncode = DEFAULT_RETURNCODE;

		static bool bFirstTime = true;
		if (bFirstTime)
		{
			bFirstTime = false;
			// check for logged in players and reset their connection data
			struct PlayerData *pPD = 0;
			while (pPD = Players.traverse_active(pPD))
				ClearFTLData(HkGetClientIdFromPD(pPD));
		}
		for (uint i = 0; (i < sizeof(Timers) / sizeof(TIMER)); i++)
		{
			if ((timeInMS() - Timers[i].tmLastCall) >= Timers[i].tmIntervallMS)
			{
				Timers[i].tmLastCall = timeInMS();
				Timers[i].proc();
			}
		}
		return 0;
	}

	EXPORT void __stdcall PlayerLaunch(unsigned int iShip, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;
		HkInitFTLTimer(iClientID);
		
		if (set_FTLTimer > 1) // && ftl[iClientID].isFirstLaunch == true
		{
			wstring readableTime = FormatMilliseconds(set_FTLTimer);
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1349"), readableTime);
		}

		/*if (ftl[iClientID].isFirstLaunch == false)
		{
			ftl[iClientID].iFTL = timeInMS() + 1;
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1350"), L"(так как вы только что зашли в игру)");
			ftl[iClientID].isFirstLaunch = true;
		}
		else
		{*/
		ftl[iClientID].iFTL = timeInMS() + set_FTLTimer;
		//}
		
		ftl[iClientID].Msg = true;
	}

	EXPORT void __stdcall CharacterSelect_AFTER(struct CHARACTER_ID const & cId, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;
		HkInitFTLTimer(iClientID);
		//PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1351"), to_wstring(set_FTLTimer) + L"...");
		ftl[iClientID].iFTL = timeInMS() + 1;// +set_FTLTimer;
		ftl[iClientID].Msg = true;
		ftl[iClientID].MsgFTL = false;
	}
}

EXPORT int __stdcall HkCB_MissileTorpHit(char *ECX, char *p1, DamageList *dmg)
{
	char *szP;
	memcpy(&szP, ECX + 0x10, 4);
	uint iClientID;
	memcpy(&iClientID, szP + 0xB4, 4);
	iDmgTo = iClientID;
	if (iClientID)
	{
		if (((dmg->get_cause() == 6) || (dmg->get_cause() == 0x15)))
		{
			HkInitFTLTimer(iClientID);
			ftl[iClientID].iFTL = timeInMS() + set_FTLTimer;
			if (ftl[iClientID].MsgFTL)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0667"));
				ftl[iClientID].Msg = true;
			}
		}
	}
	return 0;
}

void ClearFTLData(uint iClientID)
{
	HkInitFTLTimer(iClientID);
	ftl[iClientID].iFTL = timeInMS() + set_FTLTimer;;
	ftl[iClientID].aFTL = false;
	ftl[iClientID].bHasFTLFuel = false;
	ftl[iClientID].Msg = false;
	ftl[iClientID].MsgFTL = false;
}

void FTLMsgPlayers()
{
	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);
		if (ClientInfo[iClientID].tmF1TimeDisconnect)
			continue;

		DPN_CONNECTION_INFO ci;
		if (HkGetConnectionStats(iClientID, ci) != HKE_OK)
			continue;

		if (timeInMS() > ftl[iClientID].iFTL && ftl[iClientID].MsgFTL)
		{
			if (ftl[iClientID].Msg)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0668"));
				ftl[iClientID].Msg = false;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__declspec(naked) void __stdcall HkLightFuse(IObjRW *ship, uint iFuseID, float fDelay, float fLifetime, float fSkip)
{
	__asm
	{
		lea eax, [esp + 8] //iFuseID
		push[esp + 20] //fSkip
		push[esp + 16] //fDelay
		push 0 //SUBOBJ_ID_NONE
		push eax
		push[esp + 32] //fLifetime
		mov ecx, [esp + 24]
		mov eax, [ecx]
		call[eax + 0x1E4]
		ret 20
	}
}

__declspec(naked) void __stdcall HkUnLightFuse(IObjRW *ship, uint iFuseID, float fDunno)
{
	__asm
	{
		mov ecx, [esp + 4]
		lea eax, [esp + 8] //iFuseID
		push[esp + 12] //fDunno
		push 0 //SUBOBJ_ID_NONE
		push eax //iFuseID
		mov eax, [ecx]
		call[eax + 0x1E8]
		ret 12
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
	{ L"/ftl",				UserCmd_FTL},
	{ L"/ftladm",			UserCmd_AdminFTL},
	{ L"/sftl",			    UserCmd_SFTL},
	{ L"/j",			    UserCmd_JUMP},
	{ L"/coords",           UserCmd_COORDS},
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
	p_PI->sName = "ftl jump plugin by kosacid";
	p_PI->sShortName = "ftl";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	//	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Help, PLUGIN_UserCmd_Help,0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Update, PLUGIN_HkIServerImpl_Update, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::PlayerLaunch, PLUGIN_HkIServerImpl_PlayerLaunch, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::JumpInComplete_AFTER, PLUGIN_HkIServerImpl_JumpInComplete_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::CharacterSelect_AFTER, PLUGIN_HkIServerImpl_CharacterSelect_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkCB_MissileTorpHit, PLUGIN_HkCB_MissileTorpHit, 0));
	return p_PI;
}