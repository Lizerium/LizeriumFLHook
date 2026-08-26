/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 26 августа 2026 06:54:22
 * Version: 1.0.594
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
