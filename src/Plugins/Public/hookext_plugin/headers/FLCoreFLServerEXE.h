/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 03:08:00
 * Version: 1.0.467
 */

//////////////////////////////////////////////////////////////////////
//	Project FLCoreSDK v1.1, modified for use in FLHook Plugin version
//--------------------------
//
//	File:			FLCoreFLServerEXE.h
//	Module:			FLCoreFLServerEXE.lib
//	Description:	Interface to FLServer.exe
//
//	Web: www.skif.be/flcoresdk.php
//  
//
//////////////////////////////////////////////////////////////////////
#ifndef _FLCOREFLSERVEREXE_H_
#define _FLCOREFLSERVEREXE_H_

#include "FLCoreDefs.h"
#include <string>

#pragma comment( lib, "FLCoreFLServerEXE.lib" )

IMPORT  int ServerLogf(struct ErrorCode, char const *, ...);

#endif // _FLCOREFLSERVEREXE_H_