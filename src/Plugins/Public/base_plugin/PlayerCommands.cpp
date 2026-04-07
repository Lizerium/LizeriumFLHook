/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 07 апреля 2026 10:57:40
 * Version: 1.0.22
 */

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <list>
#include <map>
#include <algorithm>
#include <FLHook.h>
#include <plugin.h>
#include <PluginUtilities.h>
#include "Main.h"

#define POPUPDIALOG_BUTTONS_LEFT_YES 1
#define POPUPDIALOG_BUTTONS_CENTER_NO 2
#define POPUPDIALOG_BUTTONS_RIGHT_LATER 4
#define POPUPDIALOG_BUTTONS_CENTER_OK 8

namespace PlayerCommands
{
	void BaseHelp(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		// Separate base help out into pages. FL seems to have a limit of something like 4k per infocard.
		const uint numPages = 4;
		wstring pages[numPages];
		pages[0] = L"<TRA bold=\"true\"/><TEXT>/base help [page]</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Show this help page. Specify the page number to see the next page.</TEXT><PARA/><PARA/>" 

			L"<TRA bold=\"true\"/><TEXT>/base login [password]</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Login as base administrator. The following commands are only available if you are logged in as a base administrator.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base addpwd [password] [viewshop], /base rmpwd [password], /base lstpwd</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Add, remove and list administrator passwords for the base. Add 'viewshop' to addpwd to only allow the password to view the shop.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base addtag [tag], /base rmtag [tag], /base lsttag</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Add, remove and list ally tags for the base.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base addhostile [tag], /base rmhostile [tag], /base lsthostile</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Add, remove and list blacklisted tags for the base. They will be shot on sight so use complete tags like =LSF= or IMG| or a shipname like Crunchy_Salad.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base setmasterpwd [old password] [new password]</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Set the master password for the base.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base rep [clear]</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Set or clear the faction that this base is affiliated with. When setting the affiliation, the affiliation will be that of the player executing the command.</TEXT>";

		pages[1] = L"<TRA bold=\"true\"/><TEXT>/bank withdraw [credits], /bank deposit [credits], /bank status</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Withdraw, deposit or check the status of the credits held by the base's bank.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/shop price [item] [price] [min stock] [max stock]</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Set the [price] of [item]. If the current stock is less than [min stock]"
			L" then the item cannot be bought by docked ships. If the current stock is more or equal"
			L" to [max stock] then the item cannot be sold to the base by docked ships.</TEXT><PARA/><PARA/>"
			L"<TEXT>To prohibit selling to the base of an item by docked ships under all conditions, set [max stock] to 0."
			L"To prohibit buying from the base of an item by docked ships under all conditions, set [min stock] to 0.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/shop remove [item]</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Remove the item from the stock list. It cannot be sold to the base by docked ships unless they are base administrators.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/shop [page]</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Show the shop stock list for [page]. There are a maximum of 40 items shown per page.</TEXT>";

		pages[2] = L"<TRA bold=\"true\"/><TEXT>/base defensemode</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Control the defense mode for the base.</TEXT><PARA/>"
			L"<TEXT>Defense Mode 1 - Logic: Blacklist > Whitelist > IFF Standing.</TEXT><PARA/>"
			L"<TEXT>Docking Rights: Whitelisted ships only.</TEXT><PARA/><PARA/>"
			L"<TEXT>Defense Mode 2 - Logic: Blacklist > Whitelist > IFF Standing.</TEXT><PARA/>"
			L"<TEXT>Docking Rights: Anyone with good standing.</TEXT><PARA/><PARA/>"
			L"<TEXT>Defense Mode 3 - Logic: Blacklist > Whitelist > Hostile</TEXT><PARA/>"
			L"<TEXT>Docking Rights: Whitelisted ships only.</TEXT><PARA/><PARA/>"
			L"<TEXT>Defense Mode 4 - Logic: Blacklist > Whitelist > Neutral</TEXT><PARA/>"
			L"<TEXT>Docking Rights: Anyone with good standing.</TEXT><PARA/><PARA/>"
			L"<TEXT>Defense Mode 5 - Logic: Blacklist > Whitelist > Neutral</TEXT><PARA/>"
			L"<TEXT>Docking Rights: Whitelisted ships only.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base info</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Set the base's infocard description.</TEXT>";

		pages[3] = L"<TRA bold=\"true\"/><TEXT>/base facmod</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Control factory modules.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base defmod</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Control defense modules.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base shieldmod</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Control shield modules.</TEXT><PARA/><PARA/>"

			L"<TRA bold=\"true\"/><TEXT>/base buildmod</TEXT><TRA bold=\"false\"/><PARA/>"
			L"<TEXT>Control the construction and destruction of base modules and upgrades.</TEXT>";

		uint page = 0;
		wstring pageNum = GetParam(args, ' ', 2);
		if (pageNum.length())
		{
			page = ToUInt(pageNum) - 1;
			if (page < 0 || page > numPages - 1) {
				page = 0;
			}
		}

		wstring pagetext = pages[page];

		wchar_t titleBuf[4000];
		_snwprintf(titleBuf, sizeof(titleBuf), L"Base Help : Page %d/%d", page + 1, numPages);

		wchar_t buf[4000];
		_snwprintf(buf, sizeof(buf), L"<RDL><PUSH/>%s<POP/></RDL>", pagetext.c_str());

		HkChangeIDSString(iClientID, 500000, titleBuf);
		HkChangeIDSString(iClientID, 500001, buf);

		FmtStr caption(0, 0);
		caption.begin_mad_lib(500000);
		caption.end_mad_lib();

		FmtStr message(0, 0);
		message.begin_mad_lib(500001);
		message.end_mad_lib();

		pub::Player::PopUpDialog(iClientID, caption, message, POPUPDIALOG_BUTTONS_CENTER_OK);
	}

