/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 08 августа 2026 06:54:15
 * Version: 1.0.576
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
