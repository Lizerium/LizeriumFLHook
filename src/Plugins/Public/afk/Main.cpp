/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 30 марта 2026 12:21:08
 * Version: 1.0.11
 */

#include <set>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

// Global variables
std::set<uint> afks;

void LoadSettings() { returncode = DEFAULT_RETURNCODE; }

// This text mimics the "New Player" messages
bool RedText(uint iClientID, std::wstring message, std::wstring message2) {

	std::wstring charname =
		(const wchar_t *)Players.GetActiveCharacterName(iClientID);
	uint iSystemID;
	pub::Player::GetSystem(iClientID, iSystemID);

	std::wstring wscXMLMsg =
		L"<TRA data=\"" + set_wscDeathMsgStyleSys + L"\" mask=\"-1\"/> <TEXT>";
	wscXMLMsg += XMLText(message);
	wscXMLMsg += charname;
	wscXMLMsg += XMLText(message2);
	wscXMLMsg += L"</TEXT>";

	char szBuf[0x1000];
	uint iRet;
	if (!HKHKSUCCESS(HkFMsgEncodeXML(wscXMLMsg, szBuf, sizeof(szBuf), iRet)))
		return false;

	// Send to all players in system
	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD)) {
		uint iClientID = HkGetClientIdFromPD(pPD);
		uint iClientSystemID = 0;
		pub::Player::GetSystem(iClientID, iClientSystemID);

		if (iSystemID == iClientSystemID)
			HkFMsgSendChat(iClientID, szBuf, iRet);
	}
	return true;
}

// This command is called when a player types /afk
bool UserCmd_AFK(uint iClientID, const std::wstring &wscCmd,
	const std::wstring &wscParam, const wchar_t *usage) {
	afks.insert(iClientID);
	RedText(iClientID, L"", GetLocalized(iClientID, "MSG_1500"));
	PrintUserCmdText(
		iClientID,
		GetLocalized(iClientID, "MSG_0003"));
	return true;
}

// This command is called when a player types /back
bool UserCmd_Back(uint iClientID, const std::wstring &wscCmd,
	const std::wstring &wscParam, const wchar_t *usage) {
	if (afks.count(iClientID) > 0) {
		afks.erase(iClientID);
		std::wstring message =
			(const wchar_t *)Players.GetActiveCharacterName(iClientID);
		RedText(iClientID, GetLocalized(iClientID, "MSG_1501"), L".");
		return true;
	}
	PrintUserCmdText(
		iClientID,
		GetLocalized(iClientID, "MSG_0004"));
	return true;
}

// Clean up when a client disconnects
void DisConnect_AFTER(uint iClientID) {
	returncode = DEFAULT_RETURNCODE;

	if (afks.count(iClientID) > 0)
		afks.erase(iClientID);
}

// Hook on chat being sent
void __stdcall HkCb_SendChat(uint iClientID, uint iTo, uint iSize, void *pRDL) {
	returncode = DEFAULT_RETURNCODE;

	if (HkIsValidClientID(iTo) && afks.count(iClientID) > 0)
		PrintUserCmdText(iTo, GetLocalized(iClientID, "MSG_0005"));
}

// Client command processing
USERCMD UserCmds[] = {
	{L"/afk", UserCmd_AFK, L"Usage: /afk"},
	{L"/back", UserCmd_Back, L"Usage: /back"},
};

bool UserCmd_Process(uint iClientID, const std::wstring &wscCmd) {
	returncode = DEFAULT_RETURNCODE;

	std::wstring wscCmdLineLower = ToLower(wscCmd);

	// If the chat string does not match the USER_CMD then we do not handle the
	// command, so let other plugins or FLHook kick in. We require an exact
	// match
	for (uint i = 0; (i < sizeof(UserCmds) / sizeof(USERCMD)); i++) {

		if (wscCmdLineLower.find(UserCmds[i].wszCmd) == 0) {
			// Extract the parameters string from the chat string. It should
			// be immediately after the command and a space.
			std::wstring wscParam = L"";
			if (wscCmd.length() > wcslen(UserCmds[i].wszCmd)) {
				if (wscCmd[wcslen(UserCmds[i].wszCmd)] != ' ')
					continue;
				wscParam = wscCmd.substr(wcslen(UserCmds[i].wszCmd) + 1);
			}

			// Dispatch the command to the appropriate processing function.
			if (UserCmds[i].proc(iClientID, wscCmd, wscParam,
				UserCmds[i].usage)) {
				// We handled the command tell FL hook to stop processing this
				// chat string.
				returncode =
					SKIPPLUGINS_NOFUNCTIONCALL; // we handled the command,
												// return immediatly
				return true;
			}
		}
	}
	return false;
}

// Hook on /help
EXPORT void UserCmd_Help(uint iClientID, const std::wstring &wscParam) {
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0006"));
	PrintUserCmdText(iClientID,
		GetLocalized(iClientID, "MSG_0007"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0008"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0009"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FLHOOK STUFF
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Do things when the dll is loaded
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	srand((uint)time(0));
	// Если мы загружаемся из командной строки во время запуска FLHook, то
	// set_scCfgFile не будет пустым, поэтому загружайте настройки только
	// так, как FLHook вызывает загрузку настроек при запуске FLHook и .rehash.
	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (set_scCfgFile.length() > 0)
			LoadSettings();
	}
	else if (fdwReason == DLL_PROCESS_DETACH) {
	}
	return true;
}

// Functions to hook
EXPORT PLUGIN_INFO *Get_PluginInfo() {
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "AFK";
	p_PI->sShortName = "afk";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(
		PLUGIN_HOOKINFO((FARPROC *)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(
		PLUGIN_HOOKINFO((FARPROC *)&DisConnect_AFTER,
			PLUGIN_HkIServerImpl_DisConnect_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC *)&UserCmd_Process,
		PLUGIN_UserCmd_Process, 0));
	p_PI->lstHooks.push_back(
		PLUGIN_HOOKINFO((FARPROC *)&UserCmd_Help, PLUGIN_UserCmd_Help, 0));
	p_PI->lstHooks.push_back(
		PLUGIN_HOOKINFO((FARPROC *)&HkCb_SendChat, PLUGIN_HkCb_SendChat, 0));
	return p_PI;
}