	bool RateLimitLogins(uint iClientID, PlayerBase *base, wstring charname)
	{
		uint curr_time = (uint)time(0);
		uint big_penalty_time = 300;
		int amount_of_attempts_to_reach_penalty = 15;

		//initiate
		if (base->unsuccessful_logins_in_a_row.find(charname) == base->unsuccessful_logins_in_a_row.end())
			base->unsuccessful_logins_in_a_row[charname] = 0;

		if (base->last_login_attempt_time.find(charname) == base->last_login_attempt_time.end())
			base->last_login_attempt_time[charname] = 0;

		//nulify counter if more than N seconds passed.
		if ((curr_time - base->last_login_attempt_time[charname]) > big_penalty_time)
			base->unsuccessful_logins_in_a_row[charname] = 0;

		uint blocktime = 1;
		if (base->unsuccessful_logins_in_a_row[charname] >= amount_of_attempts_to_reach_penalty)
			blocktime = big_penalty_time;

		uint waittime = blocktime - (curr_time - base->last_login_attempt_time[charname]);
		//You are attempting to log in too often
		if ((curr_time - base->last_login_attempt_time[charname]) < blocktime)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0180"), base->unsuccessful_logins_in_a_row[charname], waittime);
			return true;
		} 
		
		if (base->unsuccessful_logins_in_a_row[charname] >= amount_of_attempts_to_reach_penalty) 
			base->unsuccessful_logins_in_a_row[charname] = 0;

