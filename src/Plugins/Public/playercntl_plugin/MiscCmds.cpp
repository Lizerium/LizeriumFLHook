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
#include <boost\regex.hpp>

#include <PluginUtilities.h>
#include "Main.h"

#include <FLCoreServer.h>
#include <FLCoreCommon.h>

namespace MiscCmds
{
	struct INFO
	{
		INFO() : bLightsOn(false), bShieldsDown(false), bSelfDestruct(false)
		{}

		/// Маяки on/off
		bool bLightsOn;

		/// Щиты up/down
		bool bShieldsDown;

		/// Самоуничтожение
		bool bSelfDestruct;
	};

	/** Список перечислений для каждой математической операции, используемой для команды dice */
	enum diceOperation
	{
		ADD,
		SUBTRACT,
		NONE
	};

	/** Список клиентов, которых атакуют */
	map<uint, INFO> mapInfo;
	typedef map<uint, INFO, less<uint> >::value_type mapInfo_map_pair_t;
	typedef map<uint, INFO, less<uint> >::iterator mapInfo_map_iter_t;

	wstring set_wscStuckMsg = L"ВНИМАНИЕ! Держитесь подальше. Буксировка игрока %player";
	wstring set_wscDiceMsg = L"%player rolled %number";
	wstring set_wscCoinMsg = L"%player tossed %result";

	/// ID музыки, которая будет воспроизводиться при нанесении удара игрокам.
	uint set_iSmiteMusicID = 0;

	/// Стоимость изменения репутации.
	int set_iRepdropCost = 0;

	/// Диапазон локального чата
	float set_iLocalChatRange = 9999;

	/// Load the configuration
	void MiscCmds::LoadSettings(const string &scPluginCfgFile)
	{
		// Load generic settings
		set_iRepdropCost = IniGetI(scPluginCfgFile, "General", "RepDropCost", 0);
		set_iLocalChatRange = IniGetF(scPluginCfgFile, "General", "LocalChatRange", 0);

		set_wscStuckMsg = stows(IniGetS(scPluginCfgFile, "General", "StuckMsg", "Внимание! Стойте. Буксирует %player"));
		set_wscDiceMsg = stows(IniGetS(scPluginCfgFile, "General", "DiceMsg", "%player разыграл %number of %max"));
		set_wscCoinMsg = stows(IniGetS(scPluginCfgFile, "General", "CoinMsg", "%player бросил %result"));

		set_iSmiteMusicID = CreateID(IniGetS(scPluginCfgFile, "General", "SmiteMusic", "music_danger").c_str());
	}

	/** Очистка при отключении клиента */
	void MiscCmds::ClearClientInfo(uint iClientID)
	{
		if (mapInfo[iClientID].bSelfDestruct)
		{
			mapInfo[iClientID].bSelfDestruct = false;
			uint dummy[3] = { 0 };
			pub::Player::SetShipAndLoadout(iClientID, CreateID("ge_fighter"), (const EquipDescVector&)dummy);
		}
		mapInfo.erase(iClientID);
	}

	void MiscCmds::CharacterInfoReq(unsigned int iClientID, bool p2)
	{
		if (mapInfo[iClientID].bSelfDestruct)
		{
			mapInfo[iClientID].bSelfDestruct = false;
			uint dummy[3] = { 0 };
			pub::Player::SetShipAndLoadout(iClientID, CreateID("ge_fighter"), (const EquipDescVector&)dummy);
		}
	}

	/** Один второй таймер */
	void MiscCmds::Timer()
	{
		// Бросайте щиты игроков и удерживайте их внизу.
		for (mapInfo_map_iter_t iter = mapInfo.begin(); iter != mapInfo.end(); iter++)
		{
			if (iter->second.bShieldsDown)
			{
				HKPLAYERINFO p;
				if (HkGetPlayerInfo((const wchar_t*)Players.GetActiveCharacterName(iter->first), p, false) == HKE_OK && p.iShip)
				{
					pub::SpaceObj::DrainShields(p.iShip);
				}
			}
		}
	}

	/** Печать текущей позиции */
	bool MiscCmds::UserCmd_Pos(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		HKPLAYERINFO p;
		if (HkGetPlayerInfo((const wchar_t*)Players.GetActiveCharacterName(iClientID), p, false) != HKE_OK || p.iShip == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1061"));
			return true;
		}

		Vector pos;
		Matrix rot;
		pub::SpaceObj::GetLocation(p.iShip, pos, rot);

		Vector erot = MatrixToEuler(rot);

		wchar_t buf[100];
		_snwprintf(buf, sizeof(buf), GetLocalized(iClientID, "MSG_1574").c_str(),
			pos.x, pos.y, pos.z, erot.x, erot.y, erot.z);
		PrintUserCmdText(iClientID, buf);
		return true;
	}

