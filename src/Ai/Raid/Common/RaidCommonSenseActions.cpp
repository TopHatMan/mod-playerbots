/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RaidCommonSenseActions.h"
#include "BotTalk.h"
#include "Group.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Playerbots.h"
#include "ServerFacade.h"
#include "SharedDefines.h"
#include "WarlockActions.h"

#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace
{
std::mutex assignLock;
std::unordered_map<uint32, uint32> brezAssigned; // druid -> target
std::unordered_map<uint32, time_t> lastCall;

constexpr uint32 SOULSTONE_AURAS[] = {20707, 20762, 20763, 20764, 20765, 27239, 47883};

bool HasSoulstone(Unit* unit)
{
    if (!unit)
        return false;
    for (uint32 id : SOULSTONE_AURAS)
        if (unit->HasAura(id))
            return true;
    return false;
}

bool InSizedGroup(Player* bot)
{
    Group* group = bot ? bot->GetGroup() : nullptr;
    return group && group->GetMembersCount() >= 5;
}

bool IsReal(Player* player)
{
    return IsRealPlayer(player) ||
           (GET_PLAYERBOT_AI(player) && GET_PLAYERBOT_AI(player)->HasGameClientMaster());
}

int Importance(Player* member, PlayerbotAI* ai)
{
    // Higher is more important for SS / brez.
    int score = 0;
    if (IsReal(member))
        score += 100;
    if (ai->IsHeal(member, true))
        score += 50;
    if (ai->IsTank(member, true))
        score += 40;
    if (ai->IsMainTank(member))
        score += 20;
    return score;
}

std::vector<Player*> Members(Player* bot)
{
    std::vector<Player*> out;
    Group* group = bot->GetGroup();
    if (!group)
        return out;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member->IsInWorld())
            out.push_back(member);
    }
    std::sort(out.begin(), out.end(),
              [](Player* a, Player* b) { return a->GetGUID().GetCounter() < b->GetGUID().GetCounter(); });
    return out;
}

std::vector<Player*> Classmates(Player* bot, uint8 cls)
{
    std::vector<Player*> out;
    for (Player* member : Members(bot))
        if (member->getClass() == cls)
            out.push_back(member);
    return out;
}

size_t IndexOf(std::vector<Player*> const& list, Player* bot)
{
    for (size_t i = 0; i < list.size(); ++i)
        if (list[i] == bot)
            return i;
    return 0;
}

bool Announce(Player* bot, uint32 key, std::string const& text, time_t cooldown = 90)
{
    time_t const now = time(nullptr);
    {
        std::lock_guard<std::mutex> g(assignLock);
        auto it = lastCall.find(key);
        if (it != lastCall.end() && now < it->second + cooldown)
            return false;
        lastCall[key] = now;
    }
    return BotTalk::RaidCall(bot, text);
}

Player* PickSoulstoneHealer(Player* bot, PlayerbotAI* ai)
{
    std::vector<Player*> healers;
    for (Player* member : Members(bot))
    {
        if (!member->IsAlive() || HasSoulstone(member) || !ai->IsHeal(member, true))
            continue;
        healers.push_back(member);
    }
    if (healers.empty())
        return nullptr;

    std::sort(healers.begin(), healers.end(), [ai](Player* a, Player* b) {
        int sa = Importance(a, ai);
        int sb = Importance(b, ai);
        if (sa != sb)
            return sa > sb;
        return a->GetGUID().GetCounter() < b->GetGUID().GetCounter();
    });

    std::vector<Player*> locks = Classmates(bot, CLASS_WARLOCK);
    size_t const idx = IndexOf(locks, bot);
    for (size_t n = 0; n < healers.size(); ++n)
    {
        Player* healer = healers[(idx + n) % healers.size()];
        if (!HasSoulstone(healer))
            return healer;
    }
    return healers[idx % healers.size()];
}

