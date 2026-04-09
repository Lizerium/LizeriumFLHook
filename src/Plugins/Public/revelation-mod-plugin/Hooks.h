/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 апреля 2026 10:59:03
 * Version: 1.0.24
 */

#pragma once

#include "FLHook.h"

namespace HkIServerImpl
{
	void __stdcall Timer();

	void __stdcall SystemSwitchOutComplete(uint iShip, uint iClientID);

	void __stdcall BaseEnter_AFTER(uint iBaseID, uint iClientID);

	void __stdcall PlayerLaunch_AFTER(uint iShip, uint iClientID);

	void __stdcall DisConnect_AFTER(uint iClientID, enum EFLConnection p2);
}
