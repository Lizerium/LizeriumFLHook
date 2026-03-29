/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 29 марта 2026 16:56:36
 * Version: 1.0.2
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include "binarytree.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <FLHook.h>
#include <plugin.h>
#include "header.h"

struct UINT_WRAP
{
	UINT_WRAP(uint u) { val = u; }
	bool operator==(UINT_WRAP uw) { return uw.val == val; }
	bool operator>=(UINT_WRAP uw) { return uw.val >= val; }
	bool operator<=(UINT_WRAP uw) { return uw.val <= val; }
	bool operator>(UINT_WRAP uw) { return uw.val > val; }
	bool operator<(UINT_WRAP uw) { return uw.val < val; }
	uint val;
};

#endif