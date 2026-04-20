/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 03:08:00
 * Version: 1.0.467
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
