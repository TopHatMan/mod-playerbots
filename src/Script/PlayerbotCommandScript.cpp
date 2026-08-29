/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BattleGroundTactics.h"
#include "Chat.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "GuildTaskMgr.h"
#include "PerfMonitor.h"
#include "PlayerbotMgr.h"
#include "RandomPlayerbotMgr.h"
#include "ScriptMgr.h"
#include "WorldSession.h"

#include <algorithm>
#include <cctype>
#include <deque>
#include <unordered_set>

using namespace Acore::ChatCommands;

namespace OrganicGuildPopulation
{
    static std::deque<ObjectGuid> loginQueue;
    static std::unordered_set<ObjectGuid> raidTimeBots;
    static uint32 updateTimer = 0;

    static std::string Lower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return value;
    }

    static bool IsRandomBotAccount(uint32 accountId)
    {
        std::string prefix = Lower(sConfigMgr->GetOption<std::string>(
            "AiPlayerbot.RandomBotAccountPrefix", "rndbot"));
        if (prefix.empty())
            return false;
        QueryResult result = LoginDatabase.Query("SELECT username FROM account WHERE id = {} LIMIT 1", accountId);
        return result && Lower(result->Fetch()[0].Get<std::string>()).rfind(prefix, 0) == 0;
    }

    static uint32 Begin(Player* caller)
    {
        loginQueue.clear();
        if (!caller || !caller->GetGuildId())
            return 0;

        QueryResult result = CharacterDatabase.Query(
            "SELECT c.guid,c.account FROM characters c INNER JOIN guild_member gm ON gm.guid=c.guid "
            "WHERE gm.guildid={} AND c.level={}", caller->GetGuildId(), sPlayerbotAIConfig.randomBotMaxLevel);
        if (!result)
            return 0;

        do
        {
            Field* fields = result->Fetch();
            ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(fields[0].Get<uint32>());
            if (IsRandomBotAccount(fields[1].Get<uint32>()) && !sRandomPlayerbotMgr.GetPlayerBot(guid))
                loginQueue.push_back(guid);
        } while (result->NextRow());
        return loginQueue.size();
    }

    static uint32 End()
    {
        loginQueue.clear();
        uint32 count = 0;
        for (ObjectGuid guid : raidTimeBots)
        {
            Player* bot = sRandomPlayerbotMgr.GetPlayerBot(guid);
            if (bot && !bot->GetGroup() && !bot->IsInCombat())
            {
                sRandomPlayerbotMgr.LogoutPlayerBot(guid);
                ++count;
            }
        }
        raidTimeBots.clear();
        return count;
    }
}

class organic_guild_population_worldscript : public WorldScript
{
public:
    organic_guild_population_worldscript() : WorldScript("organic_guild_population_worldscript") { }

    void OnUpdate(uint32 diff) override
    {
        using namespace OrganicGuildPopulation;
        updateTimer += diff;
        if (updateTimer < 5000 || loginQueue.empty())
            return;
        updateTimer = 0;
        ObjectGuid guid = loginQueue.front();
        loginQueue.pop_front();
        sRandomPlayerbotMgr.AddPlayerBot(guid, 0);
        raidTimeBots.insert(guid);
    }
};

class playerbots_commandscript : public CommandScript
{
public:
    playerbots_commandscript() : CommandScript("playerbots_commandscript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable playerbotsDebugCommandTable = {
            {"bg", HandleDebugBGCommand, SEC_GAMEMASTER, Console::Yes},
        };

        static ChatCommandTable playerbotsAccountCommandTable = {
            {"setKey", HandleSetSecurityKeyCommand, SEC_PLAYER, Console::No},
            {"link", HandleLinkAccountCommand, SEC_PLAYER, Console::No},
            {"linkedAccounts", HandleViewLinkedAccountsCommand, SEC_PLAYER, Console::No},
            {"unlink", HandleUnlinkAccountCommand, SEC_PLAYER, Console::No},
        };

