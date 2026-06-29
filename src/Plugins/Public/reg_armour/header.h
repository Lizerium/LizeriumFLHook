/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 29 июня 2026 06:54:50
 * Version: 1.0.537
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include <windows.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

struct RGA_DATA
{
	mstime tmRegenTime;
	int mTime;
	bool HasArmour;
	bool Repair;
};

void HkTimerArmourRegen();
string GetParam(string scLine, char cSplitChar, uint iPos);

#endif