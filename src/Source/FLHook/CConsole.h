/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 июля 2026 12:57:45
 * Version: 1.0.560
 */

#ifndef _CCONSOLE_
#define _CCONSOLE_

#include "CCmds.h"

class CConsole : public CCmds
{
public:
	CConsole() { this->rights = RIGHT_SUPERADMIN; };
	EXPORT void DoPrint(const wstring &wscText);
	wstring GetAdminName();
};

#endif
