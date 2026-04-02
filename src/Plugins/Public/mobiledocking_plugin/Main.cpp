/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 02 апреля 2026 06:53:11
 * Version: 1.0.17
 */

/**
 * Mobile Docking Plugin for FLHook
 * Initial Version by Cannon
 * Using some work written by Alley
 * Rework by Remnant
 */

#include "Main.h"
#include <ctime>

PLUGIN_RETURNCODE returncode;
map<uint, CLIENT_DATA> mobiledockClients;
map<uint, uint> mapPendingDockingRequests;
vector<uint> dockingModuleEquipmentIds;

// Судно, превышающее какую грузоподъемность, должно быть отклонено для использования в доке?
int cargoCapacityLimit = 275;

// Сколько времени будет дано игроку перед ударом, если авиаперевозчик захочет его выбросить за борт.
int jettisonKickTime = 15;

// Отложенные действия, которые необходимо сделать. Посмотрите HkTimerCheckKick().
vector<ActionJettison> jettisonList;

void HkTimerCheckKick()
{
	returncode = DEFAULT_RETURNCODE;

	for (vector<ActionJettison>::iterator it = jettisonList.begin(); it != jettisonList.end(); )
	{
		it->timeLeft--;
		if (it->timeLeft == 0)
		{
			uint checkCarrierClientID = HkGetClientIdFromCharname(it->carrierCharname);
			uint checkDockedClientID = HkGetClientIdFromCharname(it->dockedCharname);

			// Если и авианосец, и пристыкованный корабль все еще находятся на сервере.
			if (checkDockedClientID != -1 && checkCarrierClientID != -1)
			{
				// Если клиент все еще закреплен
				if (mobiledockClients[checkDockedClientID].wscDockedWithCharname == it->carrierCharname)
				{
					HkKickReason(ARG_CLIENTID(checkDockedClientID), L"Принудительная отстыковка.");

					// Если оператор связи умер, нам не нужно это сообщение.
					if (!mobiledockClients[checkDockedClientID].carrierDied)
						PrintUserCmdText(checkCarrierClientID, GetLocalized(checkCarrierClientID, "MSG_0781"));
				}
			}

			it = jettisonList.erase(it);
		}
		else
		{
			it++;
		}
	}
}

// Легкое создание отложенного действия.
void DelayedJettison(int delayTimeSecond, wstring carrierCharname, wstring dockedCharname)
{
	ActionJettison action;
	action.timeLeft = delayTimeSecond;
	action.carrierCharname = carrierCharname;
	action.dockedCharname = dockedCharname;
	jettisonList.push_back(action);
}

void JettisonShip(uint carrierClientID, uint dockedClientID)
{
	const wchar_t* carrierCharname = (const wchar_t*)Players.GetActiveCharacterName(carrierClientID);
	const wchar_t* dockedCharname = (const wchar_t*)Players.GetActiveCharacterName(dockedClientID);

	if (dockedClientID != -1)
	{
		if (!mobiledockClients[dockedClientID].carrierDied)
		{
			PrintUserCmdText(carrierClientID, GetLocalized(carrierClientID, "MSG_0782"), jettisonKickTime);
			PrintUserCmdText(dockedClientID, GetLocalized(dockedClientID, "MSG_0783"), jettisonKickTime);
		}
		else
		{
			PrintUserCmdText(dockedClientID, GetLocalized(dockedClientID, "MSG_0784"));
		}
		pub::Audio::PlaySoundEffect(dockedClientID, CreateID("rtc_klaxon_loop"));

		// Создайте отложенное действие.
		DelayedJettison(jettisonKickTime, carrierCharname, dockedCharname);
	}
}

// Возвращает количество установленных стыковочных модулей на корабле конкретного клиента.
uint GetInstalledModules(uint iClientID)
{
	uint modules = 0;

	// Проверьте, оборудовано ли в настоящее время отстыковочное судно стыковочным модулем.
	for (list<EquipDesc>::iterator item = Players[iClientID].equipDescList.equip.begin(); item != Players[iClientID].equipDescList.equip.end(); item++)
	{
		if (find(dockingModuleEquipmentIds.begin(), dockingModuleEquipmentIds.end(), item->iArchID) != dockingModuleEquipmentIds.end())
		{
			if (item->bMounted)
			{
				modules++;
			}
		}
	}

	return modules;
}

