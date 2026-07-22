/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 22 июля 2026 12:57:57
 * Version: 1.0.559
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
