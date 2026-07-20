/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 июля 2026 12:11:49
 * Version: 1.0.557
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

