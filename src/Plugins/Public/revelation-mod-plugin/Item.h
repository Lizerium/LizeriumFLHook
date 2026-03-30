/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 30 марта 2026 12:21:08
 * Version: 1.0.11
 */

#pragma once

#include <string>
#include "Header.h"

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				class EXPORT Base
				{
				public:
					void setNickname(wstring nName);
					wstring getNickname();

					void setName(wstring nName);
					wstring getName();

					uint getID();

				protected:
					Base();
					~Base();

					wstring nickname = L"";
					wstring name = L"";

					uint id = 0;
				};
			}
		}
	}
}