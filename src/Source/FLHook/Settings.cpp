/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 07 апреля 2026 10:57:40
 * Version: 1.0.22
 */

#include "global.h"
#include "hook.h"
#include <io.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// setting variables

string			set_scCfgFile;

// General
bool			set_bLoadedSettings = false;
uint			set_iAntiDockKill;
bool			set_bDieMsg;
bool			set_bDisableCharfileEncryption;
bool			set_bChangeCruiseDisruptorBehaviour;
uint			set_iAntiF1;
uint			set_iDisconnectDelay;
uint			set_iReservedSlots;
float			set_fTorpMissileBaseDamageMultiplier;
uint			set_iMaxGroupSize;
uint			set_iDisableNPCSpawns;

// log
bool			set_bDebug;
uint			set_iDebugMaxSize;
bool			set_bLogConnects;
bool			set_bLogAdminCmds;
bool			set_bLogSocketCmds;
bool			set_bLogLocalSocketCmds;
bool			set_bLogUserCmds;
bool			set_bPerfTimer;
uint			set_iTimerThreshold;
uint			set_iTimerDebugThreshold;

// Kick
uint			set_iAntiBaseIdle;
uint			set_iAntiCharMenuIdle;



// Style
wstring			set_wscDeathMsgStyle;
wstring			set_wscDeathMsgStyleSys;
wstring			set_wscDeathMsgTextPlayerKill;
wstring			set_wscDeathMsgTextSelfKill;
wstring			set_wscDeathMsgTextNPC;
wstring			set_wscDeathMsgTextSuicide;
wstring			set_wscDeathMsgTextAdminKill;

uint			set_iKickMsgPeriod;
wstring			set_wscKickMsg;
wstring			set_wscUserCmdStyle;
wstring			set_wscUserCmdKeyStyle;
wstring			set_wscUserCmdTypeStyle;
wstring			set_wscUserCmdValueStyle;
wstring			set_wscAdminCmdStyle;

// Socket
bool			set_bSocketActivated;
int				set_iPort;
int				set_iWPort;
int				set_iEPort;
int				set_iEWPort;
BLOWFISH_CTX	*set_BF_CTX = 0;

// UserCommands
bool			set_bUserCmdSetDieMsg;
bool			set_bUserCmdSetAudioMsg;
bool			set_bUserCmdSetChatTime;
bool			set_bUserCmdSetDieMsgSize;
bool			set_bUserCmdSetChatFont;
bool			set_bUserCmdIgnore;
uint			set_iUserCmdMaxIgnoreList;
bool			set_bAutoBuy;
bool			set_bUserCmdHelp;
bool			set_bDefaultLocalChat;

// NoPVP
list<uint>		set_lstNoPVPSystems;

// Chat
list<wstring>	set_lstChatSuppress;

// MultiKillMessages
bool			set_MKM_bActivated;
wstring			set_MKM_wscStyle;
list<MULTIKILLMESSAGE> set_MKM_lstMessages;

// bans
bool			set_bBanAccountOnMatch;
list<wstring>	set_lstBans;

// help

