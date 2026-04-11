/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 11 апреля 2026 13:40:26
 * Version: 1.0.26
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

namespace RandomEconomy
{
	/// Р—Р°РіСЂСѓР·РёС‚Рµ РЅР°СЃС‚СЂРѕР№РєРё РїР»Р°РіРёРЅР°.
	void RandomEconomy::LoadSettings(const string& scPluginCfgFile)
	{
	
	}

	/// <summary>
	/// Р§С‚Рѕ Р±СѓРґРµС‚ РїСЂРё РІС…РѕРґРµ РЅР° Р±Р°Р·Сѓ
	/// </summary>
	void RandomEconomy::BaseEnter(unsigned int iBaseID, unsigned int iClientID)
	{
		// РџРѕРёСЃРє Р±Р°Р·С‹ РІ СЃРїРёСЃРєРµ base-info
		BASE_INFO* bi = 0;
		foreach(lstBases, BASE_INFO, it3)
		{
			if (it3->iBaseID == iBaseID)
			{
				bi = &(*it3);
				break;
			}
		}

		if (!bi)
			return; // Р±Р°Р·Р° РЅРµ РЅР°Р№РґРµРЅР°

		foreach(bi->lstMarketMisc, DATA_MARKETITEM, itmi)
		{
			float fPrice;
			if (pub::Market::GetPrice(iBaseID, (*itmi).iArchID, fPrice) == -1)
				continue; // РўРѕРІР°СЂР° РЅРµС‚ РІ РЅР°Р»РёС‡РёРё

			Archetype::Equipment* eq = Archetype::GetEquipment((*itmi).iArchID);
			PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(eq->iIdsName), L": " + to_wstring(fPrice));
		}
	}
}