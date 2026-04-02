/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 02 апреля 2026 06:53:11
 * Version: 1.0.17
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

