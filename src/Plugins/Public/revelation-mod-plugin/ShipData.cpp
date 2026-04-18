/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 18 апреля 2026 14:45:47
 * Version: 1.0.466
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