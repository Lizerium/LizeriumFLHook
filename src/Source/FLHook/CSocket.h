/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 10 апреля 2026 12:33:16
 * Version: 1.0.25
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
