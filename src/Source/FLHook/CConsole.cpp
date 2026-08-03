/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 03 августа 2026 06:54:22
 * Version: 1.0.571
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