		return false;
	}

	void BaseLogin(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);

		//prevent too often login attempts
		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		if (RateLimitLogins(iClientID, base, charname)) return;

		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		wstring password = GetParam(args, ' ', 2);
		if (!password.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0182"));
			return;
		}

		//remember last time attempt to login
		base->last_login_attempt_time[charname] = (uint)time(0);

		BasePassword searchBp;
		searchBp.pass = password;
		list<BasePassword>::iterator ret = find(base->passwords.begin(), base->passwords.end(), searchBp);
		if (ret == base->passwords.end()) {
			base->unsuccessful_logins_in_a_row[charname]++; //count password failures
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		BasePassword foundBp = *ret;
		if (foundBp.admin) {
			clients[iClientID].admin = true;
			SendMarketGoodSync(base, iClientID);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0184"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0185"));
		}
		if (foundBp.viewshop) {
			clients[iClientID].viewshop = true;
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0184"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0187"));
		}

	}

	void BaseAddPwd(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring password = GetParam(args, ' ', 2);
		if (!password.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0182"));
			return;
		}

		BasePassword searchBp;
		searchBp.pass = password;

		if (find(base->passwords.begin(), base->passwords.end(), searchBp) != base->passwords.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0191"));
			return;
		}

		BasePassword bp;
		bp.pass = password;

		wstring flagsStr = GetParam(args, ' ', 3);
		int flags = 0;
		if (flagsStr.length() && flagsStr == L"viewshop")
		{
			bp.viewshop = true;
		}
		else {
			bp.admin = true;
		}

		base->passwords.push_back(bp);
		base->Save();
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}

	void BaseRmPwd(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring password = GetParam(args, ' ', 2);
		if (!password.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0182"));
		}

		BasePassword searchBp;
		searchBp.pass = password;
		list<BasePassword>::iterator ret = find(base->passwords.begin(), base->passwords.end(), searchBp);
		if (ret != base->passwords.end())
		{
			BasePassword bp = *ret;
			base->passwords.remove(bp);
			base->Save();
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
			return;
		}

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0197"));
	}

	void BaseSetMasterPwd(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring old_password = GetParam(args, ' ', 2);
		if (!old_password.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0200"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0201"));
			return;
		}

		wstring new_password = GetParam(args, ' ', 3);
		if (!new_password.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0202"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0201"));
			return;
		}

		BasePassword bp;
		bp.pass = new_password;
		bp.admin = true;

		if (find(base->passwords.begin(), base->passwords.end(), bp) != base->passwords.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0191"));
			return;
		}

		if (base->passwords.size())
		{
			if (base->passwords.front().pass != old_password)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0205"));
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0201"));
				return;
			}
		}

		base->passwords.remove(base->passwords.front());
		base->passwords.push_front(bp);
		base->Save();
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0207"), new_password.c_str());
	}

	void BaseLstPwd(uint iClientID, const wstring &cmd)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		// Do not display the first password.
		bool first = true;
		foreach(base->passwords, BasePassword, bpi)
		{
			if (first)
				first = false;
			else {
				BasePassword bp = *bpi;
				wstring *p = &(bp.pass);
				if (bp.admin)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0210"), p->c_str());
				}
				if (bp.viewshop)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0211"), p->c_str());
				}
			}
		}
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}

	void BaseAddAllyTag(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring tag = GetParam(args, ' ', 2);
		if (!tag.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0215"));
			return;
		}

		if (find(base->ally_tags.begin(), base->ally_tags.end(), tag) != base->ally_tags.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0216"));
			return;
		}

		base->ally_tags.push_back(tag);

		// Logging
		wstring thecharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring wscMsg = L": \"%sender\" added \"%victim\" to whitelist of base \"%base\"";
		wscMsg = ReplaceStr(wscMsg, L"%sender", thecharname.c_str());
		wscMsg = ReplaceStr(wscMsg, L"%victim", tag);
		wscMsg = ReplaceStr(wscMsg, L"%base", base->basename);
		string scText = wstos(wscMsg);
		BaseLogging("%s", scText.c_str());

		base->Save();
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}


	void BaseRmAllyTag(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring tag = GetParam(args, ' ', 2);
		if (!tag.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0215"));
		}

		if (find(base->ally_tags.begin(), base->ally_tags.end(), tag) == base->ally_tags.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0221"));
			return;
		}

		base->ally_tags.remove(tag);

		// Logging
		wstring thecharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring wscMsg = L": \"%sender\" removed \"%victim\" from whitelist of base \"%base\"";
		wscMsg = ReplaceStr(wscMsg, L"%sender", thecharname.c_str());
		wscMsg = ReplaceStr(wscMsg, L"%victim", tag);
		wscMsg = ReplaceStr(wscMsg, L"%base", base->basename);
		string scText = wstos(wscMsg);
		BaseLogging("%s", scText.c_str());

		base->Save();
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}

	void BaseLstAllyTag(uint iClientID, const wstring &cmd)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		foreach(base->ally_tags, wstring, i)
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0225"), i->c_str());
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}

	void BaseRep(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		bool isServerAdmin;

		wstring rights;
		if (HkGetAdmin((const wchar_t*)Players.GetActiveCharacterName(iClientID), rights) == HKE_OK && rights.find(L"superadmin") != -1)
		{
			isServerAdmin = true;
		}


		if (!clients[iClientID].admin && !isServerAdmin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring arg = GetParam(args, ' ', 2);
		if (arg == L"clear")
		{
			if (isServerAdmin) {
				base->affiliation = 0;
				base->Save();
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0229"));
			}
			else
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0230"));
			}
			return;
		}

		if (isServerAdmin || base->affiliation <= 0)
		{
			int rep;
			pub::Player::GetRep(iClientID, rep);

			uint affiliation;
			Reputation::Vibe::Verify(rep);
			Reputation::Vibe::GetAffiliation(rep, affiliation, false);
			if (affiliation == -1)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0231"));
				return;
			}

			base->affiliation = affiliation;
			base->Save();
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0232"), HkGetWStringFromIDS(Reputation::get_name(affiliation)).c_str());
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0233"));
		}
	}

	void BaseAddHostileTag(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring tag = GetParam(args, ' ', 2);
		if (!tag.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0215"));
			return;
		}

		if (find(base->perma_hostile_tags.begin(), base->perma_hostile_tags.end(), tag) != base->perma_hostile_tags.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0216"));
			return;
		}


		base->perma_hostile_tags.push_back(tag);

		// Logging
		wstring thecharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring wscMsg = L": \"%sender\" added \"%victim\" to blacklist of base \"%base\"";
		wscMsg = ReplaceStr(wscMsg, L"%sender", thecharname.c_str());
		wscMsg = ReplaceStr(wscMsg, L"%victim", tag);
		wscMsg = ReplaceStr(wscMsg, L"%base", base->basename);
		string scText = wstos(wscMsg);
		BaseLogging("%s", scText.c_str());

		base->Save();

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}


	void BaseRmHostileTag(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring tag = GetParam(args, ' ', 2);
		if (!tag.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0215"));
		}

		if (find(base->perma_hostile_tags.begin(), base->perma_hostile_tags.end(), tag) == base->perma_hostile_tags.end())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0221"));
			return;
		}

		base->perma_hostile_tags.remove(tag);

		// Logging
		wstring thecharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		wstring wscMsg = L": \"%sender\" removed \"%victim\" from blacklist of base \"%base\"";
		wscMsg = ReplaceStr(wscMsg, L"%sender", thecharname.c_str());
		wscMsg = ReplaceStr(wscMsg, L"%victim", tag);
		wscMsg = ReplaceStr(wscMsg, L"%base", base->basename);
		string scText = wstos(wscMsg);
		BaseLogging("%s", scText.c_str());


		base->Save();

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}

	void BaseLstHostileTag(uint iClientID, const wstring &cmd)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		foreach(base->perma_hostile_tags, wstring, i)
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0225"), i->c_str());
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}

	void BaseInfo(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		uint iPara = ToInt(GetParam(args, ' ', 2));
		const wstring &cmd = GetParam(args, ' ', 3);
		const wstring &msg = GetParamToEnd(args, ' ', 4);

		if (iPara > 0 && iPara <= MAX_PARAGRAPHS && cmd == L"a")
		{
			int length = base->infocard_para[iPara].length() + msg.length();
			if (length > MAX_CHARACTERS)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0250"), MAX_CHARACTERS);
				return;
			}

			base->infocard_para[iPara] += XMLText(msg);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0251"), length, MAX_CHARACTERS);

			// Update the infocard text.
			base->infocard.clear();
			for (int i = 1; i <= MAX_PARAGRAPHS; i++)
			{
				wstring wscXML = base->infocard_para[i];
				if (wscXML.length())
					base->infocard += L"<TEXT>" + wscXML + L"</TEXT><PARA/><PARA/>";
			}

			base->Save();
		}
		else if (iPara > 0 && iPara <= MAX_PARAGRAPHS && cmd == L"d")
		{
			base->infocard_para[iPara] = L"";
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));

			// Update the infocard text.
			base->infocard.clear();
			for (int i = 1; i <= MAX_PARAGRAPHS; i++)
			{
				wstring wscXML = base->infocard_para[i];
				if (wscXML.length())
					base->infocard += L"<TEXT>" + wscXML + L"</TEXT><PARA/><PARA/>";
			}

			base->Save();
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0253"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0254"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0255"), MAX_PARAGRAPHS);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0256"));
		}
	}

	void BaseDefenseMode(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		wstring wscMode = GetParam(args, ' ', 2);
		if (wscMode == L"1")
		{
			base->defense_mode = 1;
		}
		else if (wscMode == L"2")
		{
			base->defense_mode = 2;
		}
		else if (wscMode == L"3")
		{
			base->defense_mode = 3;
		}
		else if (wscMode == L"4")
		{
			base->defense_mode = 4;
		}
		else if (wscMode == L"5")
		{
			base->defense_mode = 5;
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0259"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0260"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0261"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0262"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0263"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0264"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0265"), base->defense_mode);
			return;
		}

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0266"), base->defense_mode);

		base->Save();

		base->SyncReputationForBase();
	}

	void BaseBuildMod(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		const wstring &cmd = GetParam(args, ' ', 2);
		if (cmd == L"list")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0269"));
			for (uint index = 1; index < base->modules.size(); index++)
			{
				if (base->modules[index])
				{
					Module *mod = (Module*)base->modules[index];
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0270"), index, mod->GetInfo(false).c_str());
				}
				else
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0271"), index);
				}
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
		}
		else if (cmd == L"destroy")
		{
			uint index = ToInt(GetParam(args, ' ', 3));
			if (index < 1 || index >= base->modules.size() || !base->modules[index])
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0273"));
				return;
			}

			if (base->modules[index]->type == Module::TYPE_STORAGE && base->GetRemainingCargoSpace() < STORAGE_MODULE_CAPACITY)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0274"), STORAGE_MODULE_CAPACITY);

				wstring wscCharname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
				pub::Player::SendNNMessage(iClientID, pub::GetNicknameId("nnv_anomaly_detected"));
				wstring wscMsgU = L"KITTY ALERT: Possible type 5 POB cheating by %name (Index = %index, RemainingSpace = %space)\n";
				wscMsgU = ReplaceStr(wscMsgU, L"%name", wscCharname.c_str());
				wscMsgU = ReplaceStr(wscMsgU, L"%index", stows(itos(index)).c_str());
				wscMsgU = ReplaceStr(wscMsgU, L"%space", stows(itos((int)base->GetRemainingCargoSpace())).c_str());

				ConPrint(wscMsgU);
				LogCheater(iClientID, wscMsgU);

				return;
			}

			delete base->modules[index];
			base->modules[index] = 0;
			base->Save();
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0275"));
		}
		else if (cmd == L"construct")
		{
			uint index = ToInt(GetParam(args, ' ', 3));
			uint type = ToInt(GetParam(args, ' ', 4));
			if (index < 1 || index >= base->modules.size() || base->modules[index])
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0276"));
				return;
			}

			if (type < Module::TYPE_CORE || type > Module::TYPE_LAST)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0277"));
				return;
			}

			if (type == Module::TYPE_CORE)
			{
				if (base->base_level >= 4)
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0278"));
					return;
				}
			}

			//make the nickname for inspection
			uint module_nickname = CreateID(MODULE_TYPE_NICKNAMES[type]);

			if (recipes[module_nickname].reqlevel > base->base_level)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0279"));
				return;
			}

			base->modules[index] = new BuildModule(base, type);
			base->Save();
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0280"));
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0253"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0282"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0283"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0284"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0285"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0286"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0287"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0288"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0289"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0290"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0291"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0292"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0293"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0294"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0295"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0296"));
		}
	}

	void BaseFacMod(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		const wstring &cmd = GetParam(args, ' ', 2);
		if (cmd == L"list")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0299"));
			for (uint index = 1; index < base->modules.size(); index++)
			{
				if (base->modules[index] &&
					(base->modules[index]->type == Module::TYPE_M_CLOAK
						|| base->modules[index]->type == Module::TYPE_M_HYPERSPACE_SCANNER
						|| base->modules[index]->type == Module::TYPE_M_JUMPDRIVES
						|| base->modules[index]->type == Module::TYPE_M_DOCKING
						|| base->modules[index]->type == Module::TYPE_M_CLOAKDISRUPTOR))
				{
					FactoryModule *mod = (FactoryModule*)base->modules[index];
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0270"), index, mod->GetInfo(false).c_str());
				}
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
		}
		else if (cmd == L"clear")
		{
			uint index = ToInt(GetParam(args, ' ', 3));
			if (index < 1 || index >= base->modules.size() || !base->modules[index])
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0276"));
				return;
			}

			if (!base->modules[index] ||
				(base->modules[index]->type != Module::TYPE_M_CLOAK
					&& base->modules[index]->type != Module::TYPE_M_HYPERSPACE_SCANNER
					&& base->modules[index]->type != Module::TYPE_M_JUMPDRIVES
					&& base->modules[index]->type != Module::TYPE_M_DOCKING
					&& base->modules[index]->type != Module::TYPE_M_CLOAKDISRUPTOR))
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0303"));
				return;
			}

			FactoryModule *mod = (FactoryModule*)base->modules[index];
			if (mod->ClearQueue())
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0304"));
			else
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0305"));
			base->Save();
		}
		else if (cmd == L"cancel")
		{
			uint index = ToInt(GetParam(args, ' ', 3));
			if (index < 1 || index >= base->modules.size() || !base->modules[index])
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0276"));
				return;
			}

			if (!base->modules[index] ||
				(base->modules[index]->type != Module::TYPE_M_CLOAK
					&& base->modules[index]->type != Module::TYPE_M_HYPERSPACE_SCANNER
					&& base->modules[index]->type != Module::TYPE_M_JUMPDRIVES
					&& base->modules[index]->type != Module::TYPE_M_DOCKING
					&& base->modules[index]->type != Module::TYPE_M_CLOAKDISRUPTOR))
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0303"));
				return;
			}

			FactoryModule *mod = (FactoryModule*)base->modules[index];
			mod->ClearRecipe();
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0308"));
			base->Save();
		}
		else if (cmd == L"add")
		{
			uint index = ToInt(GetParam(args, ' ', 3));
			uint type = ToInt(GetParam(args, ' ', 4));
			if (index < 1 || index >= base->modules.size() || !base->modules[index])
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0276"));
				return;
			}

			if (!base->modules[index] ||
				(base->modules[index]->type != Module::TYPE_M_CLOAK
					&& base->modules[index]->type != Module::TYPE_M_HYPERSPACE_SCANNER
					&& base->modules[index]->type != Module::TYPE_M_JUMPDRIVES
					&& base->modules[index]->type != Module::TYPE_M_DOCKING
					&& base->modules[index]->type != Module::TYPE_M_CLOAKDISRUPTOR))
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0303"));
				return;
			}

			FactoryModule *mod = (FactoryModule*)base->modules[index];
			if (mod->AddToQueue(type))
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0311"));
			else
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0312"));
			base->Save();
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0253"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0314"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0315"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0316"));

			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0317"));
			
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0318"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0319"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0320"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0321"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0322"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0323"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0324"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0325"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0326"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0327"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0328"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0329"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0330"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0331"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0332"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0333"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0334"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0335"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0336"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0337"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0338"));
		}
	}

	void BaseDefMod(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0183"));
			return;
		}

		const wstring &cmd = GetParam(args, ' ', 2);
		if (cmd == L"list")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0341"));
			for (uint index = 0; index < base->modules.size(); index++)
			{
				if (base->modules[index])
				{
					if (base->modules[index]->type == Module::TYPE_DEFENSE_1
						|| base->modules[index]->type == Module::TYPE_DEFENSE_2
						|| base->modules[index]->type == Module::TYPE_DEFENSE_3)
					{
						DefenseModule *mod = (DefenseModule*)base->modules[index];
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0342"),
							index, mod->pos.x, mod->pos.y, mod->pos.z,
							mod->rot.z, mod->rot.y, mod->rot.z);
					}
				}
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
		}
		else if (cmd == L"set")
		{
			uint index = ToInt(GetParam(args, ' ', 3));
			float x = (float)ToInt(GetParam(args, ' ', 4));
			float y = (float)ToInt(GetParam(args, ' ', 5));
			float z = (float)ToInt(GetParam(args, ' ', 6));
			float rx = (float)ToInt(GetParam(args, ' ', 7));
			float ry = (float)ToInt(GetParam(args, ' ', 8));
			float rz = (float)ToInt(GetParam(args, ' ', 9));
			if (index < base->modules.size() && base->modules[index])
			{
				if (base->modules[index]->type == Module::TYPE_DEFENSE_1
					|| base->modules[index]->type == Module::TYPE_DEFENSE_2
					|| base->modules[index]->type == Module::TYPE_DEFENSE_3)
				{
					DefenseModule *mod = (DefenseModule*)base->modules[index];

					// Distance from base is limited to 5km
					Vector new_pos = { x, y, z };
					if (HkDistance3D(new_pos, base->position) > 5000)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0344"));
						return;
					}

					mod->pos = new_pos;
					mod->rot.x = rx;
					mod->rot.y = ry;
					mod->rot.z = rz;

					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0345"),
						index, mod->pos.x, mod->pos.y, mod->pos.z,
						mod->rot.z, mod->rot.y, mod->rot.z);
					base->Save();
					mod->Reset();
				}
				else
				{
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0273"));
				}
			}
			else
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0273"));
			}
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0253"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0349"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0350"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0351"));
		}
	}

	void BaseShieldMod(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0352"));
			return;
		}

		if (!clients[iClientID].admin)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0353"));
			return;
		}

		const wstring &cmd = GetParam(args, ' ', 2);
		if (cmd == L"on")
		{
			base->shield_active_time = 3600 * 24;
		}
		else if (cmd == L"off")
		{
			base->shield_active_time = 0;
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0354"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0355"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0356"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0357"));
		}

		// Принудительно запустить таймер для модуля(ей) щита и прочитать его значения статуса.
		for (uint index = 0; index < base->modules.size(); index++)
		{
			if (base->modules[index] &&
				base->modules[index]->type == Module::TYPE_SHIELDGEN)
			{
				ShieldModule *mod = (ShieldModule*)base->modules[index];
				mod->Timer(0);
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0358"), mod->GetInfo(false).c_str());
			}
		}
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
	}

	void Bank(uint iClientID, const wstring &args)
	{
		PlayerBase *base = GetPlayerBaseForClient(iClientID);

		const wstring &cmd = GetParam(args, ' ', 1);
		int money = ToInt(GetParam(args, ' ', 2));

		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);

		if (cmd == L"withdraw")
		{
			if (!clients[iClientID].admin)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0353"));
				return;
			}

			float fValue;
			pub::Player::GetAssetValue(iClientID, fValue);

			int iCurrMoney;
			pub::Player::InspectCash(iClientID, iCurrMoney);

			if (fValue + money > 2100000000 || iCurrMoney + money > 2100000000)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0361"));
				return;
			}

			if (money > base->money || money < 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0362"));
				return;
			}

			pub::Player::AdjustCash(iClientID, money);
			base->money -= money;
			base->Save();

			AddLog("УВЕДОМЛЕНИЕ: У банка состояние: баланс=%I64d денег=%d база=%s имя персонажа=%s (%s)",
				base->money, money,
				wstos(base->basename).c_str(),
				wstos(charname).c_str(),
				wstos(HkGetAccountID(HkGetAccountByCharname(charname))).c_str());

			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0363"), money);
		}
		else if (cmd == L"deposit")
		{
			int iCurrMoney;
			pub::Player::InspectCash(iClientID, iCurrMoney);

			if (money > iCurrMoney || money < 0)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0362"));
				return;
			}

			pub::Player::AdjustCash(iClientID, 0 - money);
			base->money += money;
			base->Save();

			AddLog("УВЕДОМЛЕНИЕ: Деньги на банковский депозит=%d новый баланс=%I64d база=%s имя персонажа=%s (%s)",
				money, base->money,
				wstos(base->basename).c_str(),
				wstos(charname).c_str(),
				wstos(HkGetAccountID(HkGetAccountByCharname(charname))).c_str());

			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0365"), money);
		}
		else if (cmd == L"status")
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0366"), base->money);
		}
		else
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0354"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0368"));
		}
	}

	static void ShowShopStatus(uint iClientID, PlayerBase *base, wstring substring, int page)
	{
		int matchingItems = 0;
		for (map<UINT, MARKET_ITEM>::iterator i = base->market_items.begin(); i != base->market_items.end(); ++i)
		{
			const GoodInfo *gi = GoodList::find_by_id(i->first);
			if (!gi)
				continue;

			wstring name = HkGetWStringFromIDS(gi->iIDSName);
			if (ToLower(name).find(substring) != std::wstring::npos) {
				matchingItems++;
			}
		}

		int pages = (matchingItems / 40) + 1;
		if (page > pages)
			page = pages;
		else if (page < 1)
			page = 1;

		wchar_t buf[1000];
		_snwprintf(buf, sizeof(buf), L"Shop Management : Page %d/%d", page, pages);
		wstring title = buf;

		int start_item = ((page - 1) * 40) + 1;
		int end_item = page * 40;

		wstring status = L"<RDL><PUSH/>";
		status += L"<TEXT>Available commands:</TEXT><PARA/>";
		if (clients[iClientID].admin)
		{
			status += L"<TEXT>  /shop price [item] [price] [min stock] [max stock]</TEXT><PARA/>";
			status += L"<TEXT>  /shop remove [item]</TEXT><PARA/>";
		}
		status += L"<TEXT>  /shop [page]</TEXT><PARA/><TEXT>  /shop filter [substring] [page]</TEXT><PARA/><PARA/>";

		status += L"<TEXT>Stock:</TEXT><PARA/>";
		int item = 1;
		int globalItem = 1;

		for (map<UINT, MARKET_ITEM>::iterator i = base->market_items.begin(); i != base->market_items.end(); ++i, globalItem++)
		{
			if (item > end_item)
				break;

			const GoodInfo *gi = GoodList::find_by_id(i->first);
			if (!gi) {
				item++;
				continue;
			}

			wstring name = HkGetWStringFromIDS(gi->iIDSName);
			if (ToLower(name).find(substring) != std::wstring::npos) {
				if (item < start_item) {
					item++;
					continue;
				}
				wchar_t buf[1000];
				_snwprintf(buf, sizeof(buf), L"<TEXT>  %02u:  %ux %s %0.0f credits stock: %u min %u max</TEXT><PARA/>",
					globalItem, i->second.quantity, HtmlEncode(name).c_str(),
					i->second.price, i->second.min_stock, i->second.max_stock);
				status += buf;
				item++;
			}
		}
		status += L"<POP/></RDL>";

		HkChangeIDSString(iClientID, 500000, title);
		HkChangeIDSString(iClientID, 500001, status);

		FmtStr caption(0, 0);
		caption.begin_mad_lib(500000);
		caption.end_mad_lib();

		FmtStr message(0, 0);
		message.begin_mad_lib(500001);
		message.end_mad_lib();

		pub::Player::PopUpDialog(iClientID, caption, message, POPUPDIALOG_BUTTONS_CENTER_OK);
	}

	void Shop(uint iClientID, const wstring &args)
	{
		// Check that this player is in a player controlled base
		PlayerBase *base = GetPlayerBaseForClient(iClientID);
		if (!base)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0179"));
			return;
		}

		const wstring &cmd = GetParam(args, ' ', 1);
		if (!clients[iClientID].admin && (!clients[iClientID].viewshop || (cmd == L"price" || cmd == L"remove")))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0370"));
			return;
		}

		if (cmd == L"price")
		{
			int item = ToInt(GetParam(args, ' ', 2));
			int money = ToInt(GetParam(args, ' ', 3));
			int min_stock = ToInt(GetParam(args, ' ', 4));
			int max_stock = ToInt(GetParam(args, ' ', 5));

			if (money < 1 || money > 1000000000)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0371"));
				return;
			}

			int curr_item = 1;
			for (map<UINT, MARKET_ITEM>::iterator i = base->market_items.begin(); i != base->market_items.end(); ++i, curr_item++)
			{
				if (curr_item == item)
				{
					i->second.price = (float)money;
					i->second.min_stock = min_stock;
					i->second.max_stock = max_stock;
					SendMarketGoodUpdated(base, i->first, i->second);
					base->Save();

					int page = ((curr_item + 39) / 40);
					ShowShopStatus(iClientID, base, L"", page);
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
					return;
				}
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0373"));
		}
		else if (cmd == L"remove")
		{
			int item = ToInt(GetParam(args, ' ', 2));

			int curr_item = 1;
			for (map<UINT, MARKET_ITEM>::iterator i = base->market_items.begin(); i != base->market_items.end(); ++i, curr_item++)
			{
				if (curr_item == item)
				{
					i->second.price = 0;
					i->second.quantity = 0;
					i->second.min_stock = 0;
					i->second.max_stock = 0;
					SendMarketGoodUpdated(base, i->first, i->second);
					base->market_items.erase(i->first);
					base->Save();

					int page = ((curr_item + 39) / 40);
					ShowShopStatus(iClientID, base, L"", page);
					PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
					return;
				}
			}
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0373"));
		}
		else if (cmd == L"filter")
		{
			wstring substring = GetParam(args, ' ', 2);
			int page = ToInt(GetParam(args, ' ', 3));
			ShowShopStatus(iClientID, base, ToLower(substring), page);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
		}
		else
		{
			int page = ToInt(GetParam(args, ' ', 1));
			ShowShopStatus(iClientID, base, L"", page);
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0192"));
		}
	}

	void BaseDeploy(uint iClientID, const wstring &args)
	{
		if (set_holiday_mode)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0378"));
			return;
		}

		// Прервать обработку, если это не "heavy lifter"
		uint shiparch;
		pub::Player::GetShipID(iClientID, shiparch);
		if (set_construction_shiparch != 0 && shiparch != set_construction_shiparch)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0379"));
			return;
		}

		uint ship;
		pub::Player::GetShip(iClientID, ship);
		if (!ship)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0380"));
			return;
		}

		// Если судно движется, прервите обработку.
		Vector dir1;
		Vector dir2;
		pub::SpaceObj::GetMotion(ship, dir1, dir2);
		if (dir1.x > 5 || dir1.y > 5 || dir1.z > 5)
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0381"));
			return;
		}

		wstring password = GetParam(args, ' ', 2);
		if (!password.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0382"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0383"));
			return;
		}
		wstring basename = GetParamToEnd(args, ' ', 3);
		if (!basename.length())
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0384"));
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0383"));
			return;
		}

		// Проверьте наличие конфликтующего базового имени
		if (GetPlayerBase(CreateID(PlayerBase::CreateBaseNickname(wstos(basename)).c_str())))
		{
			PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0386"));
			return;
		}

		// Убедитесь, что на корабле есть необходимые товары.
		int hold_size;
		list<CARGO_INFO> cargo;
		HkEnumCargo((const wchar_t*)Players.GetActiveCharacterName(iClientID), cargo, hold_size);
		for (map<uint, uint>::iterator i = construction_items.begin(); i != construction_items.end(); ++i)
		{
			bool material_available = false;
			uint good = i->first;
			uint quantity = i->second;
			for (list<CARGO_INFO>::iterator ci = cargo.begin(); ci != cargo.end(); ++ci)
			{
				if (ci->iArchID == good && ci->iCount >= (int)quantity)
				{
					material_available = true;
					pub::Player::RemoveCargo(iClientID, ci->iID, quantity);
				}
			}
			if (material_available == false)
			{
				PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0387"));
				for (i = construction_items.begin(); i != construction_items.end(); ++i)
				{
					const GoodInfo *gi = GoodList::find_by_id(i->first);
					if (gi)
					{
						PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0388"), i->second, HkGetWStringFromIDS(gi->iIDSName).c_str());
					}
				}
				return;
			}
		}

		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(iClientID);
		AddLog("УВЕДОМЛЕНИЕ: База создана %s для %s (%s)",
			wstos(basename).c_str(),
			wstos(charname).c_str(),
			wstos(HkGetAccountID(HkGetAccountByCharname(charname))).c_str());

		PlayerBase *newbase = new PlayerBase(iClientID, password, basename);
		player_bases[newbase->base] = newbase;
		newbase->basetype = "legacy";
		newbase->basesolar = "legacy";
		newbase->baseloadout = "legacy";
		newbase->defense_mode = 1;

		for (map<string, ARCHTYPE_STRUCT>::iterator iter = mapArchs.begin(); iter != mapArchs.end(); iter++)
		{

			ARCHTYPE_STRUCT &thearch = iter->second;
			if (iter->first == newbase->basetype)
			{
				newbase->invulnerable = thearch.invulnerable;
				newbase->logic = thearch.logic;
			}
		}
		newbase->Spawn();
		newbase->Save();

		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0389"));
		PrintUserCmdText(iClientID, GetLocalized(iClientID, "MSG_0390"), password.c_str());
	}
}
