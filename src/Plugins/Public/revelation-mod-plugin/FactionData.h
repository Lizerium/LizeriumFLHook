/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 16 мая 2026 10:48:31
 * Version: 1.0.493
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