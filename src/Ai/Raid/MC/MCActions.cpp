/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MCActions.h"
#include "GameObject.h"
#include "MCHelpers.h"
#include "Pet.h"
#include "Playerbots.h"
#include "RtiTargetValue.h"
#include "Spell.h"
#include <algorithm>

static constexpr float LIVING_BOMB_DISTANCE = 20.0f;
static constexpr float INFERNO_DISTANCE = 20.0f;

// dedicated tank positions; prevents assist tanks from positioning Core Ragers on steep walls on pull
static const Position GOLEMAGG_TANK_POSITION{795.7308, -994.8848, -207.18661};
static const Position CORE_RAGER_TANK_POSITION{846.6453, -1019.0639, -198.9819};

// Midpoint of the two tank camps (56y apart): healers standing here reach both
static const Position GOLEMAGG_HEALER_POSITION{821.2f, -1007.0f, -203.0f};
static constexpr float HEALER_POSITION_TOLERANCE = 8.0f;

static constexpr float GOLEMAGGS_TRUST_DISTANCE = 30.0f;
static constexpr float CORE_RAGER_STEP_DISTANCE = 5.0f;

using namespace MoltenCoreHelpers;

bool CoreHoundPackState::IsBurnPhase() const
{
    if (fighting.empty())
        return false;
    if (!smoldering.empty())
        return true;
    if (fighting.size() < 2)
        return false;
    for (Creature const* hound : fighting)
        if (hound && hound->GetHealthPct() > CORE_HOUND_EXECUTE_HEALTH_PCT)
            return false;
    return true;
}

namespace MoltenCoreHelpers
{

CoreHoundPackState GetCoreHoundPackState(Player* bot)
{
    CoreHoundPackState state;
    if (!bot)
        return state;

    std::list<Creature*> hounds;
    bot->GetCreatureListWithEntryInGrid(hounds, NPC_CORE_HOUND, CORE_HOUND_PACK_RANGE);
    for (Creature* hound : hounds)
    {
        if (!hound || !hound->IsAlive() || !hound->IsInCombat())
            continue;

        if (hound->HasAura(SPELL_PLAY_DEAD))
            state.smoldering.push_back(hound);
        else
            state.fighting.push_back(hound);
    }

    return state;
}

bool IsFightingCoreHound(Unit const* unit)
{
    return unit && unit->GetEntry() == NPC_CORE_HOUND && unit->IsAlive() && !unit->HasAura(SPELL_PLAY_DEAD);
}

static uint32 GetAliveDpsIndex(Player* bot, float packRange)
{
    uint32 dpsIndex = 0;
    Group* group = bot ? bot->GetGroup() : nullptr;
    if (!group)
        return 0;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || !member->IsAlive() || !PlayerbotAI::IsDps(member))
            continue;
        if (member->GetMapId() != bot->GetMapId())
            continue;
        if (bot->GetDistance(member) > packRange)
            continue;
        if (member == bot)
            return dpsIndex;
        ++dpsIndex;
    }

    return 0;
}

Creature* SelectCoreHoundBalanceTarget(Player* bot)
{
    if (!bot || !PlayerbotAI::IsDps(bot))
        return nullptr;

    CoreHoundPackState const pack = GetCoreHoundPackState(bot);
    if (!pack.HasPack() || pack.IsBurnPhase())
        return nullptr;

    std::vector<Creature*> candidates = pack.fighting;
    std::sort(candidates.begin(), candidates.end(), [](Creature const* left, Creature const* right)
    {
        if (left->GetHealthPct() != right->GetHealthPct())
            return left->GetHealthPct() > right->GetHealthPct();
        return left->GetGUID() < right->GetGUID();
    });

    float const highestHealthPct = candidates.front()->GetHealthPct();
    if (highestHealthPct <= CORE_HOUND_EXECUTE_HEALTH_PCT)
        return nullptr;

    std::vector<Creature*> healthiestBand;
    for (Creature* hound : candidates)
    {
        if (hound->GetHealthPct() <= CORE_HOUND_EXECUTE_HEALTH_PCT)
            break;
        if (hound->GetHealthPct() + CORE_HOUND_BALANCE_BAND_PCT < highestHealthPct)
            break;
        healthiestBand.push_back(hound);
    }

    if (healthiestBand.empty())
        return nullptr;

    uint32 const maxDpsPerHound = highestHealthPct <= CORE_HOUND_LOW_HEALTH_PCT
                                      ? CORE_HOUND_MAX_DPS_PER_HOUND_LOW
                                      : CORE_HOUND_MAX_DPS_PER_HOUND_HIGH;
    uint32 const dpsIndex = GetAliveDpsIndex(bot, CORE_HOUND_PACK_RANGE);
    if (dpsIndex >= healthiestBand.size() * maxDpsPerHound)
        return nullptr;

    return healthiestBand[dpsIndex % healthiestBand.size()];
}

}

