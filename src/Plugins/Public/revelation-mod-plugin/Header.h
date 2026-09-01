/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 01 сентября 2026 08:33:51
 * Version: 1.0.599
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