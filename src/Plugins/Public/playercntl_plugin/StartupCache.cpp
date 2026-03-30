/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 30 марта 2026 12:21:08
 * Version: 1.0.11
 */

// Player Control plugin for FLHookPlugin
// Apr 2011 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

// This module uses a cache of file names to speed up FLServer startup by
// not requiring that every character file is opened and read

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

#include "Main.h"
#include <PluginUtilities.h>

namespace StartupCache
{
	// Перезапись существующего файла символов
	static int chars_loaded = 0;

	// Исходная функция чтения charname функция
	typedef int(__stdcall *_ReadCharacterName)(const char *filename, flstr *str);
	_ReadCharacterName ReadCharName;

	// Карта acc_char_path в имя персонажа
	static map<string, wstring> cache;

	static string scBaseAcctPath;

	// длина пути к пользовательским данным + accts\multiplayer удалить, 
	// чтобы мы могли искать только acc_char_path
	static int acc_path_prefix_length = 0;

	// Быстрая альтернатива встроенной функции чтения имени символа в server.dll
	static int __stdcall HkCb_ReadCharacterName(const char *filename, flstr* str)
	{
		ConPrint(L"\rRead %d\r", ++chars_loaded);

		// Если этот аккаунт/charfile можно найти в символе return, то имя немедленно.
		string acc_path(&filename[acc_path_prefix_length]);
		map<string, wstring>::iterator i = cache.find(acc_path);
		if (i != cache.end())
		{
			WStringAssign(str, i->second.c_str());
			return 1;
		}

		// В противном случае используйте исходную функцию FL для загрузки имени символа
		// и кэшируем результат и сообщаем, что это некэшированный файл
		ReadCharName(filename, str);
		cache[acc_path] = GetWCString(str);
		return 1;
	}


	struct NAMEINFO
	{
		char acc_path[27]; // accdir(11)/charfile(11).fl + terminator
		wchar_t name[25]; // max name is 24 chars + terminator
	};

	static void LoadCache()
	{
		// Откройте файл кэша имен и загрузите его в память.
		string scPath = scBaseAcctPath + "namecache.bin";

		ConPrint(L"Loading character name cache\n");
		FILE *file = fopen(scPath.c_str(), "rb");
		if (file)
		{
			NAMEINFO ni;
			while (fread(&ni, sizeof(NAMEINFO), 1, file))
			{
				string acc_path(ni.acc_path);
				wstring name(ni.name);
				cache[acc_path] = name;
			}
			fclose(file);
		}
		ConPrint(L"Loaded %d names\n", cache.size());
	}

	static void SaveCache()
	{
		// Сохраните файл кэша имен
		string scPath = scBaseAcctPath + "namecache.bin";

		FILE *file = fopen(scPath.c_str(), "wb");
		if (file)
		{
			ConPrint(L"Saving character name cache\n");
			for (map<string, wstring>::iterator i = cache.begin(); i != cache.end(); i++)
			{
				NAMEINFO ni;
				memset(&ni, 0, sizeof(ni));
				strncpy_s(ni.acc_path, 27, i->first.c_str(), i->first.size());
				wcsncpy_s(ni.name, 25, i->second.c_str(), i->second.size());
				if (!fwrite(&ni, sizeof(NAMEINFO), 1, file))
				{
					ConPrint(L"ERROR: Saving character name cache failed\n");
					break;
				}
			}
			fclose(file);
			ConPrint(L"Saved %d names\n", cache.size());
		}

		cache.clear();
	}

	// Звонок из стартапа
	void Init()
	{
		// Отключите проверку админки и запрещенных файлов.
		{
			BYTE patch[] = { 0x5f, 0x5e, 0x5d, 0x5b, 0x81, 0xC4, 0x08, 0x11, 0x00, 0x00, 0xC2, 0x04, 0x00 }; // pop regs, restore esp, ret 4
			WriteProcMem((char*)hModServer + 0x76b3e, patch, 13);
		}

		// Перехватывает имя прочитанного символа и заменяет его версией кэширования
		PatchCallAddr((char*)hModServer, 0x717be, (char*)HkCb_ReadCharacterName);

		// Сохраните ссылку на старую функцию чтения имени символа.
		ReadCharName = (_ReadCharacterName)((char*)hModServer + 0x72fe0);

		// Рассчитайте наш базовый путь
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		scBaseAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\";
		acc_path_prefix_length = scBaseAcctPath.length();

		// Загрузка кэша
		LoadCache();
	}

	// Call from Startup_AFTER
	void Done()
	{
		SaveCache();

		// Восстановление проверок администратора и заблокированных файлов
		{
			BYTE patch[] = { 0x8b, 0x35, 0xc0, 0x4b, 0xd6, 0x06, 0x6a, 0x00, 0x68, 0xB0, 0xB8, 0xD6, 0x06 };
			WriteProcMem((char*)hModServer + 0x76b3e, patch, 13);
		}

		// Восстановление проверок администратора и заблокированных файлов
		{
			BYTE patch[] = { 0xe8, 0x1d, 0x18, 0x00, 0x00 };
			WriteProcMem((char*)hModServer + 0x717be, patch, 5);
		}
	}
}