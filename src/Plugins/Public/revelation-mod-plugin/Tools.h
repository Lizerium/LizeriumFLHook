/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 18 апреля 2026 14:45:47
 * Version: 1.0.466
 */

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Header.h"

using namespace std;

typedef struct IniEditItems
{
	string Node = "";
	string Key = "";
	string NewValue = "";
	wstring NewWideValue = L"";
	enum DataType {
		STRING,
		WSTRING
	} Type = STRING;
} iniEditItems;

typedef vector <IniEditItems> IniEditItemSet;

int randNumber(int min, int max);
string ws2s(wstring str);
int dirNotExisted(const char *dir);
bool createStorageDir(const char *dir);
void putDirReadme(const char *dir);

HK_ERROR editPlayerCharFile(wstring fileName, IniEditItemSet &inis);

void lTrim(string &str);
void rTrim(string &str);
void trim(string &str);
void lTrim(wstring &str);
void rTrim(wstring &str);
void trim(wstring &str);
