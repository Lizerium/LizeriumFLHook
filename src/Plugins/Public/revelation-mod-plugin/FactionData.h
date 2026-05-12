/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 12 мая 2026 12:07:28
 * Version: 1.0.490
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