void UpdateCarrierLocationInformation(uint dockedClientId, uint carrierShip)
{
	// Подготовьте ссылки на пристыкованные корабли, копию положения и поворота корабля-носителя для манипуляций.
	Vector& carrierPos = mobiledockClients[dockedClientId].carrierPos;
	Matrix& carrierRot = mobiledockClients[dockedClientId].carrierRot;

	// Если авианосец находится в космосе, просто установите место отстыковки туда, где авианосец находится в данный момент.
	pub::SpaceObj::GetSystem(carrierShip, mobiledockClients[dockedClientId].carrierSystem);
	pub::SpaceObj::GetLocation(carrierShip, carrierPos, carrierRot);

	carrierPos.x += carrierRot.data[0][1] * set_iMobileDockOffset;
	carrierPos.y += carrierRot.data[1][1] * set_iMobileDockOffset;
	carrierPos.z += carrierRot.data[2][1] * set_iMobileDockOffset;
}

// Перегруженная функция, используемая с конкретной локацией авианосца вместо извлечения ее из самого корабля.
void UpdateCarrierLocationInformation(uint dockedClientId, Vector pos, Matrix rot)
{
	// Подготовьте ссылки на пристыкованные корабли, копию положения и поворота корабля-носителя для манипуляций.
	Vector& carrierPos = mobiledockClients[dockedClientId].carrierPos;
	Matrix& carrierRot = mobiledockClients[dockedClientId].carrierRot;

	carrierPos = pos;
	carrierRot = rot;

	carrierPos.x += carrierRot.data[0][1] * set_iMobileDockOffset;
	carrierPos.y += carrierRot.data[1][1] * set_iMobileDockOffset;
	carrierPos.z += carrierRot.data[2][1] * set_iMobileDockOffset;
}

inline void UndockShip(uint iClientID)
{
	// Если корабль был пристыкован к кому-то, удалите его из списка пристыкованных кораблей.
	if (mobiledockClients[iClientID].mobileDocked)
	{
		uint carrierClientID = HkGetClientIdFromCharname(mobiledockClients[iClientID].wscDockedWithCharname);

		// Если оператор связи есть на сервере - сделайте это, если нет - что угодно. 
		// Плагин стирает все связанные данные клиента после отключения.
		if (carrierClientID != -1 && !mobiledockClients[iClientID].carrierDied)
		{
			wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
			mobiledockClients[carrierClientID].mapDockedShips.erase(charname);
			mobiledockClients[carrierClientID].iDockingModulesAvailable++;
		}
	}

	mobiledockClients.erase(iClientID);
	mapPendingDockingRequests.erase(iClientID);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Загрузите конфигурацию
void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	// Путь к файлу данных.
	char datapath[MAX_PATH];
	GetUserDataPath(datapath);

	// Создайте каталог, если он не существует
	string moddir = string(datapath) + R"(\Accts\MultiPlayer\docking_module\)";
	CreateDirectoryA(moddir.c_str(), 0);

	// Конфигурация плагина
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\dockingmodules.cfg";

	int dockingModAmount = 0;
	INI_Reader ini;
	if (ini.open(scPluginCfgFile.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("Config"))
			{
				while (ini.read_value())
				{
					if (ini.is_value("allowedmodule"))
					{
						dockingModuleEquipmentIds.push_back(CreateID(ini.get_value_string()));
						dockingModAmount++;
					}
					else if (ini.is_value("cargo_capacity_limit"))
					{
						cargoCapacityLimit = ini.get_value_int(0);
					}
				}
			}
		}
		ini.close();
	}

	ConPrint(L"DockingModules: Loaded %u equipment\n", dockingModAmount);
	ConPrint(L"DockingModules: Allowing ships below the cargo capacity of %i to dock\n", cargoCapacityLimit);
}

