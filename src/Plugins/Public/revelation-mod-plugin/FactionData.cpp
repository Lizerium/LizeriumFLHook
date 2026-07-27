/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 27 июля 2026 14:58:25
 * Version: 1.0.564
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