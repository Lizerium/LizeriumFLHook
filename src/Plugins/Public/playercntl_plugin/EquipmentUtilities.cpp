/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 02 апреля 2026 06:53:11
 * Version: 1.0.17
 */

// Player Control plugin for FLHookPlugin
// Feb 2010 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

#include "Main.h"
#include <FLHook.h>
#include <plugin.h>
#include <PluginUtilities.h>

/// Карта хеш-кода и связанных с ним никнеймов.
static std::map<uint, std::string> mapHashToNickname;

/// Чтение ini-файла на наличие никнеймов и сохранение связанного с ним хеш-кода
static void ReadIniNicknameFile(const string &filePath)
{
	INI_Reader ini;
	if (ini.open(filePath.c_str(), false))
	{
		while (ini.read_header())
		{
			while (ini.read_value())
			{
				if (ini.is_value("nickname"))
				{
					uint hash = CreateID(Trim(ToLower(ini.get_value_string())).c_str());
					mapHashToNickname[hash] = ini.get_value_string();
				}
			}
		}
		ini.close();
	}
}

const char *EquipmentUtilities::FindNickname(uint hash)
{
	std::map<uint, std::string>::iterator i = mapHashToNickname.find(hash);
	if (i == mapHashToNickname.end())
		return "";
	return i->second.c_str();
}

/** Чтение данных фрилансера для определения настроек мода. */
void EquipmentUtilities::ReadIniNicknames()
{
	string dataDirPath = "..\\data";

	INI_Reader ini;
	if (ini.open("freelancer.ini", false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("Freelancer"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("data path"))
					{
						dataDirPath = ini.get_value_string();
						break;
					}
				}
			}
		}
		ini.close();
	}

	mapHashToNickname.clear();
	if (ini.open("freelancer.ini", false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("Data"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("equipment"))
					{
						ReadIniNicknameFile(dataDirPath + string("\\") + ini.get_value_string());
					}
					else if (ini.is_value("ships"))
					{
						ReadIniNicknameFile(dataDirPath + string("\\") + ini.get_value_string());
					}
					else if (ini.is_value("goods"))
					{
						ReadIniNicknameFile(dataDirPath + string("\\") + ini.get_value_string());
					}
					else if (ini.is_value("loadouts"))
					{
						ReadIniNicknameFile(dataDirPath + string("\\") + ini.get_value_string());
					}
				}
			}
		}
		ini.close();
	}
}