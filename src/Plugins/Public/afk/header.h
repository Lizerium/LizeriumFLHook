/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 01 апреля 2026 13:06:06
 * Version: 1.0.13
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include <FLHook.h>
#include <plugin.h>

static int set_iPluginDebug = 0;
PLUGIN_RETURNCODE returncode;

typedef bool(*_UserCmdProc)(uint, const std::wstring &, const std::wstring &,
	const wchar_t *);

struct USERCMD {
	wchar_t *wszCmd;
	_UserCmdProc proc;
	wchar_t *usage;
};

#define IS_CMD(a) !wscCmd.compare(L##a)

#endif