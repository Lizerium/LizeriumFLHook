/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 августа 2026 15:32:56
 * Version: 1.0.577
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