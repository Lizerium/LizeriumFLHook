/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 19 августа 2026 10:24:08
 * Version: 1.0.587
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
