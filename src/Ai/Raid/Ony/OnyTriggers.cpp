/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "OnyTriggers.h"
#include "EncounterHelpers.h"
#include "GenericTriggers.h"
#include "OnyHelpers.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "SpellAuras.h"

using namespace OnyxiaHelpers;

OnyxiaDeepBreathTrigger::OnyxiaDeepBreathTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony deep breath warning") {}

bool OnyxiaDeepBreathTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || !boss->HasUnitState(UNIT_STATE_CASTING))
        return false;

    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!currentSpell || !currentSpell->m_spellInfo)
        return false;

    return IsDeepBreathSpell(currentSpell->m_spellInfo->Id);
}

OnyxiaNearTailTrigger::OnyxiaNearTailTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony near tail") {}

bool OnyxiaNearTailTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || botAI->IsTank(bot) || !IsOnyxiaGrounded(boss))
        return false;

    if (!IsInFrontOrTail(boss, bot))
        return false;

    // Ranged/healers only need to leave the cones; melee also leave if they
    // drifted onto the head or tail even at swing range.
    if (PlayerbotAI::IsRanged(bot) && bot->GetDistance(boss) > RANGED_SIDE_DISTANCE)
        return false;

    return true;
}

RaidOnyxiaFireballSplashTrigger::RaidOnyxiaFireballSplashTrigger(PlayerbotAI* botAI)
    : Trigger(botAI, "ony fireball splash incoming")
{
}

bool RaidOnyxiaFireballSplashTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!IsOnyxiaAirborne(boss))
        return false;

    GuidVector nearbyUnits = AI_VALUE(GuidVector, "nearest friendly players");
    for (ObjectGuid guid : nearbyUnits)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || unit == bot || !unit->IsAlive())
            continue;

        if (bot->GetDistance(unit) < SPREAD_DISTANCE)
            return true;
    }

    return false;
}

RaidOnyxiaWhelpsSpawnTrigger::RaidOnyxiaWhelpsSpawnTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony whelps spawn") {}

bool RaidOnyxiaWhelpsSpawnTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    // Melee and tanks clump whelps for AoE. Ranged stay on Onyxia; healers heal.
    if (botAI->IsHeal(bot) || (PlayerbotAI::IsRanged(bot) && !botAI->IsTank(bot)))
        return false;

    if (!IsOnyxiaAirborne(boss) && boss->GetHealthPct() > 40.0f)
        return false;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    for (ObjectGuid guid : targets)
    {
        Creature* unit = botAI->GetCreature(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == NPC_ONYXIAN_WHELP)
            return true;
    }

    return false;
}

OnyxiaAvoidEggsTrigger::OnyxiaAvoidEggsTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony avoid eggs") {}

bool OnyxiaAvoidEggsTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    return IsNearWhelpCave(bot);
}

OnyxiaTankWallTrigger::OnyxiaTankWallTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony tank wall") {}

bool OnyxiaTankWallTrigger::IsActive()
{
    if (!PlayerbotAI::IsMainTank(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!IsOnyxiaGrounded(boss))
        return false;

    return bot->GetExactDist2d(TANK_WALL_X, TANK_WALL_Y) > TANK_WALL_TOLERANCE;
}

OnyxiaBellowingRoarTrigger::OnyxiaBellowingRoarTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony bellowing roar") {}

bool OnyxiaBellowingRoarTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || boss->IsFlying())
        return false;

    if (boss->GetHealthPct() > 41.0f)
        return false;

    if (bot->getClass() != CLASS_WARRIOR && bot->getClass() != CLASS_PRIEST && bot->getClass() != CLASS_SHAMAN)
        return false;

    if (boss->HasUnitState(UNIT_STATE_CASTING))
    {
        Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (currentSpell && currentSpell->m_spellInfo && currentSpell->m_spellInfo->Id == SPELL_BELLOWINGROAR)
            return true;
    }

    return bot->HasAuraType(SPELL_AURA_MOD_FEAR);
}

OnyxiaLavaTrigger::OnyxiaLavaTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony lava") {}

bool OnyxiaLavaTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || boss->IsFlying())
        return false;

    LiquidData const& liquid = bot->GetLiquidData();
    if ((liquid.Flags & MAP_LIQUID_TYPE_MAGMA) &&
        (liquid.Status & (LIQUID_MAP_WATER_WALK | LIQUID_MAP_IN_WATER | LIQUID_MAP_UNDER_WATER)))
        return true;

    if (bot->FindNearestCreature(NPC_ONYXIA_TRIGGER, ERUPTION_DISTANCE, true))
        return true;

    std::vector<Position> eruptions =
        EncounterHelpers::GetDynamicObjectPositions(bot, ERUPTION_DISTANCE, SPELL_ERUPTION);
    return !eruptions.empty();
}

OnyxiaTankTauntTrigger::OnyxiaTankTauntTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony tank taunt") {}

bool OnyxiaTankTauntTrigger::IsActive()
{
    if (!PlayerbotAI::IsTank(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!IsOnyxiaGrounded(boss))
        return false;

    Unit* victim = boss->GetVictim();
    if (!victim || victim == bot)
        return false;

    Player* victimPlayer = victim->ToPlayer();
    if (PlayerbotAI::IsMainTank(bot))
        return true;

    return !victimPlayer || !PlayerbotAI::IsTank(victimPlayer);
}
