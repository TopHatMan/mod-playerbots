/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SIMPOPULATIONMGR_H
#define PLAYERBOTS_SIMPOPULATIONMGR_H

#include "Define.h"
#include <string>

class Player;

// Persistent fake-player roster: personas + weekly schedules + focus.
// Same guid stays the same person. Stock Min/Max random login becomes a
// safety cap only when DisableStockRandomMgr is on.
namespace SimPopulationMgr
{
void LoadConfig();
void Update(uint32 diff);

bool IsEnabled();
bool DisableStockRandomMgr();
bool DisableGuildForcePresence();
bool OwnsGuid(uint32 guidLow);

uint32 Seed(uint32 count);

std::string StatusText();
std::string ClockText();
std::string DebugText(std::string const& name);
std::string GuildPresenceText();

bool Pause(std::string const& name);
bool Resume(std::string const& name);
bool SetFocus(std::string const& name, std::string const& focus);
} // namespace SimPopulationMgr

void AddSimCommandScripts();

#endif
