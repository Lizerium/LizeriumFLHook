/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 24 апреля 2026 06:53:29
 * Version: 1.0.472
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