Player* PickBrezTarget(Player* bot, PlayerbotAI* ai, bool mustBeDead)
{
    std::vector<Player*> candidates;
    for (Player* member : Members(bot))
    {
        if (member == bot)
            continue;
        if (mustBeDead)
        {
            if (member->IsAlive() || member->isResurrectRequested())
                continue;
        }
        else if (!member->IsAlive())
            continue;
        if (Importance(member, ai) < 40 && !IsReal(member))
            continue;
        candidates.push_back(member);
    }
    if (candidates.empty())
        return nullptr;

    std::sort(candidates.begin(), candidates.end(), [ai](Player* a, Player* b) {
        int sa = Importance(a, ai);
        int sb = Importance(b, ai);
        if (sa != sb)
            return sa > sb;
        return a->GetGUID().GetCounter() < b->GetGUID().GetCounter();
    });

    std::vector<Player*> druids = Classmates(bot, CLASS_DRUID);
    size_t const idx = IndexOf(druids, bot);
    return candidates[idx % candidates.size()];
}
} // namespace

bool RaidSoulstoneHealerAction::isUseful()
{
    return bot->getClass() == CLASS_WARLOCK && InSizedGroup(bot) && !bot->IsInCombat();
}

bool RaidSoulstoneHealerAction::Execute(Event event)
{
    Player* healer = PickSoulstoneHealer(bot, botAI);
    if (!healer)
        return false;

    float const dist = ServerFacade::instance().GetDistance2d(bot, healer);
    if (dist > 28.0f || !bot->IsWithinLOSInMap(healer))
    {
        bot->GetMotionMaster()->MovePoint(0, healer->GetPositionX(), healer->GetPositionY(), healer->GetPositionZ());
        return true;
    }

    UseSoulstoneHealerAction inner(botAI);
    bool const ok = inner.Execute(event);
    if (ok)
        Announce(bot, bot->GetGUID().GetCounter(),
                 "Soulstone on " + healer->GetName() + ".", 120);
    return ok;
}

bool RaidAssignBrezAction::isUseful()
{
    return bot->getClass() == CLASS_DRUID && InSizedGroup(bot) && !bot->IsInCombat();
}

bool RaidAssignBrezAction::Execute(Event /*event*/)
{
    Player* target = PickBrezTarget(bot, botAI, false);
    if (!target)
        return false;

    uint32 const druidLow = bot->GetGUID().GetCounter();
    uint32 const targetLow = target->GetGUID().GetCounter();
    bool changed = false;
    {
        std::lock_guard<std::mutex> g(assignLock);
        auto it = brezAssigned.find(druidLow);
        if (it == brezAssigned.end() || it->second != targetLow)
        {
            brezAssigned[druidLow] = targetLow;
            changed = true;
        }
    }

    if (changed)
        Announce(bot, druidLow + 1,
                 "I'll brez " + target->GetName() + " if they go down.", 180);
    return changed;
}

bool RaidRebirthAction::isUseful()
{
    return bot->getClass() == CLASS_DRUID && InSizedGroup(bot);
}

bool RaidRebirthAction::Execute(Event event)
{
    Player* target = nullptr;
    uint32 assigned = 0;
    {
        std::lock_guard<std::mutex> g(assignLock);
        auto it = brezAssigned.find(bot->GetGUID().GetCounter());
        if (it != brezAssigned.end())
            assigned = it->second;
    }

    if (assigned)
    {
        if (Player* member = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(assigned)))
            if (!member->IsAlive() && !member->isResurrectRequested() && member->GetMapId() == bot->GetMapId())
                target = member;
    }
    if (!target)
        target = PickBrezTarget(bot, botAI, true);
    if (!target)
        return false;

    bot->SetSelection(target->GetGUID());
    float const dist = ServerFacade::instance().GetDistance2d(bot, target);
    if (dist > sPlayerbotAIConfig.SpellDistance || !bot->IsWithinLOSInMap(target))
    {
        bot->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
        return true;
    }

    botAI->DoSpecificAction("caster form", Event(), true);
    bool const ok = botAI->CastSpell("rebirth", target);
    if (ok)
        Announce(bot, bot->GetGUID().GetCounter() + 2,
                 "Rebirthing " + target->GetName() + ".", 30);
    return ok;
}
