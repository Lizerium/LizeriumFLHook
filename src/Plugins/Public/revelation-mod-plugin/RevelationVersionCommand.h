/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 02 апреля 2026 06:53:11
 * Version: 1.0.17
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
