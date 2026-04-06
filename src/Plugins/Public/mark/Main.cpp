/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 06 апреля 2026 12:49:28
 * Version: 1.0.21
 */

#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

#define PRINT_DISABLED() PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0722"));
#define PRINT_ERROR() { for(uint i = 0; (i < sizeof(wscError)/sizeof(wstring)); i++) PrintUserCmdText(iClientID, wscError[i]); return; }

PLUGIN_RETURNCODE returncode;
MARK_INFO Mark[250];
float set_fAutoMarkRadius;
float set_fSonarRadius;
float set_fSonarReload;	//время перезарядки
float set_fSonarView;	//время видимости найденных объектов
vector<uint> vMarkSpaceObjProc;
bool set_bFlakVersion = false;
string scMarkFile;

EXPORT void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	scMarkFile = string(szCurDir) + "\\flhook_plugins\\mark.ini";
	set_fAutoMarkRadius = IniGetF(scMarkFile, "General", "AutoMarkRadius", 0.0f);
	set_fSonarRadius = IniGetF(scMarkFile, "General", "SonarRadius", 0.0f);
	set_fSonarReload = (IniGetI(scMarkFile, "General", "SonarTimeReload", 60)) * 1000;
	set_fSonarView = set_fSonarReload + (IniGetI(scMarkFile, "General", "SonarTimeView", 5) * 1000);
}

void LoadObjectsSystem(string filename, int iClientID)
{
	string systems = "..\\data\\universe\\" + filename;
	int objectCount = 0;
	INI_Reader ini;
	if (ini.open(systems.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("Object"))
			{
				uint systemHash;

				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
					{
						string systemName = ini.get_value_string(0);
						string lowerSystemName = ToLower(systemName);

						if (lowerSystemName.find("container") != std::string::npos)
						{
							systemHash = CreateID(ini.get_value_string(0));
							Mark[iClientID].mapObjects[systemHash] = systemName;
							++objectCount;
						}
					}
				}
			}
		}
		ini.close();

		string key = "<TRA data='0xFF24D700' mask='-2'/><TEXT>";
		key += wstos(GetLocalized(iClientID, "MSG_1543"));
		key += "</TEXT>";

		string val = "<TRA data='0x2093FF08' mask='-1'/><TEXT>";
		val += wstos(GetLocalized(iClientID, "MSG_1544"));
		val += std::to_string(objectCount);
		val += "</TEXT>";

		wstring scHelp = stows(key) + L"" + stows(val);
		HkFMsg(iClientID, scHelp);
	}
}

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		LoadSettings();
	return true;
}

void UserCmd_MarkObj(uint iClientID, const wstring &wscParam)
{
	uint iShip, iTargetShip;
	pub::Player::GetShip(iClientID, iShip);
	pub::SpaceObj::GetTarget(iShip, iTargetShip);

	char err = HkMarkObject(iClientID, iTargetShip);
	switch (err)
	{
	case 0:
		//PRINT_OK()
		break;
	case 1:
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0723"));
		break;
	case 2:
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0724"));
		break;
	case 3:
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0725"));
	default:
		break;
	}
}

void UserCmd_UnMarkObj(uint iClientID, const wstring &wscParam)
{
	uint iShip, iTargetShip;
	pub::Player::GetShip(iClientID, iShip);
	pub::SpaceObj::GetTarget(iShip, iTargetShip);
	char err = HkUnMarkObject(iClientID, iTargetShip);
	switch (err)
	{
		case 0:
			//PRINT_OK()
			break;
		case 1:
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0723"));
			break;
		case 2:
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0727"));
		default:
			break;
	}
}

void UserCmd_UnMarkAllObj(uint iClientID, const wstring &wscParam)
{
	HkUnMarkAllObjects(iClientID);
	//PRINT_OK()
}

void UserCmd_MarkObjGroup(uint iClientID, const wstring &wscParam)
{
	uint iShip, iTargetShip;
	pub::Player::GetShip(iClientID, iShip);
	pub::SpaceObj::GetTarget(iShip, iTargetShip);
	if (!iTargetShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0723"));
		return;
	}
	list<GROUP_MEMBER> lstMembers;
	lstMembers.clear();
	wstring wsClientID = (wchar_t*)Players.GetActiveCharacterName(iClientID);
	HkGetGroupMembers(wsClientID, lstMembers);
	foreach(lstMembers, GROUP_MEMBER, lstG)
	{
		if (Mark[lstG->iClientID].bIgnoreGroupMark)
			continue;
		uint iClientShip;
		pub::Player::GetShip(lstG->iClientID, iClientShip);
		if (iClientShip == iTargetShip)
			continue;
		HkMarkObject(lstG->iClientID, iTargetShip);
	}
}

