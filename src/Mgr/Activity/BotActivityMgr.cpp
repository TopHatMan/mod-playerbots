/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BotActivityMgr.h"
#include "BotGearLane.h"
#include "Config.h"
#include "GameTime.h"
#include "Group.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace
{
struct ActivityConfig
{
    bool playerGroupReserve = true;
    uint32 playerGroupReconcileSeconds = 15;
};

ActivityConfig cfg;
bool cfgLoaded = false;
uint32 lastReconcile = 0;

std::mutex& Lock()
{
    static std::mutex m;
    return m;
}

std::unordered_map<uint32, BotActivityMgr::Reservation>& Reservations()
{
    static std::unordered_map<uint32, BotActivityMgr::Reservation> m;
    return m;
}

uint32 Now()
{
    return uint32(GameTime::GetGameTime().count());
}

bool IsExpiredUnlocked(BotActivityMgr::Reservation const& res, uint32 now)
{
    return res.expireTime && res.expireTime <= now;
}

bool IsManagedBot(uint32 guidLow, Player* member)
{
    if (member)
        return GET_PLAYERBOT_AI(member) != nullptr;
    return sRandomPlayerbotMgr.IsRandomBot(guidLow) || sRandomPlayerbotMgr.IsAddclassBot(guidLow);
}
} // namespace

namespace BotActivityMgr
{
void LoadConfig()
{
    cfg.playerGroupReserve = sConfigMgr->GetOption<bool>("AiPlayerbot.BotActivity.PlayerGroupReserve", true);
    cfg.playerGroupReconcileSeconds =
        sConfigMgr->GetOption<uint32>("AiPlayerbot.BotActivity.PlayerGroupReconcileSeconds", 15);
    if (!cfg.playerGroupReconcileSeconds)
        cfg.playerGroupReconcileSeconds = 15;
    cfgLoaded = true;
}

bool PlayerGroupReserveEnabled()
{
    if (!cfgLoaded)
        LoadConfig();
    return cfg.playerGroupReserve;
}

bool TryReserve(uint32 guidLow, Owner owner, uint32 durationSeconds)
{
    if (!guidLow || owner == Owner::None)
        return false;

    uint32 const now = Now();
    std::lock_guard<std::mutex> g(Lock());
    auto& map = Reservations();
    auto it = map.find(guidLow);
    if (it != map.end())
    {
        if (IsExpiredUnlocked(it->second, now))
            map.erase(it);
        else if (it->second.owner != owner)
            return false;
    }

    Reservation res;
    res.owner = owner;
    res.expireTime = durationSeconds ? (now + durationSeconds) : 0;
    map[guidLow] = res;
    return true;
}

void Release(uint32 guidLow, Owner owner)
{
    if (!guidLow)
        return;

    std::lock_guard<std::mutex> g(Lock());
    auto it = Reservations().find(guidLow);
    if (it == Reservations().end())
        return;
    if (owner != Owner::None && it->second.owner != owner)
        return;
    Reservations().erase(it);
}

bool IsReserved(uint32 guidLow)
{
    return GetOwner(guidLow) != Owner::None;
}

Owner GetOwner(uint32 guidLow)
{
    if (!guidLow)
        return Owner::None;

    uint32 const now = Now();
    std::lock_guard<std::mutex> g(Lock());
    auto it = Reservations().find(guidLow);
    if (it == Reservations().end())
        return Owner::None;
    if (IsExpiredUnlocked(it->second, now))
    {
        Reservations().erase(it);
        return Owner::None;
    }
    return it->second.owner;
}

bool IsOwnedBy(uint32 guidLow, Owner owner)
{
    return GetOwner(guidLow) == owner;
}

char const* OwnerName(Owner owner)
{
    switch (owner)
    {
        case Owner::DungeonSim:
            return "dungeon-sim";
        case Owner::PvPLife:
            return "pvp-life";
        case Owner::Artisans:
            return "artisans";
        case Owner::PlayerGroup:
            return "player-group";
        case Owner::WorldRpg:
            return "world-rpg";
        case Owner::Raid:
            return "raid";
        case Owner::External:
            return "external";
        default:
            return "none";
    }
}

void SyncPlayerGroup(Group* group)
{
    if (!PlayerGroupReserveEnabled() || !group)
        return;
    if (group->isBGGroup() || group->isBFGroup())
        return;

    bool const hasReal = BotGearLane::GroupHasRealPlayer(group);
    for (Group::MemberSlot const& slot : group->GetMemberSlots())
    {
        uint32 const guidLow = slot.guid.GetCounter();
        Player* member = ObjectAccessor::FindPlayer(slot.guid);
        if (!IsManagedBot(guidLow, member))
            continue;

        if (hasReal)
            TryReserve(guidLow, Owner::PlayerGroup);
        else
            Release(guidLow, Owner::PlayerGroup);
    }
}

void SyncPlayerGroupFor(Player* player)
{
    if (!player)
        return;

    if (Group* group = player->GetGroup())
    {
        SyncPlayerGroup(group);
        return;
    }

    if (GET_PLAYERBOT_AI(player))
        Release(player->GetGUID().GetCounter(), Owner::PlayerGroup);
}

void ReconcilePlayerGroups()
{
    if (!PlayerGroupReserveEnabled())
        return;

    std::vector<uint32> guids;
    {
        uint32 const now = Now();
        std::lock_guard<std::mutex> g(Lock());
        for (auto const& [guidLow, res] : Reservations())
        {
            if (res.owner != Owner::PlayerGroup)
                continue;
            if (IsExpiredUnlocked(res, now))
                continue;
            guids.push_back(guidLow);
        }
    }

    for (uint32 guidLow : guids)
    {
        Player* bot = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(guidLow));
        if (!bot || !bot->GetGroup())
        {
            Release(guidLow, Owner::PlayerGroup);
            continue;
        }
        SyncPlayerGroup(bot->GetGroup());
    }
}

void Update()
{
    uint32 const now = Now();
    {
        std::lock_guard<std::mutex> g(Lock());
        for (auto it = Reservations().begin(); it != Reservations().end();)
        {
            if (IsExpiredUnlocked(it->second, now))
                it = Reservations().erase(it);
            else
                ++it;
        }
    }

    if (PlayerGroupReserveEnabled() &&
        (!lastReconcile || now >= lastReconcile + cfg.playerGroupReconcileSeconds))
    {
        lastReconcile = now;
        ReconcilePlayerGroups();
    }
}

bool BlocksWorldWander(uint32 guidLow)
{
    Owner const owner = GetOwner(guidLow);
    return owner != Owner::None && owner != Owner::WorldRpg;
}

bool BlocksWorldWander(Player const* bot)
{
    if (!bot)
        return false;
    return BlocksWorldWander(bot->GetGUID().GetCounter());
}
} // namespace BotActivityMgr
