/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 03:08:00
 * Version: 1.0.467
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include <vector>
#include <windows.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

void ClearClientMark(uint iClientID);
void HkUnMarkAllObjects(uint iClientID);
char HkUnMarkObject(uint iClientID, uint iObject);
char HkMarkObject(uint iClientID, uint iObject);

struct MARK_INFO
{
	bool bMarkEverything;
	bool bIgnoreGroupMark;
	float fAutoMarkRadius;
	vector<uint> vMarkedObjs;
	vector<uint> vDelayedSystemMarkedObjs;
	vector<uint> vAutoMarkedObjs;
	vector<uint> vDelayedAutoMarkedObjs;
#pragma region Sonar
	uint CurrentSystemId;
	bool bSonarEverything;
	list<uint> vAutoMarkedSonarObjs;
	float fSonarRadius;
	map <uint, string> mapObjects;
#pragma endregion
};

struct DELAY_MARK { uint iObj; mstime time; };
string ftos(float f);
float HkDistance3D(Vector v1, Vector v2);

#endif