/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 18 апреля 2026 14:45:47
 * Version: 1.0.466
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

#include "Shlwapi.h"

#define MIN_CHAR_TAG_LEN 3

namespace Rename
{
	// Стоимость переименования в кредитах
	int set_iMoveCost = 0;

	// Стоимость переименования в кредитах
	int set_iRenameCost = 0;

	// Переименование не допускается, если попытка переименования предпринята в течение отведенного времени (в секундах)
	int set_iRenameTimeLimit = 0;

	// True, если поддерживаются теги charname
	bool set_bCharnameTags = false;

	// True, если поддерживаются только теги ascii
	bool set_bAsciiCharnameOnly = false;

	// Стоимость изготовления бирки
	int set_iMakeTagCost = 50000000;

	struct TAG_DATA
	{
		wstring tag;
		wstring master_password;
		wstring rename_password;
		uint last_access;
		wstring description;
	};

	std::map<wstring, TAG_DATA> mapTagToPassword;

	struct LockedShipsStruct
	{
		wstring AccountName;
		int LockLevel;
	};

	map<wstring, LockedShipsStruct> MapLockedShips;

	void LoadSettings(const string &scPluginCfgFile)
	{
		set_iRenameCost = IniGetI(scPluginCfgFile, "Rename", "RenameCost", 5000000);
		set_iRenameTimeLimit = IniGetI(scPluginCfgFile, "Rename", "RenameTimeLimit", 3600);
		set_iMoveCost = IniGetI(scPluginCfgFile, "Rename", "MoveCost", 5000000);
		set_bCharnameTags = IniGetB(scPluginCfgFile, "Rename", "CharnameTag", false);
		set_bAsciiCharnameOnly = IniGetB(scPluginCfgFile, "Rename", "AsciiCharnameOnly", true);
		set_iMakeTagCost = IniGetI(scPluginCfgFile, "Rename", "MakeTagCost", 50000000);

		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scPath = string(szDataPath) + "\\Accts\\MultiPlayer\\tags.ini";

		INI_Reader ini;
		if (ini.open(scPath.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header("faction"))
				{
					wstring tag;
					while (ini.read_value())
					{
						if (ini.is_value("tag"))
						{
							ini_get_wstring(ini, tag);
							mapTagToPassword[tag].tag = tag;
						}
						else if (ini.is_value("master_password"))
						{
							wstring pass;
							ini_get_wstring(ini, pass);
							mapTagToPassword[tag].master_password = pass;
						}
						else if (ini.is_value("rename_password"))
						{
							wstring pass;
							ini_get_wstring(ini, pass);
							mapTagToPassword[tag].rename_password = pass;
						}
						else if (ini.is_value("last_access"))
						{
							mapTagToPassword[tag].last_access = ini.get_value_int(0);
						}
						else if (ini.is_value("description"))
						{
							wstring description;
							ini_get_wstring(ini, description);
							mapTagToPassword[tag].description = description;
						}
					}
				}
			}
			ini.close();
		}
	}

	void SaveSettings()
	{
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scPath = string(szDataPath) + "\\Accts\\MultiPlayer\\tags.ini";

		FILE *file = fopen(scPath.c_str(), "w");
		if (file)
		{
			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				fprintf(file, "[faction]\n");
				ini_write_wstring(file, "tag", i->second.tag);
				ini_write_wstring(file, "master_password", i->second.master_password);
				ini_write_wstring(file, "rename_password", i->second.rename_password);
				ini_write_wstring(file, "description", i->second.description);
				fprintf(file, "last_access = %u\n", i->second.last_access);
			}
			fclose(file);
		}
	}

	bool CreateNewCharacter(struct SCreateCharacterInfo const &si, unsigned int iClientID)
	{
		if (set_bCharnameTags)
		{
			// Если это название корабля начинается с тега с ограниченным доступом, 
			// то корабль может быть создан только с использованием rename и пароля фракции
			wstring wscCharname(si.wszCharname);
			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				if (WstrInsensitiveFind(wscCharname, i->second.tag) == 0
					&& i->second.rename_password.size() != 0)
				{
					Server.CharacterInfoReq(iClientID, true);
					return true;
				}
			}

			// Если это название судна слишком короткое, отклоните запрос
			if (wscCharname.size() < MIN_CHAR_TAG_LEN + 1)
			{
				Server.CharacterInfoReq(iClientID, true);
				return true;
			}
		}

		if (set_bAsciiCharnameOnly)
		{
			wstring wscCharname(si.wszCharname);
			for (uint i = 0; i < wscCharname.size(); i++)
			{
				wchar_t ch = wscCharname[i];
				if (ch & 0xFF80)
					return true;
			}
		}

		return false;
	}

	// Обновление списка тегов при выборе персонажа Обновите список тегов, чтобы указать, что этот тег используется.
	// Если метка не используется в течение 60 дней, удалите ее.
	void CharacterSelect_AFTER(struct CHARACTER_ID const &charId, unsigned int iClientID)
	{
		if (set_bCharnameTags)
		{
			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				if (wscCharname.find(i->second.tag) == 0)
				{
					i->second.last_access = (uint)time(0);
				}
			}
		}
	}

	bool UserCmd_MakeTag(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (set_bCharnameTags)
		{
			// Укажите ошибку, если команда отображается неправильно отформатированной, 
			// и остановите обработку, но сообщите FLHook, что мы обработали команду.
			if (wscParam.size() == 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1145"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}

			uint iBaseID;
			pub::Player::GetBase(iClientID, iBaseID);
			if (!iBaseID)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1146"));
				return true;
			}

			wstring tag = GetParam(wscParam, ' ', 0);
			wstring pass = GetParam(wscParam, ' ', 1);
			wstring description = GetParamToEnd(wscParam, ' ', 2);

			if (tag.size() < MIN_CHAR_TAG_LEN)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1147"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}

			if (!pass.size())
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1148"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}

			if (!description.size())
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1149"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}

			// Если этот тег используется, то отклоните запрос.
			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				if (tag.find(i->second.tag) == 0 || i->second.tag.find(tag) == 0)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1150"));
					return true;
				}
			}

			// Сохраните персонажа и выйдите, если его выкинули при сохранении.
			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			HkSaveChar(wscCharname);
			if (HkGetClientIdFromCharname(wscCharname) == -1)
				return false;

			int iCash;
			HK_ERROR err;
			if ((err = HkGetCash(wscCharname, iCash)) != HKE_OK)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
				return true;
			}
			if (set_iMakeTagCost > 0 && iCash < set_iMakeTagCost)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1152"));
				return true;
			}

			HkAddCash(wscCharname, 0 - set_iMakeTagCost);

			// TODO: Попробуйте проверить, использует ли кто-нибудь из игроков этот тег
			mapTagToPassword[tag].tag = tag;
			mapTagToPassword[tag].master_password = pass;
			mapTagToPassword[tag].rename_password = L"";
			mapTagToPassword[tag].last_access = (uint)time(0);
			mapTagToPassword[tag].description = description;

			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1153"), tag.c_str(), pass.c_str());
			AddLog("NOTICE: Tag %s created by %s (%s)", wstos(tag).c_str(), wstos(wscCharname).c_str(), wstos(HkGetAccountIDByClientID(iClientID)).c_str());
			SaveSettings();
			return true;
		}
		return false;
	}

	bool UserCmd_DropTag(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (set_bCharnameTags)
		{
			// Укажите ошибку, если команда отображается неправильно отформатированной, 
			// и остановите обработку, но сообщите FLHook, что мы обработали команду.
			if (wscParam.size() == 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1145"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}

			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			wstring tag = GetParam(wscParam, ' ', 0);
			wstring pass = GetParam(wscParam, ' ', 1);

			// Если этот тег используется, то отклоните запрос.
			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				if (tag == i->second.tag && pass == i->second.master_password)
				{
					mapTagToPassword.erase(tag);
					SaveSettings();
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1155"));
					AddLog("NOTICE: Tag %s dropped by %s (%s)", wstos(tag).c_str(), wstos(wscCharname).c_str(), wstos(HkGetAccountIDByClientID(iClientID)).c_str());
					return true;
				}
			}

			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1156"));
			return true;
		}
		return false;
	}

	// Создание пароля тега
	bool UserCmd_SetTagPass(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (set_bCharnameTags)
		{
			// Укажите ошибку, если команда отображается неправильно отформатированной, 
			// и остановите обработку, но сообщите FLHook, что мы обработали команду.
			if (wscParam.size() == 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1145"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}

			wstring tag = GetParam(wscParam, ' ', 0);
			wstring master_password = GetParam(wscParam, ' ', 1);
			wstring rename_password = GetParam(wscParam, ' ', 2);

			// Если этот тег используется, то отклоните запрос.
			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				if (tag == i->second.tag && master_password == i->second.master_password)
				{
					i->second.rename_password = rename_password;
					SaveSettings();
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1158"), rename_password.c_str(), tag.c_str());
					return true;
				}
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1156"));
			return true;
		}
		return false;
	}

	struct RENAME
	{
		wstring wscCharname;
		wstring wscNewCharname;

		string scSourceFile;
		string scDestFile;
		string scDestFileTemp;
	};
	std::list<RENAME> pendingRenames;

	struct MOVE
	{
		wstring wscDestinationCharname;
		wstring wscMovingCharname;

		string scSourceFile;
		string scDestFile;
		string scDestFileTemp;
	};
	std::list<MOVE> pendingMoves;

	void Timer()
	{
		// Каждые 100 секунд истекает срок действия неиспользуемых тегов и сохраняется база данных тегов
		/* uint curr_time = (uint)time(0);
		if (curr_time % 100)
		{
			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				if (i->second.last_access < (curr_time - (3600 * 24 * 30)))
				{
					mapTagToPassword.erase(i);
					break;
				}
			}
			SaveSettings();
		} */

		// Проверьте наличие ожидающих переименований и выполните их. 
		// Мы делаем это по таймеру, чтобы игрок точно не был в сети, когда мы делаем переименование.
		while (pendingRenames.size())
		{
			RENAME o = pendingRenames.front();
			if (HkGetClientIdFromCharname(o.wscCharname) != -1)
				return;

			pendingRenames.pop_front();

			CAccount *acc = HkGetAccountByCharname(o.wscCharname);

			// Удалите персонажа из существующей учетной записи, 
			// создайте нового персонажа с таким же именем в этой учетной записи, 
			// а затем скопируйте его с помощью файла сохранения персонажа.
			try
			{
				if (!acc)
					throw "no acc";

				HkLockAccountAccess(acc, true);
				HkUnlockAccountAccess(acc);

				// Переместите файл char во временный.
				if (!::MoveFileExA(o.scSourceFile.c_str(), o.scDestFileTemp.c_str(),
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
					throw "move src to temp failed";

				// Декодируйте файл char, обновите имя символа и перекодируйте его.
				// Добавьте пробел к значению, чтобы строка ini-файла выглядела как "<key> = <value>"
				// в противном случае оператор сервера Ioncross не сможет правильно расшифровать файл
				flc_decode(o.scDestFileTemp.c_str(), o.scDestFileTemp.c_str());
				IniWriteW(o.scDestFileTemp, "Player", "Name", o.wscNewCharname);
				if (!set_bDisableCharfileEncryption)
				{
					flc_encode(o.scDestFileTemp.c_str(), o.scDestFileTemp.c_str());
				}

				// Создание и удаление персонажа
				HkDeleteCharacter(acc, o.wscCharname);
				HkNewCharacter(acc, o.wscNewCharname);

				// Перемещение файлов
				if (!::MoveFileExA(o.scDestFileTemp.c_str(), o.scDestFile.c_str(),
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
					throw "move failed";
				if (::PathFileExistsA(o.scSourceFile.c_str()))
					throw "src still exists";
				if (!::PathFileExistsA(o.scDestFile.c_str()))
					throw "dest does not exist";

				// Переименование сработало. Зарегистрируйте его и сэкономьте время на переименование.
				AddLog("NOTICE: User rename %s to %s (%s)", wstos(o.wscCharname).c_str(), wstos(o.wscNewCharname).c_str(), wstos(HkGetAccountID(acc)).c_str());
			}
			catch (char *err)
			{
				AddLog("ERROR: User rename failed (%s) from %s to %s (%s)", err, wstos(o.wscCharname).c_str(), wstos(o.wscNewCharname).c_str(), wstos(HkGetAccountID(acc)).c_str());
			}
		}

		while (pendingMoves.size())
		{
			MOVE o = pendingMoves.front();
			if (HkGetClientIdFromCharname(o.wscDestinationCharname) != -1)
				return;
			if (HkGetClientIdFromCharname(o.wscMovingCharname) != -1)
				return;

			pendingMoves.pop_front();

			CAccount *acc = HkGetAccountByCharname(o.wscDestinationCharname);
			CAccount *oldAcc = HkGetAccountByCharname(o.wscMovingCharname);

			// Удалить персонажа из существующей учетной записи, cоздайте нового персонажа с помощью команды
			// в этом аккаунте, а затем скопируйте его с помощью файла с сохранением символа.
			try
			{
				HkLockAccountAccess(acc, true);
				HkUnlockAccountAccess(acc);

				HkLockAccountAccess(oldAcc, true);
				HkUnlockAccountAccess(oldAcc);

				// Перемещение файла char во временный.
				if (!::MoveFileExA(o.scSourceFile.c_str(), o.scDestFileTemp.c_str(),
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
					throw "move src to temp failed";

				// Создание и удаление персонажа
				HkDeleteCharacter(oldAcc, o.wscMovingCharname);
				HkNewCharacter(acc, o.wscMovingCharname);

				// Перемещение файлов
				if (!::MoveFileExA(o.scDestFileTemp.c_str(), o.scDestFile.c_str(),
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
					throw "move failed";
				if (::PathFileExistsA(o.scSourceFile.c_str()))
					throw "src still exists";
				if (!::PathFileExistsA(o.scDestFile.c_str()))
					throw "dest does not exist";

				// Этот ход сработал. Записать это в журнал.
				AddLog("NOTICE: Character %s moved from %s to %s",
					wstos(o.wscMovingCharname).c_str(),
					wstos(HkGetAccountID(oldAcc)).c_str(),
					wstos(HkGetAccountID(acc)).c_str());

			}
			catch (char *err)
			{
				AddLog("ERROR: Character %s move failed (%s) from %s to %s",
					wstos(o.wscMovingCharname).c_str(), err,
					wstos(HkGetAccountID(oldAcc)).c_str(),
					wstos(HkGetAccountID(acc)).c_str());
			}
		}
	}

	bool UserCmd_RenameMe(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		HK_ERROR err;

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1263"));
		return true;

		// Не указывайте ошибку, если перемещение отключено.
		if (!set_bEnableRenameMe)
			return false;

		// Укажите ошибку, если команда отображается неправильно отформатированной
		// и остановите обработку, но сообщите FLHook, что мы обработали команду.
		if (wscParam.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1145"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}
		
		uint iBaseID;
		pub::Player::GetBase(iClientID, iBaseID);
		if (!iBaseID)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1146"));
			return true;
		}

		// Если новое имя содержит пробелы, то отметьте это как ошибку.
		wstring wscNewCharname = Trim(GetParam(wscParam, L' ', 0));

		// Получите имя персонажа для этого подключения.
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		if (MapLockedShips.count(ToLower(wscCharname)) && MapLockedShips[ToLower(wscCharname)].LockLevel > 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1162"));
			wstring spurdoip;
			HkGetPlayerIP(iClientID, spurdoip);
			AddLog("SHIPLOCK: Attempt to rename locked ship %s from IP %s", wstos(wscCharname).c_str(), wstos(spurdoip).c_str());
			ConPrint(L"SHIPLOCK: Attempt to rename locked ship %s from IP %s\n", wscCharname.c_str(), spurdoip.c_str());
			return true;
		}

		if (wscNewCharname.find(L" ") != -1)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1163"));
			return true;
		}

		if (HkGetAccountByCharname(wscNewCharname))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1164"));
			return true;
		}

		if (wscNewCharname.length() > 23)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1165"));
			return true;
		}

		if (wscNewCharname.length() < MIN_CHAR_TAG_LEN)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1166"));
			return true;
		}

		if (set_bCharnameTags)
		{
			wstring wscPassword = Trim(GetParam(wscParam, L' ', 1));

			for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
			{
				if (WstrInsensitiveFind(wscNewCharname, i->first) == 0
					&& i->second.rename_password.size() != 0)
				{
					if (!wscPassword.length())
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1167"));
						return true;
					}
					else if (wscPassword != i->second.master_password
						&& wscPassword != i->second.rename_password)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1168"));
						return true;
					}
					// Пароль действителен для принадлежащего тега.
					break;
				}
			}
		}

		// Сохранение персонажей заставляет античит проверять и исправлять множество других проблем.
		HkSaveChar(wscCharname);
		if (!HkIsValidClientID(iClientID))
			return true;

		// Считайте текущее количество кредитов для игрока и проверьте, достаточно ли у персонажа денег.
		int iCash = 0;
		if ((err = HkGetCash(wscCharname, iCash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}
		if (set_iRenameCost > 0 && iCash < set_iRenameCost)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1152"));
			return true;
		}

		// Прочитайте, когда в последний раз переименовывали этого персонажа
		wstring wscDir;
		if ((err = HkGetAccountDirName(wscCharname, wscDir)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}
		string scRenameFile = scAcctPath + wstos(wscDir) + "\\" + "rename.ini";
		int lastRenameTime = IniGetI(scRenameFile, "General", wstos(wscCharname), 0);

		// Если переименование было недавно сделано этим игроком, отклоните запрос.
		// Я знаю, что time() возвращает time_t...это не должно иметь значения в течение нескольких лет.
		if ((lastRenameTime + 300) < (int)time(0))
		{
			if ((lastRenameTime + set_iRenameTimeLimit) > (int)time(0))
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1172"));
				return true;
			}
		}

		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\";

		wstring wscSourceFile;
		if ((err = HkGetCharFileName(wscCharname, wscSourceFile)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}
		wstring wscDestFile;
		if ((err = HkGetCharFileName(wscNewCharname, wscDestFile)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}

		// Снимите наличные, если взимали за них плату.
		if (set_iRenameCost > 0)
			HkAddCash(wscCharname, 0 - set_iRenameCost);


		RENAME o;
		o.wscCharname = wscCharname;
		o.wscNewCharname = wscNewCharname;
		o.scSourceFile = scAcctPath + wstos(wscDir) + "\\" + wstos(wscSourceFile) + ".fl";
		o.scDestFile = scAcctPath + wstos(wscDir) + "\\" + wstos(wscDestFile) + ".fl";
		o.scDestFileTemp = scAcctPath + wstos(wscDir) + "\\" + wstos(wscSourceFile) + ".fl.renaming";
		pendingRenames.push_back(o);

		HkKickReason(o.wscCharname, GetLocalized(iClientID, "MSG_1551"));
		IniWrite(scRenameFile, "General", wstos(o.wscNewCharname), itos((int)time(0)));
		return true;
	}

	/* Обработка команды set the move char code */
	bool Rename::UserCmd_SetMoveCharCode(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		// Не отображать ошибку, если перемещение отключено.
		if (!set_bEnableMoveChar)
			return false;

		if (wscParam.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1145"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		string scFile;
		if (!GetUserFilePath(scFile, wscCharname, "-movechar.ini"))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1176"));
			return true;
		}

		wstring wscCode = Trim(GetParam(wscParam, L' ', 0));
		if (wscCode == L"none")
		{
			IniWriteW(scFile, "Settings", "Code", L"");
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1177"));
		}
		else
		{
			IniWriteW(scFile, "Settings", "Code", wscCode);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1178") + wscCode);
		}
		return true;
	}

	static bool IsBanned(wstring charname)
	{
		char datapath[MAX_PATH];
		GetUserDataPath(datapath);

		wstring dir;
		HkGetAccountDirName(charname, dir);

		string banfile = string(datapath) + "\\Accts\\MultiPlayer\\" + wstos(dir) + "\\banned";

		// Предотвратите перемещение заблокированных учетных записей кораблей.
		FILE *f = fopen(banfile.c_str(), "r");
		if (f)
		{
			fclose(f);
			return true;
		}
		return false;
	}

	/**
	 Переместите персонажа из удаленной учетной записи в эту.
	*/
	bool Rename::UserCmd_MoveChar(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		HK_ERROR err;

		// Не указывайте ошибку, если перемещение отключено.
		if (!set_bEnableMoveChar)
			return false;

		// Укажите ошибку, если команда отображается неправильно отформатированной
		// и остановите обработку, но сообщите FLHook, что мы обработали команду.
		if (wscParam.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1145"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		uint iBaseID;
		pub::Player::GetBase(iClientID, iBaseID);
		if (!iBaseID)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1146"));
			return true;
		}

		// Получите каталог целевой учетной записи.
		string scFile;
		wstring wscMovingCharname = Trim(GetParam(wscParam, L' ', 0));
		wstring wscMovingCharnameLower = ToLower(wscMovingCharname);
		if (!GetUserFilePath(scFile, wscMovingCharname, "-movechar.ini"))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1176"));
			return true;
		}

		// Проверьте код символа перемещения.
		wstring wscCode = Trim(GetParam(wscParam, L' ', 1));
		wstring wscTargetCode = IniGetWS(scFile, "Settings", "Code", L"");
		if (!wscTargetCode.length() || wscTargetCode != wscCode)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1182"));
			return true;
		}

		// Получите имя персонажа для этого подключения.
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		if (MapLockedShips.count(wscMovingCharnameLower) && MapLockedShips[wscMovingCharnameLower].LockLevel > 0) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1183"));
			wstring spurdoip;
			HkGetPlayerIP(iClientID, spurdoip);
			AddLog("SHIPLOCK: Attempt to movechar locked ship %s from IP %s", wstos(wscMovingCharname).c_str(), wstos(spurdoip).c_str());
			ConPrint(L"SHIPLOCK: Attempt to movechar locked ship %s from IP %s\n", wscMovingCharname.c_str(), spurdoip.c_str());
			return true;
		}

		// Предотвратите перемещение заблокированных учетных записей кораблей.
		if (IsBanned(wscMovingCharname))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1184"));
			return true;
		}

		// Сохранение персонажей заставляет античит проверять и исправлять множество других проблем.
		HkSaveChar(wscCharname);
		HkSaveChar(wscMovingCharname);

		// Читайте текущее количество кредитов для игрока и проверяйте, достаточно ли у персонажа денежных средств.
		int iCash = 0;
		if ((err = HkGetCash(wscCharname, iCash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}
		if (set_iMoveCost > 0 && iCash < set_iMoveCost)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1152"));
			return true;
		}

		// Проверьте, есть ли место в этом аккаунте.
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		if (acc->iNumberOfCharacters >= 7)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1187"));
			return true;
		}

		// Скопируйте файл символов в эту учетную запись с временным именем.
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\";

		wstring wscDir;
		wstring wscSourceDir;
		wstring wscSourceFile;
		if ((err = HkGetAccountDirName(wscCharname, wscDir)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}
		if ((err = HkGetAccountDirName(wscMovingCharname, wscSourceDir)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}
		if ((err = HkGetCharFileName(wscMovingCharname, wscSourceFile)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1151") + HkErrGetText(err));
			return true;
		}

		// Снимите наличные, если взимали за них плату.
		if (set_iMoveCost > 0)
			HkAddCash(wscCharname, 0 - set_iMoveCost);
		HkSaveChar(wscCharname);

		// Запланируйте переезд
		MOVE o;
		o.wscDestinationCharname = wscCharname;
		o.wscMovingCharname = wscMovingCharname;
		o.scSourceFile = scAcctPath + wstos(wscSourceDir) + "\\" + wstos(wscSourceFile) + ".fl";
		o.scDestFile = scAcctPath + wstos(wscDir) + "\\" + wstos(wscSourceFile) + ".fl";
		o.scDestFileTemp = scAcctPath + wstos(wscDir) + "\\" + wstos(wscSourceFile) + ".fl.moving";
		pendingMoves.push_back(o);

		// Удаление кода перемещения
		::DeleteFileA(scFile.c_str());

		HkKickReason(o.wscDestinationCharname, GetLocalized(iClientID, "MSG_1583"));
		HkKickReason(o.wscMovingCharname, GetLocalized(iClientID, "MSG_1583"));
		return true;
	}

	/// Установка кода символа перемещения для всех символов в учетной записи
	void Rename::AdminCmd_SetAccMoveCode(CCmds* cmds, const wstring &wscCharname, const wstring &wscCode)
	{
		// Не указывайте ошибку, если перемещение отключено.
		if (!set_bEnableMoveChar)
			return;

		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		wstring wscDir;
		if (HkGetAccountDirName(wscCharname, wscDir) != HKE_OK)
		{
			cmds->Print(L"ERR Charname not found\n");
			return;
		}

		if (wscCode.length() == 0)
		{
			cmds->Print(L"ERR Code too small, set to none to clear.\n");
			return;
		}

		// Получите путь к учетной записи.
		char szDataPath[MAX_PATH];
		GetUserDataPath(szDataPath);
		string scPath = string(szDataPath) + "\\Accts\\MultiPlayer\\" + wstos(wscDir) + "\\*.fl";

		// Откройте итератор каталога.
		WIN32_FIND_DATA FindFileData;
		HANDLE hFileFind = FindFirstFile(scPath.c_str(), &FindFileData);
		if (hFileFind == INVALID_HANDLE_VALUE)
		{
			cmds->Print(L"ERR Account directory not found\n");
			return;
		}

		// Повторяйте его
		do
		{
			string scCharfile = FindFileData.cFileName;
			string scMoveCodeFile = string(szDataPath) + "\\Accts\\MultiPlayer\\" + wstos(wscDir) + "\\"
				+ scCharfile.substr(0, scCharfile.size() - 3) + "-movechar.ini";
			if (wscCode == L"none")
			{
				IniWriteW(scMoveCodeFile, "Settings", "Code", L"");
				cmds->Print(L"OK Movechar code cleared on " + stows(scCharfile) + L"\n");
			}
			else
			{
				IniWriteW(scMoveCodeFile, "Settings", "Code", wscCode);
				cmds->Print(L"OK Movechar code set to " + wscCode + L" on " + stows(scCharfile) + L"\n");
			}
		} while (FindNextFile(hFileFind, &FindFileData));
		FindClose(hFileFind);

		cmds->Print(L"OK\n");
	}

	/// Показывает список тегов и их паролей с описанием и последним использованием
	void AdminCmd_ShowTags(CCmds* cmds)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		uint curr_time = (uint)time(0);
		for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
		{
			int last_access = i->second.last_access;
			int days = (curr_time - last_access) / (24 * 3600);
			cmds->Print(L"tag=%s master_password=%s rename_password=%s last_access=%u days description=%s\n",
				i->second.tag.c_str(), i->second.master_password.c_str(), i->second.rename_password.c_str(), days, i->second.description.c_str());
		}
		cmds->Print(L"OK\n");
	}

	void AdminCmd_AddTag(CCmds* cmds, const wstring &tag, const wstring &password, const wstring &description)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		if (tag.size() < 3)
		{
			cmds->Print(L"ERR Tag too short\n");
			return;
		}

		if (!password.size())
		{
			cmds->Print(L"ERR Password not set\n");
			return;
		}

		if (!description.size())
		{
			cmds->Print(L"ERR Description not set\n");
			return;
		}

		// Если этот тег используется, то отклоните запрос.
		for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
		{
			if (tag.find(i->second.tag) == 0 || i->second.tag.find(tag) == 0)
			{
				cmds->Print(L"ERR Tag already exists or conflicts with another tag\n");
				return;
			}
		}

		mapTagToPassword[tag].tag = tag;
		mapTagToPassword[tag].master_password = password;
		mapTagToPassword[tag].rename_password = L"";
		mapTagToPassword[tag].last_access = (uint)time(0);
		mapTagToPassword[tag].description = description;
		cmds->Print(L"Created faction tag %s with master password %s\n", tag.c_str(), password.c_str());
		SaveSettings();
	}

	void AdminCmd_DropTag(CCmds* cmds, const wstring &tag)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		// Если этот тег используется, то отклоните запрос.
		for (std::map<wstring, TAG_DATA>::iterator i = mapTagToPassword.begin(); i != mapTagToPassword.end(); ++i)
		{
			if (tag == i->second.tag)
			{
				mapTagToPassword.erase(tag);
				SaveSettings();
				cmds->Print(L"OK Tag dropped\n");
				return;
			}
		}

		cmds->Print(L"ERR tag is invalid\n");
		return;
	}
}

void Rename::ReloadLockedShips()
{
	// Путь к файлу конфигурации.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\alley_lockedships.cfg";

	MapLockedShips.clear();

	INI_Reader ini;
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("ships"))
			{
				int numberoflockedships = 0;
				while (ini.read_value())
				{
					if (ini.is_value("ship"))
					{
						LockedShipsStruct info;

						wstring ship = stows(ini.get_value_string(0));
						wstring filename;
						HkGetCharFileName(ship, filename);

						info.AccountName = filename + L".fl";
						info.LockLevel = ini.get_value_int(1);

						MapLockedShips[ToLower(ship)] = info;
						++numberoflockedships;
					}
				}
				ConPrint(L"Notice: Loaded %i locked ships\n", numberoflockedships);
			}
		}
	}
	ini.close();

	return;
}

