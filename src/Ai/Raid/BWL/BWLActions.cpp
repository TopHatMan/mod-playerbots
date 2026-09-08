/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BWLActions.h"
#include "BWLHelpers.h"
#include "EncounterHelpers.h"
#include "GameObject.h"
#include "Pet.h"
#include "Playerbots.h"
#include "RtiTargetValue.h"
#include <initializer_list>

using namespace BlackwingLairHelpers;
using namespace EncounterHelpers;

static constexpr float INCREMENTAL_MOVE_STEP_DISTANCE = 3.0f;

static constexpr float RAZORGORE_CONE_RADIUS = 15.0f;
static constexpr float RAZORGORE_CONE_ARC = M_PI;
static constexpr float RAZORGORE_BACK_FUZZ_ARC = M_PI_4;
static constexpr float RAZORGORE_RANGE_DISTANCE = 15.0f;
static constexpr float RAZORGORE_MELEE_DISTANCE = 3.0f;

static constexpr float VAELASTRASZ_REPULSION_RANGE = 30.0f;
static constexpr float VAELASTRASZ_TAIL_SWEEP_RECOVERY_RANGE = 30.0f;
static constexpr float VAELASTRASZ_RANGED_BIAS_THRESHOLD = 25.0f;
static constexpr float VAELASTRASZ_RANGED_BIAS_FLEE_WEIGHT = 0.7f;
static constexpr float VAELASTRASZ_RANGED_BIAS_BOSS_WEIGHT = 1.0f - VAELASTRASZ_RANGED_BIAS_FLEE_WEIGHT;

// General

bool BwlOnyxiaScaleCloakAuraCheckAction::Execute(Event /*event*/)
{
    bot->AddAura(static_cast<uint32>(BlackwingLairSpells::SPELL_ONYXIA_SCALE_CLOAK), bot);
    return true;
}

bool BwlOnyxiaScaleCloakAuraCheckAction::isUseful()
{
    return !bot->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_ONYXIA_SCALE_CLOAK));
}

bool BwlTurnOffSuppressionDeviceAction::Execute(Event /*event*/)
{
    GuidVector gos = AI_VALUE(GuidVector, "nearest game objects");
    for (auto i = gos.begin(); i != gos.end(); ++i)
    {
        GameObject* go = botAI->GetGameObject(*i);
        if (IsActiveSuppressionDeviceInRange(go, bot))
        {
            go->SetGoState(GO_STATE_ACTIVE);
        }
    }
    return true;
}

// Razorgore the Untamed

bool BwlRazorgoreAvoidAoeAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorgore the untamed");
    if (!boss)
        return false;

    // The current target should not move.
    // Also prevents non-tanks from rotating the boss if they have aggro.
    if (boss->GetVictim() == bot)
        return false;

    float distance = bot->GetDistance2d(boss);

    // Bot is too close and standing in Razorgore's frontal cone
    if (distance <= RAZORGORE_CONE_RADIUS && boss->HasInArc(RAZORGORE_CONE_ARC, bot))
    {
        // Target position: directly behind the boss with a small random fuzz
        // to prevent all bots stacking on the same spot
        float moveAngle = boss->GetOrientation() + M_PI + frand(-RAZORGORE_BACK_FUZZ_ARC, RAZORGORE_BACK_FUZZ_ARC);
        float razorgoreRadius = PlayerbotAI::IsRanged(bot) ? RAZORGORE_RANGE_DISTANCE : RAZORGORE_MELEE_DISTANCE;

        float targetMoveX = boss->GetPositionX() + razorgoreRadius * cos(moveAngle);
        float targetMoveY = boss->GetPositionY() + razorgoreRadius * sin(moveAngle);

        // Move incrementally toward the target. Allows course correction.
        float dX = targetMoveX - bot->GetPositionX();
        float dY = targetMoveY - bot->GetPositionY();
        float dist = sqrt(dX * dX + dY * dY);
        if (dist == 0.0f)
            dist = 0.1f;

        float moveX = bot->GetPositionX() + (dX / dist) * INCREMENTAL_MOVE_STEP_DISTANCE;
        float moveY = bot->GetPositionY() + (dY / dist) * INCREMENTAL_MOVE_STEP_DISTANCE;

        return MoveTo(boss->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false,
                      false, false, MovementPriority::MOVEMENT_COMBAT, true);
    }

    // Ranged outside the cone but too close => back off to avoid War Stomp
    if (PlayerbotAI::IsRanged(bot))
    {
        float distToTravel = RAZORGORE_RANGE_DISTANCE - distance;
        if (distToTravel > 0)
            return MoveAway(boss, distToTravel);
    }

    return false;
}

bool BwlRazorgoreMarkBossAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorgore the untamed");
    if (!boss)
        return false;

    if (AreRazorgoreEggsAlive(botAI))
    {
        if (IsRazorgoreOffTank(bot))
        {
            if (MarkTargetWithMoon(bot, boss))
                return true;

            SetRtiTarget(botAI, "moon");

            if (AI_VALUE(Unit*, "current target") != boss)
                return Attack(boss);
        }
    }
    else
    {
        ClearTargetIcon(bot, RtiTargetValue::moonIndex);
    }

    return false;
}

bool BwlRazorgoreMarkBossAction::isUseful()
{
    return PlayerbotAI::IsTank(bot);
}

bool BwlRazorgoreControlAction::isUseful()
{
    return IsRazorgoreController(bot) && AreRazorgoreEggsAlive(botAI);
}

bool BwlRazorgoreControlAction::Execute(Event /*event*/)
{
    if (bot->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_MIND_EXHAUSTION)))
        return false;

    if (Unit* charm = bot->GetCharm())
    {
        if (charm->GetEntry() != static_cast<uint32>(BlackwingLairNPCs::NPC_RAZORGORE))
            return false;

        GameObject* egg = FindNearestRazorgoreEgg(bot);
        if (!egg)
            return false;

        constexpr float eggRange = 8.0f;
        if (charm->GetDistance(egg) > eggRange)
        {
            charm->GetMotionMaster()->Clear();
            charm->GetMotionMaster()->MovePoint(0, egg->GetPositionX(), egg->GetPositionY(), egg->GetPositionZ());
            return true;
        }

        if (charm->HasUnitState(UNIT_STATE_CASTING))
            return true;

        charm->CastSpell(egg, static_cast<uint32>(BlackwingLairSpells::SPELL_EGG_DESTROY), false);
        return true;
    }

    GameObject* orb = FindOrbOfDomination(bot);
    if (!orb)
        return false;

    if (bot->GetDistance(orb) > INTERACTION_DISTANCE)
        return MoveTo(orb->GetMapId(), orb->GetPositionX(), orb->GetPositionY(), orb->GetPositionZ(),
                      false, false, false, false, MovementPriority::MOVEMENT_COMBAT);

    if (Pet* pet = bot->GetPet())
        bot->RemovePet(pet, PET_SAVE_NOT_IN_SLOT);

    orb->Use(bot);
    return true;
}

bool BwlRazorgoreFocusAddsAction::Execute(Event event)
{
    Unit* add = SelectRazorgoreAddTarget(bot);
    if (!add)
        return false;

    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ add->GetGUID() });
    if (bot->GetVictim() != add)
        return Attack(add);

    if (PlayerbotAI::IsTank(bot) && add->GetVictim() != bot)
        return botAI->DoSpecificAction("taunt spell", event, true);

    return false;
}

bool BwlRazorgoreAvoidConflagrationAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorgore the untamed");
    if (!boss)
        return false;

    Unit* victim = boss->GetVictim();
    if (!victim || !victim->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_CONFLAGRATION)))
        return false;

    if (victim == bot)
        return false;

    float const dist = bot->GetDistance2d(victim);
    if (dist >= RAZORGORE_CONFLAG_DISTANCE)
        return false;

    bot->CastStop();
    return MoveAway(victim, RAZORGORE_CONFLAG_DISTANCE - dist);
}

// Vaelastrasz the Corrupt

bool BwlVaelastraszMoveAwayAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "vaelastrasz the corrupt");

    // Current target with Burning Adrenaline stays put when Vaelastrasz is alive.
    if (boss && boss->IsAlive() && boss->GetVictim() == bot)
        return false;

    float fleeX, fleeY;
    if (!CalculateFleeDirection(boss, fleeX, fleeY))
        return false;

    return MoveAlongFleeDirection(boss, fleeX, fleeY);
}