static void StopCoreHoundDamage(PlayerbotAI* botAI, Player* bot)
{
    bot->CastStop();
    bot->AttackStop();
    if (Pet* pet = bot->GetPet())
        pet->AttackStop();

    if (bot->getClass() == CLASS_HUNTER)
    {
        if (Spell const* spell = bot->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL))
            if (spell->m_spellInfo->Id == 75)
                bot->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
    }

    Unit* current = botAI->GetAiObjectContext()->GetValue<Unit*>("current target")->Get();
    if (current && current->GetEntry() == NPC_CORE_HOUND)
        botAI->GetAiObjectContext()->GetValue<Unit*>("current target")->Set(nullptr);
}

bool McMoveFromGroupAction::Execute(Event /*event*/)
{
    return MoveFromGroup(LIVING_BOMB_DISTANCE);
}

bool McMoveFromBaronGeddonAction::Execute(Event /*event*/)
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "baron geddon"))
    {
        float distToTravel = INFERNO_DISTANCE - bot->GetDistance2d(boss);
        if (distToTravel > 0)
        {
            // Stop channeling spell first
            bot->AttackStop();
            bot->CastStop();

            return MoveAway(boss, distToTravel);
        }
    }
    return false;
}

bool McShazzrahMoveAwayAction::Execute(Event /*event*/)
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "shazzrah"))
    {
        float distToTravel = ARCANE_EXPLOSION_DISTANCE - bot->GetDistance2d(boss);
        if (distToTravel > 0)
            return MoveAway(boss, distToTravel);
    }
    return false;
}

bool McMoveFromLavaAction::Execute(Event /*event*/)
{
    // Escape to the nearest raid member on dry ground: raids fight on the
    // rock shelves, so a dry teammate marks the closest safe floor.
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

    // No dry raid member in range: stay put and let the in-lava trigger fire
    // again — any other destination (the current target included) can path
    // straight back into the pool.
    if (!dryTarget)
        return false;

    bot->CastStop();

    if (!MoveTo(dryTarget->GetMapId(), dryTarget->GetPositionX(), dryTarget->GetPositionY(),
                dryTarget->GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_FORCED))
        return false;

    // Hold the AI so rotation/facing actions can't cancel the escape spline
    // mid-lava. Capped at 2s on purpose: the in-lava trigger refires and
    // re-issues the escape while the bot is still swimming.
    constexpr uint32 MAX_ESCAPE_HOLD_MS = 2000;
    constexpr float MIN_ESCAPE_SPEED = 0.1f;

    float const speed = bot->GetSpeed(MOVE_RUN);
    if (speed > MIN_ESCAPE_SPEED)
        botAI->SetNextCheckDelay(std::min(MAX_ESCAPE_HOLD_MS, uint32(1000.0f * bot->GetDistance(dryTarget) / speed)));

    return true;
}

bool McMagmadarMoveFromFireAction::Execute(Event /*event*/)
{
    GameObject* fire = bot->FindNearestGameObject(GO_MAGMADAR_LAVA_BOMB, MAGMADAR_FIRE_DISTANCE, true);
    if (!fire)
        return false;

    bot->CastStop();
    return FleePosition(fire->GetPosition(), MAGMADAR_FIRE_DISTANCE);
}

bool McGehennasMoveFromRainAction::Execute(Event /*event*/)
{
    bot->CastStop();
    return Move(frand(0.0f, 2.0f * static_cast<float>(M_PI)), GEHENNAS_RAIN_DISTANCE);
}

