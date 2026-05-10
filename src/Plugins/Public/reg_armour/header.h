/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 10 мая 2026 07:44:00
 * Version: 1.0.488
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