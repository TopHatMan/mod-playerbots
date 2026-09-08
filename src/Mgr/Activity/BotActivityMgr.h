/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BOTACTIVITYMGR_H
#define PLAYERBOTS_BOTACTIVITYMGR_H

#include "Define.h"

class Group;
class Player;

// Canonical bot-activity reservation bus.
//
// Sibling mods (dungeon-sim, pvp-life, artisans) must reserve through this
// manager instead of keeping private guid sets. A reserved bot is not free
// for grind/rpg/travel teleports.
namespace BotActivityMgr
{
enum class Owner : uint8
{
    None = 0,
    DungeonSim,
    PvPLife,
    Artisans,
    PlayerGroup,
    WorldRpg,
    Raid,
    External
};

struct Reservation
{
    Owner owner = Owner::None;
    uint32 expireTime = 0; // unix seconds; 0 = until Release
};

// guidLow is Player::GetGUID().GetCounter().
// Same owner may refresh. A different owner fails.
bool TryReserve(uint32 guidLow, Owner owner, uint32 durationSeconds = 0);
void Release(uint32 guidLow, Owner owner = Owner::None);
bool IsReserved(uint32 guidLow);
Owner GetOwner(uint32 guidLow);
bool IsOwnedBy(uint32 guidLow, Owner owner);
char const* OwnerName(Owner owner);

void LoadConfig();
bool PlayerGroupReserveEnabled();

// Claim every bot in a real player's group as Owner::PlayerGroup so
// dungeon-sim / pvp-life / artisans cannot snatch them. Releases that
// owner when the real player is gone. Never steals from another owner.
void SyncPlayerGroup(Group* group);
void SyncPlayerGroupFor(Player* player);
void ReconcilePlayerGroups();

// Drop expired timed reservations and periodically reconcile player groups.
void Update();

// True when grind/rpg/travel/out-of-party should stand down for this bot.
bool BlocksWorldWander(uint32 guidLow);
bool BlocksWorldWander(Player const* bot);
} // namespace BotActivityMgr

#endif