void UserCmd_UnMarkObjGroup(uint iClientID, const wstring &wscParam)
{
	uint iShip, iTargetShip;
	pub::Player::GetShip(iClientID, iShip);
	pub::SpaceObj::GetTarget(iShip, iTargetShip);
	if (!iTargetShip)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0723"));
		return;
	}
	list<GROUP_MEMBER> lstMembers;
	lstMembers.clear();
	wstring wsClientID = (wchar_t*)Players.GetActiveCharacterName(iClientID);
	HkGetGroupMembers(wsClientID, lstMembers);
	foreach(lstMembers, GROUP_MEMBER, lstG)
	{
		HkUnMarkObject(lstG->iClientID, iTargetShip);
	}
}

void UserCmd_SetIgnoreGroupMark(uint iClientID, const wstring &wscParam)
{
	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0348"),
		GetLocalized(iClientID, "MSG_1545"),
	};

	if (ToLower(wscParam) == L"off")
	{
		Mark[iClientID].bIgnoreGroupMark = false;
		wstring wscDir, wscFilename;
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
		{
			string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
			string scSection = "general_" + wstos(wscFilename);
			IniWrite(scUserFile, scSection, "automarkenabled", "no");
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0730"));
		}
	}
	else if (ToLower(wscParam) == L"on")
	{
		Mark[iClientID].bIgnoreGroupMark = true;
		wstring wscDir, wscFilename;
		CAccount *acc = Players.FindAccountFromClientID(iClientID);
		if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
		{
			string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
			string scSection = "general_" + wstos(wscFilename);
			IniWrite(scUserFile, scSection, "automarkenabled", "yes");
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0731"));
		}
	}
	else
	{
		PRINT_ERROR();
	}
}

