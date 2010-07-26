/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSBan : public Command
{
 public:
	CommandCSBan(const Anope::string &cname) : Command(cname, 2, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string target = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "Requested";

		Channel *c = findchan(chan);
		ChannelInfo *ci;
		User *u2;

		int is_same;

		is_same = target.equals_ci(u->nick);

		if (c)
			ci = c->ci;

		if (!c)
			notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan.c_str());
		else if (is_same ? !(u2 = u) : !(u2 = finduser(target)))
			notice_lang(Config.s_ChanServ, u, NICK_X_NOT_IN_USE, target.c_str());
		else if (!is_same ? !check_access(u, ci, CA_BAN) : !check_access(u, ci, CA_BANME))
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		else if (!is_same && (ci->HasFlag(CI_PEACE)) && (get_access(u2, ci) >= get_access(u, ci)))
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		/*
		 * Dont ban/kick the user on channels where he is excepted
		 * to prevent services <-> server wars.
		 */
		else if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && is_excepted(ci, u2))
			notice_lang(Config.s_ChanServ, u, CHAN_EXCEPTED, u2->nick.c_str(), ci->name.c_str());
		else if (u2->IsProtected())
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		else
		{
			Anope::string mask;

			get_idealban(ci, u2, mask);
			c->SetMode(NULL, CMODE_BAN, mask);

			/* We still allow host banning while not allowing to kick */
			if (!c->FindUser(u2))
				return MOD_CONT;

			if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(u, ci, CA_SIGNKICK)))
				c->Kick(whosends(ci), u2, "%s (%s)", reason.c_str(), u->nick.c_str());
			else
				c->Kick(whosends(ci), u2, "%s", reason.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_BAN);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "BAN", CHAN_BAN_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_BAN);
	}
};

class CSBan : public Module
{
 public:
	CSBan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, new CommandCSBan("BAN"));
		this->AddCommand(ChanServ, new CommandCSBan("KB"));
	}
};

MODULE_INIT(CSBan)