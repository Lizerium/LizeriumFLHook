/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 апреля 2026 06:54:28
 * Version: 1.0.471
 */

// Player Control plugin for FLHookPlugin
// Feb 2010 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

#include <windows.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <io.h>
#include <string>
#include <time.h>

#include <FLHook.h>
#include <plugin.h>
#include <FLCoreServer.h>
#include <FLCoreCommon.h>
#include <PluginUtilities.h>
#include "Mail.h"
#include "ZoneUtilities.h"

#include <math.h>
#include <map>
#include <list>
#include <vector>

#include "Main.h"

#include <hookext_exports.h>

#define GET_USERFILE(a) string a; { CAccount *acc = Players.FindAccountFromClientID(iClientID); wstring wscDir; HkGetAccountDirName(acc, wscDir); a = scAcctPath + wstos(wscDir) + "\\flhookuser.ini"; }

/// Диапазон локального чата
float set_iLocalChatRangeUtl = 9999;
/// Запись людей с помощью /pm /r и /t
/// TODO: Превратите это в общую функцию ведения журнала и переместите ее в PluginUtilities
FILE *PMLogfile = fopen("./flhook_logs/private_chats.log", "at");

void PMLogging(const char *szString, ...)
{
	char szBufString[1024];
	va_list marker;
	va_start(marker, szString);
	_vsnprintf(szBufString, sizeof(szBufString) - 1, szString, marker);

	char szBuf[64];
	time_t tNow = time(0);
	struct tm *t = localtime(&tNow);
	strftime(szBuf, sizeof(szBuf), "%d/%m/%Y %H:%M:%S", t);
	fprintf(PMLogfile, "%s %s\n", szBuf, szBufString);
	fflush(PMLogfile);
	fclose(PMLogfile);
	PMLogfile = fopen("./flhook_logs/private_chats.log", "at");
}

/// Load the configuration
void LoadSettingsUtl()
{
	// The path to the configuration file.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\playercntl.cfg";

	set_iLocalChatRangeUtl = IniGetF(scPluginCfgFile, "General", "LocalChatRange", 0);
}

/** Отправка сообщения об отправке игрока в локальную систему */
void SendLocalSystemChat(uint iFromClientID, const wstring &wscText)
{
	wstring wscSender = (const wchar_t*)Players.GetActiveCharacterName(iFromClientID);

	// Узнайте текущую систему игроков и их местоположение в системе.
	uint iSystemID;
	pub::Player::GetSystem(iFromClientID, iSystemID);

	uint iFromShip;
	pub::Player::GetShip(iFromClientID, iFromShip);

	Vector vFromShipLoc;
	Matrix mFromShipDir;
	pub::SpaceObj::GetLocation(iFromShip, vFromShipLoc, mFromShipDir);

	// Для всех игроков в системе...
	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		// Узнайте текущую систему этого игрока и его местоположение в системе.
		uint iClientID = HkGetClientIdFromPD(pPD);
		uint iClientSystemID = 0;
		pub::Player::GetSystem(iClientID, iClientSystemID);
		if (iSystemID != iClientSystemID)
			continue;

		uint iShip;
		pub::Player::GetShip(iClientID, iShip);

		Vector vShipLoc;
		Matrix mShipDir;
		pub::SpaceObj::GetLocation(iShip, vShipLoc, mShipDir);

		// Обман в расчете расстояния. Игнорирование оси Y.
		float fDistance = sqrt(pow(vShipLoc.x - vFromShipLoc.x, 2) + pow(vShipLoc.z - vFromShipLoc.z, 2));

		// Находится ли игрок в пределах диапазона сканера (15K) от отправляющего символа.
		if (fDistance > set_iLocalChatRangeUtl)
			continue;

		// Отправить сообщение игроку в этой системе.
		FormatSendChat(iClientID, wscSender, wscText, L"FF8F40");
	}
}

/** Отправить сообщение игроку */
void SendPrivateChat(uint iFromClientID, uint iToClientID, const wstring &wscText)
{
	wstring wscSender = (const wchar_t*)Players.GetActiveCharacterName(iFromClientID);

	if (set_bUserCmdIgnore)
	{
		foreach(ClientInfo[iToClientID].lstIgnore, IGNORE_INFO, it)
		{
			if (HAS_FLAG(*it, L"p"))
				return;
		}
	}

	// Отправка сообщения отправителю и получателю.
	FormatSendChat(iToClientID, wscSender, wscText, L"19BD3A");
	FormatSendChat(iFromClientID, wscSender, wscText, L"19BD3A");
	//Alleymarker02

	wstring wscCharnameSender = (const wchar_t*)Players.GetActiveCharacterName(iFromClientID);
	wstring wscCharnameReceiver = (const wchar_t*)Players.GetActiveCharacterName(iToClientID);

	wstring wscMsg = L"%sender->%receiver: %message";
	wscMsg = ReplaceStr(wscMsg, L"%sender", wscCharnameSender.c_str());
	wscMsg = ReplaceStr(wscMsg, L"%receiver", wscCharnameReceiver.c_str());
	wscMsg = ReplaceStr(wscMsg, L"%message", wscText);
	string scText = wstos(wscMsg);
	//PMLogging("much strange");
	//PrintUserCmdText(iFromClientID, GetLocalized(iClientID, "MSG_0989"), stows(scText).c_str());
	//PrintUserCmdText(iFromClientID, GetLocalized(iClientID, "MSG_0990"), wscCharnameSender.c_str());
	//PrintUserCmdText(iFromClientID, GetLocalized(iClientID, "MSG_0991"), wscCharnameReceiver.c_str());
	PMLogging("%s", scText.c_str());

}

/** Отправка сообщения об отправке игрока в систему */
void SendSystemChat(uint iFromClientID, const wstring &wscText)
{
	wstring wscSender = (const wchar_t*)Players.GetActiveCharacterName(iFromClientID);

	// Получите текущую систему игроков.
	uint iSystemID;
	pub::Player::GetSystem(iFromClientID, iSystemID);

	// Для всех игроков в системе...
	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint iClientID = HkGetClientIdFromPD(pPD);
		uint iClientSystemID = 0;
		pub::Player::GetSystem(iClientID, iClientSystemID);
		if (iSystemID == iClientSystemID)
		{
			// Отправьте сообщение игроку в этой системе.
			FormatSendChat(iClientID, wscSender, wscText, L"E6C684");
		}
	}
}

namespace Message
{
	/** Журнал сообщений плагина обмена сообщениями для оффлайн игроков */
	static string MSG_LOG = "-mail.ini";

	/** Данные для одного онлайн-игрока */
	class INFO
	{
	public:
		INFO() : ulastPmClientID(-1), uTargetClientID(-1), bShowChatTime(false), bGreetingShown(false), iSwearWordWarnings(0) {}

		static const int NUMBER_OF_SLOTS = 10;
		wstring slot[NUMBER_OF_SLOTS];

		static const int NUMBER_OF_COORDSLOTS = 10;
		wstring coordslot[NUMBER_OF_COORDSLOTS];

		// ID клиента последнего PM.
		uint ulastPmClientID;

		// Идентификатор клиента выбранной цели
		uint uTargetClientID;

		// Текущие настройки времени чата
		bool bShowChatTime;

