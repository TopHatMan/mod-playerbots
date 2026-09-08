/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BOTGEARLANE_H
#define PLAYERBOTS_BOTGEARLANE_H

#include "ObjectGuid.h"

class Group;
class Player;

// Gear split for this realm:
//   PlayerGroupPersonal — bots grouped WITH a real player get companion
//                         personal loot (dungeon-sim PersonalLoot).
//   AutonomousSim       — bots WITHOUT a real player get dungeon-sim
//                         completion gear.
// Factory BiS is not a living-world lane.
namespace BotGearLane
{
enum class Lane
{
    None = 0,
    PlayerGroupPersonal,
    AutonomousSim
};

bool IsRealParticipant(Player* player);
bool SlotLooksLikeRealPlayer(ObjectGuid guid);
bool GroupHasRealPlayer(Group* group);
bool IsGroupedWithRealPlayer(Player* bot);
bool IsAutonomousRandomBot(Player* bot);
Lane LaneFor(Player* bot);
} // namespace BotGearLane

#endif
