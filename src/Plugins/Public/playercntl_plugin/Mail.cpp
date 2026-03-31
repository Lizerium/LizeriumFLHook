/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 31 марта 2026 10:53:40
 * Version: 1.0.12
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
#include <FLHook.h>
#include <plugin.h>
#include <PluginUtilities.h>
#include <math.h>
#include <list>

#include "Main.h"
#include "Mail.h"

bool extern set_bLocalTime;

namespace Mail
{
	static const int MAX_MAIL_MSGS = 40;

	/** Показать пять сообщений с указанной стартовой позиции. */
	void MailShow(const wstring &wscCharname, const string &scExtension, int iFirstMsg)
	{
		// Убедитесь, что персонаж вошел в систему.
		uint iClientID = HkGetClientIdFromCharname(wscCharname);
		if (iClientID == -1)
			return;

		// Получите файл сообщений с целевыми игроками.
		string scFilePath = GetUserFilePath(wscCharname, scExtension);
		if (scFilePath.length() == 0)
			return;

		int iLastMsg = iFirstMsg;
		for (int iMsgSlot = iFirstMsg, iMsgCount = 0; iMsgSlot < MAX_MAIL_MSGS && iMsgCount < 5; iMsgSlot++, iMsgCount++)
		{
			wstring wscTmpMsg = IniGetWS(scFilePath, "Msgs", itos(iMsgSlot), L"");
			if (wscTmpMsg.length() == 0)
				break;
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0943"), iMsgSlot, wscTmpMsg.c_str());
			IniWrite(scFilePath, "MsgsRead", itos(iMsgSlot), "yes");
			iLastMsg = iMsgSlot;
		}
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0944"), iFirstMsg, iLastMsg, MailCount(wscCharname, scExtension));
	}

	/** Возвращает количество непрочитанных сообщений. */
	int MailCountUnread(const wstring &wscCharname, const string &scExtension)
	{
		// Получение файла сообщений для целевых игроков.
		string scFilePath = GetUserFilePath(wscCharname, scExtension);
		if (scFilePath.length() == 0)
			return 0;

		int iUnreadMsgs = 0;
		for (int iMsgSlot = 1; iMsgSlot < MAX_MAIL_MSGS; iMsgSlot++)
		{
			if (IniGetS(scFilePath, "Msgs", itos(iMsgSlot), "").length() == 0)
				break;
			if (!IniGetB(scFilePath, "MsgsRead", itos(iMsgSlot), false))
				iUnreadMsgs++;
		}
		return iUnreadMsgs;
	}

	/** Возвращает количество сообщений. */
	int MailCount(const wstring &wscCharname, const string &scExtension)
	{
		// Получение файла сообщений для целевых игроков.
		string scFilePath = GetUserFilePath(wscCharname, scExtension);
		if (scFilePath.length() == 0)
			return 0;

		int iMsgs = 0;
		for (int iMsgSlot = 1; iMsgSlot < MAX_MAIL_MSGS; iMsgSlot++)
		{
			if (IniGetS(scFilePath, "Msgs", itos(iMsgSlot), "").length() == 0)
				break;
			iMsgs++;
		}
		return iMsgs;
	}


	/** Проверка на наличие новых или непрочитанных сообщений. */
	void MailCheckLog(const wstring &wscCharname, const string &scExtension)
	{
		// Проверка на наличие новых или непрочитанных сообщений.
		uint iClientID = HkGetClientIdFromCharname(wscCharname);
		if (iClientID == -1)
			return;

		// Получение файла сообщений для целевых игроков.
		string scFilePath = GetUserFilePath(wscCharname, scExtension);
		if (scFilePath.length() == 0)
			return;

		// Если есть непрочитанные сообщения, сообщите об этом игроку
		int iUnreadMsgs = MailCountUnread(wscCharname, scExtension);
		if (iUnreadMsgs > 0)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0945"), iUnreadMsgs);
		}
	}

	/**
	 Сохраняем сообщение на диск, чтобы мы могли сообщить принимающему персонажу, когда он войдет в систему.
	*/
	bool MailSend(const wstring &wscCharname, const string &scExtension, const wstring &wscMsg)
	{
		// Получите файл сообщений с целевыми игроками.
		string scFilePath = GetUserFilePath(wscCharname, scExtension);
		if (scFilePath.length() == 0)
			return false;

		// Переместите всю почту на один слот вверх, начиная с конца.
		// Мы автоматически отбрасываем самые старые сообщения.
		for (int iMsgSlot = MAX_MAIL_MSGS - 1; iMsgSlot > 0; iMsgSlot--)
		{
			wstring wscTmpMsg = IniGetWS(scFilePath, "Msgs", itos(iMsgSlot), L"");
			IniWriteW(scFilePath, "Msgs", itos(iMsgSlot + 1), wscTmpMsg);

			bool bTmpRead = IniGetB(scFilePath, "MsgsRead", itos(iMsgSlot), false);
			IniWrite(scFilePath, "MsgsRead", itos(iMsgSlot + 1), (bTmpRead ? "yes" : "no"));
		}

		// Напишите сообщение в слот
		IniWriteW(scFilePath, "Msgs", "1", GetTimeString(set_bLocalTime) + L" " + wscMsg);
		IniWrite(scFilePath, "MsgsRead", "1", "no");
		return true;
	}

	/**
		Как удалить сообщение
	*/
	bool MailDel(const wstring &wscCharname, const string &scExtension, int iMsg)
	{
		// Получите файл сообщений с целевыми игроками.
		string scFilePath = GetUserFilePath(wscCharname, scExtension);
		if (scFilePath.length() == 0)
			return false;

		// Переместите всю почту в одну ячейку, начиная с удаленного сообщения, чтобы перезаписать его
		for (int iMsgSlot = iMsg; iMsgSlot < MAX_MAIL_MSGS; iMsgSlot++)
		{
			wstring wscTmpMsg = IniGetWS(scFilePath, "Msgs", itos(iMsgSlot + 1), L"");
			IniWriteW(scFilePath, "Msgs", itos(iMsgSlot), wscTmpMsg);

			bool bTmpRead = IniGetB(scFilePath, "MsgsRead", itos(iMsgSlot + 1), false);
			IniWrite(scFilePath, "MsgsRead", itos(iMsgSlot), (bTmpRead ? "yes" : "no"));
		}
		return true;
	}
}