bool McMagmadarTranqAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "magmadar");
    if (!boss || !boss->HasAura(SPELL_MAGMADAR_FRENZY))
        return false;
    return botAI->DoSpecificAction("tranquilizing shot", event, true);
}

bool McGolemaggBackOffAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "golemagg the incinerator");
    if (!boss)
        return false;

    // Out of swing range the stack stops growing and expires 30s after
    // the last application; the multiplier blocks re-engaging until then.
    bot->CastStop();

    float distToTravel = MAGMA_SPLASH_BACK_OFF_DISTANCE - bot->GetDistance2d(boss);
    if (distToTravel <= 0.0f)
        return true;

    return MoveAway(boss, distToTravel);
}

bool McGolemaggMarkBossAction::Execute(Event /*event*/)
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "golemagg the incinerator"))
    {
        if (Group* group = bot->GetGroup())
        {
            ObjectGuid currentSkullGuid = group->GetTargetIcon(RtiTargetValue::skullIndex);
            if (currentSkullGuid.IsEmpty() || currentSkullGuid != boss->GetGUID())
            {
                group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), boss->GetGUID());
                return true;
            }
        }
    }
    return false;
}

bool McGolemaggTankAction::MoveUnitToPosition(Unit* target, Position const& tankPosition, float maxDistance,
                                              float stepDistance)
{
    if (bot->GetVictim() != target)
        return Attack(target);
    if (target->GetVictim() == bot)
    {
        float distanceToTankPosition = bot->GetExactDist2d(tankPosition.GetPositionX(), tankPosition.GetPositionY());
        if (distanceToTankPosition > maxDistance)
        {
            float dX = tankPosition.GetPositionX() - bot->GetPositionX();
            float dY = tankPosition.GetPositionY() - bot->GetPositionY();
            float dist = sqrt(dX * dX + dY * dY);
            float moveX = bot->GetPositionX() + (dX / dist) * stepDistance;
            float moveY = bot->GetPositionY() + (dY / dist) * stepDistance;
            return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false,
                          false, false, MovementPriority::MOVEMENT_COMBAT, true,
                          true);
        }
    }
    else if (botAI->DoSpecificAction("taunt spell", Event(), true))
        return true;
    return false;
}

bool McGolemaggTankAction::FindCoreRagers(Unit*& coreRager1, Unit*& coreRager2) const
{
    coreRager1 = coreRager2 = nullptr;
    for (auto const& target : AI_VALUE(GuidVector, "possible targets no los"))
    {
        Unit* unit = botAI->GetUnit(target);
        if (unit && unit->IsAlive() && unit->GetEntry() == NPC_CORE_RAGER)
        {
            if (coreRager1 == nullptr)
                coreRager1 = unit;
            else if (coreRager2 == nullptr)
            {
                coreRager2 = unit;
                break; // There should be no third Core Rager.
            }
        }
    }
    return coreRager1 != nullptr && coreRager2 != nullptr;
}

bool McGolemaggMainTankAttackGolemaggAction::Execute(Event /*event*/)
{
    // At this point, we know we are not the last living tank in the group.
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "golemagg the incinerator"))
    {
        Unit* coreRager1;
        Unit* coreRager2;
        if (!FindCoreRagers(coreRager1, coreRager2))
            return false; // safety check

        // We only need to move if the Core Ragers still have Golemagg's Trust
        if (coreRager1->HasAura(SPELL_GOLEMAGGS_TRUST) || coreRager2->HasAura(SPELL_GOLEMAGGS_TRUST))
            return MoveUnitToPosition(boss, GOLEMAGG_TANK_POSITION, boss->GetCombatReach());
    }
    return false;
}

bool McGolemaggHealerPositionAction::Execute(Event /*event*/)
{
    if (bot->GetExactDist2d(GOLEMAGG_HEALER_POSITION) <= HEALER_POSITION_TOLERANCE)
        return false;

    return MoveTo(bot->GetMapId(), GOLEMAGG_HEALER_POSITION.GetPositionX(), GOLEMAGG_HEALER_POSITION.GetPositionY(),
                  GOLEMAGG_HEALER_POSITION.GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT, true, false);
}

