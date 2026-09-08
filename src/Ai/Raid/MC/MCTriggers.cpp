/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MCTriggers.h"
#include "MCHelpers.h"
#include "SharedDefines.h"
#include "SpellAuras.h"

using namespace MoltenCoreHelpers;

bool McLivingBombDebuffTrigger::IsActive()
{
    // No check for Baron Geddon, because bots may have the bomb even after Geddon died.
    return bot->HasAura(SPELL_LIVING_BOMB);
}

bool McBaronGeddonInfernoTrigger::IsActive()
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "baron geddon"))
        return boss->HasAura(SPELL_INFERNO);
    return false;
}

bool McShazzrahRangedTrigger::IsActive()
{
    // Only fire inside Arcane Explosion range. The move-away action no-ops
    // beyond it, so an unconditional trigger makes every already-safe ranged
    // bot attempt a failing move each tick.
    if (!PlayerbotAI::IsRanged(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "shazzrah");
    return boss && bot->GetDistance2d(boss) < ARCANE_EXPLOSION_DISTANCE;
}

bool McInLavaTrigger::IsActive()
{
    LiquidData const& liquid = bot->GetLiquidData();
    return (liquid.Flags & MAP_LIQUID_TYPE_MAGMA) &&
           (liquid.Status & (LIQUID_MAP_WATER_WALK | LIQUID_MAP_IN_WATER | LIQUID_MAP_UNDER_WATER));
}

bool McGolemaggMagmaSplashTrigger::IsActive()
{
    if (PlayerbotAI::IsTank(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "golemagg the incinerator");
    if (!boss)
        return false;

    // Below 10% the fight is a burn race (Earthquake pulses, Core Ragers
    // converge): stack management stops mattering, melee return to dps.
    if (boss->GetHealthPct() <= 10.0f)
        return false;

    Aura* splash = bot->GetAura(SPELL_MAGMA_SPLASH);
    if (!splash || splash->GetStackAmount() < MAGMA_SPLASH_BACK_OFF_STACKS)
        return false;

    // Only fire while still inside swing range; once the bot has backed off,
    // the multiplier keeps it from re-engaging until the stack expires.
    return bot->GetDistance2d(boss) < MAGMA_SPLASH_BACK_OFF_DISTANCE;
}

bool McGolemaggMarkBossTrigger::IsActive()
{
    // any tank may mark the boss
    return PlayerbotAI::IsTank(bot) && AI_VALUE2(Unit*, "find target", "golemagg the incinerator");
}

bool McGolemaggIsMainTankTrigger::IsActive()
{
    return PlayerbotAI::IsMainTank(bot) && AI_VALUE2(Unit*, "find target", "golemagg the incinerator");
}

bool McGolemaggIsAssistTankTrigger::IsActive()
{
    return PlayerbotAI::IsAssistTank(bot) && AI_VALUE2(Unit*, "find target", "golemagg the incinerator");
}

bool McGolemaggIsHealerTrigger::IsActive()
{
    return PlayerbotAI::IsHeal(bot) && AI_VALUE2(Unit*, "find target", "golemagg the incinerator");
}

bool McMajordomoTankAddsTrigger::IsActive()
{
    if (!PlayerbotAI::IsTank(bot) || !IsMajordomoEncounterActive(bot))
        return false;
    if (PlayerbotAI::IsMainTank(bot))
        return FindMajordomo(bot) || SelectMajordomoTankTarget(bot);
    return SelectMajordomoTankTarget(bot) != nullptr;
}

bool McMajordomoFocusAddsTrigger::IsActive()
{
    return PlayerbotAI::IsDps(bot) && SelectMajordomoKillTarget(bot) != nullptr;
}

bool McMajordomoPolymorphHealerTrigger::IsActive()
{
    return bot->getClass() == CLASS_MAGE && IsMajordomoEncounterActive(bot) &&
           SelectMajordomoSheepTarget(bot) != nullptr;
}

bool McMajordomoAvoidBlastWaveTrigger::IsActive()
{
    if (PlayerbotAI::IsTank(bot) || !IsMajordomoEncounterActive(bot))
        return false;

    Unit* elite = FindNearestMajordomoElite(bot);
    if (!elite)
        return false;

    Unit* killTarget = SelectMajordomoKillTarget(bot);
    if (killTarget && killTarget->GetEntry() == NPC_FLAMEWAKER_ELITE && killTarget == elite)
        return false;

    return bot->GetDistance2d(elite) < MAJORDOMO_BLAST_WAVE_DISTANCE;
}

bool McMajordomoStopForShieldTrigger::IsActive()
{
    Unit* boss = FindMajordomo(bot);
    if (!boss)
        return false;

    if (boss->HasAura(SPELL_MAGIC_REFLECTION) && PlayerbotAI::IsRanged(bot) && PlayerbotAI::IsDps(bot))
        return true;
    if (boss->HasAura(SPELL_DAMAGE_REFLECTION) && PlayerbotAI::IsMelee(bot) && !PlayerbotAI::IsTank(bot))
        return true;
    return false;
}

bool McMajordomoRunToRaidTrigger::IsActive()
{
    if (!IsMajordomoEncounterActive(bot))
        return false;

    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || member == bot || !member->IsAlive() || member->GetMapId() != bot->GetMapId())
                continue;
            if (bot->GetDistance(member) <= MAJORDOMO_COALS_REJOIN_DISTANCE)
                return false;
        }
        return true;
    }
    return false;
}

bool McCoreHoundMarkTrigger::IsActive()
{
    if (!PlayerbotAI::IsDps(bot))
        return false;

    CoreHoundPackState const pack = GetCoreHoundPackState(bot);
    if (!pack.HasPack() || pack.IsBurnPhase())
        return false;

    return SelectCoreHoundBalanceTarget(bot) != nullptr;
}

bool McCoreHoundHoldTrigger::IsActive()
{
    if (!PlayerbotAI::IsDps(bot))
        return false;

    CoreHoundPackState const pack = GetCoreHoundPackState(bot);
    if (!pack.HasPack() || pack.IsBurnPhase())
        return false;

    // Unassigned DPS: this hound is already at the execute floor, or the
    // healthiest band already has its DPS cap. Stop hitting it.
    return SelectCoreHoundBalanceTarget(bot) == nullptr;
}

bool McCoreHoundBurnTrigger::IsActive()
{
    if (PlayerbotAI::IsHeal(bot))
        return false;

    return GetCoreHoundPackState(bot).IsBurnPhase();
}
