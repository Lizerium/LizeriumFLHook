/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 30 июля 2026 06:53:53
 * Version: 1.0.567
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"
#include <vector>

list<INISECTIONVALUE> lstRanks;
wstring HkGetWStringFromIDS(uint iIDS);
void HkLoadDLLConf(const char *szFLConfigFile);
int ToInt(string scStr);

#endif