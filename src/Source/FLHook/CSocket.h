/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 мая 2026 11:15:14
 * Version: 1.0.500
 */

#include "CCmds.h"

#ifndef _CSOCKET_
#define _CSOCKET_

class CSocket : public CCmds
{
public:
	SOCKET s;
	BLOWFISH_CTX *bfc;
	bool bAuthed;
	bool bEventMode;
	bool bUnicode;
	bool bEncrypted;
	string sIP;
	ushort iPort;

	CSocket() { bAuthed = false; bEventMode = false; bUnicode = false; }
	void DoPrint(const wstring &wscText);
	wstring GetAdminName();
};


#endif
