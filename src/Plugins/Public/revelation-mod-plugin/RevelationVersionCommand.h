/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 04 сентября 2026 07:40:54
 * Version: 1.0.602
 */

#pragma once

#include "Command.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Commands
			{

				class RevelationVersionCommand : public CommandBase
				{
				public:
					RevelationVersionCommand();
					virtual ~RevelationVersionCommand();

					virtual bool execute(shared_ptr<Clients::Client> &client, vector <wstring> &parameters);
				};

			}
		}
	}
}