void __stdcall BaseExit(uint iBaseID, uint iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	mobiledockClients[iClientID].iDockingModulesInstalled = GetInstalledModules(iClientID);

	// Нормализовать доступные стыковочные модули с учетом количества людей, пристыкованных в данный момент.
	mobiledockClients[iClientID].iDockingModulesAvailable = (mobiledockClients[iClientID].iDockingModulesInstalled - mobiledockClients[iClientID].mapDockedShips.size());

	// Если это корабль, который в настоящее время пришвартован, очистите рынок.
	if (mobiledockClients[iClientID].mobileDocked)
	{
		SendResetMarketOverride(iClientID);
	}
}

// Временное хранилище данных клиента, которые будут обрабатываться в LaunchPosHook.
CLIENT_DATA undockingShip;

void __stdcall PlayerLaunch(unsigned int iShip, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	uint carrier_client = HkGetClientIdFromCharname(mobiledockClients[iClientID].wscDockedWithCharname);

	wstring clientName = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

	if (mobiledockClients[iClientID].mobileDocked)
	{
		returncode = SKIPPLUGINS;

		// Установить последнюю базу как последнюю реальную базу, на которой находился этот корабль. 
		// Поддержка POB будет добавлена в версии 0.9.X.
		Players[iClientID].iLastBaseID = mobiledockClients[iClientID].iLastBaseID;

		// Проверьте, умер ли носитель.
		if (mobiledockClients[iClientID].carrierDied)
		{
			// Если погиб авианосец и пристыкованный корабль находятся в одной системе.
			if (Players[iClientID].iSystemID == mobiledockClients[iClientID].carrierSystem)
			{
				// Теперь, когда данные подготовлены, отправьте игрока в локацию перевозчика.
				undockingShip = mobiledockClients[iClientID];

				// Удалите клиент из mobiledockClients теперь, когда он больше не закреплен.
				mobiledockClients.erase(iClientID);
			}
			// Если нет - перенаправить на базу прокси в системе операторов связи.
			else
			{
				wstring scProxyBase = HkGetSystemNickByID(mobiledockClients[iClientID].carrierSystem) + L"_proxy_base";
				if (pub::GetBaseID(mobiledockClients[iClientID].proxyBaseID, (wstos(scProxyBase)).c_str()) == -4)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0785"), scProxyBase);
					return;
				}

				mobiledockClients[iClientID].carrierSystem = -1;
			}
			return;
		}
		else if (carrier_client == -1)
		{
			// Если перевозчик по каким-то причинам не существует, заставить клиента состыковаться с последней известной базой
			uint iBaseID = mobiledockClients[iClientID].iLastBaseID;
			mobiledockClients[iClientID].undockBase = Universe::get_base(iBaseID);
			mobiledockClients[iClientID].baseUndock = true;
			return;
		}

		//Получить информацию о корабле-перевозчике
		uint carrierShip;
		pub::Player::GetShip(carrier_client, carrierShip);

		uint clientShip;
		pub::Player::GetShip(iClientID, clientShip);

		// Проверьте, находится ли оператор связи в данный момент на базе. 
		// Если да, заставьте клиента стыковаться с этой базой.
		if (!carrierShip)
		{
			uint iBaseID;
			pub::Player::GetBase(carrier_client, iBaseID);

			if (iBaseID)
			{
				// Установите флаги, которые PlayerLaunch_AFTER использует для телепортации на базу операторов связи.
				mobiledockClients[iClientID].undockBase = Universe::get_base(iBaseID);
				mobiledockClients[carrier_client].mapDockedShips.erase(clientName);
				mobiledockClients[iClientID].baseUndock = true;
				return;
			}
		}

		// Если авианосец и пристыкованный корабль находятся в одной системе.
		if (Players[iClientID].iSystemID == Players[carrier_client].iSystemID)
		{
			// Обновите внутренние значения пристыкованного корабля, относящегося к авианосцу.
			UpdateCarrierLocationInformation(iClientID, carrierShip);

			// Теперь, когда данные подготовлены, отправьте игрока в локацию перевозчика.
			undockingShip = mobiledockClients[iClientID];

			// Удалите клиент из mobiledockClients теперь, когда он больше не закреплен.
			mobiledockClients.erase(iClientID);

			// Удалить имя персонажа из карты перевозчиковDockedShips.
			mobiledockClients[carrier_client].mapDockedShips.erase(clientName);
			mobiledockClients[carrier_client].iDockingModulesAvailable++;
		}
		// Если нет - перенаправить на базу прокси в системе операторов связи.
		else
		{
			mobiledockClients[iClientID].carrierSystem = -1;
		}
	}
}