bool McGolemaggAssistTankAttackCoreRagerAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "golemagg the incinerator");
    if (!boss)
        return false;

    // Step 0: Filter additional assist tanks. We only need 2.
    bool isFirstAssistTank = PlayerbotAI::IsAssistTankOfIndex(bot, 0, true);
    bool isSecondAssistTank = PlayerbotAI::IsAssistTankOfIndex(bot, 1, true);
    if (!isFirstAssistTank && !isSecondAssistTank)
        return Attack(boss);

    // Step 1: Find both Core Ragers
    Unit* coreRager1;
    Unit* coreRager2;
    if (!FindCoreRagers(coreRager1, coreRager2))
        return false; // safety check

    // Step 2: Assign Core Rager to bot
    Unit* myCoreRager = nullptr;
    Unit* otherCoreRager = nullptr;
    if (isFirstAssistTank)
    {
        myCoreRager = coreRager1;
        otherCoreRager = coreRager2;
    }
    else // isSecondAssistTank is always true here
    {
        myCoreRager = coreRager2;
        otherCoreRager = coreRager1;
    }

    // Step 3: Select the right target
    if (myCoreRager->GetVictim() != bot)
    {
        // Step 3.1: My Core Rager isn't attacking me. Attack until it does.
        if (bot->GetVictim() != myCoreRager)
            return Attack(myCoreRager);
        return botAI->DoSpecificAction("taunt spell", event, true);
    }

    Unit* otherCoreRagerVictim = otherCoreRager->GetVictim();
    if (otherCoreRagerVictim) // Core Rager victim can be NULL
    {
        // Step 3.2: Check if the other Core Rager isn't attacking its assist tank.
        Player* otherCoreRagerPlayerVictim = otherCoreRagerVictim->ToPlayer();
        if (otherCoreRagerPlayerVictim &&
            !PlayerbotAI::IsAssistTankOfIndex(otherCoreRagerPlayerVictim, 0, true) &&
            !PlayerbotAI::IsAssistTankOfIndex(otherCoreRagerPlayerVictim, 1, true))
        {
            // Assume we are the only assist tank or the other assist tank is dead => pick up other Core Rager!
            if (bot->GetVictim() != otherCoreRager)
                return Attack(otherCoreRager);
            return botAI->DoSpecificAction("taunt spell", event, true);
        }
    }

    if (bot->GetVictim() != myCoreRager)
        return Attack(myCoreRager); // Step 3.3: Attack our Core Rager in case we previously switched in 3.2.

    // Step 4: Prevent Golemagg's Trust on Core Ragers
    if (myCoreRager->HasAura(SPELL_GOLEMAGGS_TRUST) ||
        (otherCoreRagerVictim == bot && otherCoreRager->HasAura(SPELL_GOLEMAGGS_TRUST)))
    {
        // Step 4.1: Move Core Ragers to dedicated tank position (only if Golemagg is far enough away from said position)
        float bossDistanceToCoreRagerTankPosition = boss->GetExactDist2d(
            CORE_RAGER_TANK_POSITION.GetPositionX(), CORE_RAGER_TANK_POSITION.GetPositionY());
        if (bossDistanceToCoreRagerTankPosition > GOLEMAGGS_TRUST_DISTANCE)
        {
            float distanceToTankPosition = bot->GetExactDist2d(CORE_RAGER_TANK_POSITION.GetPositionX(),
                                                               CORE_RAGER_TANK_POSITION.GetPositionY());
            if (distanceToTankPosition > CORE_RAGER_STEP_DISTANCE)
                return MoveUnitToPosition(myCoreRager, CORE_RAGER_TANK_POSITION, CORE_RAGER_STEP_DISTANCE);
        }

        // Step 4.2: if boss is too close to tank position, or we are already there, move away from Golemagg to try to out-range Golemagg's Trust
        return MoveAway(boss, CORE_RAGER_STEP_DISTANCE, true);
    }

    return false;
}

