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
	/// Загрузите настройки плагина.
	void RandomEconomy::LoadSettings(const string& scPluginCfgFile)
	{
	
	}

	/// <summary>
	/// Что будет при входе на базу
	/// </summary>
	void RandomEconomy::BaseEnter(unsigned int iBaseID, unsigned int iClientID)
	{
		// Поиск базы в списке base-info
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
			return; // база не найдена

		foreach(bi->lstMarketMisc, DATA_MARKETITEM, itmi)
		{
			float fPrice;
			if (pub::Market::GetPrice(iBaseID, (*itmi).iArchID, fPrice) == -1)
				continue; // Товара нет в наличии

			Archetype::Equipment* eq = Archetype::GetEquipment((*itmi).iArchID);
			PrintUserCmdTextColorKV(iClientID, HkGetWStringFromIDS(eq->iIdsName), L": " + to_wstring(fPrice));
		}
	}
}