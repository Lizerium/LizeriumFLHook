/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 16 июля 2026 14:15:23
 * Version: 1.0.553
 */

#pragma once

#include "FLHook.h"
#include "plugin.h"

#define REVELATION_HOST_PLUGIN_CALL_LEVEL 255
#define REVELATION_SUB_PLUGIN_CALL_LEVEL 128
#define REVELATION_CMD_PREFIX Language::Get()->lang("DEFAULT_CMD_PREFIX", L"Revelation")

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			extern PLUGIN_RETURNCODE PluginReturnCode;
		}
	}
}

static bool pluginInited = false;