/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "Chat.h"
#include "ScriptMgr.h"
#include "SimPopulationMgr.h"

#include <cstdlib>
#include <string>

using namespace Acore::ChatCommands;

class sim_commandscript : public CommandScript
{
public:
    sim_commandscript() : CommandScript("sim_commandscript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable simCommandTable = {
            {"status",        HandleStatus,        SEC_GAMEMASTER, Console::Yes},
            {"clock",         HandleClock,         SEC_GAMEMASTER, Console::Yes},
            {"debug",         HandleDebug,         SEC_GAMEMASTER, Console::Yes},
            {"pause",         HandlePause,         SEC_GAMEMASTER, Console::Yes},
            {"resume",        HandleResume,        SEC_GAMEMASTER, Console::Yes},
            {"seed",          HandleSeed,          SEC_GAMEMASTER, Console::Yes},
            {"focus",         HandleFocus,         SEC_GAMEMASTER, Console::Yes},
            {"guildpresence", HandleGuildPresence, SEC_GAMEMASTER, Console::Yes},
        };

        static ChatCommandTable commandTable = {
            {"sim", simCommandTable},
        };

        return commandTable;
    }

    static bool HandleStatus(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage(SimPopulationMgr::StatusText());
        return true;
    }

    static bool HandleClock(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage(SimPopulationMgr::ClockText());
        return true;
    }

    static bool HandleDebug(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .sim debug Name");
            return false;
        }
        handler->SendSysMessage(SimPopulationMgr::DebugText(args));
        return true;
    }

    static bool HandlePause(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .sim pause Name");
            return false;
        }
        if (!SimPopulationMgr::Pause(args))
        {
            handler->PSendSysMessage("Unknown sim character '{}'.", args);
            return false;
        }
        handler->PSendSysMessage("Paused {}.", args);
        return true;
    }

    static bool HandleResume(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .sim resume Name");
            return false;
        }
        if (!SimPopulationMgr::Resume(args))
        {
            handler->PSendSysMessage("Unknown sim character '{}'.", args);
            return false;
        }
        handler->PSendSysMessage("Resumed {}.", args);
        return true;
    }

    static bool HandleSeed(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .sim seed N");
            return false;
        }
        uint32 n = uint32(atoi(args));
        if (!n)
        {
            handler->SendSysMessage("Usage: .sim seed N");
            return false;
        }
        uint32 stamped = SimPopulationMgr::Seed(n);
        handler->PSendSysMessage("Seeded {} existing rndbot characters (requested {}).", stamped, n);
        return true;
    }

    static bool HandleFocus(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .sim focus Name level|dungeon|pvp|raid|gold|explore|social");
            return false;
        }
        std::string text = args;
        auto space = text.find(' ');
        if (space == std::string::npos)
        {
            handler->SendSysMessage("Usage: .sim focus Name level|dungeon|pvp|raid|gold|explore|social");
            return false;
        }
        std::string name = text.substr(0, space);
        std::string focus = text.substr(space + 1);
        while (!focus.empty() && focus.front() == ' ')
            focus.erase(focus.begin());
        if (!SimPopulationMgr::SetFocus(name, focus))
        {
            handler->PSendSysMessage("Failed to set focus '{}' on '{}'.", focus, name);
            return false;
        }
        handler->PSendSysMessage("{} focus -> {}", name, focus);
        return true;
    }

    static bool HandleGuildPresence(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage(SimPopulationMgr::GuildPresenceText());
        return true;
    }
};

void AddSimCommandScripts()
{
    new sim_commandscript();
}
