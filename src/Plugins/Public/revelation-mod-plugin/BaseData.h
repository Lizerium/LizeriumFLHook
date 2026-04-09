/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 апреля 2026 10:59:03
 * Version: 1.0.24
 */

#pragma once

#include "Item.h"

#include "SystemData.h"
#include "FactionData.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				class EXPORT BaseData : public Base
				{
				public:
					BaseData();
					~BaseData();

					void setSystem(wstring sNickName);
					SystemData* getSystem();

					void setFaction(wstring fNickname);
					FactionData* getFaction();

				protected:
					wstring systemNickName = L"";
					wstring factionNickName = L"";
				};
			}
		}
	}
}