	/** Немного переместите корабль, если он застрял в базе */
	bool MiscCmds::UserCmd_Stuck(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		float fTradeLaneRingRadiusSafeDistance = 245.0f;
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		HKPLAYERINFO p;
		if (HkGetPlayerInfo(wscCharname, p, false) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1061"));
			return true;
		}

		Vector dir1;
		Vector dir2;
		pub::SpaceObj::GetMotion(p.iShip, dir1, dir2);
		if (dir1.x > 5 || dir1.y > 5 || dir1.z > 5)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1063"));
			return true;
		}

		Vector pos;
		Matrix rot;
		pub::SpaceObj::GetLocation(p.iShip, pos, rot);

		// Попробуйте разобраться, застряли ли вы в полосе движения или просто застряли вообще
		uint iTarget, iTargetType;
		Vector tpos;
		Matrix trot;
		pub::SpaceObj::GetTarget(p.iShip, iTarget);
		if (iTarget)
		{
			pub::SpaceObj::GetType(iTarget, iTargetType);
			if (iTargetType == OBJ_TRADELANE_RING)
			{
				pub::SpaceObj::GetHardpoint(iTarget, "HpLeftLane", &tpos, &trot);
				if (HkDistance3D(pos, tpos) < fTradeLaneRingRadiusSafeDistance)
					pos = tpos;
				else
				{
					pub::SpaceObj::GetHardpoint(iTarget, "HpRightLane", &tpos, &trot);
					if (HkDistance3D(pos, tpos) < fTradeLaneRingRadiusSafeDistance)
						pos = tpos;
				}
			}
		}
		else
		{
			pos.x += 20;
			pos.y += 20;
			pos.z += 20;
		}
		HkRelocateClient(iClientID, pos, rot);

		wstring wscMsg = set_wscStuckMsg;
		wscMsg = ReplaceStr(wscMsg, L"%player", wscCharname);
		PrintLocalUserCmdText(iClientID, wscMsg, set_iLocalChatRange);

