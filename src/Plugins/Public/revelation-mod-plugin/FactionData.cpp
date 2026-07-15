/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 15 июля 2026 11:56:05
 * Version: 1.0.552
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