        static ChatCommandTable playerbotsCommandTable = {
            {"bot", HandlePlayerbotCommand, SEC_PLAYER, Console::No},
            {"gtask", HandleGuildTaskCommand, SEC_GAMEMASTER, Console::Yes},
            {"pmon", HandlePerfMonCommand, SEC_GAMEMASTER, Console::Yes},
            {"rndbot", HandleRandomPlayerbotCommand, SEC_GAMEMASTER, Console::Yes},
            {"raidtime", HandleRaidTimeCommand, SEC_PLAYER, Console::No},
            {"unraidtime", HandleUnraidTimeCommand, SEC_PLAYER, Console::No},
            {"debug", playerbotsDebugCommandTable},
            {"account", playerbotsAccountCommandTable},
        };

        static ChatCommandTable commandTable = {
            {"playerbots", playerbotsCommandTable},
        };

        return commandTable;
    }

    static bool HandlePlayerbotCommand(ChatHandler* handler, char const* args)
    {
        return PlayerbotMgr::HandlePlayerbotMgrCommand(handler, args);
    }

    static bool HandleRandomPlayerbotCommand(ChatHandler* handler, char const* args)
    {
        return RandomPlayerbotMgr::HandlePlayerbotConsoleCommand(handler, args);
    }

    static bool HandleRaidTimeCommand(ChatHandler* handler, char const*)
    {
        uint32 queued = OrganicGuildPopulation::Begin(handler->GetPlayer());
        handler->PSendSysMessage("Raid time: {} raid-ready random guild bots will log in gradually.", queued);
        return true;
    }

    static bool HandleUnraidTimeCommand(ChatHandler* handler, char const*)
    {
        uint32 loggedOut = OrganicGuildPopulation::End();
        handler->PSendSysMessage("Raid time ended: {} idle raid-time bots logged out; grouped or fighting bots were left alone.", loggedOut);
        return true;
    }

    static bool HandleGuildTaskCommand(ChatHandler* handler, char const* args)
    {
        return GuildTaskMgr::HandleConsoleCommand(handler, args);
    }

    static bool HandlePerfMonCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!strcmp(args, "reset"))
        {
            sPerfMonitor.Reset();
            return true;
        }

        if (!strcmp(args, "tick"))
        {
            sPerfMonitor.PrintStats(true, false);
            sPerfMonitor.DumpJson(true);
            return true;
        }

        if (!strcmp(args, "stack"))
        {
            sPerfMonitor.PrintStats(false, true);
            sPerfMonitor.DumpJson(false);
            return true;
        }

        if (!strcmp(args, "toggle"))
        {
            sPlayerbotAIConfig.perfMonEnabled = !sPlayerbotAIConfig.perfMonEnabled;
            if (sPlayerbotAIConfig.perfMonEnabled)
                LOG_INFO("playerbots", "Performance monitor enabled");
            else
                LOG_INFO("playerbots", "Performance monitor disabled");
            return true;
        }

        sPerfMonitor.PrintStats();
        sPerfMonitor.DumpJson(false);
        return true;
    }

    static bool HandleDebugBGCommand(ChatHandler* handler, char const* args)
    {
        return BGTactics::HandleConsoleCommand(handler, args);
    }

    static bool HandleSetSecurityKeyCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->PSendSysMessage("Usage: .playerbots account setKey <securityKey>");
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        std::string key = args;

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleSetSecurityKeyCommand(player, key);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }

    static bool HandleLinkAccountCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
            return false;

        char* accountName = strtok((char*)args, " ");
        char* key = strtok(nullptr, " ");

        if (!accountName || !key)
        {
            handler->PSendSysMessage("Usage: .playerbots account link <accountName> <securityKey>");
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleLinkAccountCommand(player, accountName, key);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }

    static bool HandleViewLinkedAccountsCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleViewLinkedAccountsCommand(player);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }

    static bool HandleUnlinkAccountCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
            return false;

        char* accountName = strtok((char*)args, " ");
        if (!accountName)
        {
            handler->PSendSysMessage("Usage: .playerbots account unlink <accountName>");
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleUnlinkAccountCommand(player, accountName);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }
};

void AddPlayerbotsCommandscripts()
{
    new playerbots_commandscript();
    new organic_guild_population_worldscript();
}
