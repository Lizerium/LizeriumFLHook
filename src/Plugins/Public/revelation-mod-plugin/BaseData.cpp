/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 04 апреля 2026 13:00:07
 * Version: 1.0.19
 */

#include "BaseData.h"

#include <algorithm>

#include "Data.h"
#include "Print.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				BaseData::BaseData()
				{
				}

				BaseData::~BaseData()
				{
				}

				void BaseData::setSystem(wstring sNickName)
				{
					systemNickName = ToLower(sNickName);
				}

				SystemData* BaseData::getSystem()
				{
					return Data::getSystem(systemNickName);
				}

				void BaseData::setFaction(wstring fNickname)
				{
					factionNickName = ToLower(ToLower(fNickname));
				}

				FactionData* BaseData::getFaction()
				{
					return Data::getFaction(factionNickName);
				}

			}
		}
	}
}