/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "OnyActions.h"
#include "EncounterHelpers.h"
#include "GenericSpellActions.h"
#include "LastMovementValue.h"
#include "MovementActions.h"
#include "OnyHelpers.h"
#include "Playerbots.h"
#include "PositionAction.h"

using namespace OnyxiaHelpers;

bool RaidOnyxiaMoveToSideAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || !IsOnyxiaGrounded(boss))
        return false;

    float const bossFacing = boss->GetOrientation();
    float const leftAngle = bossFacing + static_cast<float>(M_PI_2);
    float const rightAngle = bossFacing - static_cast<float>(M_PI_2);
    float const sideDist = PlayerbotAI::IsRanged(bot) ? RANGED_SIDE_DISTANCE : MELEE_SIDE_DISTANCE;

    float const leftX = boss->GetPositionX() + sideDist * cos(leftAngle);
    float const leftY = boss->GetPositionY() + sideDist * sin(leftAngle);
    float const rightX = boss->GetPositionX() + sideDist * cos(rightAngle);
    float const rightY = boss->GetPositionY() + sideDist * sin(rightAngle);

    bool const useLeft = bot->GetExactDist2d(leftX, leftY) < bot->GetExactDist2d(rightX, rightY);
    float const targetX = useLeft ? leftX : rightX;
    float const targetY = useLeft ? leftY : rightY;

    if (bot->GetExactDist2d(targetX, targetY) < 2.0f)
        return false;

    return MoveTo(boss->GetMapId(), targetX, targetY, boss->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool RaidOnyxiaSpreadOutAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!IsOnyxiaAirborne(boss))
        return false;

    return MoveFromGroup(SPREAD_DISTANCE + 1.0f);
}

bool RaidOnyxiaMoveToSafeZoneAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!currentSpell || !currentSpell->m_spellInfo || !IsDeepBreathSpell(currentSpell->m_spellInfo->Id))
        return false;

    uint32 const spellId = currentSpell->m_spellInfo->Id;
    std::vector<SafeZone> safeZones = GetSafeZonesForBreath(spellId);
    if (safeZones.empty())
        return false;

    SafeZone* bestZone = nullptr;
    float bestDist = std::numeric_limits<float>::max();
    for (auto& zone : safeZones)
    {
        float dist = bot->GetExactDist2d(zone.pos.GetPositionX(), zone.pos.GetPositionY());
        if (dist < bestDist)
        {
            bestDist = dist;
            bestZone = &zone;
        }
    }

    if (!bestZone)
        return false;

    bot->AttackStop();
    bot->CastStop();

    if (bot->IsWithinDist2d(bestZone->pos.GetPositionX(), bestZone->pos.GetPositionY(), bestZone->radius))
    {
        botAI->SetNextCheckDelay(DEEP_BREATH_HOLD_MS);
        return true;
    }

    return MoveTo(bot->GetMapId(), bestZone->pos.GetPositionX(), bestZone->pos.GetPositionY(),
                  bestZone->pos.GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
}

bool RaidOnyxiaKillWhelpsAction::Execute(Event /*event*/)
{
    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (currentTarget && currentTarget->GetEntry() == NPC_ONYXIAN_WHELP)
        return false;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    for (ObjectGuid guid : targets)
    {
        Creature* unit = botAI->GetCreature(guid);
        if (!unit || !unit->IsAlive() || !unit->IsInWorld())
            continue;

        if (unit->GetEntry() == NPC_ONYXIAN_WHELP)
            return Attack(unit);
    }
    return false;
}

bool OnyxiaAvoidEggsAction::Execute(Event /*event*/)
{
    Position const botPos(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());

    float x;
    float y;
    if (botPos.GetExactDist2d(-35.0f, -165.0f) <= EGG_AVOID_RADIUS)
    {
        x = -10.0f;
        y = -180.0f;
    }
    else if (botPos.GetExactDist2d(-35.0f, -260.0f) <= EGG_AVOID_RADIUS)
    {
        x = -16.0f;
        y = -250.0f;
    }
    else
        return false;

    return MoveTo(bot->GetMapId(), x, y, bot->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool OnyxiaTankWallAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!IsOnyxiaGrounded(boss) || !PlayerbotAI::IsMainTank(bot))
        return false;

    if (bot->GetExactDist2d(TANK_WALL_X, TANK_WALL_Y) <= TANK_WALL_TOLERANCE)
        return false;

    return MoveTo(bot->GetMapId(), TANK_WALL_X, TANK_WALL_Y, TANK_WALL_Z, false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool OnyxiaAntiFearAction::Execute(Event event)
{
    switch (bot->getClass())
    {
        case CLASS_WARRIOR:
            return botAI->DoSpecificAction("berserker rage", event, true);
        case CLASS_PRIEST:
        {
            Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
            Unit* tank = boss ? boss->GetVictim() : nullptr;
            if (!tank)
                tank = bot;
            if (botAI->HasAura("fear ward", tank))
                return false;
            return botAI->CastSpell("fear ward", tank);
        }
        case CLASS_SHAMAN:
            return botAI->DoSpecificAction("tremor totem", event, true);
        default:
            return false;
    }
}

bool OnyxiaMoveFromLavaAction::Execute(Event /*event*/)
{
    bot->CastStop();

    if (Creature* trigger = bot->FindNearestCreature(NPC_ONYXIA_TRIGGER, ERUPTION_DISTANCE, true))
        return FleePosition(trigger->GetPosition(), ERUPTION_DISTANCE);

    std::vector<Position> eruptions =
        EncounterHelpers::GetDynamicObjectPositions(bot, ERUPTION_DISTANCE, SPELL_ERUPTION);
    if (!eruptions.empty())
        return FleePosition(eruptions.front(), ERUPTION_DISTANCE);

    Unit* dryTarget = nullptr;
    float bestDist = 100.0f;
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || member == bot || !member->IsAlive() || member->GetMapId() != bot->GetMapId())
                continue;
            if (member->GetLiquidData().Flags & MAP_LIQUID_TYPE_MAGMA)
                continue;
            float dist = bot->GetDistance(member);
            if (dist < bestDist)
            {
                bestDist = dist;
                dryTarget = member;
            }
        }
    }

    if (!dryTarget)
        return Move(frand(0.0f, 2.0f * static_cast<float>(M_PI)), ERUPTION_DISTANCE);

    return MoveTo(dryTarget->GetMapId(), dryTarget->GetPositionX(), dryTarget->GetPositionY(),
                  dryTarget->GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_FORCED);
}

bool OnyxiaTankTauntAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    if (bot->GetVictim() != boss)
        return Attack(boss);

    return botAI->DoSpecificAction("taunt spell", event, true);
}
