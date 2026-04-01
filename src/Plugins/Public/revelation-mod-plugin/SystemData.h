/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 01 апреля 2026 13:06:06
 * Version: 1.0.13
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
