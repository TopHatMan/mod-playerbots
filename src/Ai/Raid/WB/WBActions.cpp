/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WBActions.h"
#include "EncounterHelpers.h"
#include "Playerbots.h"
#include "WBHelpers.h"
#include <initializer_list>

using namespace WorldBossHelpers;

bool WbAzuregosSpreadAction::Execute(Event /*event*/)
{
    if (!FindAzuregos(bot))
        return false;
    return MoveFromGroup(SPREAD_DISTANCE + 1.0f);
}

bool WbAzuregosMoveFromStormAction::Execute(Event /*event*/)
{
    Unit* boss = FindAzuregos(bot);
    if (!boss)
        return false;

    bot->CastStop();

    std::vector<Position> storms = EncounterHelpers::GetDynamicObjectPositions(bot, 12.0f, SPELL_MANA_STORM);
    if (!storms.empty())
        return FleePosition(storms.front(), 12.0f);

    return Move(frand(0.0f, 2.0f * static_cast<float>(M_PI)), 10.0f);
}

bool WbAzuregosStopForReflectAction::Execute(Event /*event*/)
{
    Unit* boss = FindAzuregos(bot);
    if (!boss || !boss->HasAura(SPELL_REFLECT))
        return false;

    bot->CastStop();
    bot->AttackStop();
    return true;
}

bool WbAzuregosTankTauntAction::Execute(Event event)
{
    Unit* boss = FindAzuregos(bot);
    if (!boss)
        return false;

    if (bot->GetVictim() != boss)
        return Attack(boss);

    return botAI->DoSpecificAction("taunt spell", event, true);
}

bool WbAzuregosMoveToFlankAction::Execute(Event /*event*/)
{
    Unit* boss = FindAzuregos(bot);
    if (!boss)
        return false;

    float const bossFacing = boss->GetOrientation();
    float const sideAngle = bossFacing + static_cast<float>(M_PI_2);
    float const dist = PlayerbotAI::IsRanged(bot) ? 25.0f : 5.5f;
    float const x = boss->GetPositionX() + dist * cos(sideAngle);
    float const y = boss->GetPositionY() + dist * sin(sideAngle);
    if (bot->GetExactDist2d(x, y) < 3.0f)
        return false;
    return MoveTo(boss->GetMapId(), x, y, boss->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool WbKazzakIsolateAction::Execute(Event /*event*/)
{
    if (!HasMarkOfKazzak(bot))
        return false;

    float fleeX = 0.0f;
    float fleeY = 0.0f;
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || member == bot || !member->IsAlive() || member->GetMapId() != bot->GetMapId())
                continue;
            float dist = bot->GetDistance2d(member);
            if (dist < KAZZAK_MARK_ISOLATE_DISTANCE && dist > 0.1f)
            {
                float weight = 1.0f - dist / KAZZAK_MARK_ISOLATE_DISTANCE;
                fleeX += (bot->GetPositionX() - member->GetPositionX()) / dist * weight;
                fleeY += (bot->GetPositionY() - member->GetPositionY()) / dist * weight;
            }
        }
    }

    float len = sqrt(fleeX * fleeX + fleeY * fleeY);
    if (len < 0.1f)
        return true;

    fleeX /= len;
    fleeY /= len;
    bot->CastStop();
    return MoveTo(bot->GetMapId(), bot->GetPositionX() + 8.0f * fleeX, bot->GetPositionY() + 8.0f * fleeY,
                  bot->GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_FORCED);
}

bool WbKazzakSpreadAction::Execute(Event /*event*/)
{
    if (!FindKazzak(bot))
        return false;
    return MoveFromGroup(KAZZAK_SPREAD_DISTANCE + 1.0f);
}

bool WbKazzakDispelAction::Execute(Event event)
{
    auto tryDispel = [&](std::initializer_list<std::string> names) -> bool
    {
        for (std::string const& name : names)
            if (botAI->DoSpecificAction(name, event, true))
                return true;
        return false;
    };

    switch (bot->getClass())
    {
        case CLASS_PRIEST:
            return tryDispel({"dispel magic", "dispel magic on party"});
        case CLASS_PALADIN:
            return tryDispel({"cleanse magic", "cleanse magic on party"});
        case CLASS_SHAMAN:
            return tryDispel({"purge"});
        case CLASS_MAGE:
            return tryDispel({"remove curse", "remove curse on party"});
        case CLASS_DRUID:
            return tryDispel({"remove curse on party"});
        default:
            return false;
    }
}