namespace MoltenCoreHelpers
{

bool IsMajordomoEncounterActive(Player* bot)
{
    if (!bot)
        return false;

    for (uint32 entry : { NPC_MAJORDOMO_EXECUTUS, NPC_FLAMEWAKER_HEALER, NPC_FLAMEWAKER_ELITE })
    {
        std::list<Creature*> creatures;
        bot->GetCreatureListWithEntryInGrid(creatures, entry, 150.0f);
        if (std::any_of(creatures.begin(), creatures.end(), [](Creature* creature)
            { return creature && creature->IsAlive() && creature->IsInCombat(); }))
            return true;
    }
    return false;
}

std::vector<Unit*> FindMajordomoAdds(Player* bot)
{
    std::vector<Unit*> adds;
    if (!bot)
        return adds;

    for (uint32 entry : { NPC_FLAMEWAKER_HEALER, NPC_FLAMEWAKER_ELITE })
    {
        std::list<Creature*> creatures;
        bot->GetCreatureListWithEntryInGrid(creatures, entry, 150.0f);
        for (Creature* creature : creatures)
            if (creature && creature->IsAlive() && creature->IsInCombat())
                adds.push_back(creature);
    }
    std::sort(adds.begin(), adds.end(), [](Unit const* left, Unit const* right)
    {
        if (left->GetEntry() != right->GetEntry())
            return left->GetEntry() == NPC_FLAMEWAKER_HEALER;
        if (left->GetHealthPct() != right->GetHealthPct())
            return left->GetHealthPct() < right->GetHealthPct();
        return left->GetGUID() < right->GetGUID();
    });
    return adds;
}

static std::vector<Unit*> MajordomoAddsOfEntry(Player* bot, uint32 entry)
{
    std::vector<Unit*> result;
    for (Unit* add : FindMajordomoAdds(bot))
        if (add->GetEntry() == entry)
            result.push_back(add);
    return result;
}

static bool IsTankedByATank(Unit* add)
{
    Unit* victim = add ? add->GetVictim() : nullptr;
    Player* player = victim ? victim->ToPlayer() : nullptr;
    return player && player->IsAlive() && PlayerbotAI::IsTank(player);
}

Unit* SelectMajordomoKillTarget(Player* bot)
{
    std::vector<Unit*> healers = MajordomoAddsOfEntry(bot, NPC_FLAMEWAKER_HEALER);
    std::vector<Unit*> burnable;
    for (Unit* healer : healers)
        if (!healer->IsPolymorphed())
            burnable.push_back(healer);
    if (!burnable.empty())
        return burnable.front();

    std::vector<Unit*> elites = MajordomoAddsOfEntry(bot, NPC_FLAMEWAKER_ELITE);
    return elites.empty() ? nullptr : elites.front();
}

Unit* SelectMajordomoSheepTarget(Player* bot)
{
    Unit* killTarget = SelectMajordomoKillTarget(bot);
    std::vector<Unit*> healers = MajordomoAddsOfEntry(bot, NPC_FLAMEWAKER_HEALER);
    std::vector<Unit*> sheepable;
    for (Unit* healer : healers)
    {
        if (healer == killTarget || healer->IsPolymorphed())
            continue;
        sheepable.push_back(healer);
    }
    if (sheepable.empty())
        return nullptr;

    uint32 mageIndex = 0;
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || !member->IsAlive() || member->getClass() != CLASS_MAGE)
                continue;
            if (member->GetMapId() != bot->GetMapId())
                continue;
            if (member == bot)
                break;
            ++mageIndex;
        }
    }
    return sheepable[mageIndex % sheepable.size()];
}