		// Отображение текущего времени чата в настройках смерти
		bool bShowChatDieTime = true;

		// True, если был показан баннер входа.
		bool bGreetingShown;

		// Уровень предупреждения о нецензурном слове
		int iSwearWordWarnings;
	};

	/** Уровень предупреждения о нецензурном слове (по ID клиента) */
	static map<uint, INFO> mapInfo;

	/** Кэш предустановленных сообщений для онлайн-игроков (по ID клиента) */
	static map<uint, INFO> mapCoordsInfo;

	/** Текст справки о том, когда пользователь вводит текст /help */
	static list<INISECTIONVALUE> set_lstHelpLines;

	/** Текст справки о том, когда пользователь вводит текст /help */
	static map<wstring, pair<wstring, list<INISECTIONVALUE>>> set_lstHelpsLines;

	/** Текст для списка команд */
	static list<INISECTIONVALUE> set_lstCommandListLines;

	/** текст приветствия для ввода пользователем /help */
	static list<wstring> set_lstGreetingBannerLines;

	/** специальный текст баннера для ввода пользователем /help */
	static list<wstring> set_lstSpecialBannerLines;

	/** специальный текст баннера для ввода пользователем /help */
	static vector<list<wstring> > set_vctStandardBannerLines;

	/** Секундное время на повторный показ специального баннера */
	static int set_iSpecialBannerTimeout;

	/** Время в секунду на повторный показ стандартного баннера */
	static int set_iStandardBannerTimeout;

	/** true, если мы переопределяем встроенную помощь flhook */
	static bool set_bCustomHelp;

	/** true, если мы переопределяем встроенную помощь FLHOOK */
	static bool set_bCmdEcho;

	/** true, если мы не будем выводить опечатки в командах пользователя и администратора другим игрокам. */
	static bool set_bCmdHide;

	/** Если true, то поддерживайте /showmsg и /setmsg Команды */
	static bool set_bSetMsg;

	//лист выгруженных из файла смс
	list<wstring> MsgList;

	//лист выгруженных из файла координат
	list<wstring> CoordsList;

	/** Цвет эх-команд */
	static wstring set_wscCmdEchoStyle;

	static wstring set_wscDisconnectSwearingInSpaceMsg;

	static float set_fDisconnectSwearingInSpaceRange;

	/** Список нецензурных слов */
	static std::list<wstring> set_lstSwearWords;

	std::string ws2s(const std::wstring& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
		std::string r(len, '\0');
		WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
		return r;
	}

	/** Загрузка сообщений для указанного идентификатора клиента в память. */
	static void LoadMsgs(uint iClientID)
	{
		if (!set_bSetMsg)
			return;

		// очистка листа сообщений текущего пользователя
		MsgList.clear();
		CoordsList.clear();
		// ЗАГРУЗКА свежих данных из файла пользователя
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);
		string scUserFileMsg = scAcctPath + wstos(wscDir) + "\\setmsg.ini";
		for (uint i = 1; i < INFO::NUMBER_OF_SLOTS; i++)
		{
			char szBuf[64];
			sprintf(szBuf, "msg.%u", i);
			string msgnumname = IniGetS(scUserFileMsg, "", szBuf, "");
			MsgList.push_back(stows(msgnumname));
		} // - получил лист смс
		for (uint i = 1; i < INFO::NUMBER_OF_COORDSLOTS; i++)
		{
			char szBuf[64];
			sprintf(szBuf, "coordmsg.%u", i);
			string msgnumname = IniGetS(scUserFileMsg, "", szBuf, "");
			CoordsList.push_back(stows(msgnumname));
		} // - получил лист координат

		int slotsd = 0;
		// перебор с помощью итераторов
		for (auto iter = MsgList.begin(); iter != MsgList.end(); iter++)
		{
			if (slotsd < 9)
			{
				mapInfo[iClientID].slot[slotsd] = *iter;
			}
			slotsd++;
		}

		int slotsds = 0;
		// перебор с помощью итераторов
		for (auto iter = CoordsList.begin(); iter != CoordsList.end(); iter++)
		{
			if (slotsds < 9)
			{
				mapCoordsInfo[iClientID].slot[slotsds] = *iter;
			}
			slotsds++;
		}

