/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BotGearLane.h"
#include "Group.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"

namespace BotGearLane
{
bool IsRealParticipant(Player* player)
{
    if (!player)
        return false;

    // Manual client: no playerbot AI at all.
    if (IsRealPlayer(player))
        return true;

    // Logged-in player running a selfbot, or an alt with a game client master.
    PlayerbotAI* ai = GET_PLAYERBOT_AI(player);
    if (!ai)
        return true;
    if (IsSelfBot(player))
        return true;
    if (ai->HasGameClientMaster())
        return true;
    return false;
}

bool SlotLooksLikeRealPlayer(ObjectGuid guid)
{
    if (!guid)
        return false;

    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return IsRealParticipant(player);

    // Offline: random/addclass bots stay bots. Anyone else in the group is
    // treated as a real player so logging out does not free the party for
    // dungeon-sim to snatch.
    uint32 const guidLow = guid.GetCounter();
    if (sRandomPlayerbotMgr.IsRandomBot(guidLow) || sRandomPlayerbotMgr.IsAddclassBot(guidLow))
        return false;
    return true;
}

bool GroupHasRealPlayer(Group* group)
{
    if (!group)
        return false;

    for (Group::MemberSlot const& slot : group->GetMemberSlots())
    {
        if (SlotLooksLikeRealPlayer(slot.guid))
            return true;
    }
    return false;
}

bool IsGroupedWithRealPlayer(Player* bot)
{
    if (!bot)
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return IsRealParticipant(bot);

    return GroupHasRealPlayer(group);
}

bool IsAutonomousRandomBot(Player* bot)
{
    if (!bot)
        return false;
    if (IsGroupedWithRealPlayer(bot))
        return false;
    return sRandomPlayerbotMgr.IsRandomBot(bot);
}

Lane LaneFor(Player* bot)
{
    if (!bot)
        return Lane::None;
    if (IsGroupedWithRealPlayer(bot))
        return Lane::PlayerGroupPersonal;
    if (sRandomPlayerbotMgr.IsRandomBot(bot))
        return Lane::AutonomousSim;
    return Lane::None;
}
} // namespace BotGearLane
