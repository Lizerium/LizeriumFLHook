/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 27 апреля 2026 09:43:01
 * Version: 1.0.475
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
