/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 13 июня 2026 14:02:35
 * Version: 1.0.521
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

