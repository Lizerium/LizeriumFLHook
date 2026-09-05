/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 05 сентября 2026 08:38:01
 * Version: 1.0.603
 */

#include "SystemData.h"

#include "Data.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				SystemData::SystemData()
				{
				}

				SystemData::~SystemData()
				{
				}

				void SystemData::setFaction(wstring fNickname)
				{
					factionNickName = (fNickname);
				}

				FactionData* SystemData::getFaction()
				{
					return Data::getFaction(factionNickName);
				}

				void SystemData::setTerritory(wstring fTerritory)
				{
					systemTerritory = fTerritory;
				}

				wstring SystemData::getTerritory()
				{
					return systemTerritory;
				}
			}
		}
	}
}