Unit* SelectMajordomoTankTarget(Player* bot)
{
    std::vector<Unit*> elites = MajordomoAddsOfEntry(bot, NPC_FLAMEWAKER_ELITE);
    if (elites.empty())
    {
        // Healers only remaining: hold whatever is punching a non-tank.
        for (Unit* healer : MajordomoAddsOfEntry(bot, NPC_FLAMEWAKER_HEALER))
            if (!healer->IsPolymorphed() && !IsTankedByATank(healer))
                return healer;
        return nullptr;
    }

    std::sort(elites.begin(), elites.end(), [](Unit const* left, Unit const* right)
    {
        return left->GetGUID() < right->GetGUID();
    });

    std::vector<Player*> tanks;
    Player* mainTank = nullptr;
    std::vector<Player*> assists;
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || !member->IsAlive() || !PlayerbotAI::IsTank(member))
                continue;
            if (member->GetMapId() != bot->GetMapId())
                continue;
            if (PlayerbotAI::IsMainTank(member))
                mainTank = member;
            else
                assists.push_back(member);
        }
    }
    if (mainTank)
        tanks.push_back(mainTank);
    tanks.insert(tanks.end(), assists.begin(), assists.end());
    if (tanks.empty())
        tanks.push_back(bot);

    uint32 tankIndex = 0;
    for (size_t i = 0; i < tanks.size(); ++i)
    {
        if (tanks[i] == bot)
        {
            tankIndex = static_cast<uint32>(i);
            break;
        }
    }

    Unit* assigned = elites[tankIndex % elites.size()];
    if (!IsTankedByATank(assigned))
        return assigned;

    // Assigned elite is held. Rescue any other loose elite before idling.
    for (Unit* elite : elites)
        if (!IsTankedByATank(elite))
            return elite;

    return assigned;
}

Unit* FindMajordomo(Player* bot)
{
    if (!bot)
        return nullptr;

    std::list<Creature*> creatures;
    bot->GetCreatureListWithEntryInGrid(creatures, NPC_MAJORDOMO_EXECUTUS, 150.0f);
    for (Creature* creature : creatures)
        if (creature && creature->IsAlive() && creature->IsInCombat())
            return creature;
    return nullptr;
}

Unit* FindNearestMajordomoElite(Player* bot)
{
    Unit* nearest = nullptr;
    float best = MAJORDOMO_BLAST_WAVE_DISTANCE;
    for (Unit* elite : MajordomoAddsOfEntry(bot, NPC_FLAMEWAKER_ELITE))
    {
        float const dist = bot->GetDistance2d(elite);
        if (dist < best)
        {
            best = dist;
            nearest = elite;
        }
    }
    return nearest;
}

}

bool McMajordomoTankAddsAction::Execute(Event event)
{
    Unit* target = nullptr;
    if (PlayerbotAI::IsMainTank(bot))
    {
        target = FindMajordomo(bot);
        if (!target)
            target = SelectMajordomoTankTarget(bot);
    }
    else
        target = SelectMajordomoTankTarget(bot);

    if (!target)
    {
        botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Reset();
        return false;
    }

    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ target->GetGUID() });
    if (bot->GetVictim() != target)
        return Attack(target);
    if (target->GetEntry() != NPC_MAJORDOMO_EXECUTUS && target->GetVictim() != bot)
        return botAI->DoSpecificAction("taunt spell", event, true);
    return false;
}

bool McMajordomoAvoidBlastWaveAction::Execute(Event /*event*/)
{
    Unit* elite = FindNearestMajordomoElite(bot);
    if (!elite)
        return false;

    bot->CastStop();
    bot->AttackStop();
    float distToTravel = MAJORDOMO_BLAST_WAVE_DISTANCE - bot->GetDistance2d(elite);
    if (distToTravel <= 0.0f)
        return true;
    return MoveAway(elite, distToTravel);
}

bool McMajordomoStopForShieldAction::Execute(Event /*event*/)
{
    Unit* boss = FindMajordomo(bot);
    if (!boss)
        return false;

    bot->CastStop();
    bot->AttackStop();
    return true;
}

bool McMajordomoRunToRaidAction::Execute(Event /*event*/)
{
    Unit* ally = nullptr;
    float best = 200.0f;
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || member == bot || !member->IsAlive() || member->GetMapId() != bot->GetMapId())
                continue;
            float dist = bot->GetDistance(member);
            if (dist < best)
            {
                best = dist;
                ally = member;
            }
        }
    }
    if (!ally)
        return false;

    bot->CastStop();
    return MoveTo(ally->GetMapId(), ally->GetPositionX(), ally->GetPositionY(), ally->GetPositionZ(),
                  false, false, false, true, MovementPriority::MOVEMENT_FORCED);
}