bool __stdcall LaunchPosHook(uint space_obj, struct CEqObj &p1, Vector &pos, Matrix &rot, int dock_mode)
{
	returncode = DEFAULT_RETURNCODE;

	// Перенаправьте корабль на позицию авианосца. Возможна ошибка в плагине POB, могут потребоваться изменения.
	if (undockingShip.proxyBaseID == space_obj)
	{
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		rot = undockingShip.carrierRot;
		pos = undockingShip.carrierPos;
		undockingShip.proxyBaseID = 0;
	}
	return true;
}

void __stdcall PlayerLaunch_AFTER(unsigned int ship, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// Помечен ли клиент для стыковки с базой после выхода?
	if (mobiledockClients[iClientID].baseUndock)
	{
		uint systemID = Players[iClientID].iSystemID;

		pub::Player::ForceLand(iClientID, mobiledockClients[iClientID].undockBase->iBaseID);

		if (mobiledockClients[iClientID].undockBase->iSystemID != systemID)
		{
			// Обновите текущую системную статистику в списке игроков, чтобы она отображалась соответствующим образом.
			Server.BaseEnter(mobiledockClients[iClientID].undockBase->iBaseID, iClientID);
			Server.BaseExit(mobiledockClients[iClientID].undockBase->iBaseID, iClientID);
			wstring wscCharFileName;
			HkGetCharFileName((const wchar_t*)Players.GetActiveCharacterName(iClientID), wscCharFileName);
			wscCharFileName += L".fl";
			CHARACTER_ID cID;
			strcpy(cID.szCharFilename, wstos(wscCharFileName.substr(0, 14)).c_str());
			Server.CharacterSelect(cID, iClientID);
		}

		mobiledockClients.erase(iClientID);
	}

	// Высадите корабль на прокси-базу в системе перевозчиков, если они находятся в разных системах.
	if (mobiledockClients[iClientID].carrierSystem == -1)
	{
		if (!mobiledockClients[iClientID].carrierDied)
		{
			string scProxyBase = HkGetPlayerSystemS(HkGetClientIdFromCharname(mobiledockClients[iClientID].wscDockedWithCharname)) + "_proxy_base";
			if (pub::GetBaseID(mobiledockClients[iClientID].proxyBaseID, scProxyBase.c_str()) == -4)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0786"), scProxyBase);
				return;
			}
		}

		pub::Player::ForceLand(iClientID, mobiledockClients[iClientID].proxyBaseID);

		// Отправьте сообщение, потому что если авианосец перейдет в другую систему, пристыкованные корабли останутся в предыдущей с устаревшей навигационной картой системы.
		// Мы уведомляем клиента о том, что он обновляется.
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0787"));

		// Обновите текущую системную статистику в списке игроков, чтобы она отображалась соответствующим образом.
		Server.BaseEnter(mobiledockClients[iClientID].proxyBaseID, iClientID);
		Server.BaseExit(mobiledockClients[iClientID].proxyBaseID, iClientID);
		wstring wscCharFileName;
		HkGetCharFileName((const wchar_t*)Players.GetActiveCharacterName(iClientID), wscCharFileName);
		wscCharFileName += L".fl";
		CHARACTER_ID cID;
		strcpy(cID.szCharFilename, wstos(wscCharFileName.substr(0, 14)).c_str());
		Server.CharacterSelect(cID, iClientID);

		// Обновить текущую системную статистику в данных плагина.
		Universe::IBase* base = Universe::get_base(mobiledockClients[iClientID].proxyBaseID);
		mobiledockClients[iClientID].carrierSystem = base->iSystemID;
	}
}

