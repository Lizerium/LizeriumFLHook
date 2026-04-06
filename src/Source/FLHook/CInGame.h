/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 06 апреля 2026 12:49:28
 * Version: 1.0.21
 */

#include "CCmds.h"

#ifndef _CINGAME_
#define _CINGAME_

class CInGame : public CCmds
{
public:
	uint iClientID;
	wstring wscAdminName;
	void DoPrint(const wstring &wscText);
	void ReadRights(const string &scIniFile);
	wstring GetAdminName();
};

#endif