bool McMajordomoFocusAddsAction::Execute(Event /*event*/)
{
    Unit* target = SelectMajordomoKillTarget(bot);
    if (!target)
    {
        botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Reset();
        return false;
    }

    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ target->GetGUID() });
    Unit* current = AI_VALUE(Unit*, "current target");
    if ((current == target || bot->GetVictim() == target) && !target->IsPolymorphed())
        return false;

    bot->CastStop();
    bot->AttackStop();
    return Attack(target);
}

bool McMajordomoPolymorphHealerAction::Execute(Event /*event*/)
{
    Unit* target = SelectMajordomoSheepTarget(bot);
    if (!target)
        return false;
    return botAI->CastSpell("polymorph", target);
}

Unit* McCoreHoundMarkAction::GetTarget()
{
    return SelectCoreHoundBalanceTarget(bot);
}

bool McCoreHoundMarkAction::Execute(Event /*event*/)
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ target->GetGUID() });

    // Already on the assigned hound: return false so the GCD is spent on the
    // class rotation instead of this raid action eating every tick.
    Unit* current = AI_VALUE(Unit*, "current target");
    if (IsFightingCoreHound(target) && current == target &&
        (!bot->GetVictim() || bot->GetVictim() == target))
        return false;

    // Changing the AI target alone does not interrupt the old swing or cast.
    StopCoreHoundDamage(botAI, bot);
    return Attack(target);
}

bool McCoreHoundHoldAction::Execute(Event /*event*/)
{
    Unit* current = AI_VALUE(Unit*, "current target");
    Unit* victim = bot->GetVictim();
    bool const onHound = (current && current->GetEntry() == NPC_CORE_HOUND) ||
                         (victim && victim->GetEntry() == NPC_CORE_HOUND);
    if (!onHound)
        return false;

    StopCoreHoundDamage(botAI, bot);
    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Reset();
    return true;
}

bool McCoreHoundBurnAction::Execute(Event event)
{
    CoreHoundPackState const pack = GetCoreHoundPackState(bot);
    if (pack.fighting.empty())
        return false;

    Creature* closest = nullptr;
    float bestDistance = 0.0f;
    for (Creature* hound : pack.fighting)
    {
        float const distance = bot->GetDistance(hound);
        if (!closest || distance < bestDistance)
        {
            closest = hound;
            bestDistance = distance;
        }
    }

    if (closest)
    {
        botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ closest->GetGUID() });
        context->GetValue<Unit*>("current target")->Set(closest);
        bot->SetSelection(closest->GetGUID());
        if (bot->GetVictim() != closest)
            bot->Attack(closest, bot->IsWithinMeleeRange(closest) || botAI->IsMelee(bot));
    }

    // Generic AoE triggers count attackers on this bot. Raid DPS usually have
    // no attackers because the hounds are on tanks, so explicitly invoke a
    // registered class AoE once every living hound is inside the burn window.
    std::vector<std::string> actions;
    switch (bot->getClass())
    {
        case CLASS_MAGE:        actions = {"arcane explosion", "flamestrike", "blizzard"}; break;
        case CLASS_WARLOCK:     actions = {"rain of fire", "seed of corruption", "hellfire"}; break;
        case CLASS_WARRIOR:     actions = {"whirlwind", "thunder clap", "cleave"}; break;
        case CLASS_PALADIN:     actions = {"consecration", "divine storm"}; break;
        case CLASS_PRIEST:      actions = {"mind sear", "holy nova"}; break;
        case CLASS_ROGUE:       actions = {"fan of knives", "blade flurry"}; break;
        case CLASS_SHAMAN:      actions = {"fire nova", "chain lightning", "magma totem"}; break;
        case CLASS_DRUID:       actions = {"starfall", "hurricane", "swipe"}; break;
        case CLASS_HUNTER:      actions = {"volley", "multi-shot", "explosive trap"}; break;
        case CLASS_DEATH_KNIGHT: actions = {"death and decay", "howling blast", "blood boil", "pestilence"}; break;
        default: break;
    }

    for (std::string const& action : actions)
        if (botAI->DoSpecificAction(action, event, true))
            return true;

    // AoE was on cooldown or out of range: let the class rotation dump into
    // the closest fighting hound rather than stalling the execute.
    return false;
}
