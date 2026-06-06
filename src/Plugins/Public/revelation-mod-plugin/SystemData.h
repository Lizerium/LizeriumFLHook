/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 06 июня 2026 08:49:28
 * Version: 1.0.514
 */

#pragma once

#include "Item.h"

#include "FactionData.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				class EXPORT SystemData : public Base
				{
				public:
					SystemData();
					~SystemData();

					void setFaction(wstring fNickname);
					FactionData* SystemData::getFaction();

					void setTerritory(wstring fTerritory);
					wstring getTerritory();

				protected:
					wstring factionNickName = L"";
					wstring systemTerritory = L"";
				};
			}
		}
	}
}