		return true;
	}

	/** Команда, помогающая удалить любую принадлежность к фракции, которая может у вас есть */
	bool MiscCmds::UserCmd_DropRep(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		HK_ERROR err;

		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		wstring wscRepGroupNick;
		if (HkFLIniGet(wscCharname, L"rep_group", wscRepGroupNick) != HKE_OK || wscRepGroupNick.length() == 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1064"));
			return true;
		}

		// Читайте текущее количество кредитов для игрока и проверяйте, достаточно ли у персонажа денежных средств.
		int iCash = 0;
		if ((err = HkGetCash(wscCharname, iCash)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1065"), HkErrGetText(err).c_str());
			return true;
		}
		if (set_iRepdropCost > 0 && iCash < set_iRepdropCost)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1066"));
			return true;
		}

		float fValue = 0.0f;
		if ((err = HkGetRep(wscCharname, wscRepGroupNick, fValue)) != HKE_OK)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1065"), HkErrGetText(err).c_str());
			return true;
		}

		HkSetRep(wscCharname, wscRepGroupNick, 0.599f);
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1068"));

		// Снимите наличные, если взимали за них плату.
		if (set_iRepdropCost > 0)
		{
			HkAddCash(wscCharname, 0 - set_iRepdropCost);
		}

		return true;
	}

	/*
		Снимите наличные, если взимали за них плату. Поддерживает форматирование 1d20.
	*/
	bool MiscCmds::UserCmd_Dice(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		boost::wregex expr(L"(\\d{1,2})[Dd](\\d{1,3})(([+\\-*])?(\\d{1,5}))?");
		boost::wsmatch sm;

		// Если регулярное выражение находит совпадение, 
		// обозначающее правильный формат броска, запускают случайные числа
		if (boost::regex_match(wscParam, sm, expr))
		{
			// Индекс Smatch [1] представляет собой количество бросков
			int rollCount = _wtoi(sm[1].str().c_str());

			// Индекс Smatch [2] представляет собой количество кубиков
			int diceCount = _wtoi(sm[2].str().c_str());

			// Индекс Smatch [3] представляет собой любое числовое значение модификатора. 
			// Это устанавливается ТОЛЬКО в том случае, если мы используем mod-операцию
			int modifierValue;

			diceOperation operation;
			if (sm[3].str().find(L"+") == 0)
			{
				operation = diceOperation::ADD;
				modifierValue = _wtoi(sm[5].str().c_str());
			}
			else if (sm[3].str().find(L"-") == 0)
			{
				operation = diceOperation::SUBTRACT;
				modifierValue = _wtoi(sm[5].str().c_str());
			}
			else
			{
				operation = diceOperation::NONE;
			}

			string diceResultSteps = "";
			uint number = 0;

			for (int i = 0; i < rollCount; i++)
			{
				int randValue = (rand() % diceCount) + 1;

				// Если у нас есть модификатор, примените его
				if (operation == diceOperation::ADD)
				{
					number += (randValue + modifierValue);
					diceResultSteps.append("(").append(itos(randValue)).append(" + ").append(itos(modifierValue).append(")"));
				}
				else if (operation == diceOperation::SUBTRACT)
				{
					number += (randValue - modifierValue);
					diceResultSteps.append("(").append(itos(randValue)).append(" - ").append(itos(modifierValue).append(")"));
				}
				else
				{
					number += randValue;
					diceResultSteps.append("(").append(itos(randValue)).append(")");
				}

				// Разве мы не на последнем значении? Сохраните веревочку красивой, добавив еще одну +
				if (i < rollCount - 1)
				{
					diceResultSteps.append(" + ");
				}
			}

			wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

			// Распечатать результаты
			wstring diceAlert = GetLocalized(iClientID, "MSG_1575");
			diceAlert = ReplaceStr(diceAlert, L"%player", wscCharname);
			diceAlert = ReplaceStr(diceAlert, L"%value", stows(itos(number)));
			diceAlert = ReplaceStr(diceAlert, L"%formula", sm[0].str().c_str());

			PrintLocalUserCmdText(iClientID, diceAlert, set_iLocalChatRange);

			// Выводите выполненные шаги только в том случае, если было брошено менее 10 кубиков.
			if (rollCount < 10)
			{
				PrintLocalUserCmdText(iClientID, stows(diceResultSteps), set_iLocalChatRange);
			}
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1069"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1070"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1071"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1072"));
		}
		return true;
	}

	/** Бросайте кости и расскажите всем игрокам в радиусе 6 км */
	bool MiscCmds::UserCmd_Coin(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		uint number = (rand() % 2);
		wstring wscMsg = set_wscCoinMsg;
		wscMsg = ReplaceStr(wscMsg, L"%player", wscCharname);
		wscMsg = ReplaceStr(wscMsg, L"%result", (number == 1) ? L"heads" : L"tails");
		PrintLocalUserCmdText(iClientID, wscMsg, set_iLocalChatRange);
		return true;
	}

	/** Поразите всех игроков в радиусе действия радаров */
	void MiscCmds::AdminCmd_SmiteAll(CCmds* cmds)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return;
		}

		bool bKillAll = cmds->ArgStr(1) == L"die";

		Vector vFromShipLoc;
		Matrix mFromShipDir;
		pub::SpaceObj::GetLocation(adminPlyr.iShip, vFromShipLoc, mFromShipDir);

		pub::Audio::Tryptich music;
		music.iDunno = 0;
		music.iDunno2 = 0;
		music.iDunno3 = 0;
		music.iMusicID = set_iSmiteMusicID;
		pub::Audio::SetMusic(adminPlyr.iClientID, music);

		// Для всех игроков в системе...
		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			// Узнайте текущую систему игрока и его местоположение в системе.
			uint iClientID = HkGetClientIdFromPD(pPD);
			if (iClientID == adminPlyr.iClientID)
				continue;

			uint iClientSystem = 0;
			pub::Player::GetSystem(iClientID, iClientSystem);
			if (adminPlyr.iSystem != iClientSystem)
				continue;

			uint iShip;
			pub::Player::GetShip(iClientID, iShip);

			Vector vShipLoc;
			Matrix mShipDir;
			pub::SpaceObj::GetLocation(iShip, vShipLoc, mShipDir);

			// Находится ли игрок в пределах диапазона сканера (15K) от отправляющего символа.
			float fDistance = HkDistance3D(vShipLoc, vFromShipLoc);
			if (fDistance > 14999)
				continue;

			pub::Audio::Tryptich music;
			music.iDunno = 0;
			music.iDunno2 = 0;
			music.iDunno3 = 0;
			music.iMusicID = set_iSmiteMusicID;
			pub::Audio::SetMusic(iClientID, music);

			mapInfo[iClientID].bShieldsDown = true;

			if (bKillAll)
			{
				IObjInspectImpl *obj = HkGetInspect(iClientID);
				if (obj)
				{
					HkLightFuse((IObjRW*)obj, CreateID("death_comm"), 0.0f, 0.0f, 0.0f);
				}
			}
		}

		cmds->Print(L"OK\n");
		return;
	}

	/** Боб Команд */
	void MiscCmds::AdminCmd_Bob(CCmds* cmds, const wstring &wscCharname)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		HKPLAYERINFO targetPlyr;
		if (HkGetPlayerInfo(wscCharname, targetPlyr, false) != HKE_OK)
		{
			cmds->Print(L"ERR Player not found\n");
			return;
		}

		pub::Player::SetMonkey(targetPlyr.iClientID);

		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			uint iClientsID = HkGetClientIdFromPD(pPD);

			wstring wscMsg = GetLocalized(iClientsID, "MSG_1576");
			wscMsg = ReplaceStr(wscMsg, L"%p", wscCharname);
			HkFMsg(iClientsID, wscMsg);
		}
		return;
	}

	static void SetLights(uint iClientID, bool bOn)
	{
		uint iShip;
		pub::Player::GetShip(iClientID, iShip);
		if (!iShip)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1061"));
			return;
		}

		bool bLights = false;
		list<EquipDesc> &eqLst = Players[iClientID].equipDescList.equip;
		for (list<EquipDesc>::iterator eq = eqLst.begin(); eq != eqLst.end(); eq++)
		{
			string hp = ToLower(eq->szHardPoint.value);
			if (hp.find("dock") != string::npos)
			{
				XActivateEquip ActivateEq;
				ActivateEq.bActivate = bOn;
				ActivateEq.iSpaceID = iShip;
				ActivateEq.sID = eq->sID;
				Server.ActivateEquip(iClientID, ActivateEq);
				bLights = true;
			}
		}

		if (bLights)
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1074"), !bOn ? GetLocalized(iClientID, "MSG_1578") 
				: GetLocalized(iClientID, "MSG_1579"));
		else
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1075"));
	}

	bool MiscCmds::UserCmd_Lights(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		mapInfo[iClientID].bLightsOn = !mapInfo[iClientID].bLightsOn;
		SetLights(iClientID, mapInfo[iClientID].bLightsOn);
		return true;
	}

	void MiscCmds::BaseEnter(unsigned int iBaseID, unsigned int iClientID)
	{
	}

	bool MiscCmds::UserCmd_SelfDestruct(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1076"));

		/*IObjInspectImpl *obj = HkGetInspect(iClientID);
		if (!obj)
		{*/
			//PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1077"));
		/*	return true;
		}

		if (wscParam == L"0000")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1078"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1079"));
			HkLightFuse((IObjRW*)obj, CreateID("death_comm"), 0.0f, 0.0f, 0.0f);
			mapInfo[iClientID].bSelfDestruct = true;
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1080"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1081"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1082"));
		}*/
		return true;
	}

	bool MiscCmds::UserCmd_Screenshot(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		Message::UserCmd_Time(iClientID, L"", L"", L"");
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1083"));
		return true;
	}

	bool MiscCmds::UserCmd_Shields(uint iClientID, const wstring &wscCmd, const wstring &wscParam, const wchar_t *usage)
	{
		mapInfo[iClientID].bShieldsDown = !mapInfo[iClientID].bShieldsDown;
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_1084"), mapInfo[iClientID].bShieldsDown ?
			GetLocalized(iClientID, "MSG_1579")
			: GetLocalized(iClientID, "MSG_1578"));
		return true;
	}

	void AdminCmd_PlayMusic(CCmds* cmds, const wstring &wscMusicname)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		uint MusictoID = CreateID(wstos(wscMusicname).c_str());

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return;
		}

		Vector vFromShipLoc;
		Matrix mFromShipDir;
		pub::SpaceObj::GetLocation(adminPlyr.iShip, vFromShipLoc, mFromShipDir);

		pub::Audio::Tryptich music;
		music.iDunno = 0;
		music.iDunno2 = 0;
		music.iDunno3 = 0;
		music.iMusicID = MusictoID;
		pub::Audio::SetMusic(adminPlyr.iClientID, music);

		// Для всех игроков в системе...
		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			// Узнайте текущую систему игрока и его местоположение в системе.
			uint iClientID = HkGetClientIdFromPD(pPD);
			if (iClientID == adminPlyr.iClientID)
				continue;

			uint iClientSystem = 0;
			pub::Player::GetSystem(iClientID, iClientSystem);
			if (adminPlyr.iSystem != iClientSystem)
				continue;

			uint iShip;
			pub::Player::GetShip(iClientID, iShip);

			Vector vShipLoc;
			Matrix mShipDir;
			pub::SpaceObj::GetLocation(iShip, vShipLoc, mShipDir);

			// Является ли игрок в пределах (50K) от отправляющего символа.
			float fDistance = HkDistance3D(vShipLoc, vFromShipLoc);
			if (fDistance > 50000)
				continue;

			pub::Audio::Tryptich music;
			music.iDunno = 0;
			music.iDunno2 = 0;
			music.iDunno3 = 0;
			music.iMusicID = MusictoID;
			pub::Audio::SetMusic(iClientID, music);
		}

		cmds->Print(L"OK\n");
		return;
	}

	void AdminCmd_PlaySound(CCmds* cmds, const wstring &wscSoundname)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		uint MusictoID = CreateID(wstos(wscSoundname).c_str());

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return;
		}

		Vector vFromShipLoc;
		Matrix mFromShipDir;
		pub::SpaceObj::GetLocation(adminPlyr.iShip, vFromShipLoc, mFromShipDir);

		pub::Audio::PlaySoundEffect(adminPlyr.iClientID, MusictoID);

		// Для всех игроков в системе...
		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			// Узнайте текущую систему игрока и его местоположение в системе.
			uint iClientID = HkGetClientIdFromPD(pPD);
			if (iClientID == adminPlyr.iClientID)
				continue;

			uint iClientSystem = 0;
			pub::Player::GetSystem(iClientID, iClientSystem);
			if (adminPlyr.iSystem != iClientSystem)
				continue;

			uint iShip;
			pub::Player::GetShip(iClientID, iShip);

			Vector vShipLoc;
			Matrix mShipDir;
			pub::SpaceObj::GetLocation(iShip, vShipLoc, mShipDir);

			// Является ли игрок в пределах (50K) от отправляющего символа.
			float fDistance = HkDistance3D(vShipLoc, vFromShipLoc);
			if (fDistance > 50000)
				continue;

			pub::Audio::PlaySoundEffect(iClientID, MusictoID);
		}

		cmds->Print(L"OK\n");
		return;
	}

	void AdminCmd_PlayNNM(CCmds* cmds, const wstring &wscSoundname)
	{
		if (cmds->rights != RIGHT_SUPERADMIN)
		{
			cmds->Print(L"ERR No permission\n");
			return;
		}

		string MusictoID = wstos(wscSoundname);

		HKPLAYERINFO adminPlyr;
		if (HkGetPlayerInfo(cmds->GetAdminName(), adminPlyr, false) != HKE_OK || adminPlyr.iShip == 0)
		{
			cmds->Print(L"ERR Not in space\n");
			return;
		}

		Vector vFromShipLoc;
		Matrix mFromShipDir;
		pub::SpaceObj::GetLocation(adminPlyr.iShip, vFromShipLoc, mFromShipDir);

		pub::Player::SendNNMessage(adminPlyr.iClientID, pub::GetNicknameId(MusictoID.c_str()));

		// Для всех игроков в системе...
		struct PlayerData *pPD = 0;
		while (pPD = Players.traverse_active(pPD))
		{
			// Узнайте текущую систему игрока и его местоположение в системе.
			uint iClientID = HkGetClientIdFromPD(pPD);
			if (iClientID == adminPlyr.iClientID)
				continue;

			uint iClientSystem = 0;
			pub::Player::GetSystem(iClientID, iClientSystem);
			if (adminPlyr.iSystem != iClientSystem)
				continue;

			uint iShip;
			pub::Player::GetShip(iClientID, iShip);

			Vector vShipLoc;
			Matrix mShipDir;
			pub::SpaceObj::GetLocation(iShip, vShipLoc, mShipDir);

			// Является ли игрок в пределах (50K) от отправляющего символа.
			float fDistance = HkDistance3D(vShipLoc, vFromShipLoc);
			if (fDistance > 50000)
				continue;

			pub::Player::SendNNMessage(iClientID, pub::GetNicknameId(MusictoID.c_str()));
		}

		cmds->Print(L"OK\n");
		return;
	}
}