bool WbDragonFlankAction::Execute(Event /*event*/)
{
    Unit* boss = FindEmeraldDragon(bot);
    if (!boss)
        return false;

    float const bossFacing = boss->GetOrientation();
    float const left = bossFacing + static_cast<float>(M_PI_2);
    float const right = bossFacing - static_cast<float>(M_PI_2);
    float const dist = PlayerbotAI::IsRanged(bot) ? 25.0f : 5.5f;
    float const leftX = boss->GetPositionX() + dist * cos(left);
    float const leftY = boss->GetPositionY() + dist * sin(left);
    float const rightX = boss->GetPositionX() + dist * cos(right);
    float const rightY = boss->GetPositionY() + dist * sin(right);
    bool const useLeft = bot->GetExactDist2d(leftX, leftY) < bot->GetExactDist2d(rightX, rightY);
    float const x = useLeft ? leftX : rightX;
    float const y = useLeft ? leftY : rightY;
    if (bot->GetExactDist2d(x, y) < 3.0f)
        return false;
    return MoveTo(boss->GetMapId(), x, y, boss->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool WbDragonSpreadAction::Execute(Event /*event*/)
{
    if (!FindEmeraldDragon(bot))
        return false;
    return MoveFromGroup(SPREAD_DISTANCE + 1.0f);
}

bool WbDragonTankSwapAction::Execute(Event event)
{
    Unit* boss = FindEmeraldDragon(bot);
    if (!boss)
        return false;
    if (bot->GetVictim() != boss)
        return Attack(boss);
    return botAI->DoSpecificAction("taunt spell", event, true);
}

bool WbDragonMoveFromFogAction::Execute(Event /*event*/)
{
    Creature* fog = bot->FindNearestCreature(NPC_DREAM_FOG, FOG_DISTANCE, true);
    if (!fog)
        return false;
    bot->CastStop();
    return FleePosition(fog->GetPosition(), FOG_DISTANCE);
}

bool WbEmerissAvoidMushroomAction::Execute(Event /*event*/)
{
    if (!FindEmeraldDragon(bot) || FindEmeraldDragon(bot)->GetEntry() != NPC_EMERISS)
        return false;

    Unit* corpse = nullptr;
    float best = MUSHROOM_CORPSE_DISTANCE;
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || member == bot || member->IsAlive() || member->GetMapId() != bot->GetMapId())
                continue;
            float dist = bot->GetDistance(member);
            if (dist < best)
            {
                best = dist;
                corpse = member;
            }
        }
    }
    if (!corpse)
        return false;
    bot->CastStop();
    return FleePosition(corpse->GetPosition(), MUSHROOM_CORPSE_DISTANCE);
}

bool WbEmerissDispelAction::Execute(Event event)
{
    switch (bot->getClass())
    {
        case CLASS_DRUID:
            return botAI->DoSpecificAction("abolish poison", event, true) ||
                   botAI->DoSpecificAction("abolish poison on party", event, true) ||
                   botAI->DoSpecificAction("cure poison", event, true) ||
                   botAI->DoSpecificAction("cure poison on party", event, true);
        case CLASS_SHAMAN:
            return botAI->DoSpecificAction("cure poison", event, true) ||
                   botAI->DoSpecificAction("cure poison on party", event, true);
        case CLASS_PALADIN:
            return botAI->DoSpecificAction("cleanse poison", event, true) ||
                   botAI->DoSpecificAction("cleanse poison on party", event, true);
        default:
            return false;
    }
}

bool WbLethonKillShadeAction::Execute(Event /*event*/)
{
    Creature* nearest = bot->FindNearestCreature(NPC_SPIRIT_SHADE, 80.0f, true);
    if (!nearest || !nearest->IsAlive())
        return false;

    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ nearest->GetGUID() });
    if (bot->GetVictim() == nearest)
        return false;
    return Attack(nearest);
}
