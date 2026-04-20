/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 16:23:13
 * Version: 1.0.468
 */

#include "hook.h"

#define PRINT_ERROR() { for(uint i = 0; (i < sizeof(wscError)/sizeof(wstring)); i++) PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1261"), wscError[i].c_str()); return; }
#define PRINT_OK() PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1262"));
#define PRINT_DISABLED() PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1263"));
#define GET_USERFILE(a) string a; { CAccount *acc = Players.FindAccountFromClientID(iClientID); wstring wscDir; HkGetAccountDirName(acc, wscDir); a = scAcctPath + wstos(wscDir) + "\\flhookuser.ini"; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void(*_UserCmdProc)(uint, const wstring &);

struct USERCMD
{
	wchar_t *wszCmd;
	_UserCmdProc proc;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrintUserCmdText(uint iClientID, wstring wscText, ...)
{
	wchar_t wszBuf[1024 * 8] = L"";
	va_list marker;
	va_start(marker, wscText);
	_vsnwprintf(wszBuf, sizeof(wszBuf) - 1, wscText.c_str(), marker);

	wstring wscXML = L"<TRA data=\"" + set_wscUserCmdStyle + L"\" mask=\"-1\"/><TEXT>" + XMLText(wszBuf) + L"</TEXT>";
	HkFMsg(iClientID, wscXML);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrintUserCmdTextColorKV(uint iClientID, const wstring& wscKey, const wstring& wscValue)
{
	wchar_t wszKeyBuf[1024] = L"";  // Буффер для ключа
	wchar_t wszValueBuf[1024] = L""; // Буффер для значения

	// Форматируем текст в буфферы
	swprintf(wszKeyBuf, L"%s", wscKey.c_str());
	swprintf(wszValueBuf, L"%s", wscValue.c_str());

	wstring wscXML = L"<TRA data=\"" + set_wscUserCmdKeyStyle + L"\" mask=\"-1\"/><TEXT>" + XMLText(wszKeyBuf) + L"</TEXT>" 
		+ L"<TRA data=\"" + set_wscUserCmdValueStyle + L"\" mask=\"-1\"/><TEXT>" + XMLText(wszValueBuf) + L"</TEXT>";

	HkFMsg(iClientID, wscXML);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// <summary>
/// Выводит ключ значение с конкретным указанным цветом
/// </summary>
/// <param name="iClientID">id</param>
/// <param name="wscKey">wstring text key</param>
/// <param name="wscColorKey">"0xFF920540"</param>
/// <param name="wscValue">wstring text value</param>
/// <param name="wscColorValue">"0xFF920540"</param>
void PrintUserCmdTextColorKVCustom(uint iClientID, const wstring& wscKey, const wstring& wscColorKey, const wstring& wscValue, const wstring& wscColorValue)
{
	wchar_t wszKeyBuf[1024] = L"";  // Буффер для ключа
	wchar_t wszValueBuf[1024] = L""; // Буффер для значения

	// Форматируем текст в буфферы
	swprintf(wszKeyBuf, L"%s", wscKey.c_str());
	swprintf(wszValueBuf, L"%s", wscValue.c_str());

	wstring wscXML = L"<TRA data=\"" + wscColorKey + L"\" mask=\"-1\"/><TEXT>" + XMLText(wszKeyBuf) + L"</TEXT>"
		+ L"<TRA data=\"" + wscColorValue + L"\" mask=\"-1\"/><TEXT>" + XMLText(wszValueBuf) + L"</TEXT>";

	HkFMsg(iClientID, wscXML);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrintUserCmdTextColor4KV(uint iClientID, const wstring& wscKey1, const wstring& wscKey2, const wstring& wscKey3, const wstring& wscKey4)
{
	wchar_t wszKeyBuf1[1024] = L"";  // Буффер для ключа
	wchar_t wszKeyBuf2[1024] = L"";  // Буффер для ключа
	wchar_t wszKeyBuf3[1024] = L"";  // Буффер для ключа
	wchar_t wszKeyBuf4[1024] = L"";  // Буффер для ключа

	// Форматируем текст в буфферы
	swprintf(wszKeyBuf1, L"%s", wscKey1.c_str());
	swprintf(wszKeyBuf2, L"%s", wscKey2.c_str());
	swprintf(wszKeyBuf3, L"%s", wscKey3.c_str());
	swprintf(wszKeyBuf4, L"%s", wscKey4.c_str());

	wstring wscXML = L"<TRA data=\"" + set_wscUserCmdValueStyle + L"\" bold=\"false\" italic=\"default\" underline=\"default\" mask=\"-1\"/><TEXT>" + XMLText(wszKeyBuf1) + L"</TEXT>"
		+ L"<TRA data=\"" + set_wscUserCmdTypeStyle + L"\" bold=\"false\" italic=\"default\" underline=\"default\" mask=\"-1\"/><TEXT>" + XMLText(wszKeyBuf2) + L"</TEXT>"
		+ L"<TRA data=\"" + set_wscUserCmdValueStyle + L"\" bold=\"false\" italic=\"default\" underline=\"default\" mask=\"-1\"/><TEXT>" + XMLText(wszKeyBuf3) + L"</TEXT>"
		+ L"<TRA data=\"" + set_wscUserCmdKeyStyle + L"\" mask=\"-1\"/><TEXT>" + XMLText(wszKeyBuf4) + L"</TEXT>";

	HkFMsg(iClientID, wscXML);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckUserExistInDb(uint iClientID)
{
	CAccount* acc = Players.FindAccountFromClientID(iClientID);
	if (acc)
	{
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);

		bool setupLang = DatabaseManager::Instance().UserExists(wstos(wscDir));
		return setupLang;
	}

	return false;
}

bool AddUserInDb(uint iClientID)
{
	CAccount* acc = Players.FindAccountFromClientID(iClientID);
	if (acc)
	{
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);

		bool setupLang = DatabaseManager::Instance().InsertUser(wstos(wscDir));
		return setupLang;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_SetDieMsg(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdSetDieMsg)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		GetLocalized(iClientID, "MSG_1596"),
		GetLocalized(iClientID, "MSG_1597"),
	};

	wstring wscDieMsg = ToLower(GetParam(wscParam, ' ', 0));;

	DIEMSGTYPE dieMsg;
	if (!wscDieMsg.compare(L"all"))
		dieMsg = DIEMSG_ALL;
	else if (!wscDieMsg.compare(L"system"))
		dieMsg = DIEMSG_SYSTEM;
	else if (!wscDieMsg.compare(L"none"))
		dieMsg = DIEMSG_NONE;
	else if (!wscDieMsg.compare(L"self"))
		dieMsg = DIEMSG_SELF;
	else
		PRINT_ERROR();

	// save to ini
	GET_USERFILE(scUserFile);
	IniWrite(scUserFile, "settings", "DieMsg", itos(dieMsg));

	// save in ClientInfo
	ClientInfo[iClientID].dieMsg = dieMsg;

	// send confirmation msg
	PRINT_OK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_SetDieMsgSize(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdSetDieMsgSize)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		GetLocalized(iClientID, "MSG_1598"),
		GetLocalized(iClientID, "MSG_1599"),
	};

	wstring wscDieMsgSize = ToLower(GetParam(wscParam, ' ', 0));
	//	wstring wscDieMsgStyle = ToLower(GetParam(wscParam, ' ', 1));

	CHATSIZE dieMsgSize;
	if (!wscDieMsgSize.compare(L"small"))
		dieMsgSize = CS_SMALL;
	else if (!wscDieMsgSize.compare(L"default"))
		dieMsgSize = CS_DEFAULT;
	else
		PRINT_ERROR();

	/*	CHATSTYLE dieMsgStyle;
		if(!wscDieMsgStyle.compare(L"default"))
			dieMsgStyle = CST_DEFAULT;
		else if(!wscDieMsgStyle.compare(L"bold"))
			dieMsgStyle = CST_BOLD;
		else if(!wscDieMsgStyle.compare(L"italic"))
			dieMsgStyle = CST_ITALIC;
		else if(!wscDieMsgStyle.compare(L"underline"))
			dieMsgStyle = CST_UNDERLINE;
		else
			PRINT_ERROR(); */

			// save to ini
	GET_USERFILE(scUserFile);
	IniWrite(scUserFile, "Settings", "DieMsgSize", itos(dieMsgSize));
	//	IniWrite(scUserFile, "Settings", "DieMsgStyle", itos(dieMsgStyle));

		// save in ClientInfo
	ClientInfo[iClientID].dieMsgSize = dieMsgSize;
	//	ClientInfo[iClientID].dieMsgStyle = dieMsgStyle;

		// send confirmation msg
	PRINT_OK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_SetChatFont(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdSetChatFont)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		GetLocalized(iClientID, "MSG_1600"),
		GetLocalized(iClientID, "MSG_1601"),
		GetLocalized(iClientID, "MSG_1602"),
	};

	wstring wscChatSize = ToLower(GetParam(wscParam, ' ', 0));
	wstring wscChatStyle = ToLower(GetParam(wscParam, ' ', 1));

	CHATSIZE chatSize;
	if (!wscChatSize.compare(L"small"))
		chatSize = CS_SMALL;
	else if (!wscChatSize.compare(L"default"))
		chatSize = CS_DEFAULT;
	else if (!wscChatSize.compare(L"big"))
		chatSize = CS_BIG;
	else
		PRINT_ERROR();

	CHATSTYLE chatStyle;
	if (!wscChatStyle.compare(L"default"))
		chatStyle = CST_DEFAULT;
	else if (!wscChatStyle.compare(L"bold"))
		chatStyle = CST_BOLD;
	else if (!wscChatStyle.compare(L"italic"))
		chatStyle = CST_ITALIC;
	else if (!wscChatStyle.compare(L"underline"))
		chatStyle = CST_UNDERLINE;
	else
		PRINT_ERROR();

	// save to ini
	GET_USERFILE(scUserFile);
	IniWrite(scUserFile, "settings", "ChatSize", itos(chatSize));
	IniWrite(scUserFile, "settings", "ChatStyle", itos(chatStyle));

	// save in ClientInfo
	ClientInfo[iClientID].chatSize = chatSize;
	ClientInfo[iClientID].chatStyle = chatStyle;

	// send confirmation msg
	PRINT_OK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_Ignore(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdIgnore)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		GetLocalized(iClientID, "MSG_1603"),
		GetLocalized(iClientID, "MSG_1604"),
		GetLocalized(iClientID, "MSG_1605"),
		GetLocalized(iClientID, "MSG_1606"),
		GetLocalized(iClientID, "MSG_1607"),
		GetLocalized(iClientID, "MSG_1608"),
		GetLocalized(iClientID, "MSG_1609"),
		GetLocalized(iClientID, "MSG_1610"),
		GetLocalized(iClientID, "MSG_1611"),
		GetLocalized(iClientID, "MSG_1612"),
	};

