/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 13 июля 2026 06:54:02
 * Version: 1.0.550
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