bool get_bUserCmdSetDieMsg(uint iClientID) { return set_bUserCmdSetDieMsg; }
bool get_bUserCmdSetAudioMsg(uint iClientID) { return set_bUserCmdSetAudioMsg; }
bool get_bUserCmdSetChatTime(uint iClientID) { return set_bUserCmdSetChatTime; }
bool get_bUserCmdSetDieMsgSize(uint iClientID) { return set_bUserCmdSetDieMsgSize; }
bool get_bUserCmdSetChatFont(uint iClientID) { return set_bUserCmdSetChatFont; }
bool get_bUserCmdIgnore(uint iClientID) { return set_bUserCmdIgnore; }
bool get_bAutoBuy(uint iClientID) { return set_bAutoBuy; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LoadSettings()
{
	// init cfg filename
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	set_scCfgFile = szCurDir;

	// Use flhook.cfg if it is available. It is used in some installations (okay just cannon's)
	// to avoid FLErrorChecker whining retardly about ini entries it does not understand.
	if (_access(string(set_scCfgFile + "\\FLHook.cfg").c_str(), 0) != -1)
		set_scCfgFile += "\\FLHook.cfg";
	else
		set_scCfgFile += "\\FLHook.ini";


	// General
	set_iAntiDockKill = IniGetI(set_scCfgFile, "General", "AntiDockKill", 0);
	set_bDieMsg = IniGetB(set_scCfgFile, "General", "EnableDieMsg", false);
	set_bDisableCharfileEncryption = IniGetB(set_scCfgFile, "General", "DisableCharfileEncryption", false);
	set_bChangeCruiseDisruptorBehaviour = IniGetB(set_scCfgFile, "General", "ChangeCruiseDisruptorBehaviour", false);
	set_iDisableNPCSpawns = IniGetI(set_scCfgFile, "General", "DisableNPCSpawns", 0);
	set_iAntiF1 = IniGetI(set_scCfgFile, "General", "AntiF1", 0);
	set_iDisconnectDelay = IniGetI(set_scCfgFile, "General", "DisconnectDelay", 0);
	set_iReservedSlots = IniGetI(set_scCfgFile, "General", "ReservedSlots", 0);
	set_fTorpMissileBaseDamageMultiplier = IniGetF(set_scCfgFile, "General", "TorpMissileBaseDamageMultiplier", 1.0f);
	set_iMaxGroupSize = IniGetI(set_scCfgFile, "General", "MaxGroupSize", 8);

	// Log
	set_bDebug = IniGetB(set_scCfgFile, "Log", "Debug", false);
	set_iDebugMaxSize = IniGetI(set_scCfgFile, "Log", "DebugMaxSize", 100);
	set_iDebugMaxSize *= 1000;
	set_bLogConnects = IniGetB(set_scCfgFile, "Log", "LogConnects", false);
	set_bLogAdminCmds = IniGetB(set_scCfgFile, "Log", "LogAdminCommands", false);
	set_bLogSocketCmds = IniGetB(set_scCfgFile, "Log", "LogSocketCommands", false);
	set_bLogLocalSocketCmds = IniGetB(set_scCfgFile, "Log", "LogLocalSocketCommands", false);
	set_bLogUserCmds = IniGetB(set_scCfgFile, "Log", "LogUserCommands", false);
	set_bPerfTimer = IniGetB(set_scCfgFile, "Log", "LogPerformanceTimers", false);
	set_iTimerThreshold = IniGetI(set_scCfgFile, "Log", "TimerThreshold", 100);
	set_iTimerDebugThreshold = IniGetI(set_scCfgFile, "Log", "TimerDebugThreshold", 0);

	// Kick
	set_iAntiBaseIdle = IniGetI(set_scCfgFile, "Kick", "AntiBaseIdle", 0);
	set_iAntiCharMenuIdle = IniGetI(set_scCfgFile, "Kick", "AntiCharMenuIdle", 0);


	// Style
	set_wscDeathMsgStyle = stows(IniGetS(set_scCfgFile, "Style", "DeathMsgStyle", "0x19198C01"));
	set_wscDeathMsgStyleSys = stows(IniGetS(set_scCfgFile, "Style", "DeathMsgStyleSys", "0x1919BD01"));
	set_wscDeathMsgTextPlayerKill = stows(IniGetS(set_scCfgFile, "Style", "DeathMsgTextPlayerKill", "Death: %victim was killed by %killer (%type)"));
	set_wscDeathMsgTextSelfKill = stows(IniGetS(set_scCfgFile, "Style", "DeathMsgTextSelfKill", "Death: %victim killed himself (%type)"));
	set_wscDeathMsgTextNPC = stows(IniGetS(set_scCfgFile, "Style", "DeathMsgTextNPC", "Death: %victim was killed by an NPC"));
	set_wscDeathMsgTextSuicide = stows(IniGetS(set_scCfgFile, "Style", "DeathMsgTextSuicide", "Death: %victim committed suicide"));
	set_wscDeathMsgTextAdminKill = stows(IniGetS(set_scCfgFile, "Style", "DeathMsgTextAdminKill", "Death: %victim was killed by an admin"));

	set_wscKickMsg = stows(IniGetS(set_scCfgFile, "Style", "KickMsg", "<TRA data=\"0x0000FF10\" mask=\"-1\"/><TEXT>You will be kicked. Reason: %s</TEXT>"));
	set_iKickMsgPeriod = IniGetI(set_scCfgFile, "Style", "KickMsgPeriod", 5000);
	set_wscUserCmdStyle = stows(IniGetS(set_scCfgFile, "Style", "UserCmdStyle", "0x00FF0090"));
	set_wscUserCmdKeyStyle = stows(IniGetS(set_scCfgFile, "Style", "UserCmdKeyStyle", "0xFF920540"));
	set_wscUserCmdTypeStyle = stows(IniGetS(set_scCfgFile, "Style", "UserCmdTypeStyle", "0xFFFF0540"));
	set_wscUserCmdValueStyle = stows(IniGetS(set_scCfgFile, "Style", "UserCmdValueStyle", "0xFF24D700"));
	set_wscAdminCmdStyle = stows(IniGetS(set_scCfgFile, "Style", "UserCmdKeyStyle", "0x00FF0090"));

	// Socket
	set_bSocketActivated = IniGetB(set_scCfgFile, "Socket", "Activated", false);
	set_iPort = IniGetI(set_scCfgFile, "Socket", "Port", 0);
	set_iWPort = IniGetI(set_scCfgFile, "Socket", "WPort", 0);
	set_iEPort = IniGetI(set_scCfgFile, "Socket", "EPort", 0);
	set_iEWPort = IniGetI(set_scCfgFile, "Socket", "EWPort", 0);
	string scEncryptKey = IniGetS(set_scCfgFile, "Socket", "Key", "");
	if (scEncryptKey.length())
	{
		if (!set_BF_CTX)
			set_BF_CTX = (BLOWFISH_CTX*)malloc(sizeof(BLOWFISH_CTX));
		Blowfish_Init(set_BF_CTX, (unsigned char *)scEncryptKey.data(), (int)scEncryptKey.length());
	}

	// UserCommands
	set_bUserCmdSetDieMsg = IniGetB(set_scCfgFile, "UserCommands", "SetDieMsg", false);
	set_bUserCmdSetAudioMsg = IniGetB(set_scCfgFile, "UserCommands", "AudioMsg", true);
	set_bUserCmdSetChatTime = IniGetB(set_scCfgFile, "UserCommands", "ChatTime", true);
	set_bUserCmdSetDieMsgSize = IniGetB(set_scCfgFile, "UserCommands", "SetDieMsgSize", false);
	set_bUserCmdSetChatFont = IniGetB(set_scCfgFile, "UserCommands", "SetChatFont", false);
	set_bUserCmdIgnore = IniGetB(set_scCfgFile, "UserCommands", "Ignore", false);
	set_iUserCmdMaxIgnoreList = IniGetI(set_scCfgFile, "UserCommands", "MaxIgnoreListEntries", 30);
	set_bAutoBuy = IniGetB(set_scCfgFile, "UserCommands", "AutoBuy", false);
	set_bUserCmdHelp = IniGetB(set_scCfgFile, "UserCommands", "Help", false);
	set_bDefaultLocalChat = IniGetB(set_scCfgFile, "UserCommands", "DefaultLocalChat", false);

	// NoPVP
	set_lstNoPVPSystems.clear();
	for (uint i = 0;; i++)
	{
		char szBuf[64];
		sprintf(szBuf, "System%u", i);
		string scSystem = IniGetS(set_scCfgFile, "NoPVP", szBuf, "");

		if (!scSystem.length())
			break;

		uint iSystemID;
		pub::GetSystemID(iSystemID, scSystem.c_str());
		set_lstNoPVPSystems.push_back(iSystemID);
	}

	// read chat suppress
	set_lstChatSuppress.clear();
	for (uint i = 0;; i++)
	{
		char szBuf[64];
		sprintf(szBuf, "Suppress%u", i);
		string scSuppress = IniGetS(set_scCfgFile, "Chat", szBuf, "");

		if (!scSuppress.length())
			break;

		set_lstChatSuppress.push_back(stows(scSuppress));
	}

	// MultiKillMessages
	set_MKM_bActivated = IniGetB(set_scCfgFile, "MultiKillMessages", "Activated", false);
	set_MKM_wscStyle = stows(IniGetS(set_scCfgFile, "MultiKillMessages", "Style", "0x1919BD01"));

	set_MKM_lstMessages.clear();
	list<INISECTIONVALUE> lstValues;
	IniGetSection(set_scCfgFile, "MultiKillMessages", lstValues);
	foreach(lstValues, INISECTIONVALUE, it)
	{
		if (!atoi(it->scKey.c_str()))
			continue;

		MULTIKILLMESSAGE mkm;
		mkm.iKillsInARow = atoi(it->scKey.c_str());
		mkm.wscMessage = stows(it->scValue);
		set_MKM_lstMessages.push_back(mkm);
	}

	// bans
	set_bBanAccountOnMatch = IniGetB(set_scCfgFile, "Bans", "BanAccountOnMatch", false);
	set_lstBans.clear();
	IniGetSection(set_scCfgFile, "Bans", lstValues);
	if (!lstValues.empty())
	{
		lstValues.pop_front();
		foreach(lstValues, INISECTIONVALUE, itisv)
			set_lstBans.push_back(stows(itisv->scKey));
	}

	// help
	HkAddHelpEntry(L"/set diemsg", L"<visibility>", L"Устанавливает видимость сообщений о смерти. Параметры: all, system, self, none.", L"", get_bUserCmdSetDieMsg);
	HkAddHelpEntry(L"/set diemsgsize", L"<size>", L"Устанавливает размер текста сообщений о смерти. Параметры: small, default.", L"", get_bUserCmdSetDieMsgSize);
	HkAddHelpEntry(L"/set chatfont", L"<size> <style>", L"Установка шрифта сообщений в чате. Варианты small, default или big для <size> и default, bold, italic или underline для <style>.", L"", get_bUserCmdSetChatFont);
	HkAddHelpEntry(L"/ignore", L"<charname> [<flags>]", L"Игнорирует все сообщения от данного персонажа.", L"Возможные флаги:\n p - Влияет только на приватный чат\n i - <charname> может совпадать частично\nПримеры:\n\"/ignore SomeDude\" игнорирует все сообщения в чате от SomeDude\n\"/ignore PlayerX p\" игнорирует все сообщения в приватном чате от PlayerX\n\"/ignore idiot i\" игнорирует все сообщения в чате от игроков, чье имя содержит \"idiot\" (e.g. \"[XYZ]IDIOT\", \"MrIdiot\", etc)\n\"/ignore Fool pi\" игнорирует все сообщения в приватном чате от игроков, чье имя содержит \"fool\"", get_bUserCmdIgnore);
	HkAddHelpEntry(L"/ignoreid", L"<client-id> [<flags>]", L"Игнорирует все сообщения от персонажа с ассоциированным идентификатором клиента (см. /id). Используйте флаг p, чтобы повлиять только на приватный чат.", L"", get_bUserCmdIgnore);
	HkAddHelpEntry(L"/ignorelist", L"", L"Отображает все игнорируемые в данный момент символы.", L"", get_bUserCmdIgnore);
	HkAddHelpEntry(L"/delignore", L"<id> [<id2> <id3> ...]", L"Удаляет символы с соответствующим идентификатором игнорирования (см. /ignorelist) из списка игнорирования. * deletes all.", L"", get_bUserCmdIgnore);
	//HkAddHelpEntry(L"/autobuy", L"<param> [<on/off>]", L"Auomatically покупает данные элементы после стыковки. Для получения дополнительной информации см. подробную справку.", L"/autobuy info\" показать информацию об автопокупке", get_bAutoBuy);
	HkAddHelpEntry(L"/ids", L"", L"Список всех персонажей с соответствующими идентификаторами клиента.", L"", get_bTrue);
	HkAddHelpEntry(L"/id", L"", L"Предоставляет ваш собственный идентификатор клиента.", L"", get_bTrue);
	//HkAddHelpEntry(L"/i$", L"<client-id>", L"Приглашает заданный идентификатор клиента.", L"", get_bTrue);
	//HkAddHelpEntry(L"/invite$", L"<client-id>", L"Приглашает заданный идентификатор клиента.", L"", get_bTrue);
	//HkAddHelpEntry(L"/credits", L"", L"Отображает кредиты FLHooks.", L"", get_bTrue);
	//HkAddHelpEntry(L"/help", L"[<command>]", L"Отображение экрана справки. Указание a <command> дает подробную информацию для этой команды.", L"", get_bTrue);

	set_bLoadedSettings = true;
}
