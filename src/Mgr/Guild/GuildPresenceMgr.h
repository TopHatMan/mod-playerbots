/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_GUILDPRESENCEMGR_H
#define PLAYERBOTS_GUILDPRESENCEMGR_H

#include "Define.h"
#include <string>

class Player;

// Transplanted from mod-npc-chat-llm (NpcChat.Bot.GuildPresence.*).
// Chat modules must not login bots. This is the only guild force-login path.
//
// Default is off. Sim.DisableGuildForcePresence also suppresses this and
// BotActiveAloneForceWhenInGuild even if the old keys were left on.
namespace GuildPresenceMgr
{
void LoadConfig();

// True when the transplanted roster-scan login is allowed to run.
bool IsLoginEnabled();

// True when Sim.DisableGuildForcePresence (or equivalent) forbids
// "in a real player's guild" from keeping bots logged in / always-active.
bool IsForcePresenceSuppressed();

void EnsureGuildBotsOnline(Player* realPlayer);

std::string StatusText();
} // namespace GuildPresenceMgr

#endif
