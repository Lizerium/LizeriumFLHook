/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 12 июля 2026 06:54:09
 * Version: 1.0.549
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
