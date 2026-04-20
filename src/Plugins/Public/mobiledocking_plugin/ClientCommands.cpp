/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 03:08:00
 * Version: 1.0.467
 */

#include "main.h"

/// Send a command to the client at destination ID 0x9999
void SendCommand(uint client, const wstring &message)
{
	HkFMsg(client, L"<TEXT>" + XMLText(message) + L"</TEXT>");
}

void SendSetBaseInfoText2(UINT client, const wstring &message)
{
	SendCommand(client, wstring(L" SetBaseInfoText2 ") + message);
}

void SendResetMarketOverride(UINT client)
{
	SendCommand(client, L" ResetMarketOverride");
	SendCommand(client, L" SetMarketOverride 0 0 0 0");
}
