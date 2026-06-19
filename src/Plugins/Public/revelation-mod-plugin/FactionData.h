/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 19 июня 2026 06:53:58
 * Version: 1.0.527
 */

#pragma once

#include "Item.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				class EXPORT FactionData : public Base
				{
				public:
					FactionData();
					~FactionData();

					wstring getShort();
					void setShort(wstring shortName);

				protected:
					wstring factionShort = L"";
				};
			}
		}
	}
}