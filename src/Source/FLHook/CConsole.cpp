/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 05 мая 2026 07:03:06
 * Version: 1.0.483
 */

#include "CConsole.h"
#include <wchar.h>

void CConsole::DoPrint(const wstring &wscText)
{
	ConPrint(wscText);
}

wstring CConsole::GetAdminName()
{
	return L"Admin console";
}
