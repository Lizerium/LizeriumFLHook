/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 27 июля 2026 14:58:25
 * Version: 1.0.564
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