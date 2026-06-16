/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 16 июня 2026 06:54:06
 * Version: 1.0.524
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