bool BwlVaelastraszMoveAwayAction::CalculateFleeDirection(Unit const* boss, float& fleeX, float& fleeY) const
{
    fleeX = 0.0f;
    fleeY = 0.0f;
    bool bossDead = !boss || !boss->IsAlive();

    if (Group const* group = bot->GetGroup())
    {
        for (GroupReference const* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* p = gref->GetSource();
            if (!p || p == bot || !p->IsAlive() || p->GetMapId() != bot->GetMapId())
                continue;

            // Boss alive: ignore other Burning Adrenaline bots so they can group together
            if (!bossDead && p->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BURNING_ADRENALINE)))
                continue;

            // Compute a flee direction via weighted repulsion from other bots.
            // Weighted repulsion: closer = stronger push (linear falloff to 20y)
            float dist = bot->GetDistance2d(p);
            if (dist < VAELASTRASZ_REPULSION_RANGE && dist > 0.1f)
            {
                float weight = 1.0f - dist / VAELASTRASZ_REPULSION_RANGE;
                fleeX += (bot->GetPositionX() - p->GetPositionX()) / dist * weight;
                fleeY += (bot->GetPositionY() - p->GetPositionY()) / dist * weight;
            }
        }
    }

    float fleeLen = sqrt(fleeX * fleeX + fleeY * fleeY);

    // No repulsion
    if (fleeLen < 0.1f)
    {
        // Tail Sweep recovery: move back toward boss
        if (!bossDead && bot->GetDistance2d(boss) > VAELASTRASZ_TAIL_SWEEP_RECOVERY_RANGE)
        {
            float toBossX = boss->GetPositionX() - bot->GetPositionX();
            float toBossY = boss->GetPositionY() - bot->GetPositionY();
            float dist = sqrt(toBossX * toBossX + toBossY * toBossY);
            if (dist > 0.1f)
            {
                fleeX = toBossX / dist;
                fleeY = toBossY / dist;
                return true;
            }
        }
        return false;
    }

    fleeX /= fleeLen;
    fleeY /= fleeLen;

    // Ranged: blend flee direction toward boss when >25y to stay in cast range
    if (!bossDead && PlayerbotAI::IsRanged(bot))
    {
        float bossDist = bot->GetDistance2d(boss);
        if (bossDist > VAELASTRASZ_RANGED_BIAS_THRESHOLD)
        {
            float toBossX = (boss->GetPositionX() - bot->GetPositionX()) / bossDist;
            float toBossY = (boss->GetPositionY() - bot->GetPositionY()) / bossDist;
            if (fleeX * toBossX + fleeY * toBossY < 0.0f)
            {
                fleeX = fleeX * VAELASTRASZ_RANGED_BIAS_FLEE_WEIGHT + toBossX * VAELASTRASZ_RANGED_BIAS_BOSS_WEIGHT;
                fleeY = fleeY * VAELASTRASZ_RANGED_BIAS_FLEE_WEIGHT + toBossY * VAELASTRASZ_RANGED_BIAS_BOSS_WEIGHT;
                float len = sqrt(fleeX * fleeX + fleeY * fleeY);
                fleeX /= len;
                fleeY /= len;
            }
        }
    }

    return true;
}

bool BwlVaelastraszMoveAwayAction::MoveAlongFleeDirection(Unit const* boss, float fleeX, float fleeY)
{
    bool bossDead = !boss || !boss->IsAlive();
    float baseAngle = atan2(fleeY, fleeX);

    // Pass 1: ranged try LOS positions first
    // Pass 2: everyone, no LOS req
    for (bool requireLos : {true, false})
    {
        // Ranged bots try LOS positions first, then fall back to no-LOS.
        if (requireLos && (!PlayerbotAI::IsRanged(bot) || bossDead))
            continue;

        // Try 5 angles (0°, ±45°, ±90°) around the flee direction to slide along walls.
        static constexpr float angleOffsets[] = {0.0f, M_PI_4, -M_PI_4, M_PI_2, -M_PI_2};
        for (float offset : angleOffsets)
        {
            float angle = baseAngle + offset;
            float targetX = bot->GetPositionX() + INCREMENTAL_MOVE_STEP_DISTANCE * cos(angle);
            float targetY = bot->GetPositionY() + INCREMENTAL_MOVE_STEP_DISTANCE * sin(angle);

            // Reject if LOS to boss would break (only checked in Pass 1 for ranged)
            if (requireLos && boss && !boss->IsWithinLOS(targetX, targetY, bot->GetPositionZ()))
                continue;

            if (MoveTo(bot->GetMapId(), targetX, targetY, bot->GetPositionZ(), false, false,
                       false, false, MovementPriority::MOVEMENT_FORCED, true))
                return true;
        }
    }

    return false;
}

bool BwlVaelastraszTankSwapAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "vaelastrasz the corrupt");
    if (!boss)
        return false;

    if (bot->GetVictim() != boss)
        return Attack(boss);

    return botAI->DoSpecificAction("taunt spell", event, true);
}

bool BwlBroodlordAvoidBlastWaveAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "broodlord lashlayer");
    if (!boss)
        return false;

    float distToTravel = BROODLORD_BLASTWAVE_DISTANCE - bot->GetDistance2d(boss);
    if (distToTravel <= 0.0f)
        return false;

    bot->CastStop();
    return MoveAway(boss, distToTravel);
}

bool BwlBroodlordTankAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "broodlord lashlayer");
    if (!boss)
        return false;

    if (bot->GetVictim() != boss)
        return Attack(boss);

    return botAI->DoSpecificAction("taunt spell", event, true);
}

bool BwlDrakeWingBuffetAction::Execute(Event event)
{
    Unit* drake = FindBwlDrake(bot);
    if (!drake)
        return false;

    if (bot->GetVictim() != drake)
        return Attack(drake);

    return botAI->DoSpecificAction("taunt spell", event, true);
}

bool BwlFiremawBreakLosAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "firemaw");
    if (!boss)
        return false;

    if (GetFlameBuffetStacks(bot) == 0)
        return false;

    if (!boss->IsWithinLOSInMap(bot))
        return true;

    bot->CastStop();
    bot->AttackStop();

    static constexpr float step = 10.0f;
    for (int i = 0; i < 8; ++i)
    {
        float const angle = i * static_cast<float>(M_PI) / 4.0f;
        float const x = bot->GetPositionX() + step * cos(angle);
        float const y = bot->GetPositionY() + step * sin(angle);
        float const z = bot->GetPositionZ();
        if (!boss->IsWithinLOS(x, y, z))
            return MoveTo(bot->GetMapId(), x, y, z, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
    }

    return MoveAway(boss, step);
}

bool BwlEbonrocTauntAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ebonroc");
    if (!boss)
        return false;

    if (bot->GetVictim() != boss)
        return Attack(boss);

    return botAI->DoSpecificAction("taunt spell", event, true);
}

bool BwlFlamegorTranqAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "flamegor");
    if (!boss || !boss->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_FLAMEGOR_FRENZY)))
        return false;
    return botAI->DoSpecificAction("tranquilizing shot", event, true);
}

// Chromaggus

bool BwlUseHourglassSandAction::Execute(Event /*event*/)
{
    return botAI->CastSpell(static_cast<uint32>(BlackwingLairSpells::SPELL_HOURGLASS_SAND), bot);
}

bool BwlChromaggusBreakLosAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "chromaggus");
    if (!boss)
        return false;

    if (!boss->IsWithinLOSInMap(bot))
        return true;

    bot->CastStop();
    bot->AttackStop();

    static constexpr float step = 10.0f;
    for (int i = 0; i < 8; ++i)
    {
        float const angle = i * static_cast<float>(M_PI) / 4.0f;
        float const x = bot->GetPositionX() + step * cos(angle);
        float const y = bot->GetPositionY() + step * sin(angle);
        float const z = bot->GetPositionZ();
        if (!boss->IsWithinLOS(x, y, z))
            return MoveTo(bot->GetMapId(), x, y, z, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
    }

    return MoveAway(boss, step);
}

bool BwlChromaggusFrenzyTranqAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "chromaggus");
    if (!boss || !boss->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_CHROMAGGUS_FRENZY)))
        return false;
    return botAI->DoSpecificAction("tranquilizing shot", event, true);
}

bool BwlChromaggusSheepAction::Execute(Event /*event*/)
{
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    for (ObjectGuid guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() ||
            !unit->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_CHROMATIC_MUTATION)))
            continue;
        if (unit->IsPolymorphed())
            continue;
        return botAI->CastSpell("polymorph", unit);
    }
    return false;
}

