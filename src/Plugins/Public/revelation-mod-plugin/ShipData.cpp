/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 12 мая 2026 12:07:28
 * Version: 1.0.490
 */

#include "ShipData.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				ShipData::ShipData()
				{
				}

				ShipData::~ShipData()
				{
				}

				string ShipData::getType()
				{
					return shipType;
				}

				void ShipData::setType(string type)
				{
					shipType = ToLower(type);
				}
			}
		}
	}
}