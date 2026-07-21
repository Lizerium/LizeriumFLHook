/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 21 июля 2026 13:32:15
 * Version: 1.0.558
 */

#include "FactionData.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				FactionData::FactionData()
				{
				}

				FactionData::~FactionData()
				{
				}

				wstring FactionData::getShort()
				{
					return factionShort;
				}

				void FactionData::setShort(wstring shortName)
				{
					factionShort = shortName;
				}
			}
		}
	}
}