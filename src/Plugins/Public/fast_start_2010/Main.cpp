/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 03 апреля 2026 11:33:45
 * Version: 1.0.18
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
#include "header.h"

PLUGIN_RETURNCODE returncode;

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return true;
}

namespace StartupCache
{
	// Количество загруженных символов.
	static int chars_loaded = 0;
	
	// Исходная функция считывает функцию charname
	typedef int (__stdcall *_ReadCharacterName)(const char *filename, flstr *str);
	_ReadCharacterName ReadCharName;

	// сопоставление acc_char_path с именем символа
	static map<string, wstring> cache;

	static string scBaseAcctPath;

	// длина пути к пользовательским данным + удалить accts\multiplayer, чтобы
	// мы можем искать только для acc_char_path
	static int acc_path_prefix_length = 0;

	// Быстрая альтернатива встроенной функции чтения имени символа в server.dll
	static int __stdcall HkCb_ReadCharacterName(const char *filename, flstr* str)
	{
		ConPrint(L"\rDvurechensky Read %d\r", ++chars_loaded);

		// Если эта учетная запись/charfile может быть найдена в возвращаемом символе
		// затем имя сразу.
		string acc_path(&filename[acc_path_prefix_length]);
		map<string, wstring>::iterator i = cache.find(acc_path);
		if (i != cache.end())
		{
			WStringAssign(str, i->second.c_str());
			return 1;
		}

		// В противном случае используйте исходную функцию FL для загрузки имени символа
		// и кешируем результат и сообщаем, что это некэшированный файл
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
		// Открываем файл кэша имен и загружаем его в память.
		string scPath = scBaseAcctPath + "namecache.bin";

		ConPrint(L"Dvurechensky Loading character name cache\n");
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
		ConPrint(L"Dvurechensky Loaded %d names\n", cache.size());
	}

	static void SaveCache()
	{	
		// Сохраняем файл кеша имени
		string scPath = scBaseAcctPath + "namecache.bin";

		FILE *file = fopen(scPath.c_str(), "wb");
		if (file)
		{
			ConPrint(L"Dvurechensky Saving character name cache\n");
			for (map<string, wstring>::iterator i = cache.begin(); i != cache.end(); i++)
			{
				NAMEINFO ni;
				memset(&ni, 0, sizeof(ni));
				strncpy_s(ni.acc_path, 27, i->first.c_str(), i->first.size());
				wcsncpy_s(ni.name, 25, i->second.c_str(), i->second.size());
				if (!fwrite(&ni, sizeof(NAMEINFO), 1, file))
				{			
					ConPrint(L"ERROR: Dvurechensky Saving character name cache failed\n");
					break;
				}
			}
			fclose(file);
			ConPrint(L"Dvurechensky Saved %d names\n", cache.size());
		}
		
		cache.clear();
	}

	// Вызов из автозагрузки
	void Init()
	{
		// Отключить проверку администратора и запрещенных файлов.
		{
			BYTE patch[] = { 0x5f, 0x5e, 0x5d, 0x5b, 0x81, 0xC4, 0x08, 0x11, 0x00, 0x00, 0xC2, 0x04, 0x00}; // pop regs, restore esp, ret 4
			WriteProcMem((char*)hModServer + 0x76b3e, patch, 13);
		}

		// Перехватываем имя прочитанного символа и заменяем его кешируемой версией
		PatchCallAddr((char*)hModServer, 0x717be, (char*)HkCb_ReadCharacterName); 
	
		// Сохраняем ссылку на старую функцию чтения имени символа.
		ReadCharName = (_ReadCharacterName) ((char*)hModServer + 0x72fe0);

		// Рассчитываем наш базовый путь
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		scBaseAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\";
		acc_path_prefix_length = scBaseAcctPath.length();

		// Загружаем кеш
		LoadCache();
	}

	// Call from Startup_AFTER
	void Done()
	{
		SaveCache();

		// Восстановить админку и проверки запрещенных файлов
		{
			BYTE patch[] = { 0x8b, 0x35, 0xc0, 0x4b, 0xd6, 0x06, 0x6a, 0x00, 0x68, 0xB0, 0xB8, 0xD6, 0x06};
			WriteProcMem((char*)hModServer + 0x76b3e, patch, 13);
		}

		// Отключить функцию чтения имени символа.
		{
			BYTE patch[] = { 0xe8, 0x1d, 0x18, 0x00, 0x00 };
			WriteProcMem((char*)hModServer + 0x717be, patch, 5);
		}
	}
}

namespace HkIServerImpl
{
	EXPORT bool __stdcall Startup_AFTER(struct SStartupInfo const &p1)
	{
		returncode = DEFAULT_RETURNCODE;
		StartupCache::Done();
		return true;
	}

	EXPORT bool __stdcall Startup(struct SStartupInfo const &p1)
	{
		returncode = DEFAULT_RETURNCODE;
		StartupCache::Init();
		return true;
	}

	EXPORT void __stdcall Login(struct SLoginInfo const &li, unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;

		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		if (acc)
		{
			wstring wscDir;
			HkGetAccountDirName(acc, wscDir);
			
			char szDataPath[MAX_PATH];
			GetUserDataPath(szDataPath);
	
			string path = string(szDataPath) + "\\Accts\\MultiPlayer\\" + wstos(wscDir) + "\\banned";

			FILE *file = fopen(path.c_str(), "r");
			if (file)
			{
				fclose(file);
			
				// Забанить игрока
				flstr *flStr = CreateWString(acc->wszAccID);
				Players.BanAccount(*flStr, true);
				FreeWString(flStr);

				// Пнуть его
				acc->ForceLogout();

				// И остановить дальнейшую обработку.
				returncode = SKIPPLUGINS_NOFUNCTIONCALL;
			}
		}
	}
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "fast start plugin by Cannon";
	p_PI->sShortName = "faststart";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Startup_AFTER, PLUGIN_HkIServerImpl_Startup_AFTER,0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Startup, PLUGIN_HkIServerImpl_Startup,0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Login, PLUGIN_HkIServerImpl_Login,0));
	return p_PI;
}