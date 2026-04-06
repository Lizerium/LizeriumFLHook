/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 06 апреля 2026 12:49:28
 * Version: 1.0.21
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
