/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 июля 2026 12:57:45
 * Version: 1.0.560
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