/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 22 мая 2026 11:41:47
 * Version: 1.0.499
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
