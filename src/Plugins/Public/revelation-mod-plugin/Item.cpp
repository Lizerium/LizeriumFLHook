/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 03 апреля 2026 11:33:45
 * Version: 1.0.18
 */

#include "Item.h"
#include "Tools.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				Base::Base()
				{
				}

				Base::~Base()
				{
				}

				void Base::setNickname(wstring nName)
				{
					nickname = ToLower(nName);

					id = CreateID(ws2s(nickname).c_str());
				}

				wstring Base::getNickname()
				{
					return nickname;
				}

				uint Base::getID()
				{
					return id;
				}

				void Base::setName(wstring nName)
				{
					name = nName;
				}

				wstring Base::getName()
				{
					return name;
				}
			}
		}
	}
}