/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 29 марта 2026 16:56:36
 * Version: 1.0.2
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <list>
#include <map>
#include <algorithm>
#include <FLHook.h>
#include <plugin.h>
#include <PluginUtilities.h>

wstring Trim(wstring wscIn);
string Trim(string scIn);
string GetParam(string scLine, char cSplitChar, uint iPos);
string utos(uint i);
string HkGetStringFromIDS(uint iIDS);
void HkLoadDLLConf(const char *szFLConfigFile, CCmds* cmds);
void DoHashList();
void DoHashListLogic(CCmds* cmds);

#endif