bool Rename::DestroyCharacter(struct CHARACTER_ID const &cId, unsigned int iClientID)
{
	// Скопируйте файл символов в эту учетную запись с временным именем.
	char szDataPath[MAX_PATH];
	GetUserDataPath(szDataPath);
	string scAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\";

	wstring file = stows(cId.szCharFilename);
	// Получите имя персонажа для этого подключения.
	//wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

	for (map<wstring, LockedShipsStruct>::iterator i = MapLockedShips.begin(); i != MapLockedShips.end(); ++i)
	{
		if ((i->second.AccountName == file) && (i->second.LockLevel > 0))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1191"));
			wstring spurdoip;
			HkGetPlayerIP(iClientID, spurdoip);
			AddLog("SHIPLOCK: Attempt to delete locked ship %s from IP %s", wstos(i->first).c_str(), wstos(spurdoip).c_str());
			ConPrint(L"SHIPLOCK: Attempt to delete locked ship %s from IP %s\n", i->first.c_str(), spurdoip.c_str());
			HkKick(ARG_CLIENTID(iClientID));
			return true;
		}
	}

	return false;
}

bool Rename::IsLockedShip(uint iClientID, int PermissionLevel)
{
	wstring wsccharname = ToLower((const wchar_t*)Players.GetActiveCharacterName(iClientID));
	if (MapLockedShips.find(wsccharname) != MapLockedShips.end())
	{
		if (MapLockedShips[wsccharname].LockLevel >= PermissionLevel)
		{
			return true;
		}
	}

	return false;
}