// Если это запрос на стыковку с кораблем игрока, обработайте его.
int __cdecl Dock_Call(unsigned int const &iShip, unsigned int const &iBaseID, int iCancel, enum DOCK_HOST_RESPONSE response)
{
	returncode = DEFAULT_RETURNCODE;

	UINT iClientID = HkGetClientIDByShip(iShip);
	if (iClientID)
	{
		// Если цели нет, игнорируйте запрос.
		uint iTargetShip;
		pub::SpaceObj::GetTarget(iShip, iTargetShip);
		if (!iTargetShip)
			return 0;

		uint iType;
		pub::SpaceObj::GetType(iTargetShip, iType);
		if (iType != OBJ_FREIGHTER)
			return 0;

		// Если целью не является корабль игрока или корабль находится слишком далеко, игнорируйте запрос.
		const uint iTargetClientID = HkGetClientIDByShip(iTargetShip);
		if (!iTargetClientID || HkDistance3DByShip(iShip, iTargetShip) > 1000.0f)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0788"));
			return 0;
		}

		// Убедитесь, что у целевого корабля есть пустой стыковочный модуль.
		if (mobiledockClients[iTargetClientID].iDockingModulesAvailable == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0789"));
			return 0;
		}

		// Убедитесь, что запрашивающее судно имеет подходящий размер для стыковки.
		CShip* cship = dynamic_cast<CShip*>(HkGetEqObjFromObjRW(reinterpret_cast<IObjRW*>(HkGetInspect(iClientID))));
		if (cship->shiparch()->fHoldSize > cargoCapacityLimit)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0790"));
			return 0;
		}

		returncode = SKIPPLUGINS_NOFUNCTIONCALL;

		// Создайте запрос на стыковку и отправьте уведомление целевому кораблю.
		mapPendingDockingRequests[iClientID] = iTargetClientID;
		PrintUserCmdText(iTargetClientID, GetLocalized(iClientID, "MSG_0791"), Players.GetActiveCharacterName(iClientID));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0792"), Players.GetActiveCharacterName(iTargetClientID));
		return -1;
	}
	return 0;
}

void __stdcall BaseEnter(uint iBaseID, uint iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	if (mobiledockClients[iClientID].mobileDocked)
	{
		// Очистите рынок. Мы не поддерживаем переводы в этой версии.
		SendResetMarketOverride(iClientID);

		// Установите базовое имя
		wstring status = L"<RDL><PUSH/>";
		status += L"<TEXT>" + XMLText(mobiledockClients[iClientID].wscDockedWithCharname) + L"</TEXT><PARA/><PARA/>";
		status += L"<POP/></RDL>";
		SendSetBaseInfoText2(iClientID, status);
	}
}

void __stdcall ShipDestroyed(DamageList *_dmg, DWORD *ecx, uint kill)
{
	returncode = DEFAULT_RETURNCODE;

	CShip *cship = (CShip*)ecx[4];
	uint iClientID = cship->GetOwnerPlayer();
	if (kill)
	{
		if (iClientID)
		{
			// Если это авианосец, то сбросьте все пристыкованные корабли в космос.
			if (!mobiledockClients[iClientID].mapDockedShips.empty())
			{
				// Отправьте системный переключатель, чтобы заставить каждый корабль спуститься на воду.
				for (map<wstring, wstring>::iterator i = mobiledockClients[iClientID].mapDockedShips.begin();
					i != mobiledockClients[iClientID].mapDockedShips.end(); ++i)
				{
					uint iDockedClientID = HkGetClientIdFromCharname(i->second);

					// Обновите координаты, куда должен отправиться данный корабль.
					UpdateCarrierLocationInformation(iDockedClientID, cship->get_position(), cship->get_orientation());

					// Перевозчика больше нет. Установите флаг.
					mobiledockClients[iDockedClientID].carrierDied = true;

					// Поскольку оператор связи больше не существует, нам приходится извлечь системную информацию из исторического местоположения оператора связи.
					mobiledockClients[iDockedClientID].carrierSystem = cship->iSystem;

					JettisonShip(iClientID, iDockedClientID);
				}

				// Удалить оператора связи из списка
				mobiledockClients[iClientID].mapDockedShips.clear();
				mobiledockClients[iClientID].iDockingModulesAvailable = mobiledockClients[iClientID].iDockingModulesInstalled;
			}
		}
	}
}

