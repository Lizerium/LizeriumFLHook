/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 03 сентября 2026 07:19:01
 * Version: 1.0.601
 */

#pragma once

#include <string>
#include <map>

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			class Language
			{
			public:
				static Language* Get();

				void import(const char* path);
				const wstring& lang(const string key, const wstring defaultStr);

			protected:
				map <const string, wstring> languages;

				Language();
				~Language();

				static bool parse(string input, string &key, wstring &val);
			};

		}
	}
}