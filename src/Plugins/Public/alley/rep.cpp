/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 06 апреля 2026 12:49:28
 * Version: 1.0.21
 */

// AlleyPlugin for FLHookPlugin
// April 2015 by Alley
//
// 
// Это свободное программное обеспечение; вы можете распространять его и/или изменять
// по своему усмотрению без каких - либо ограничений.Если вы это сделаете, я был бы признателен
// за уведомление и/или упоминание где-либо.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Включает в себя
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
//Сооружения и всякое такое дерьмо, йоу
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void REP::LoadSettings()
{
	// Путь к файлу конфигурации.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\alley_rep.cfg";

	INI_Reader ini;
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("reputations"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("rep"))
					{
						uint solarnick = CreateID(ini.get_value_string(0));
						const char* newrep = ini.get_value_string(1);

						ConPrint(L"DEBUG: Rep of %s is %s \n", stows(ini.get_value_string(0)).c_str(), stows(ini.get_value_string(1)).c_str());

						uint obj_rep_group;
						pub::Reputation::GetReputationGroup(obj_rep_group, newrep);
						pub::Reputation::SetAffiliation(solarnick, obj_rep_group);
					}
				}
			}
		}
		ini.close();
	}
}