void UserCmd_Sonar(uint iClientID, const wstring& wscParam)
{
	if (set_fSonarRadius <= 0.0f) //sonar disabled
	{
		PRINT_DISABLED();
		return;
	}
	
	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0348"),
		GetLocalized(iClientID, "MSG_1546"),
	};

	wstring wscEnabled = ToLower(GetParam(wscParam, ' ', 0));

	if (!wscParam.length() || (wscEnabled != L"on" && wscEnabled != L"off"))
	{
		PRINT_ERROR();
		return;
	}

	if (!Mark[iClientID].bSonarEverything)
	{
		if (wscEnabled == L"on") //Сонар включен
		{
			Mark[iClientID].bSonarEverything = true;
			CAccount* acc = Players.FindAccountFromClientID(iClientID);
			wstring wscDir, wscFilename;
			if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
			{
				string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
				string scSection = "general_" + wstos(wscFilename);
				IniWrite(scUserFile, scSection, "sonar", "yes");
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0732"), Mark[iClientID].fSonarRadius / 1000);
		}
	}
	else
	{
		if (wscEnabled == L"off") //Сонар отключается
		{
			Mark[iClientID].bSonarEverything = false;
			CAccount* acc = Players.FindAccountFromClientID(iClientID);
			wstring wscDir, wscFilename;
			if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
			{
				string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
				string scSection = "general_" + wstos(wscFilename);
				IniWrite(scUserFile, scSection, "sonar", "no");
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0733"));
		}
	}
}

void UserCmd_AutoMark(uint iClientID, const wstring &wscParam)
{
	//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0734"));
	if (set_fAutoMarkRadius <= 0.0f) //automarking disabled
	{
		PRINT_DISABLED();
		return;
	}

	wstring wscError[] =
	{
		GetLocalized(iClientID, "MSG_0348"),
		GetLocalized(iClientID, "MSG_1547"),
	};
	

	wstring wscEnabled = ToLower(GetParam(wscParam, ' ', 0));

	if (!wscParam.length() || (wscEnabled != L"on" && wscEnabled != L"off"))
	{
		PRINT_ERROR();
		return;
	}

	wstring wscRadius = GetParam(wscParam, ' ', 1);
	float fRadius = 0.0f;
	if (wscRadius.length())
	{
		fRadius = ToFloat(wscRadius);
	}

	if (fRadius > 100000)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0735"));
		return;
	}

	// Я думаю, что этот раздел можно было бы сделать лучше, но сейчас я не могу об этом думать ..
	if (!Mark[iClientID].bMarkEverything)
	{
		if (wscRadius.length())
			Mark[iClientID].fAutoMarkRadius = fRadius;
		if (wscEnabled == L"on") //Автомаркировка включена
		{
			Mark[iClientID].bMarkEverything = true;
			CAccount *acc = Players.FindAccountFromClientID(iClientID);
			wstring wscDir, wscFilename;
			if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
			{
				string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
				string scSection = "general_" + wstos(wscFilename);
				IniWrite(scUserFile, scSection, "automarkenabled", "yes");
				if (wscRadius.length())
					IniWrite(scUserFile, scSection, "automarkradius", ftos(Mark[iClientID].fAutoMarkRadius));
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0736"), Mark[iClientID].fAutoMarkRadius / 1000);
		}
		else if (wscRadius.length())
		{
			CAccount *acc = Players.FindAccountFromClientID(iClientID);
			wstring wscDir, wscFilename;
			if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
			{
				string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
				string scSection = "general_" + wstos(wscFilename);
				IniWrite(scUserFile, scSection, "automarkradius", ftos(Mark[iClientID].fAutoMarkRadius));
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0737"), fRadius);
		}
	}
	else
	{
		if (wscRadius.length())
			Mark[iClientID].fAutoMarkRadius = fRadius;
		if (wscEnabled == L"off") //AutoMark is being disabled
		{
			Mark[iClientID].bMarkEverything = false;
			CAccount *acc = Players.FindAccountFromClientID(iClientID);
			wstring wscDir, wscFilename;
			if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
			{
				string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
				string scSection = "general_" + wstos(wscFilename);
				IniWrite(scUserFile, scSection, "automarkenabled", "no");
				if (wscRadius.length())
					IniWrite(scUserFile, scSection, "automarkradius", ftos(Mark[iClientID].fAutoMarkRadius));
			}
			if (wscRadius.length())
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0738"), Mark[iClientID].fAutoMarkRadius / 1000);
			else
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0739"));
		}
		else if (wscRadius.length())
		{
			CAccount *acc = Players.FindAccountFromClientID(iClientID);
			wstring wscDir, wscFilename;
			if (HKHKSUCCESS(HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename)) && HKHKSUCCESS(HkGetAccountDirName(acc, wscDir)))
			{
				string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
				string scSection = "general_" + wstos(wscFilename);
				IniWrite(scUserFile, scSection, "automarkradius", ftos(Mark[iClientID].fAutoMarkRadius));
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0737"), fRadius);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char HkMarkObject(uint iClientID, uint iObject)
{
	if (!iObject)
		return 1;

	uint iClientIDMark = HkGetClientIDByShip(iObject);
	uint iSystemID, iObjectSystemID;
	pub::Player::GetSystem(iClientID, iSystemID);
	pub::SpaceObj::GetSystem(iObject, iObjectSystemID);

	if (iSystemID == iObjectSystemID)
	{
		for (uint i = 0; i < Mark[iClientID].vMarkedObjs.size(); i++)
		{
			if (Mark[iClientID].vMarkedObjs[i] == iObject)
				return 3; //already marked
		}
	}
	else
	{
		for (uint j = 0; j < Mark[iClientID].vDelayedSystemMarkedObjs.size(); j++)
		{
			if (Mark[iClientID].vDelayedSystemMarkedObjs[j] == iObject)
				return 3; //already marked
		}
		Mark[iClientID].vDelayedSystemMarkedObjs.push_back(iObject);
		pub::Audio::PlaySoundEffect(iClientID, 2460046221); //CreateID("ui_select_add")
		return 0;
	}

	pub::Player::MarkObj(iClientID, iObject, 1);
	for (uint i = 0; i < Mark[iClientID].vAutoMarkedObjs.size(); i++) //remove from automarked vector
	{
		if (Mark[iClientID].vAutoMarkedObjs[i] == iObject)
		{
			if (i != Mark[iClientID].vAutoMarkedObjs.size() - 1)
			{
				Mark[iClientID].vAutoMarkedObjs[i] = Mark[iClientID].vAutoMarkedObjs[Mark[iClientID].vAutoMarkedObjs.size() - 1];
			}
			Mark[iClientID].vAutoMarkedObjs.pop_back();
			break;
		}
	}
	Mark[iClientID].vMarkedObjs.push_back(iObject);
	pub::Audio::PlaySoundEffect(iClientID, 2460046221); //CreateID("ui_select_add")
	return 0;
}

char HkUnMarkObject(uint iClientID, uint iObject)
{
	if (!iObject)
		return 1;

	for (uint i = 0; i < Mark[iClientID].vMarkedObjs.size(); i++)
	{
		if (Mark[iClientID].vMarkedObjs[i] == iObject)
		{
			if (i != Mark[iClientID].vMarkedObjs.size() - 1)
			{
				Mark[iClientID].vMarkedObjs[i] = Mark[iClientID].vMarkedObjs[Mark[iClientID].vMarkedObjs.size() - 1];
			}
			Mark[iClientID].vMarkedObjs.pop_back();
			pub::Player::MarkObj(iClientID, iObject, 0);
			pub::Audio::PlaySoundEffect(iClientID, 2939827141); //CreateID("ui_select_remove")
			return 0;
		}
	}

	for (uint j = 0; j < Mark[iClientID].vAutoMarkedObjs.size(); j++)
	{
		if (Mark[iClientID].vAutoMarkedObjs[j] == iObject)
		{
			if (j != Mark[iClientID].vAutoMarkedObjs.size() - 1)
			{
				Mark[iClientID].vAutoMarkedObjs[j] = Mark[iClientID].vAutoMarkedObjs[Mark[iClientID].vAutoMarkedObjs.size() - 1];
			}
			Mark[iClientID].vAutoMarkedObjs.pop_back();
			pub::Player::MarkObj(iClientID, iObject, 0);
			pub::Audio::PlaySoundEffect(iClientID, 2939827141); //CreateID("ui_select_remove")
			return 0;
		}
	}

	for (uint k = 0; k < Mark[iClientID].vDelayedSystemMarkedObjs.size(); k++)
	{
		if (Mark[iClientID].vDelayedSystemMarkedObjs[k] == iObject)
		{
			if (k != Mark[iClientID].vDelayedSystemMarkedObjs.size() - 1)
			{
				Mark[iClientID].vDelayedSystemMarkedObjs[k] = Mark[iClientID].vDelayedSystemMarkedObjs[Mark[iClientID].vDelayedSystemMarkedObjs.size() - 1];
			}
			Mark[iClientID].vDelayedSystemMarkedObjs.pop_back();
			pub::Audio::PlaySoundEffect(iClientID, 2939827141); //CreateID("ui_select_remove")
			return 0;
		}
	}
	return 2;
}

void HkUnMarkAllObjects(uint iClientID)
{
	for (uint i = 0; i < Mark[iClientID].vMarkedObjs.size(); i++)
	{
		pub::Player::MarkObj(iClientID, (Mark[iClientID].vMarkedObjs[i]), 0);
	}
	Mark[iClientID].vMarkedObjs.clear();
	for (uint i = 0; i < Mark[iClientID].vAutoMarkedObjs.size(); i++)
	{
		pub::Player::MarkObj(iClientID, (Mark[iClientID].vAutoMarkedObjs[i]), 0);
	}
	Mark[iClientID].vAutoMarkedObjs.clear();
	Mark[iClientID].vDelayedSystemMarkedObjs.clear();
	pub::Audio::PlaySoundEffect(iClientID, 2939827141); //CreateID("ui_select_remove")
}

float HkDistance3D(Vector v1, Vector v2)
{
	float sq1 = v1.x - v2.x, sq2 = v1.y - v2.y, sq3 = v1.z - v2.z;
	return sqrt(sq1*sq1 + sq2 * sq2 + sq3 * sq3);
}

string ftos(float f)
{
	char szBuf[16];
	sprintf_s(szBuf, "%f", f);
	return szBuf;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClearClientMark(uint iClientID)
{
	Mark[iClientID].bMarkEverything = false;
	Mark[iClientID].vMarkedObjs.clear();
	Mark[iClientID].vDelayedSystemMarkedObjs.clear();
	Mark[iClientID].vAutoMarkedObjs.clear();
	Mark[iClientID].vAutoMarkedSonarObjs.clear();
	Mark[iClientID].vDelayedAutoMarkedObjs.clear();
}

string GetUserFilePath(const wstring& wscCharname, const string& scExtension)
{
	// init variables
	char szDataPath[MAX_PATH];
	GetUserDataPath(szDataPath);
	string scAcctPath = string(szDataPath) + "\\Accts\\MultiPlayer\\";

	wstring wscDir;
	wstring wscFile;
	if (HkGetAccountDirName(wscCharname, wscDir) != HKE_OK)
		return "";
	if (HkGetCharFileName(wscCharname, wscFile) != HKE_OK)
		return "";

	return scAcctPath + wstos(wscDir) + "\\" + wstos(wscFile) + scExtension;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Timers

/// <summary>
/// Очищает метки созданные сонаром видимые в диапазоне (188000 - 180000 = 8000 - 8 секунд)
/// </summary>
void HkTimerContainerUnMark()
{
	try {
		if (set_fSonarRadius <= 0.0f) //сонар отключён
			return;

		struct PlayerData* pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint iShip, iClientID = HkGetClientIdFromPD(pPD);

			pub::Player::GetShip(iClientID, iShip);
			if (!iShip || Mark[iClientID].fSonarRadius <= 0.0f || !Mark[iClientID].bSonarEverything) //состыкован или не желает работы сонара
				continue;

			uint iSystem;
			pub::Player::GetSystem(iClientID, iSystem);
			// Если данные были не загружены (первый вход) или игрок сменил текущую систему
			// загружаем информацию
			if (Mark[iClientID].CurrentSystemId != iSystem)
			{
				//Перенести сохранение объектов и текущей системы для каждого игрока в struct MARK_INFO или другую там создать
				const Universe::ISystem* SystemPlayer = Universe::get_system(iSystem);
				std::string filename(SystemPlayer->file);
				LoadObjectsSystem(filename, iClientID);
				Mark[iClientID].CurrentSystemId = iSystem;
			}

			Vector VClientPosCustom;
			Matrix MClientOriCustom;
			pub::SpaceObj::GetLocation(iShip, VClientPosCustom, MClientOriCustom);
			for each (pair<uint, string> object in Mark[iClientID].mapObjects)
			{
				Vector VTargetPos;
				Matrix MTargetOri;

				uint id = object.first;
				string name = object.second;

				pub::SpaceObj::GetLocation(id, VTargetPos, MTargetOri);
				float distance = HkDistance3D(VTargetPos, VClientPosCustom);
				float radius = Mark[iClientID].fSonarRadius;
				
				pub::Player::MarkObj(iClientID, id, 0);
				Mark[iClientID].vAutoMarkedSonarObjs.remove(id);
			}
		}
	}
	catch (...) {
	}
}

/// <summary>
/// Сонар помечает цели
/// </summary>
void HkTimerContainerMark()
{
	try {
		if (set_fSonarRadius <= 0.0f) //сонар отключён
			return;

		struct PlayerData* pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint iShip, iClientID = HkGetClientIdFromPD(pPD);
			pub::Player::GetShip(iClientID, iShip);
			if (!iShip || Mark[iClientID].fSonarRadius <= 0.0f || !Mark[iClientID].bSonarEverything) //состыкован или не желает работы сонара
				continue;

			uint iSystem;
			pub::Player::GetSystem(iClientID, iSystem);
			// Если данные были не загружены (первый вход) или игрок сменил текущую систему
			// загружаем информацию
			if (Mark[iClientID].CurrentSystemId != iSystem)
			{
				//Перенести сохранение объектов и текущей системы для каждого игрока в struct MARK_INFO или другую там создать
				const Universe::ISystem* SystemPlayer = Universe::get_system(iSystem);
				std::string filename(SystemPlayer->file);
				LoadObjectsSystem(filename, iClientID);
				Mark[iClientID].CurrentSystemId = iSystem;
			}

			//kill_one sonar
			pub::Audio::PlaySoundEffect(iClientID, CreateID("sonar"));

			Vector VClientPosCustom;
			Matrix MClientOriCustom;
			pub::SpaceObj::GetLocation(iShip, VClientPosCustom, MClientOriCustom);
			for each (pair<uint, string> object in Mark[iClientID].mapObjects)
			{
				Vector VTargetPos;
				Matrix MTargetOri;

				uint id = object.first;
				string name = object.second;

				pub::SpaceObj::GetLocation(id, VTargetPos, MTargetOri);
				float distance = HkDistance3D(VTargetPos, VClientPosCustom);
				float radius = Mark[iClientID].fSonarRadius;
				if (distance < radius) // если объект в пределах радиуса то маркируем
				{
					pub::Player::MarkObj(iClientID, id, 1);
					Mark[iClientID].vAutoMarkedSonarObjs.push_front(id);
				}
				else // очищаем те, что уже далеко от игрока и снимаем маркировку
				{
					pub::Player::MarkObj(iClientID, id, 0);
					Mark[iClientID].vAutoMarkedSonarObjs.remove(id);
				}
			}
		}
	}
	catch (...) {
	}
}

void HkTimerSpaceObjMark()
{
	try {
		if (set_fAutoMarkRadius <= 0.0f) //automarking disabled
			return;

		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint iShip, iClientID = HkGetClientIdFromPD(pPD);
			pub::Player::GetShip(iClientID, iShip);
			if (!iShip || Mark[iClientID].fAutoMarkRadius <= 0.0f || !Mark[iClientID].bMarkEverything) //состыкован или не желает никакой маркировки
				continue;

			uint iSystem;
			pub::Player::GetSystem(iClientID, iSystem);
			
			Vector VClientPos;
			Matrix MClientOri;
			pub::SpaceObj::GetLocation(iShip, VClientPos, MClientOri);
			for (uint i = 0; i < Mark[iClientID].vAutoMarkedObjs.size(); i++)
			{
				Vector VTargetPos;
				Matrix MTargetOri;
				pub::SpaceObj::GetLocation(Mark[iClientID].vAutoMarkedObjs[i], VTargetPos, MTargetOri);

				if (HkDistance3D(VTargetPos, VClientPos) > Mark[iClientID].fAutoMarkRadius)
				{
					pub::Player::MarkObj(iClientID, Mark[iClientID].vAutoMarkedObjs[i], 0);
					Mark[iClientID].vDelayedAutoMarkedObjs.push_back(Mark[iClientID].vAutoMarkedObjs[i]);
					if (i != Mark[iClientID].vAutoMarkedObjs.size() - 1)
					{
						Mark[iClientID].vAutoMarkedObjs[i] = Mark[iClientID].vAutoMarkedObjs[Mark[iClientID].vAutoMarkedObjs.size() - 1];
						i--;
					}
					Mark[iClientID].vAutoMarkedObjs.pop_back();
				}
			}

			for (uint i = 0; i < Mark[iClientID].vDelayedAutoMarkedObjs.size(); i++)
			{
				if (pub::SpaceObj::ExistsAndAlive(Mark[iClientID].vDelayedAutoMarkedObjs[i]))
				{
					if (i != Mark[iClientID].vDelayedAutoMarkedObjs.size() - 1)
					{
						Mark[iClientID].vDelayedAutoMarkedObjs[i] = Mark[iClientID].vDelayedAutoMarkedObjs[Mark[iClientID].vDelayedAutoMarkedObjs.size() - 1];
						i--;
					}
					Mark[iClientID].vDelayedAutoMarkedObjs.pop_back();
					continue;
				}
				Vector VTargetPos;
				Matrix MTargetOri;
				pub::SpaceObj::GetLocation(Mark[iClientID].vDelayedAutoMarkedObjs[i], VTargetPos, MTargetOri);
				if (!(HkDistance3D(VTargetPos, VClientPos) > Mark[iClientID].fAutoMarkRadius))
				{
					pub::Player::MarkObj(iClientID, Mark[iClientID].vDelayedAutoMarkedObjs[i], 1);
					Mark[iClientID].vAutoMarkedObjs.push_back(Mark[iClientID].vDelayedAutoMarkedObjs[i]);
					if (i != Mark[iClientID].vDelayedAutoMarkedObjs.size() - 1)
					{
						Mark[iClientID].vDelayedAutoMarkedObjs[i] = Mark[iClientID].vDelayedAutoMarkedObjs[Mark[iClientID].vDelayedAutoMarkedObjs.size() - 1];
						i--;
					}
					Mark[iClientID].vDelayedAutoMarkedObjs.pop_back();
				}
			}

			for (uint i = 0; i < vMarkSpaceObjProc.size(); i++)
			{
				uint iMarkSpaceObjProcShip = vMarkSpaceObjProc[i];
				if (set_bFlakVersion)
				{
					uint iType;
					pub::SpaceObj::GetType(iMarkSpaceObjProcShip, iType);
					if (iType != OBJ_CAPITAL && ((Mark[iClientID].bMarkEverything && iType == OBJ_FIGHTER)/* || iType==OBJ_FREIGHTER*/))
					{
						uint iSpaceObjSystem;
						pub::SpaceObj::GetSystem(iMarkSpaceObjProcShip, iSpaceObjSystem);
						Vector VTargetPos;
						Matrix MTargetOri;
						pub::SpaceObj::GetLocation(iMarkSpaceObjProcShip, VTargetPos, MTargetOri);
						if (iSpaceObjSystem != iSystem || HkDistance3D(VTargetPos, VClientPos) > Mark[iClientID].fAutoMarkRadius)
						{
							Mark[iClientID].vDelayedAutoMarkedObjs.push_back(iMarkSpaceObjProcShip);
						}
						else
						{
							pub::Player::MarkObj(iClientID, iMarkSpaceObjProcShip, 1);
							Mark[iClientID].vAutoMarkedObjs.push_back(iMarkSpaceObjProcShip);
						}
					}
				}
				else // просто все помечаем
				{
					uint iSpaceObjSystem;
					pub::SpaceObj::GetSystem(iMarkSpaceObjProcShip, iSpaceObjSystem);
					Vector VTargetPos;
					Matrix MTargetOri;
					pub::SpaceObj::GetLocation(iMarkSpaceObjProcShip, VTargetPos, MTargetOri);
					if (iSpaceObjSystem != iSystem || HkDistance3D(VTargetPos, VClientPos) > Mark[iClientID].fAutoMarkRadius)
					{
						Mark[iClientID].vDelayedAutoMarkedObjs.push_back(iMarkSpaceObjProcShip);
					}
					else
					{
						pub::Player::MarkObj(iClientID, iMarkSpaceObjProcShip, 1);
						Mark[iClientID].vAutoMarkedObjs.push_back(iMarkSpaceObjProcShip);
					}
				}
			}

			vMarkSpaceObjProc.clear();
		}
	}
	catch (...) {
	}
}

list<DELAY_MARK> g_lstDelayedMarks;
void HkTimerMarkDelay()
{
	if (!g_lstDelayedMarks.size())
		return;

	mstime tmTimeNow = timeInMS();
	for (list<DELAY_MARK>::iterator mark = g_lstDelayedMarks.begin(); mark != g_lstDelayedMarks.end(); )
	{
		if (tmTimeNow - mark->time > 3000)
		{
			Matrix mTemp;
			Vector vItem, vPlayer;
			pub::SpaceObj::GetLocation(mark->iObj, vItem, mTemp);
			uint iItemSystem;
			pub::SpaceObj::GetSystem(mark->iObj, iItemSystem);
			// для всех игроков
			struct PlayerData *pPD = 0;
			while (pPD = Players.traverse_active(pPD))
			{
				uint iClientID = HkGetClientIdFromPD(pPD);
				if (Players[iClientID].iSystemID == iItemSystem)
				{
					pub::SpaceObj::GetLocation(Players[iClientID].iShipID, vPlayer, mTemp);
					if (HkDistance3D(vPlayer, vItem) <= LOOT_UNSEEN_RADIUS)
					{
						HkMarkObject(iClientID, mark->iObj);
					}
				}
			}
			mark = g_lstDelayedMarks.erase(mark);
		}
		else
		{
			mark++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace HkIServerImpl
{
	EXPORT void __stdcall JumpInComplete(unsigned int iSystemID, unsigned int iShip)
	{
		uint iClientID = HkGetClientIDByShip(iShip);
		if (!iClientID)
			return;
		vector<uint> vTempMark;
		for (uint i = 0; i < Mark[iClientID].vDelayedSystemMarkedObjs.size(); i++)
		{
			if (pub::SpaceObj::ExistsAndAlive(Mark[iClientID].vDelayedSystemMarkedObjs[i]))
			{
				if (i != Mark[iClientID].vDelayedSystemMarkedObjs.size() - 1)
				{
					Mark[iClientID].vDelayedSystemMarkedObjs[i] = Mark[iClientID].vDelayedSystemMarkedObjs[Mark[iClientID].vDelayedSystemMarkedObjs.size() - 1];
					i--;
				}
				Mark[iClientID].vDelayedSystemMarkedObjs.pop_back();
				continue;
			}

			uint iTargetSystem;
			pub::SpaceObj::GetSystem(Mark[iClientID].vDelayedSystemMarkedObjs[i], iTargetSystem);
			if (iTargetSystem == iSystemID)
			{
				pub::Player::MarkObj(iClientID, Mark[iClientID].vDelayedSystemMarkedObjs[i], 1);
				vTempMark.push_back(Mark[iClientID].vDelayedSystemMarkedObjs[i]);
				if (i != Mark[iClientID].vDelayedSystemMarkedObjs.size() - 1)
				{
					Mark[iClientID].vDelayedSystemMarkedObjs[i] = Mark[iClientID].vDelayedSystemMarkedObjs[Mark[iClientID].vDelayedSystemMarkedObjs.size() - 1];
					i--;
				}
				Mark[iClientID].vDelayedSystemMarkedObjs.pop_back();
			}
		}
		for (uint i = 0; i < Mark[iClientID].vMarkedObjs.size(); i++)
		{
			if (!pub::SpaceObj::ExistsAndAlive(Mark[iClientID].vMarkedObjs[i]))
				Mark[iClientID].vDelayedSystemMarkedObjs.push_back(Mark[iClientID].vMarkedObjs[i]);
		}
		Mark[iClientID].vMarkedObjs = vTempMark;
	}

	EXPORT void __stdcall LaunchComplete(unsigned int iBaseID, unsigned int iShip)
	{
		uint iClientID = HkGetClientIDByShip(iShip);
		if (!iClientID)
			return;
		for (uint i = 0; i < Mark[iClientID].vMarkedObjs.size(); i++)
		{
			if (pub::SpaceObj::ExistsAndAlive(Mark[iClientID].vMarkedObjs[i]))
			{
				if (i != Mark[iClientID].vMarkedObjs.size() - 1)
				{
					Mark[iClientID].vMarkedObjs[i] = Mark[iClientID].vMarkedObjs[Mark[iClientID].vMarkedObjs.size() - 1];
					i--;
				}
				Mark[iClientID].vMarkedObjs.pop_back();
				continue;
			}
			pub::Player::MarkObj(iClientID, Mark[iClientID].vMarkedObjs[i], 1);
		}
	}

	EXPORT void __stdcall BaseEnter(unsigned int iBaseID, unsigned int iClientID)
	{
		Mark[iClientID].vAutoMarkedSonarObjs.clear();
		Mark[iClientID].vAutoMarkedObjs.clear();
		Mark[iClientID].vDelayedAutoMarkedObjs.clear();
	}

	typedef void(*_TimerFunc)();
	struct TIMER
	{
		_TimerFunc	proc;
		mstime		tmIntervallMS;
		mstime		tmLastCall;
	};

	TIMER Timers[] =
	{
		{HkTimerMarkDelay,			50,					0},
		{HkTimerSpaceObjMark,		50,					0},
		{HkTimerContainerMark,		set_fSonarReload,	0},
		{HkTimerContainerUnMark,	set_fSonarView,		0},
	};

	EXPORT int __stdcall Update()
	{
		returncode = DEFAULT_RETURNCODE;
		static bool bFirstTime = true;
		if (bFirstTime)
		{
			bFirstTime = false;
			// проверить наличие залогиненных игроков и сбросить данные их подключения
			struct PlayerData *pPD = 0;
			while (pPD = Players.traverse_active(pPD))
				ClearClientMark(HkGetClientIdFromPD(pPD));
		}

		if (Timers[2].tmIntervallMS != set_fSonarReload)
			Timers[2].tmIntervallMS = set_fSonarReload;
		if (Timers[3].tmIntervallMS != set_fSonarView)
			Timers[3].tmIntervallMS = set_fSonarView;

		for (uint i = 0; (i < sizeof(Timers) / sizeof(TIMER)); i++)
		{
			if ((timeInMS() - Timers[i].tmLastCall) >= Timers[i].tmIntervallMS)
			{
				Timers[i].tmLastCall = timeInMS();
				Timers[i].proc();
			}
		}
		return 0;
	}
}

EXPORT void LoadUserCharSettings(uint iClientID)
{
	CAccount *acc = Players.FindAccountFromClientID(iClientID);
	wstring wscDir;
	HkGetAccountDirName(acc, wscDir);
	string scUserFile = scAcctPath + wstos(wscDir) + "\\flhookuser.ini";
	wstring wscFilename;
	HkGetCharFileName(ARG_CLIENTID(iClientID), wscFilename);
	string scFilename = wstos(wscFilename);
	string scSection = "general_" + scFilename;
	Mark[iClientID].bMarkEverything = IniGetB(scUserFile, scSection, "automarkenabled", false);
	Mark[iClientID].bSonarEverything = IniGetB(scUserFile, scSection, "sonar", false);
	Mark[iClientID].bIgnoreGroupMark = IniGetB(scUserFile, scSection, "ignoregroupmarkenabled", false);
	Mark[iClientID].fAutoMarkRadius = IniGetF(scUserFile, scSection, "automarkradius", set_fAutoMarkRadius);
	Mark[iClientID].fSonarRadius = IniGetF(scUserFile, scSection, "sonarradius", set_fSonarRadius);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT void UserCmd_Help(uint iClientID, const wstring &wscParam)
{
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0741"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0742"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0743"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0744"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0745"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0746"));
	PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0747"));
}

typedef void(*_UserCmdProc)(uint, const wstring &);

struct USERCMD
{
	wchar_t *wszCmd;
	_UserCmdProc proc;
};

USERCMD UserCmds[] =
{
	{ L"/mark",					        UserCmd_MarkObj},
	{ L"/m",					        UserCmd_MarkObj},
	{ L"/unmark",   					UserCmd_UnMarkObj},
	{ L"/um",   					    UserCmd_UnMarkObj},
	{ L"/unmarkall",					UserCmd_UnMarkAllObj},
	{ L"/uma",					        UserCmd_UnMarkAllObj},
	{ L"/groupmark",					UserCmd_MarkObjGroup},
	{ L"/gm",					        UserCmd_MarkObjGroup},
	{ L"/groupunmark",					UserCmd_UnMarkObjGroup},
	{ L"/gum",					        UserCmd_UnMarkObjGroup},
	{ L"/ignoregroupmarks",				UserCmd_SetIgnoreGroupMark},
	{ L"/automark",					    UserCmd_AutoMark},
	{ L"/sonar",					    UserCmd_Sonar},
};

EXPORT bool UserCmd_Process(uint iClientID, const wstring &wscCmd)
{
	wstring wscCmdLower = ToLower(wscCmd);
	for (uint i = 0; (i < sizeof(UserCmds) / sizeof(USERCMD)); i++)
	{
		if (wscCmdLower.find(ToLower(UserCmds[i].wszCmd)) == 0)
		{
			wstring wscParam = L"";
			if (wscCmd.length() > wcslen(UserCmds[i].wszCmd))
			{
				if (wscCmd[wcslen(UserCmds[i].wszCmd)] != ' ')
					continue;
				wscParam = wscCmd.substr(wcslen(UserCmds[i].wszCmd) + 1);
			}
			UserCmds[i].proc(iClientID, wscParam);
			returncode = SKIPPLUGINS_NOFUNCTIONCALL; // we handled the command, return immediatly
			return true;
		}
	}
	returncode = DEFAULT_RETURNCODE; // we did not handle the command, so let other plugins or FLHook kick in
	return false;
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "Mark plugin by M0tah";
	p_PI->sShortName = "mark";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 0));
	//	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Help, PLUGIN_UserCmd_Help,0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::JumpInComplete, PLUGIN_HkIServerImpl_JumpInComplete, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::LaunchComplete, PLUGIN_HkIServerImpl_LaunchComplete, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::BaseEnter, PLUGIN_HkIServerImpl_BaseEnter, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Update, PLUGIN_HkIServerImpl_Update, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadUserCharSettings, PLUGIN_LoadUserCharSettings, 0));
	return p_PI;
}