	wstring wscAllowedFlags = L"pi";

	wstring wscCharname = GetParam(wscParam, ' ', 0);
	wstring wscFlags = ToLower(GetParam(wscParam, ' ', 1));

	if (!wscCharname.length())
		PRINT_ERROR();

	// check if flags are valid
	for (uint i = 0; (i < wscFlags.length()); i++)
	{
		if (wscAllowedFlags.find_first_of(wscFlags[i]) == -1)
			PRINT_ERROR();
	}

	if (ClientInfo[iClientID].lstIgnore.size() > set_iUserCmdMaxIgnoreList)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1264"));
		return;
	}

	// save to ini
	GET_USERFILE(scUserFile);
	IniWriteW(scUserFile, "IgnoreList", itos((int)ClientInfo[iClientID].lstIgnore.size() + 1), (wscCharname + L" " + wscFlags));

	// save in ClientInfo
	IGNORE_INFO ii;
	ii.wscCharname = wscCharname;
	ii.wscFlags = wscFlags;
	ClientInfo[iClientID].lstIgnore.push_back(ii);

	// send confirmation msg
	PRINT_OK();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UserCmd_SetAudioMsg(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdSetAudioMsg)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		GetLocalized(iClientID, "MSG_1613"),
		GetLocalized(iClientID, "MSG_1614"),
		GetLocalized(iClientID, "MSG_1615"),
		GetLocalized(iClientID, "MSG_1608"),
		GetLocalized(iClientID, "MSG_1616"),
		GetLocalized(iClientID, "MSG_1617"),
	};

	wstring wscType = ToLower(GetParam(wscParam, ' ', 0));
	wstring wscSwitch = ToLower(GetParam(wscParam, ' ', 1));

	if (!wscType.length() || !wscSwitch.length() || ((wscSwitch.compare(L"on") != 0) && (wscSwitch.compare(L"off") != 0)))
		PRINT_ERROR();

	if (wscSwitch.compare(L"on") == 0)
	{
		pub::Audio::PlaySoundEffect(iClientID, CreateID("sounds_on"));
	}
	if (wscSwitch.compare(L"off") == 0)
	{
		pub::Audio::PlaySoundEffect(iClientID, CreateID("sounds_off"));
	}

	GET_USERFILE(scUserFile);

	wstring wscFilename;
	HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename);
	string scSection = "audiomsg_" + wstos(wscFilename);

	bool bEnable = !wscSwitch.compare(L"on") ? true : false;

	if (!wscType.compare(L"sounds")) {
		ClientInfo[iClientID].bAudioMsgSounds = bEnable;
		IniWrite(scUserFile, scSection, "sounds", bEnable ? "yes" : "no");
	}
	else {
		PRINT_ERROR();
	}

	PRINT_OK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_IgnoreID(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdIgnore)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		GetLocalized(iClientID, "MSG_1618"),
		GetLocalized(iClientID, "MSG_1619"),
		GetLocalized(iClientID, "MSG_1620"),
	};

	wstring wscClientID = GetParam(wscParam, ' ', 0);
	wstring wscFlags = ToLower(GetParam(wscParam, ' ', 1));

	if (!wscClientID.length())
		PRINT_ERROR();

	if (wscFlags.length() && wscFlags.compare(L"p") != 0)
		PRINT_ERROR();

	if (ClientInfo[iClientID].lstIgnore.size() > set_iUserCmdMaxIgnoreList)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1264"));
		return;
	}

	uint iClientIDTarget = ToInt(wscClientID);
	if (!HkIsValidClientID(iClientIDTarget) || HkIsInCharSelectMenu(iClientIDTarget))
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1266"));
		return;
	}

	wstring wscCharname = (wchar_t*)Players.GetActiveCharacterName(iClientIDTarget);

	// save to ini
	GET_USERFILE(scUserFile);
	IniWriteW(scUserFile, "IgnoreList", itos((int)ClientInfo[iClientID].lstIgnore.size() + 1), (wscCharname + L" " + wscFlags));

	// save in ClientInfo
	IGNORE_INFO ii;
	ii.wscCharname = wscCharname;
	ii.wscFlags = wscFlags;
	ClientInfo[iClientID].lstIgnore.push_back(ii);

	// send confirmation msg
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1267"), wscCharname.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_IgnoreList(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdIgnore)
	{
		PRINT_DISABLED();
		return;
	}

	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1268"));
	int i = 1;
	foreach(ClientInfo[iClientID].lstIgnore, IGNORE_INFO, it)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1269"), i, it->wscCharname.c_str(), it->wscFlags.c_str());
		i++;
	}

	// send confirmation msg
	PRINT_OK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_DelIgnore(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdIgnore)
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		GetLocalized(iClientID, "MSG_1621"),
		GetLocalized(iClientID, "MSG_1622"),
	};

	wstring wscID = GetParam(wscParam, ' ', 0);

	if (!wscID.length())
		PRINT_ERROR();

	GET_USERFILE(scUserFile);

	if (!wscID.compare(L"*"))
	{ // delete all
		IniDelSection(scUserFile, "IgnoreList");
		ClientInfo[iClientID].lstIgnore.clear();
		PRINT_OK();
		return;
	}

	list<uint> lstDelete;
	for (uint j = 1; wscID.length(); j++)
	{
		uint iID = ToInt(wscID.c_str());
		if (!iID || (iID > ClientInfo[iClientID].lstIgnore.size()))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1270"));
			return;
		}

		lstDelete.push_back(iID);
		wscID = GetParam(wscParam, ' ', j);
	}

	lstDelete.sort(greater<uint>());

	ClientInfo[iClientID].lstIgnore.reverse();
	foreach(lstDelete, uint, it)
	{
		uint iCurID = (uint)ClientInfo[iClientID].lstIgnore.size();
		foreach(ClientInfo[iClientID].lstIgnore, IGNORE_INFO, it2)
		{
			if (iCurID == (*it))
			{
				ClientInfo[iClientID].lstIgnore.erase(it2);
				break;
			}
			iCurID--;
		}
	}
	ClientInfo[iClientID].lstIgnore.reverse();

	// send confirmation msg
	IniDelSection(scUserFile, "IgnoreList");
	int i = 1;
	foreach(ClientInfo[iClientID].lstIgnore, IGNORE_INFO, it3)
	{
		IniWriteW(scUserFile, "IgnoreList", itos(i), ((*it3).wscCharname + L" " + (*it3).wscFlags));
		i++;
	}
	PRINT_OK();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_IDs(uint iClientID, const wstring &wscParam)
{
	wchar_t wszLine[128] = L"";
	list<HKPLAYERINFO> lstPlayers = HkGetPlayers();
	foreach(lstPlayers, HKPLAYERINFO, i)
	{
		wchar_t wszBuf[1024];
		swprintf(wszBuf, L"%s = %u | ", (*i).wscCharname.c_str(), (*i).iClientID);
		if ((wcslen(wszBuf) + wcslen(wszLine)) >= sizeof(wszLine) / 2) {
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1271"), wszLine);
			wcscpy(wszLine, wszBuf);
		}
		else
			wcscat(wszLine, wszBuf);
	}

	if (wcslen(wszLine))
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1271"), wszLine);
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1262"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_ID(uint iClientID, const wstring &wscParam)
{
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1274"), iClientID);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserLocalization_ID(uint iClientID, const wstring& wscParam)
{
	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_1530"),
		GetLocalized(iClientID, "MSG_1531"),
	};

	wstring wscClientLocales = GetParam(wscParam, ' ', 0);

	if (!wscClientLocales.length())
	{
		PRINT_ERROR();
		return;
	}

	// Если не ru и не en → ошибка
	if (wscClientLocales != L"ru" && wscClientLocales != L"en")
	{
		PRINT_ERROR();
		return;
	}

	CAccount* acc = Players.FindAccountFromClientID(iClientID);
	if (acc)
	{
		wstring wscDir;
		HkGetAccountDirName(acc, wscDir);

		bool setupLang = DatabaseManager::Instance().SetUserLocale(wstos(wscDir), wstos(wscClientLocales));
		if (setupLang)
		{
			// available localized from database
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1276"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1277"), wscDir);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void UserCmd_InviteID(uint iClientID, const wstring &wscParam)
{
	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0253"),
		L"Usage: /i$ <client-id>",
	};

	wstring wscClientID = GetParam(wscParam, ' ', 0);

	if(!wscClientID.length())
		PRINT_ERROR();

	uint iClientIDTarget = ToInt(wscClientID);
	if(!HkIsValidClientID(iClientIDTarget) || HkIsInCharSelectMenu(iClientIDTarget))
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1278"));
		return;
	}

	wstring wscCharname = (wchar_t*)Players.GetActiveCharacterName(iClientIDTarget);

	wstring wscXML = L"<TEXT>/i " + XMLText(wscCharname) + L"</TEXT>";
	char szBuf[0xFFFF];
	uint iRet;
	if(!HKHKSUCCESS(HkFMsgEncodeXML(wscXML, szBuf, sizeof(szBuf), iRet)))
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1279"));
		return;
	}

	CHAT_ID cID;
	cID.iID = iClientID;
	CHAT_ID cIDTo;
	cIDTo.iID = 0x00010001;
	Server.SubmitChat(cID, iRet, szBuf, cIDTo, -1);
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_Credits(uint iClientID, const wstring &wscParam)
{
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1280") + VERSION);
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1281"));

	bool bRunning = false;
	foreach(lstPlugins, PLUGIN_DATA, it) {
		if (it->bPaused)
			continue;

		bRunning = true;
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1282"), stows(it->sName).c_str());
	}
	if (!bRunning)
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1283"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserCmd_Help(uint iClientID, const wstring &wscParam)
{
	if (!set_bUserCmdHelp)
	{
		PRINT_DISABLED();
		return;
	}

	bool singleCommandHelp = wscParam.length() > 1;

	if (!singleCommandHelp)
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1284"));

	wstring boldHelp = set_wscUserCmdStyle.substr(0, set_wscUserCmdStyle.length() - 1) + L"1";
	wstring normal = set_wscUserCmdStyle;

	foreach(lstHelpEntries, stHelpEntry, he) {
		if (he->fnIsDisplayed(iClientID)) {
			if (singleCommandHelp) {
				if (he->wszCommand == wscParam) {
					set_wscUserCmdStyle = boldHelp;
					PrintUserCmdText(iClientID, he->wszCommand + L" " + he->wszArguments);
					set_wscUserCmdStyle = normal;
					int pos = 0;
					while (pos != wstring::npos) {
						int nextPos = he->wszLongHelp.find('\n', pos + 1);
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1285") + he->wszLongHelp.substr(pos, (nextPos - pos)));
						pos = nextPos;
					}
					return;
				}
			}
			else 
			{
				//<TRA data='0x4000FF08' mask='-1'/><TEXT>/set diemsg</TEXT>
				wstring commandXml = L"<TRA data='0x4000FF08' mask='-1'/><TEXT>";
				commandXml += he->wszCommand;
				commandXml += L"</TEXT>";
				HkFMsg(iClientID, commandXml);
				//set_wscUserCmdStyle = boldHelp;
				//PrintUserCmdText(iClientID, he->wszCommand + L" " + he->wszArguments);
				set_wscUserCmdStyle = normal;
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1286") + he->wszShortHelp);
			}
		}
	}

	if (singleCommandHelp) {
		set_wscUserCmdStyle = boldHelp;
		PrintUserCmdText(iClientID, wscParam);
		set_wscUserCmdStyle = normal;

		if (!UserCmd_Process(iClientID, wscParam))
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1287"));
	}
	else {
		CALL_PLUGINS_NORET(PLUGIN_UserCmd_Help, , (uint iClientID, const wstring &wscParam), (iClientID, wscParam));
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

USERCMD UserCmds[] =
{
	{ L"/set diemsg",			UserCmd_SetDieMsg},
	{ L"/set diemsgsize",		UserCmd_SetDieMsgSize},
	{ L"/set chatfont",			UserCmd_SetChatFont},
	{ L"/ignorelist",			UserCmd_IgnoreList},
	{ L"/delignore",			UserCmd_DelIgnore},
	{ L"/ignore",				UserCmd_Ignore},
	{ L"/ignoreid",				UserCmd_IgnoreID},
	{ L"/audiomsg",				UserCmd_SetAudioMsg},
	//{ L"/autobuy",				UserCmd_AutoBuy},
	{ L"/ids",					UserCmd_IDs},
	{ L"/id",					UserCmd_ID},
	{ L"/lang",					UserLocalization_ID}
	//{ L"/i",					UserCmd_InviteID},
	  //{ L"/i$",					UserCmd_InviteID},
	//{ L"/invite",				UserCmd_InviteID},
	  //{ L"/invite$",				UserCmd_InviteID},
	  //{ L"/credits",				UserCmd_Credits},
	  //{ L"/help",					UserCmd_Help},
};

bool UserCmd_Process(uint iClientID, const wstring &wscCmd)
{
	CALL_PLUGINS(PLUGIN_UserCmd_Process, bool, , (uint iClientID, const wstring &wscCmd), (iClientID, wscCmd));

	wstring wscCmdLower = ToLower(wscCmd);

	for (uint i = 0; (i < sizeof(UserCmds) / sizeof(USERCMD)); i++)
	{
		if (wscCmdLower.find(UserCmds[i].wszCmd) == 0)
		{
			wstring wscParam = L"";
			if (wscCmd.length() > wcslen(UserCmds[i].wszCmd))
			{
				if (wscCmd[wcslen(UserCmds[i].wszCmd)] != ' ')
					continue;
				wscParam = wscCmd.substr(wcslen(UserCmds[i].wszCmd) + 1);
			}

			// addlog
			if (set_bLogUserCmds) {
				wstring wscCharname = (wchar_t*)Players.GetActiveCharacterName(iClientID);
				HkAddUserCmdLog("%s: %s", wstos(wscCharname).c_str(), wstos(wscCmd).c_str());
			}

			try {
				UserCmds[i].proc(iClientID, wscParam);
				if (set_bLogUserCmds)
					HkAddUserCmdLog("finished");
			}
			catch (...) {
				if (set_bLogUserCmds)
					HkAddUserCmdLog("exception");
			}

			return true;
		}
	}

	return false;
}