		//очистка листа сообщений текущего пользователя после передачи в лист mapInfo
		MsgList.clear();
		CoordsList.clear();
		// Chat time settings.
		mapInfo[iClientID].bShowChatTime = HookExt::IniGetB(iClientID, "msg.chat_time");
	}

	/** Показать поздравительный баннер указанному игроку */
	static void ShowGreetingBanner(int iClientID)
	{
		if (!mapInfo[iClientID].bGreetingShown)
		{
			mapInfo[iClientID].bGreetingShown = true;
			foreach(set_lstGreetingBannerLines, wstring, iter)
			{
				if (iter->find(L"<TRA") == 0)
					HkFMsg(iClientID, *iter);
				else
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0992"), iter->c_str());
			}
		}
	}

	/** Покажите специальный баннер всем игрокам. */
	static void ShowSpecialBanner()
	{
		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint iClientID = HkGetClientIdFromPD(pPD);
			foreach(set_lstSpecialBannerLines, wstring, iter)
			{
				if (iter->find(L"<TRA") == 0)
					HkFMsg(iClientID, *iter);
				else
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0992"), iter->c_str());
			}
		}
	}

	/** Покажите специальный баннер всем игрокам. */
	static void ShowStandardBanner()
	{
		if (set_vctStandardBannerLines.size() == 0)
			return;

		static size_t iCurStandardBanner = 0;
		if (++iCurStandardBanner >= set_vctStandardBannerLines.size())
			iCurStandardBanner = 0;

		list<wstring> &lstStandardBannerSection = set_vctStandardBannerLines[iCurStandardBanner];

		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint iClientID = HkGetClientIdFromPD(pPD);

			foreach(lstStandardBannerSection, wstring, iter)
			{
				if (iter->find(L"<TRA") == 0)
					HkFMsg(iClientID, *iter);
				else
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0992"), iter->c_str());
			}
		}
	}

	static wstring SetSizeToSmall(const wstring &wscDataFormat)
	{
		uint iFormat = wcstoul(wscDataFormat.c_str() + 2, 0, 16);
		wchar_t wszStyleSmall[32];
		wcscpy(wszStyleSmall, wscDataFormat.c_str());
		swprintf(wszStyleSmall + wcslen(wszStyleSmall) - 2, sizeof(wszStyleSmall) / sizeof(wchar_t) + wcslen(wszStyleSmall) - 2, L"%02X", 0x90 | (iFormat & 7));
		return wszStyleSmall;
	}

	/** Замените теги #t и #c текущим именем цели и текущим местоположением судна.
	Возвращает false, если теги не могут быть заменены. */
	static bool ReplaceMessageTags(uint iClientID, INFO &clientData, wstring &wscMsg)
	{
		if (wscMsg.find(L"#t") != -1)
		{
			if (clientData.uTargetClientID == -1)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0995"));
				return false;
			}

			wstring wscTargetName = (const wchar_t*)Players.GetActiveCharacterName(clientData.uTargetClientID);
			wscMsg = ReplaceStr(wscMsg, L"#t", wscTargetName);
		}

		if (wscMsg.find(L"#c") != -1)
		{
			wstring wscCurrLocation = GetLocation(iClientID);
			wscMsg = ReplaceStr(wscMsg, L"#c", wscCurrLocation.c_str());
		}

		return true;
	}

	/** Очистка при отключении клиента */
	void Message::ClearClientInfo(uint iClientID)
	{
		mapInfo.erase(iClientID);
	}

	/**
	Эта функция вызывается при вызове команды администратора rehash и при загрузке модуля.
	*/
	void Message::LoadSettings(const string &scPluginCfgFile, const string& scPluginHelpsCfgFile)
	{
		set_bCustomHelp = IniGetB(scPluginCfgFile, "Message", "CustomHelp", true);
		set_bCmdEcho = IniGetB(scPluginCfgFile, "Message", "CmdEcho", true);
		set_bCmdHide = IniGetB(scPluginCfgFile, "Message", "CmdHide", true);
		set_wscCmdEchoStyle = stows(IniGetS(scPluginCfgFile, "Message", "CmdEchoStyle", "0x00AA0090"));
		set_iStandardBannerTimeout = IniGetI(scPluginCfgFile, "Message", "StandardBannerDelay", 5);
		set_iSpecialBannerTimeout = IniGetI(scPluginCfgFile, "Message", "SpecialBannerDelay", 60);
		set_wscDisconnectSwearingInSpaceMsg = stows(IniGetS(scPluginCfgFile, "Message", "DisconnectSwearingInSpaceMsg", "%player has been kicked for swearing"));
		set_fDisconnectSwearingInSpaceRange = IniGetF(scPluginCfgFile, "Message", "DisconnectSwearingInSpaceRange", 5000.0f);
		set_bSetMsg = IniGetB(scPluginCfgFile, "Message", "SetMsg", false);
		// Для каждого активного игрока загрузите свои настройки msg.
		list<HKPLAYERINFO> players = HkGetPlayers();
		foreach(players, HKPLAYERINFO, p)
		{
			LoadMsgs(p->iClientID);
		}

		// Загрузите справку, список команд, текст приветствия и баннера
		IniGetSection(scPluginCfgFile, "Help", set_lstHelpLines);
		IniGetSection(scPluginCfgFile, "CommandList", set_lstCommandListLines);

		set_lstGreetingBannerLines.clear();
		set_lstSpecialBannerLines.clear();
		set_vctStandardBannerLines.clear();

		INI_Reader ini;
		if (ini.open(scPluginCfgFile.c_str(), false))
		{
			while (ini.read_header())
			{
				if (ini.is_header("GreetingBanner"))
				{
					while (ini.read_value())
					{
						set_lstGreetingBannerLines.push_back(Trim(stows(ini.get_value_string())));
					}
				}
				else if (ini.is_header("SpecialBanner"))
				{
					while (ini.read_value())
					{
						set_lstSpecialBannerLines.push_back(Trim(stows(ini.get_value_string())));
					}
				}
				else if (ini.is_header("StandardBanner"))
				{
					list<wstring> lstStandardBannerSection;
					while (ini.read_value())
					{
						lstStandardBannerSection.push_back(Trim(stows(ini.get_value_string())));
					}
					set_vctStandardBannerLines.push_back(lstStandardBannerSection);
				}
				else if (ini.is_header("SwearWords"))
				{
					while (ini.read_value())
					{
						wstring word = Trim(stows(ini.get_value_string()));
						word = ReplaceStr(word, L"_", L" ");
						set_lstSwearWords.push_back(word);
					}
				}
			}
			ini.close();
		}

		list<INISECTIONVALUE> helpList;
		IniGetSection(scPluginHelpsCfgFile, "HelpAll", helpList);
		// Русскоязычное описание — второй элемент
		wstring groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"all", {groupDescription, helpList}});
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpJump", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"jump", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpConfig", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"config", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpConfig", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"config", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpGroup", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"group", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpChat", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"chat", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpInfo", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"info", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpMark", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"mark", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpMoneys", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"moneys", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpOther", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"other", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpShip", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"ship", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpTag", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"tag", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpCraft", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"craft", {groupDescription, helpList} });
		helpList.clear();
		IniGetSection(scPluginHelpsCfgFile, "HelpStorage", helpList);
		groupDescription = stows(helpList.front().scValue);
		set_lstHelpsLines.insert({ L"store", {groupDescription, helpList} });
	}

	/// На этом таймере отображаются баннеры
	void Message::Timer()
	{
		static int iSpecialBannerTimer = 0;
		static int iStandardBannerTimer = 0;

		if (++iSpecialBannerTimer > set_iSpecialBannerTimeout)
		{
			iSpecialBannerTimer = 0;
			ShowSpecialBanner();
		}

		if (++iStandardBannerTimer > set_iStandardBannerTimeout)
		{
			iStandardBannerTimer = 0;
			ShowStandardBanner();
		}
	}

	/// При отключении клиента удалите все ссылки на этот клиент.
	void Message::DisConnect(uint iClientID, enum EFLConnection p2)
	{
		map<uint, INFO>::iterator iter = mapInfo.begin();
		while (iter != mapInfo.end())
		{
			if (iter->second.ulastPmClientID == iClientID)
				iter->second.ulastPmClientID = -1;
			if (iter->second.uTargetClientID == iClientID)
				iter->second.uTargetClientID = -1;
			++iter;
		}
	}

	/// На клиента F1 или вход в меню выбора символа.
	void  Message::CharacterInfoReq(unsigned int iClientID, bool p2)
	{
		map<uint, INFO>::iterator iter = mapInfo.begin();
		while (iter != mapInfo.end())
		{
			if (iter->second.ulastPmClientID == iClientID)
				iter->second.ulastPmClientID = -1;
			if (iter->second.uTargetClientID == iClientID)
				iter->second.uTargetClientID = -1;
			++iter;
		}
	}

	/** Этот параметр отправляется, когда мы отправляем временную шкалу чата, чтобы мы не печатали
	время чата рекурсивно. */
	static bool bSendingTime = false;

	/// О событиях запуска и перезагрузите кэш msg для клиента.
	void Message::PlayerLaunch(uint iShip, unsigned int iClientID)
	{
		if (!mapInfo[iClientID].bShowChatTime && ClientInfo[iClientID].bChatTime)
		{
			// Отправляйте время серым цветом (BEBEBE) мелким шрифтом (90) над строкой чата.
			bSendingTime = true;
			HkFMsg(iClientID, L"<TRA data=\"0xBEBEBE90\" mask=\"-1\"/><TEXT>" + XMLText(GetTimeString(set_bLocalTime)) + L"</TEXT>");
			bSendingTime = false;
		}
		LoadMsgs(iClientID);
		ShowGreetingBanner(iClientID);
		Mail::MailCheckLog((const wchar_t*)Players.GetActiveCharacterName(iClientID), MSG_LOG);
	}

	/// На событиях входа базы и перезагрузите кэш msg для клиента.
	void Message::BaseEnter(uint iBaseID, uint iClientID)
	{
		LoadMsgs(iClientID);
		ShowGreetingBanner(iClientID);
		Mail::MailCheckLog((const wchar_t*)Players.GetActiveCharacterName(iClientID), MSG_LOG);
	}

	/// Когда персонаж выбирает цель, а целью является корабль игрока, то записывайте идентификатор клиента цели.
	void Message::SetTarget(uint uClientID, struct XSetTarget const &p2)
	{
		// iSpaceID *появляется* как идентификатор корабля игрока, когда он является мишенью, но это может быть не так.
		// Также обратите внимание, что HkGetClientIDByShip возвращает 0 при сбое, а не -1.
		uint uTargetClientID = HkGetClientIDByShip(p2.iSpaceID);
		if (uTargetClientID)
		{
			map<uint, INFO>::iterator iter = mapInfo.find(uClientID);
			if (iter != mapInfo.end())
			{
				iter->second.uTargetClientID = uTargetClientID;
			}
		}
	}

	bool Message::SubmitChat(CHAT_ID cId, unsigned long iSize, const void *rdlReader, CHAT_ID cIdTo, int p2)
	{
		// Игнорирование команд присоединения к группе и выхода из нее
		if (cIdTo.iID == 0x10004)
			return false;

		// Извлечение текста из rdlReader
		BinaryRDLReader rdl;
		wchar_t wszBuf[1024];
		try
		{
			uint iRet1;
			rdl.extract_text_from_buffer((unsigned short*)wszBuf, sizeof(wszBuf), iRet1, (const char*)rdlReader, iSize);
		}
		catch (...)
		{
			AddLog("SubmitChat Exception\n");
			return true;
		}

		wstring wscChatMsg = ToLower(wszBuf);
		uint iClientID = cId.iID;

		bool bIsGroup = (cIdTo.iID == 0x10003 || !wscChatMsg.find(L"/g ") || !wscChatMsg.find(L"/group "));
		if (!bIsGroup)
		{
			// Если в сообщении появляется запрещенное слово, примите соответствующие меры.
			foreach(set_lstSwearWords, wstring, worditer)
			{
				if (wscChatMsg.find(*worditer) != -1)
				{
					if (*worditer == (L"cock"))
					{
						if (wscChatMsg.find(L"cockpit") != -1)
						{
							return false;
						}
						else if (wscChatMsg.find(L"cockroach") != -1)
						{
							return false;
						}
					}
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0996"));
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0997"));

					mapInfo[iClientID].iSwearWordWarnings++;
					if (mapInfo[iClientID].iSwearWordWarnings > 2)
					{
						wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
						AddLog("NOTICE: Swearing tempban on %s (%s) reason='%s'",
							wstos(wscCharname).c_str(), wstos(HkGetAccountID(HkGetAccountByCharname(wscCharname))).c_str(),
							wstos(wscChatMsg).c_str());
						HkTempBan(iClientID, 10);
						HkDelayedKick(iClientID, 1);

						if (set_fDisconnectSwearingInSpaceRange > 0.0f)
						{
							wstring wscMsg = set_wscDisconnectSwearingInSpaceMsg;
							wscMsg = ReplaceStr(wscMsg, L"%time", GetTimeString(set_bLocalTime));
							wscMsg = ReplaceStr(wscMsg, L"%player", wscCharname);
							PrintLocalUserCmdText(iClientID, wscMsg, set_fDisconnectSwearingInSpaceRange);
						}
					}
					return true;
				}
			}
		}

		/// Когда сообщение в личном чате отправляется от одного клиента к другой записи
		/// кто отправил сообщение, чтобы получатель мог ответить с помощью команды /r */
		if (iClientID < 0x10000 && cIdTo.iID>0 && cIdTo.iID < 0x10000)
		{
			map<uint, INFO>::iterator iter = mapInfo.find(cIdTo.iID);
			if (iter != mapInfo.end())
			{
				iter->second.ulastPmClientID = iClientID;
			}
		}
		return false;
	}

	/** Когда сообщение чата отправлено клиенту, и у этого клиента есть время чата на
		вставьте время в строку непосредственно перед сообщением чата */
	bool Message::HkCb_SendChat(uint iClientID, uint iTo, uint iSize, void *rdlReader)
	{
		// Немедленно вернитесь к разговору, если время в чате.
		if (bSendingTime)
			return false;

		// Игнорировать групповые сообщения (Я не знаю, доберутся ли они когда-нибудь сюда
		if (iTo == 0x10004)
			return false;

		if (set_bCmdHide)
		{
			// Извлекаем текст из rdlReader
			BinaryRDLReader rdl;
			wchar_t wszBuf[1024];
			uint iRet1;
			rdl.extract_text_from_buffer((unsigned short*)wszBuf, sizeof(wszBuf), iRet1, (const char*)rdlReader, iSize);
			wstring wscChatMsg = wszBuf;


			// Находим ":", обозначающее конец имени отправляющего игрока.
			size_t iTextStartPos = wscChatMsg.find(L": ");
			if (iTextStartPos != string::npos)
			{
				if ((wscChatMsg.find(L": /") == iTextStartPos && wscChatMsg.find(L": //") != iTextStartPos)
					|| wscChatMsg.find(L": .") == iTextStartPos)
				{
					return true;
				}
				if (wscChatMsg.find(L": crc") == string::npos && wscChatMsg.find(L": time") == string::npos && wscChatMsg.find(L": process") == string::npos && wscChatMsg.find(L": test") == string::npos)
				{
					if (!mapInfo[iClientID].bShowChatTime && ClientInfo[iClientID].bChatTime)
					{
						// Отправляйте время серым цветом (BEBEBE) мелким шрифтом (90) над строкой чата.
						bSendingTime = true;
						HkFMsg(iClientID, L"<TRA data=\"0xBEBEBE90\" mask=\"-1\"/><TEXT>" + XMLText(GetTimeString(set_bLocalTime)) + L"</TEXT>");
						bSendingTime = false;
					}
				}
			}
		}

		return false;
	}

	/** Установка предустановленного сообщения */
	bool Message::UserCmd_SetMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		int iMsgSlot = ToInt(GetParam(wscParam, ' ', 0));
		wstring wscMsg = GetParamToEnd(wscParam, ' ', 1);

		if (iMsgSlot < 1 || iMsgSlot>9 || wscParam.size() == 0 || wscMsg.length() < 1)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		//Прогружаю адрес файла юзера
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);
		string scUserFileMsg = scAcctPath + wstos(wscDir) + "\\setmsg.ini";
		//записываю строчку в файл
		IniWrite(scUserFileMsg, "", "msg." + itos(iMsgSlot), ws2s(wscMsg));

		// Перезагрузите кэш символов
		LoadMsgs(iClientID);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0999"));
		return true;
	}

	/** Показать предустановленные сообщения */
	bool Message::UserCmd_ShowMsgs(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1000"));
			return true;
		}

		for (int i = 0; i < INFO::NUMBER_OF_SLOTS - 1; i++)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1001"), i + 1, iter->second.slot[i].c_str());
		}
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0999"));
		return true;
	}

	/** Установка предустановленной координаты */
	bool Message::UserCmd_SaveCoords(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		int iCoordMsgSlot = ToInt(GetParam(wscParam, ' ', 0));
		wstring wscMsg = GetParamToEnd(wscParam, ' ', 1);

		if (iCoordMsgSlot < 1 || iCoordMsgSlot > 9 || wscParam.size() == 0 || wscMsg.length() < 1)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		//Прогружаю адрес файла юзера
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);
		string scUserFileMsg = scAcctPath + wstos(wscDir) + "\\setmsg.ini";
		//записываю строчку в файл
		IniWrite(scUserFileMsg, "", "coordmsg." + itos(iCoordMsgSlot), ws2s(wscMsg));

		// Перезагрузите кэш символов
		LoadMsgs(iClientID);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0999"));
		return true;
	}

	/** Показать предустановленные координаты */
	bool Message::UserCmd_ShowCoords(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		map<uint, INFO>::iterator iter = mapCoordsInfo.find(iClientID);
		if (iter == mapCoordsInfo.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1000"));
			return true;
		}

		for (int i = 0; i < INFO::NUMBER_OF_COORDSLOTS - 1; i++)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1001"), i + 1, iter->second.slot[i].c_str());
		}
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0999"));
		return true;
	}

	/** Загрузка предустановленной координаты */
	bool Message::UserCmd_LoadCoords(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		int iCoordMsgSlot = ToInt(wscCmd.substr(2, 1));
		if (iCoordMsgSlot < 1 || iCoordMsgSlot > 9)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		map<uint, INFO>::iterator iter = mapCoordsInfo.find(iClientID);
		if (iter == mapCoordsInfo.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1009"));
			return true;
		}

		// Замените тег #t на имя целевого игрока.
		wstring wscMsg = iter->second.coordslot[iCoordMsgSlot - 1];
		if (!ReplaceMessageTags(iClientID, iter->second, wscMsg))
			return true;

		HyperJump::UserCmd_SetCoords(iClientID, wscMsg, wscMsg, wscMsg.c_str());

		return true;
	}

	/** Отправить сообщение в системный чат. */
	bool Message::UserCmd_SystemMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscSender = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring wscMsg = GetParamToEnd(wscParam, ' ', 0);

		if (wscMsg.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		SendSystemChat(iClientID, wscMsg);
		SendChatEvent(iClientID, 0x10001, wscMsg);
		return true;
	}

	/** Отправка предустановленного сообщения в системный чат */
	bool Message::UserCmd_SMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		int iMsgSlot = ToInt(wscCmd.substr(1, 1));
		if (iMsgSlot < 1 || iMsgSlot>9)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end() || iter->second.slot[iMsgSlot - 1].size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
			return true;
		}

		// Отправка предустановленного сообщения в системный чат
		wstring wscMsg = iter->second.slot[iMsgSlot - 1];
		if (!ReplaceMessageTags(iClientID, iter->second, wscMsg))
			return true;

		SendSystemChat(iClientID, wscMsg);
		SendChatEvent(iClientID, 0x10001, wscMsg);

		return true;
	}

	/** Отправьте предустановленное сообщение в локальный системный чат. 
	Уродливый хак, потому что fl не понимает, что он должен делать */
	bool Message::UserCmd_DRMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		int iMsgSlot = ToInt(wscCmd.substr(1, 1));
		if (iMsgSlot < 1 || iMsgSlot > 9)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end() || iter->second.slot[iMsgSlot - 1].size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
			return true;
		}

		// Замените тег #t на имя целевого игрока.
		wstring wscMsg = iter->second.slot[iMsgSlot - 1];
		if (!ReplaceMessageTags(iClientID, iter->second, wscMsg))
			return true;

		SendLocalSystemChat(iClientID, wscMsg);
		SendChatEvent(iClientID, 0x10002, wscMsg);

		return true;
	}

	/** Отправьте сообщение в локальный системный чат. */
	bool Message::UserCmd_LocalMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscSender = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring wscMsg = GetParamToEnd(wscParam, ' ', 0);

		if (wscMsg.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		SendLocalSystemChat(iClientID, wscMsg);
		SendChatEvent(iClientID, 0x10002, wscMsg);
		return true;
	}

	/** Отправка предустановленного сообщения в локальный системный чат */
	bool Message::UserCmd_LMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		int iMsgSlot = ToInt(wscCmd.substr(2, 1));
		if (iMsgSlot < 1 || iMsgSlot > 9)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end() || iter->second.slot[iMsgSlot - 1].size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
			return true;
		}

		// Замените тег #t на имя целевого игрока.
		wstring wscMsg = iter->second.slot[iMsgSlot - 1];
		if (!ReplaceMessageTags(iClientID, iter->second, wscMsg))
			return true;

		SendLocalSystemChat(iClientID, wscMsg);
		SendChatEvent(iClientID, 0x10002, wscMsg);

		return true;
	}

	/** Отправьте сообщение в групповой чат. */
	bool Message::UserCmd_GroupMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscSender = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring wscMsg = GetParamToEnd(wscParam, ' ', 0);

		if (wscMsg.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		SendGroupChat(iClientID, wscMsg);
		SendChatEvent(iClientID, 0x10003, wscMsg);
		return true;
	}

	/** Отправка предустановленного сообщения в групповой чат */
	bool Message::UserCmd_GMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (!set_bSetMsg)
			return false;

		int iMsgSlot = ToInt(wscCmd.substr(2, 1));
		if (iMsgSlot < 1 || iMsgSlot > 9)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end() || iter->second.slot[iMsgSlot - 1].size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
			return true;
		}

		// Замените тег #t на имя целевого игрока.
		wstring wscMsg = iter->second.slot[iMsgSlot - 1];
		if (!ReplaceMessageTags(iClientID, iter->second, wscMsg))
			return true;

		SendGroupChat(iClientID, wscMsg);
		SendChatEvent(iClientID, 0x10003, wscMsg);
		return true;
	}

	/** Отправьте сообщение последнему человеку, который отправил сообщение этому клиенту. */
	bool Message::UserCmd_ReplyToLastPMSender(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end())
		{
			// Этого никак не может случиться! Да, верно.
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1021"));
			return true;
		}

		wstring wscMsg = GetParamToEnd(wscParam, ' ', 0);

		// Если это команда /rN, то настройте предустановленное сообщение
		if (set_bSetMsg && wscCmd.size() == 3 && wscMsg.size() == 0)
		{
			int iMsgSlot = ToInt(wscCmd.substr(2, 1));
			if (iMsgSlot < 1 || iMsgSlot > 9)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}
			if (iter->second.slot[iMsgSlot - 1].size() == 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
				return true;
			}
			// Если это команда /rN, то настройте предустановленное сообщение
			wscMsg = iter->second.slot[iMsgSlot - 1];
			if (!ReplaceMessageTags(iClientID, iter->second, wscMsg))
				return true;
		}
		else if (wscMsg.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		if (iter->second.ulastPmClientID == -1)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1025"));
			return true;
		}

		mapInfo[iter->second.ulastPmClientID].ulastPmClientID = iClientID;
		SendPrivateChat(iClientID, iter->second.ulastPmClientID, wscMsg);
		SendChatEvent(iClientID, iter->second.ulastPmClientID, wscMsg);
		return true;
	}

	/** Отправка сообщения последнему/текущему целевому объекту. */
	bool Message::UserCmd_SendToLastTarget(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end())
		{
			// Этого никак не может случиться! Да, верно.
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
			return true;
		}

		wstring wscMsg = GetParamToEnd(wscParam, ' ', 0);

		// Если это команда /tN, то настройте предустановленное сообщение
		if (set_bSetMsg && wscCmd.size() == 3 && wscMsg.size() == 0)
		{
			int iMsgSlot = ToInt(wscCmd.substr(2, 1));
			if (iMsgSlot < 1 || iMsgSlot > 9)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
				PrintUserCmdText(iClientID, usage);
				return true;
			}
			if (iter->second.slot[iMsgSlot - 1].size() == 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
				return true;
			}
			// Замените тег #t на имя целевого игрока.
			wscMsg = iter->second.slot[iMsgSlot - 1];
			if (!ReplaceMessageTags(iClientID, iter->second, wscMsg))
				return true;
		}
		else if (wscMsg.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		if (iter->second.uTargetClientID == -1)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1025"));
			return true;
		}

		mapInfo[iter->second.uTargetClientID].ulastPmClientID = iClientID;
		SendPrivateChat(iClientID, iter->second.uTargetClientID, wscMsg);
		SendChatEvent(iClientID, iter->second.uTargetClientID, wscMsg);
		return true;
	}

	/** Показывает отправителя последнего PM и последнего целевого символа */
	bool Message::UserCmd_ShowLastPMSender(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter == mapInfo.end())
		{
			// Это невозможно! да правильно.
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1012"));
			return true;
		}

		wstring wscSenderCharname = GetLocalized(iClientID, "MSG_1571") + stows(itos(iter->second.ulastPmClientID));
		if (iter->second.ulastPmClientID != -1 && HkIsValidClientID(iter->second.ulastPmClientID))
			wscSenderCharname = (const wchar_t*)Players.GetActiveCharacterName(iter->second.ulastPmClientID);

		wstring wscTargetCharname = GetLocalized(iClientID, "MSG_1571") + stows(itos(iter->second.uTargetClientID));
		if (iter->second.uTargetClientID != -1 && HkIsValidClientID(iter->second.uTargetClientID))
			wscTargetCharname = (const wchar_t*)Players.GetActiveCharacterName(iter->second.uTargetClientID);

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1032") + wscSenderCharname + GetLocalized(iClientID, "MSG_1572") + wscTargetCharname);
		return true;
	}

	/** 
		Отправьте личное сообщение на указанный charname. 
		Если игрок не в сети, сообщение будет доставлено при следующем входе в систему.
	 */
	bool Message::UserCmd_PrivateMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		const wstring &wscTargetCharname = GetParam(wscParam, ' ', 0);
		const wstring &wscMsg = GetParamToEnd(wscParam, ' ', 1);

		if (wscCharname.size() == 0 || wscMsg.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		if (!HkGetAccountByCharname(wscTargetCharname))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1034"));
			return true;
		}

		uint iToClientID = HkGetClientIdFromCharname(wscTargetCharname);
		if (iToClientID == -1)
		{
			Mail::MailSend(wscTargetCharname, MSG_LOG, wscCharname + L": " + wscMsg);
			Mail::MailCheckLog(wscTargetCharname, MSG_LOG);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1035"));
		}
		else
		{
			mapInfo[iToClientID].ulastPmClientID = iClientID;
			SendPrivateChat(iClientID, iToClientID, wscMsg);
		}

		return true;
	}

	/** Отправьте личное сообщение на указанный clientid. */
	bool Message::UserCmd_PrivateMsgID(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		const wstring &wscClientID = GetParam(wscParam, ' ', 0);
		const wstring &wscMsg = GetParamToEnd(wscParam, ' ', 1);

		uint iToClientID = ToInt(wscClientID);
		if (!HkIsValidClientID(iToClientID) || HkIsInCharSelectMenu(iToClientID))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1036"));
			return true;
		}

		mapInfo[iToClientID].ulastPmClientID = iClientID;
		SendPrivateChat(iClientID, iToClientID, wscMsg);
		return true;
	}

	/** Отправьте сообщение всем игрокам с определенным префиксом. */
	bool Message::UserCmd_FactionMsg(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscSender = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		const wstring &wscCharnamePrefix = GetParam(wscParam, ' ', 0);
		const wstring &wscMsg = GetParamToEnd(wscParam, ' ', 1);

		if (wscCharnamePrefix.size() < 3 || wscMsg.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		bool bSenderReceived = false;
		bool bMsgSent = false;
		list<HKPLAYERINFO> lst = HkGetPlayers();
		foreach(lst, HKPLAYERINFO, iter)
		{
			if (ToLower(iter->wscCharname).find(ToLower(wscCharnamePrefix)) == string::npos)
				continue;

			if (iter->iClientID == iClientID)
				bSenderReceived = true;

			FormatSendChat(iter->iClientID, wscSender, wscMsg, L"00CCFF");
			bMsgSent = true;
		}
		if (!bSenderReceived)
			FormatSendChat(iClientID, wscSender, wscMsg, L"00CCFF");

		if (bMsgSent == false)
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1038"));
		return true;
	}

	bool Message::UserCmd_Invite(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		const wstring &wscTargetCharname = GetParam(wscParam, ' ', 0);

		if (wscTargetCharname.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		if (!HkGetAccountByCharname(wscTargetCharname))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1034"));
			return true;
		}

		uint iToClientID = HkGetClientIdFromCharname(wscTargetCharname);
		if (iToClientID == -1)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1041"));
			return true;
		}
		else
		{
			wstring wscXML = L"<TEXT>/i " + XMLText(wscTargetCharname) + L"</TEXT>";
			char szBuf[0xFFFF];
			uint iRet;
			if (!HKHKSUCCESS(HkFMsgEncodeXML(wscXML, szBuf, sizeof(szBuf), iRet)))
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1042"));
				return true;
			}

			CHAT_ID cID;
			cID.iID = iClientID;
			CHAT_ID cIDTo;
			cIDTo.iID = 0x00010001;
			Server.SubmitChat(cID, iRet, szBuf, cIDTo, -1);
		}
		return true;
	}

	/** Отправьте сообщение с приглашением фракции всем игрокам с определенным префиксом. */
	bool Message::UserCmd_FactionInvite(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		const wstring &wscCharnamePrefix = GetParam(wscParam, ' ', 0);

		bool msgSent = false;

		if (wscCharnamePrefix.size() < 3)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		list<HKPLAYERINFO> lst = HkGetPlayers();
		foreach(lst, HKPLAYERINFO, iter)
		{
			if (ToLower(iter->wscCharname).find(ToLower(wscCharnamePrefix)) == string::npos)
				continue;
			if (iter->iClientID == iClientID)
				continue;

			wstring wscXML = L"<TEXT>/i " + XMLText(iter->wscCharname) + L"</TEXT>";
			char szBuf[0xFFFF];
			uint iRet;
			if (!HKHKSUCCESS(HkFMsgEncodeXML(wscXML, szBuf, sizeof(szBuf), iRet)))
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1042"));
				return true;
			}

			CHAT_ID cID;
			cID.iID = iClientID;
			CHAT_ID cIDTo;
			cIDTo.iID = 0x00010001;
			Server.SubmitChat(cID, iRet, szBuf, cIDTo, -1);

			msgSent = true;
		}

		if (msgSent == false)
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1038"));

		return true;
	}

	bool Message::UserCmd_SetChatTime(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		GET_USERFILE(scUserFile);

		wstring wscParam1 = ToLower(GetParam(wscParam, ' ', 0));
		wstring wscFilename;
		HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename);
		string scSection = "chattime_" + wstos(wscFilename);

		bool bShowChatTime = true;
		bool bEnable = !wscParam1.compare(L"on") ? true : false;
		ClientInfo[iClientID].bChatTime = bEnable;
		IniWrite(scUserFile, scSection, "chattimemsg", bEnable ? "yes" : "no");

		if (wscParam1.compare(L"on") == 0)
		{
			pub::Audio::PlaySoundEffect(iClientID, CreateID("sounds_on"));
		}
		if (wscParam1.compare(L"off") == 0)
		{
			pub::Audio::PlaySoundEffect(iClientID, CreateID("sounds_off"));
		}

		if (!wscParam1.compare(L"on"))
		{
			bShowChatTime = false;
		}
		else if (!wscParam1.compare(L"off"))
		{
			bShowChatTime = true;
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
		}

		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		HookExt::IniSetB(iClientID, "msg.chat_time", bShowChatTime);

		// Обновите кэш клиента.
		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter != mapInfo.end())
		{
			iter->second.bShowChatTime = bShowChatTime;
		}

		// Отправить сообщение с подтверждением
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0999"));
		return true;
	}

	bool Message::UserCmd_SetDeathTime(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscParam1 = ToLower(GetParam(wscParam, ' ', 0));
		bool bShowChatDieTime = true;
		if (!wscParam1.compare(L"on"))
			bShowChatDieTime = true;
		else if (!wscParam1.compare(L"off"))
			bShowChatDieTime = false;
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		HookExt::IniSetB(iClientID, "msg.chat_dietime", bShowChatDieTime);

		// Обновите кэш клиента.
		map<uint, INFO>::iterator iter = mapInfo.find(iClientID);
		if (iter != mapInfo.end())
			iter->second.bShowChatDieTime = bShowChatDieTime;

		// Отправить сообщение с подтверждением
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0999"));
		return true;
	}

	bool Message::UserCmd_Time(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		// Время отправки серым цветом (BEBEBE) мелким текстом (90) над строкой чата.
		PrintUserCmdText(iClientID, GetTimeString(set_bLocalTime));
		return true;
	}

	/** Печать пользовательской справки с переопределением встроенной справки */
	bool Message::UserCmd_CustomHelp(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (set_bCustomHelp)
		{
			// Печать любых пользовательских строк справки
			foreach(set_lstHelpLines, INISECTIONVALUE, iter)
			{
				string var = iter->scKey;
				var += "=";
				var += iter->scValue;

				wstring scHelp = stows(var);
				if (scHelp.find(L"<TRA") == 0)
					HkFMsg(iClientID, scHelp);
				else
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1050"), wstos(scHelp));
			}
			return true;
		}
		return false;
	}

	bool Message::UserCmd_CommandList(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscHelpParam = ToLower(GetParam(wscParam, ' ', 0)); // Какой список помощи мы пытаемся получить?

		if (!wscHelpParam.length()) // Если параметр не был указан, выполните итерацию по списку.
		{
			PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1367"), GetLocalized(iClientID, "MSG_1573"));
			for (auto it = set_lstHelpsLines.begin(); it != set_lstHelpsLines.end(); ++it)
			{
				const wstring& groupKey = it->first;                    // "store", "base", "etc"
				const wstring& groupDescription = it->second.first;     // Русскоязычное имя группы

				if (groupDescription.find(L"<TRA") != 0)
				{
					wstring varColor = stows("<TRA data=");
					varColor += groupDescription;
					wstring readableColor = ExtractDataHex(varColor);

					wstring readableText = ExtractTextFromXMLTag(groupDescription, L"TEXT");
					PrintUserCmdTextColorKVCustom(iClientID, GetLocalized(iClientID, "MSG_1368"),
						GetLocalized(iClientID, "MSG_1369"), readableText, readableColor);
				}
				// Передай описание туда, где оно нужно
				PrintUserCmdTextColorKV(iClientID, GetLocalized(iClientID, "MSG_1370"), groupKey.c_str());
			}
			return false;
		}

		//защита от не верных ключей 
		bool isExist = false;
		for (auto it = set_lstHelpsLines.begin(); it != set_lstHelpsLines.end(); ++it)
		{
			const wstring& groupKey = it->first;                    // "store", "base", "etc"
			const wstring& groupDescription = it->second.first;     // Русскоязычное имя группы
			const list<INISECTIONVALUE>& commands = it->second.second;
			if (wscHelpParam == groupKey)
				isExist = true;
		}

		if (isExist == false) return false;

		for (auto it = set_lstHelpsLines.begin(); it != set_lstHelpsLines.end(); ++it)
		{
			const wstring& groupKey = it->first;                    // "store", "base", "etc"
			const wstring& groupDescription = it->second.first;     // Русскоязычное имя группы
			const list<INISECTIONVALUE>& commands = it->second.second;
			if (wscHelpParam == it->first.c_str())
			{
				for (const auto& iter : commands)
				{
					wstring varColor = stows(iter.scKey);
					varColor += L"=";
					varColor += stows(iter.scValue);

					string var = iter.scValue;
					wstring scHelp = stows(var);
					wstring readableColor = ExtractDataHex(varColor);
					wstring readableText = ExtractTextFromXMLTag(scHelp, L"TEXT");
					PrintUserCmdTextColorKVCustom(iClientID, GetLocalized(iClientID, "MSG_1371"),
						GetLocalized(iClientID, "MSG_1372"), readableText, readableColor);
				}
			}
		}
		return true;
	}

	/** Распечатайте справку для встроенных команд flhook */
	bool Message::UserCmd_BuiltInCmdHelp(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (wscParam.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0998"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}
		return false;
	}

	/** Показать почту */
	bool Message::UserCmd_MailShow(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		int iNumberUnreadMsgs = Mail::MailCountUnread((const wchar_t*)Players.GetActiveCharacterName(iClientID), MSG_LOG);
		int iNumberMsgs = Mail::MailCount((const wchar_t*)Players.GetActiveCharacterName(iClientID), MSG_LOG);
		if (iNumberMsgs == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1052"));
			return true;
		}

		int iFirstMsg = ToInt(ToLower(GetParam(wscParam, ' ', 0)));
		if (iFirstMsg == 0)
		{
			if (iNumberUnreadMsgs > 0)
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1053"), iNumberUnreadMsgs);
			else
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1054"), iNumberMsgs);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1055"));
			return true;
		}

		if (iFirstMsg > iNumberMsgs)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1056"));
			return true;
		}

		Mail::MailShow((const wchar_t*)Players.GetActiveCharacterName(iClientID), MSG_LOG, iFirstMsg);
		return true;
	}

	/** Удалить почту */
	bool Message::UserCmd_MailDel(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		if (wscParam.size() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1057"));
			PrintUserCmdText(iClientID, usage);
			return true;
		}

		int iNumberMsgs = Mail::MailCount((const wchar_t*)Players.GetActiveCharacterName(iClientID), MSG_LOG);
		int iMsg = ToInt(ToLower(GetParam(wscParam, ' ', 0)));
		if (iMsg == 0 || iMsg > iNumberMsgs)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1056"));
			return true;
		}

		if (Mail::MailDel((const wchar_t*)Players.GetActiveCharacterName(iClientID), MSG_LOG, iMsg))
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0999"));
		else
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1060"));
		return true;
	}

	void Message::UserCmd_Process(uint iClientID, const wstring &wscCmd)
	{
		wstring wscCmdLineLower = ToLower(wscCmd);

		// Эхо команды вернется на консоль отправителя, но только если она начинается с /или .
		if (set_bCmdEcho)
		{
			wstring wscCmd = GetParam(wscCmdLineLower, ' ', 0);
			if (wscCmd.find(L"/") == 0 || wscCmd.find(L".") == 0)
			{
				if (!(wscCmd == L"/l" || wscCmd == L"/local"
					|| wscCmd == L"/s" || wscCmd == L"/system"
					|| wscCmd == L"/g" || wscCmd == L"/group"
					|| wscCmd == L"/t" || wscCmd == L"/target"
					|| wscCmd == L"/r" || wscCmd == L"/reply"
					|| wscCmd.find(L"//") == 0 || wscCmd.find(L"*") == (wscCmd.length() - 1)))
				{
					wstring wscXML = L"<TRA data=\"" + set_wscCmdEchoStyle + L"\" mask=\"-1\"/><TEXT>" + XMLText(wscCmdLineLower) + L"</TEXT>";
					HkFMsg(iClientID, wscXML);
				}
			}
		}
	}


	void Message::AdminCmd_SendMail(CCmds *cmds, const wstring &wscCharname, const wstring &wscMsg)
	{
		Mail::MailSend(wscCharname, MSG_LOG, cmds->GetAdminName() + L": " + wscMsg);
		cmds->Print(L"OK message saved to mailbox\n");
	}

	/// Крюк для разрушения корабля. Его легче зацепить, чем PlayerDeath.
	/// Сброс процента от груза + Немного лута, представляющего собой биты корабля.
	void Message::SendDeathMsg(const wstring &wscMsg, uint iSystemID, uint iClientIDVictim, uint iClientIDKiller)
	{
		// encode xml string(default and small)
		// non-sys
		wstring wscXMLMsg = L"<TRA data=\"" + set_wscDeathMsgStyle + L"\" mask=\"-1\"/> <TEXT>";
		wscXMLMsg += XMLText(wscMsg);
		wscXMLMsg += L"</TEXT>";

		char szBuf[0xFFFF];
		uint iRet;
		if (!HKHKSUCCESS(HkFMsgEncodeXML(wscXMLMsg, szBuf, sizeof(szBuf), iRet)))
			return;

		wstring wscStyleSmall = SetSizeToSmall(set_wscDeathMsgStyle);
		wstring wscXMLMsgSmall = wstring(L"<TRA data=\"") + wscStyleSmall + L"\" mask=\"-1\"/> <TEXT>";
		wscXMLMsgSmall += XMLText(wscMsg);
		wscXMLMsgSmall += L"</TEXT>";
		char szBufSmall[0xFFFF];
		uint iRetSmall;
		if (!HKHKSUCCESS(HkFMsgEncodeXML(wscXMLMsgSmall, szBufSmall, sizeof(szBufSmall), iRetSmall)))
			return;

		// sys
		wstring wscXMLMsgSys = L"<TRA data=\"" + set_wscDeathMsgStyleSys + L"\" mask=\"-1\"/> <TEXT>";
		wscXMLMsgSys += XMLText(wscMsg);
		wscXMLMsgSys += L"</TEXT>";
		char szBufSys[0xFFFF];
		uint iRetSys;
		if (!HKHKSUCCESS(HkFMsgEncodeXML(wscXMLMsgSys, szBufSys, sizeof(szBufSys), iRetSys)))
			return;

		wstring wscStyleSmallSys = SetSizeToSmall(set_wscDeathMsgStyleSys);
		wstring wscXMLMsgSmallSys = L"<TRA data=\"" + wscStyleSmallSys + L"\" mask=\"-1\"/> <TEXT>";
		wscXMLMsgSmallSys += XMLText(wscMsg);
		wscXMLMsgSmallSys += L"</TEXT>";
		char szBufSmallSys[0xFFFF];
		uint iRetSmallSys;
		if (!HKHKSUCCESS(HkFMsgEncodeXML(wscXMLMsgSmallSys, szBufSmallSys, sizeof(szBufSmallSys), iRetSmallSys)))
			return;

		// Отправить для всех игроков
		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint iClientID = HkGetClientIdFromPD(pPD);
			uint iClientSystemID = 0;
			pub::Player::GetSystem(iClientID, iClientSystemID);

			char *szXMLBuf;
			int iXMLBufRet;
			char *szXMLBufSys;
			int iXMLBufRetSys;
			if (set_bUserCmdSetDieMsgSize && (ClientInfo[iClientID].dieMsgSize == CS_SMALL)) {
				szXMLBuf = szBufSmall;
				iXMLBufRet = iRetSmall;
				szXMLBufSys = szBufSmallSys;
				iXMLBufRetSys = iRetSmallSys;
			}
			else {
				szXMLBuf = szBuf;
				iXMLBufRet = iRet;
				szXMLBufSys = szBufSys;
				iXMLBufRetSys = iRetSys;
			}

			if (ClientInfo[iClientID].dieMsg == DIEMSG_ALL || !set_bUserCmdSetDieMsg) {
				if (mapInfo[iClientID].bShowChatDieTime)
				{
					bSendingTime = true;
					HkFMsg(iClientID, L"<TRA data=\"0xBEBEBE90\" mask=\"-1\"/><TEXT>" + XMLText(GetTimeString(set_bLocalTime)) + L"</TEXT>");
					bSendingTime = false;
				}

				if (iSystemID == iClientSystemID)
				{
					HkFMsgSendChat(iClientID, szXMLBufSys, iXMLBufRetSys);
				}
				else
				{
					HkFMsgSendChat(iClientID, szXMLBuf, iXMLBufRet);
				}
			}
			else if ((ClientInfo[iClientID].dieMsg == DIEMSG_SYSTEM) && (iSystemID == iClientSystemID))
			{
				// Добавление информации о времени
				if (mapInfo[iClientID].bShowChatDieTime)
				{
					bSendingTime = true;
					HkFMsg(iClientID, L"<TRA data=\"0xBEBEBE90\" mask=\"-1\"/><TEXT>" + XMLText(GetTimeString(set_bLocalTime)) + L"</TEXT>");
					bSendingTime = false;
				}

				HkFMsgSendChat(iClientID, szXMLBufSys, iXMLBufRetSys);
			}
			else if ((ClientInfo[iClientID].dieMsg == DIEMSG_SELF) && ((iClientID == iClientIDVictim) || (iClientID == iClientIDKiller)))
			{
				// Добавление информации о времени
				if (mapInfo[iClientID].bShowChatDieTime)
				{
					bSendingTime = true;
					HkFMsg(iClientID, L"<TRA data=\"0xBEBEBE90\" mask=\"-1\"/><TEXT>" + XMLText(GetTimeString(set_bLocalTime)) + L"</TEXT>");
					bSendingTime = false;
				}

				HkFMsgSendChat(iClientID, szXMLBufSys, iXMLBufRetSys);
			}
		}
	}
}