bool UserCmd_Process(uint iClientID, const wstring &wscCmd)
{
	returncode = DEFAULT_RETURNCODE;

	if (wscCmd.find(L"/listdocked") == 0)
	{
		if (mobiledockClients[iClientID].mapDockedShips.empty())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0793"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0794"));
			for (map<wstring, wstring>::iterator i = mobiledockClients[iClientID].mapDockedShips.begin();
				i != mobiledockClients[iClientID].mapDockedShips.end(); ++i)
			{
				PrintUserCmdText(iClientID, i->first);
			}
		}
		return true;
	}
	else if (wscCmd.find(L"/conn") == 0 || wscCmd.find(L"/return") == 0)
	{
		// Этот плагин всегда запускается до того, как плагин Conn запустит свою функцию /conn.. 
		// Убедитесь, что нет пристыкованных кораблей..
		if (!mobiledockClients[iClientID].mapDockedShips.empty())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0795"));
			returncode = SKIPPLUGINS;
			return true;
		}
	}
	else if (wscCmd.find(L"/jettisonship") == 0)
	{
		// Получите предполагаемый корабль, который мы должны катапультировать, из параметров команды.
		wstring charname = Trim(GetParam(wscCmd, ' ', 1));
		if (charname.empty())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0796"));
			return true;
		}

		// Разрешить выбрасывать корабль за борт только в том случае, если авианосец отстыкован.
		uint carrierShip;
		pub::Player::GetShip(iClientID, carrierShip);
		if (!carrierShip)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0797"));
			return true;
		}

		// Проверьте, действительно ли указанный пользователь в данный момент состыкован с оператором связи.
		if (mobiledockClients[iClientID].mapDockedShips.find(charname) == mobiledockClients[iClientID].mapDockedShips.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0798"), charname);
			return true;
		}

		// Игрок существует. Удалите его из списка закрепленных и выкиньте в космос.
		const uint iDockedClientID = HkGetClientIdFromCharname(charname);
		if (iDockedClientID != -1)
		{
			// Обновите клиент, указав текущее местоположение оператора связи.
			UpdateCarrierLocationInformation(iDockedClientID, carrierShip);

			// Заставьте пристыкованный корабль спуститься на воду. Координаты телепорта заданы предыдущим способом.
			JettisonShip(iClientID, iDockedClientID);
		}

		return true;
	}
	else if (wscCmd.find(L"/allowdock") == 0)
	{
		//Если были не в космосе, то проигнорируйте просьбу
		uint iShip;
		pub::Player::GetShip(iClientID, iShip);
		if (!iShip)
			return true;

		//Если в данный момент целью не является корабль, игнорируйте запрос.
		uint iTargetShip;
		pub::SpaceObj::GetTarget(iShip, iTargetShip);
		if (!iTargetShip)
			return true;

		// Если цель не является кораблем игрока или корабль находится слишком далеко, игнорируйте
		const uint iTargetClientID = HkGetClientIDByShip(iTargetShip);
		if (!iTargetClientID || HkDistance3DByShip(iShip, iTargetShip) > 1000.0f)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0788"));
			return true;
		}

		// Find the docking request. If none, ignore.
		if (mapPendingDockingRequests.find(iTargetClientID) == mapPendingDockingRequests.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0800"));
			return true;
		}

		// Check that there is an empty docking module
		if (mobiledockClients[iClientID].iDockingModulesAvailable <= 0)
		{
			mapPendingDockingRequests.erase(iTargetClientID);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0801"));
			return true;
		}

		// The iClientID is free to dock, erase from the pending list and handle
		mapPendingDockingRequests.erase(iTargetClientID);

		string scProxyBase = HkGetPlayerSystemS(iClientID) + "_proxy_base";
		uint iBaseID;
		if (pub::GetBaseID(iBaseID, scProxyBase.c_str()) == -4)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0802"), scProxyBase);
			return true;
		}

		// Save the carrier info
		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iTargetClientID);
		mobiledockClients[iClientID].mapDockedShips[charname] = charname;
		pub::SpaceObj::GetSystem(iShip, mobiledockClients[iClientID].carrierSystem);
		if (mobiledockClients[iClientID].iLastBaseID != 0)
			mobiledockClients[iClientID].iLastBaseID = Players[iClientID].iLastBaseID;

		// Save the docking ship info
		mobiledockClients[iTargetClientID].mobileDocked = true;
		mobiledockClients[iTargetClientID].wscDockedWithCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		mobiledockClients[iTargetClientID].iLastBaseID = Players[iTargetClientID].iLastBaseID;
		mobiledockClients[iTargetClientID].proxyBaseID = iBaseID;
		pub::SpaceObj::GetSystem(iShip, mobiledockClients[iTargetClientID].carrierSystem);

		mobiledockClients[iClientID].iDockingModulesAvailable--;

		// Land the ship on the proxy base
		pub::Player::ForceLand(iTargetClientID, iBaseID);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0803"));

		return true;
	}
	return false;
}

