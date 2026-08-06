/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 06 августа 2026 06:55:15
 * Version: 1.0.574
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