bool BwlChromaggusDispelAction::Execute(Event event)
{
    auto tryDispel = [&](std::initializer_list<std::string> actions) -> bool
    {
        for (std::string const& action : actions)
            if (botAI->DoSpecificAction(action, event, true))
                return true;
        return false;
    };

    switch (bot->getClass())
    {
        case CLASS_MAGE:
            return tryDispel({"remove curse", "remove curse on party"});
        case CLASS_DRUID:
            return tryDispel({"remove curse on party", "abolish poison", "abolish poison on party",
                              "cure poison", "cure poison on party"});
        case CLASS_PRIEST:
            return tryDispel({"dispel magic", "dispel magic on party", "abolish disease",
                              "abolish disease on party", "cure disease", "cure disease on party"});
        case CLASS_PALADIN:
            return tryDispel({"cleanse magic", "cleanse magic on party", "cleanse poison",
                              "cleanse poison on party", "cleanse disease", "cleanse disease on party",
                              "purify poison", "purify poison on party", "purify disease",
                              "purify disease on party"});
        case CLASS_SHAMAN:
            return tryDispel({"cure poison", "cure poison on party", "cure disease", "cure disease on party"});
        default:
            return false;
    }
}

bool BwlChromaggusTimeLapseTankAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "chromaggus");
    if (!boss)
        return false;

    if (bot->GetVictim() != boss)
        return Attack(boss);

    return botAI->DoSpecificAction("taunt spell", event, true);
}

bool BwlNefarianFearWardAction::Execute(Event /*event*/)
{
    Unit* nefarian = AI_VALUE2(Unit*, "find target", "nefarian");
    if (!nefarian)
        return false;

    Unit* victim = nefarian->GetVictim();
    if (!victim)
        return false;

    return botAI->CastSpell("fear ward", victim);
}

// Trash

static constexpr float WYRMGUARD_SAFE_DISTANCE = 16.0f;

Unit* BwlDeathTalonWyrmguardTankMoveAwayAction::GetTarget()
{
    // Find a nearby wyrmguard within 16 yards being tanked by someone else
    for (auto const& [guid, ref] : bot->GetThreatMgr().GetThreatenedByMeList())
    {
        Unit* unit = ref->GetOwner();
        if (!unit || !unit->IsAlive() || unit->GetEntry() != static_cast<uint32>(BlackwingLairNPCs::NPC_DEATH_TALON_WYRMGUARD))
            continue;
        if (bot->GetDistance2d(unit) > WYRMGUARD_SAFE_DISTANCE)
            continue;
        Unit* victim = unit->GetVictim();
        if (victim && victim != bot && victim->IsPlayer() && PlayerbotAI::IsTank(victim->ToPlayer()))
            return unit;
    }
    return nullptr;
}

bool BwlDeathTalonWyrmguardTankMoveAwayAction::isUseful()
{
    if (!GetTarget())
        return false;

    // Must be actively tanking a wyrmguard before moving away from another tank's
    for (auto const& [guid, ref] : bot->GetThreatMgr().GetThreatenedByMeList())
    {
        Unit* unit = ref->GetOwner();
        if (unit && unit->IsAlive() && unit->GetEntry() == static_cast<uint32>(BlackwingLairNPCs::NPC_DEATH_TALON_WYRMGUARD) &&
            unit->GetVictim() == bot)
            return true;
    }
    return false;
}

bool BwlDeathTalonWyrmguardTankMoveAwayAction::Execute(Event /*event*/)
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    float distToTravel = WYRMGUARD_SAFE_DISTANCE - bot->GetDistance2d(target);
    if (distToTravel <= 0.0f)
        return false;

    return MoveAway(target, distToTravel);
}

Unit* BwlDeathTalonWyrmguardRangedMoveAwayAction::GetTarget()
{
    // Find the closest wyrmguard within 16 yards
    Unit* closest = nullptr;
    float closestDist = WYRMGUARD_SAFE_DISTANCE;
    for (auto const& [guid, ref] : bot->GetThreatMgr().GetThreatenedByMeList())
    {
        Unit* unit = ref->GetOwner();
        if (!unit || !unit->IsAlive() || unit->GetEntry() != static_cast<uint32>(BlackwingLairNPCs::NPC_DEATH_TALON_WYRMGUARD))
            continue;
        float dist = bot->GetDistance2d(unit);
        if (dist < closestDist)
        {
            closestDist = dist;
            closest = unit;
        }
    }
    return closest;
}

bool BwlDeathTalonWyrmguardRangedMoveAwayAction::Execute(Event /*event*/)
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    float distToTravel = WYRMGUARD_SAFE_DISTANCE - bot->GetDistance2d(target);
    if (distToTravel <= 0.0f)
        return false;

    return MoveAway(target, distToTravel);
}