void __stdcall DisConnect(uint iClientID, enum EFLConnection p2)
{
	returncode = DEFAULT_RETURNCODE;

	UndockShip(iClientID);
}

void __stdcall CharacterSelect_AFTER(struct CHARACTER_ID const & cId, unsigned int iClientID)
{
	returncode = DEFAULT_RETURNCODE;

	// Erase all plugin info associated with the iClientID in case if the person has switched characters to prevent any bugs.
	UndockShip(iClientID);

	// Update count of installed modules in case if iClientID left his ship in open space before.
	mobiledockClients[iClientID].iDockingModulesAvailable = mobiledockClients[iClientID].iDockingModulesInstalled = GetInstalledModules(iClientID);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	srand((uint)time(0));
	// If we're being loaded from the command line while FLHook is running then
	// set_scCfgFile will not be empty so load the settings as FLHook only
	// calls load settings on FLHook startup and .rehash.
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (set_scCfgFile.length() > 0)
			LoadSettings();
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
	}
	return true;
}

#define IS_CMD(a) !wscCmd.compare(L##a)

bool ExecuteCommandString_Callback(CCmds* classptr, const wstring &wscCmd)
{
	returncode = DEFAULT_RETURNCODE;

	if (IS_CMD("logactivity"))
	{
		// Log current time in file name.
		std::time_t rawtime;
		std::tm* timeinfo;
		char buffer[80];

		std::time(&rawtime);
		timeinfo = std::localtime(&rawtime);

		std::strftime(buffer, 80, "[%Y-%m-%d]%H-%M-%S", timeinfo);

		string path = "./flhook_logs/logactivity " + string(buffer) + ".log";

		// Lines list to add in new file.
		vector<string> Lines;

		Lines.push_back("DOCK: mobiledockClients | Size = " + to_string(mobiledockClients.size()));

		vector<wstring> DOCKcharnames;
		for (map<uint, CLIENT_DATA>::iterator it = mobiledockClients.begin(); it != mobiledockClients.end(); ++it)
		{
			wstring charname;

			try
			{
				charname = (const wchar_t*)Players.GetActiveCharacterName(it->first);
			}
			catch (...)
			{
				charname = L"<Error>";
			}

			string ID = to_string(it->first);
			string Charname = wstos(charname);
			string Type = it->second.iDockingModulesInstalled == 0 ? "Docked" : "Carrier";
			if (Type == "Carrier")
			{
				wstring docked = L"";
				for (map<wstring, wstring>::iterator cit = it->second.mapDockedShips.begin(); cit != it->second.mapDockedShips.end(); cit++)
				{
					if (docked != L"")
						docked += L" | ";
					docked += cit->first;
				}

				Type += "[" + to_string(it->second.iDockingModulesInstalled) + "](" + wstos(docked) + ")";
			}
			else
			{

				if (it->second.wscDockedWithCharname.empty())
				{
					if (!it->second.mobileDocked)
						continue;
					Type += "[" + wstos(L"<Error>") + "]";
				}
				else
					Type += "[" + wstos(it->second.wscDockedWithCharname) + "]";
			}
			string State = "";

			if (it->first == 0 || it->first > MAX_CLIENT_ID)
				State += "Out of range";

			if (find(DOCKcharnames.begin(), DOCKcharnames.end(), charname) != DOCKcharnames.end())
			{
				if (State != "")
					State += " | ";
				State += "Doubled";
			}

			if (State == "")
				State = "Fine";

			if (charname != L"<Error>")
				DOCKcharnames.push_back(charname);

			Lines.push_back(ID + " " + Charname + " " + Type + " " + State);
		}

		Lines.push_back("");


		vector<string> SERVERlines;

		vector<wstring> SERVERcharnames;
		vector<uint> IDs;
		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint clientID;
			try
			{
				clientID = HkGetClientIdFromPD(pPD);
			}
			catch (...)
			{
				clientID = 0;
			}

			string ID;

			if (clientID == 0)
				ID = "<Error>";
			else
				ID = to_string(clientID);

			wstring charname;
			wstring wscIP;

			try
			{
				charname = (const wchar_t*)Players.GetActiveCharacterName(clientID);
			}
			catch (...)
			{
				charname = L"<NotLogged>";
			}

			try
			{
				HkGetPlayerIP(clientID, wscIP);
			}
			catch (...)
			{
				wscIP = L"<Error>";
			}

			string Charname = wstos(charname);
			string State = "";

			if (clientID > MAX_CLIENT_ID)
				State += "Out of range";


			if (find(SERVERcharnames.begin(), SERVERcharnames.end(), charname) != SERVERcharnames.end())
			{
				if (State != "")
					State += " | ";
				State += "DoubledName";
			}

			if (find(IDs.begin(), IDs.end(), clientID) != IDs.end())
			{
				if (State != "")
					State += " | ";
				State += "DoubledID";
			}

			if (State == "")
				State = "Fine";

			if (charname != L"<NotLogged>")
				SERVERcharnames.push_back(charname);
			IDs.push_back(clientID);

			SERVERlines.push_back(ID + " " + Charname + " " + wstos(wscIP) + " " + State);
		}

		Lines.push_back("SERVER: PlayersDB | Size = " + to_string(SERVERlines.size()));
		Lines.insert(Lines.end(), SERVERlines.begin(), SERVERlines.end());

		// Create new file.
		FILE *newfile = fopen(path.c_str(), "w");
		if (newfile)
		{
			for (vector<string>::iterator it = Lines.begin(); it != Lines.end(); ++it)
			{
				fprintf(newfile, (*it + "\n").c_str());
			}

			fclose(newfile);
		}

		ConPrint(L"Saved to: " + stows(path) + L"\n");
		returncode = SKIPPLUGINS_NOFUNCTIONCALL;
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "Mobile Docking Plugin";
	p_PI->sShortName = "dock";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ExecuteCommandString_Callback, PLUGIN_ExecuteCommandString_Callback, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&PlayerLaunch_AFTER, PLUGIN_HkIServerImpl_PlayerLaunch_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CharacterSelect_AFTER, PLUGIN_HkIServerImpl_CharacterSelect_AFTER, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ShipDestroyed, PLUGIN_ShipDestroyed, 0));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkTimerCheckKick, PLUGIN_HkTimerCheckKick, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmd_Process, PLUGIN_UserCmd_Process, 3));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Dock_Call, PLUGIN_HkCb_Dock_Call, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseEnter, PLUGIN_HkIServerImpl_BaseEnter, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&BaseExit, PLUGIN_HkIServerImpl_BaseExit, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&PlayerLaunch, PLUGIN_HkIServerImpl_PlayerLaunch, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LaunchPosHook, PLUGIN_LaunchPosHook, -1));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&DisConnect, PLUGIN_HkIServerImpl_DisConnect, 0));

	return p_PI;
}
