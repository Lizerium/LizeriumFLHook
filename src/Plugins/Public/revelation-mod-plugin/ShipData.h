/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 30 мая 2026 15:44:39
 * Version: 1.0.507
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
				class EXPORT ShipData : public Base
				{
				public:
					ShipData();
					~ShipData();

					string getType();
					void setType(string type);
                    
				protected:
					string shipType = "";

				};
			}
		}
	}
}

