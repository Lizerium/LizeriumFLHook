/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 01 сентября 2026 08:33:51
 * Version: 1.0.599
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