/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BWLMultipliers.h"
#include "AttackAction.h"
#include "BWLActions.h"
#include "BWLHelpers.h"
#include "ChooseTargetActions.h"
#include "EncounterHelpers.h"
#include "GenericSpellActions.h"
#include "HunterActions.h"
#include "Playerbots.h"
#include "ReachTargetActions.h"

using namespace BlackwingLairHelpers;

static constexpr float VAELASTRASZ_BA_SAFE_DISTANCE = 30.0f;
static constexpr float VAELASTRASZ_BA_BOSS_DISTANCE = 35.0f;

float RazorgoreTankMultiplier::GetValue(Action* action)
{
    if (IsRazorgoreController(bot) && AreRazorgoreEggsAlive(botAI))
    {
        if (dynamic_cast<BwlRazorgoreControlAction*>(action))
            return 1.0f;
        if (dynamic_cast<AttackAction*>(action) || dynamic_cast<TankAssistAction*>(action))
            return 0.0f;
    }

    if (!PlayerbotAI::IsTank(bot))
        return 1.0f;

    Unit* boss = AI_VALUE2(Unit*, "find target", "razorgore the untamed");
    if (!boss)
        return 1.0f;

    if (AreRazorgoreEggsAlive(botAI))
    {
        // Off-tank picks up an uncontrolled Razorgore so he cannot kill the orb user.
        if (IsRazorgoreOffTank(bot) && bot->GetVictim() != nullptr && dynamic_cast<TankAssistAction*>(action))
            return 0.0f;
        return 1.0f;
    }

    // All eggs destroyed. Tanks not targeted by the boss don't run into the Cleave.
    if (boss->GetVictim() != bot && dynamic_cast<TankFaceAction*>(action))
        return 0.0f;
    return 1.0f;
}

float VaelastraszTankMultiplier::GetValue(Action* action)
{
    if (!PlayerbotAI::IsTank(bot))
        return 1.0f;

    Unit* boss = AI_VALUE2(Unit*, "find target", "vaelastrasz the corrupt");
    if (!boss)
        return 1.0f;

    if (dynamic_cast<BwlVaelastraszTankSwapAction*>(action))
        return 1.0f;

    Unit* victim = boss->GetVictim();
    Player* victimPlayer = victim ? victim->ToPlayer() : nullptr;
    bool const victimHasBA = victim && victim->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BURNING_ADRENALINE));

    // Stay at rear flank position and don't move in front of the boss.
    if (dynamic_cast<TankFaceAction*>(action))
    {
        if (victim != bot && victimPlayer && PlayerbotAI::IsTank(victimPlayer) && !victimHasBA)
            return 0.0f;
    }

    // Don't tank-assist a BA'd tank; the next tank in the chain taunts instead.
    if (victimHasBA && victim != bot && dynamic_cast<TankAssistAction*>(action))
        return 0.0f;

    return 1.0f;
}

float VaelastraszBurningAdrenalineMultiplier::GetValue(Action* action)
{
    if (bot->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BURNING_ADRENALINE)))
    {
        if (dynamic_cast<MovementAction*>(action))
        {
            if (dynamic_cast<BwlVaelastraszMoveAwayAction*>(action))
            {
                // Allow movement when boss is dead or bot was knocked away
                Unit* boss = AI_VALUE2(Unit*, "find target", "vaelastrasz the corrupt");
                if (!boss || !boss->IsAlive() || bot->GetDistance2d(boss) > VAELASTRASZ_BA_BOSS_DISTANCE)
                    return 1.0f;
            }

            // Block ALL other movement once bot is far enough from non-BA bots
            if (!IsNonBABotNearPosition(bot, bot->GetPosition(), VAELASTRASZ_BA_SAFE_DISTANCE) ||
                !dynamic_cast<BwlVaelastraszMoveAwayAction*>(action))
                return 0.0f;
        }
        // Also block charge actions
        if (dynamic_cast<CastReachTargetSpellAction*>(action))
            return 0.0f;
    }

    // BA victims die at the end of the debuff; don't dump heals into a corpse-to-be.
    if (auto* heal = dynamic_cast<CastHealingSpellAction*>(action))
    {
        Unit* target = heal->GetTarget();
        if (target && target->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BURNING_ADRENALINE)) &&
            target->GetHealthPct() < 20.0f)
            return 0.0f;
    }

    return 1.0f;
}

float BroodlordSharedThreatMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "broodlord lashlayer");
    if (!boss)
        return 1.0f;

    if (dynamic_cast<BwlBroodlordTankAction*>(action) || dynamic_cast<BwlBroodlordAvoidBlastWaveAction*>(action))
        return 1.0f;

    // Knock Away is not a taunt-swap: never taunt a tank who still has the boss.
    Unit* victim = boss->GetVictim();
    Player* victimPlayer = victim ? victim->ToPlayer() : nullptr;
    if (victimPlayer && PlayerbotAI::IsTank(victimPlayer) && victim != bot &&
        EncounterHelpers::IsTauntAction(bot, action))
        return 0.0f;

    if (PlayerbotAI::IsDps(bot) && EncounterHelpers::IsDpsCooldownAction(bot, action))
        return 0.0f;

    return 1.0f;
}

float FiremawFlameBuffetMultiplier::GetValue(Action* action)
{
    if (!AI_VALUE2(Unit*, "find target", "firemaw"))
        return 1.0f;

    if (dynamic_cast<BwlFiremawBreakLosAction*>(action))
        return 1.0f;

    uint32 const stacks = GetFlameBuffetStacks(bot);
    uint32 const threshold = PlayerbotAI::IsTank(bot) ? 8 : static_cast<uint32>(FLAME_BUFFET_LOS_STACKS);
    if (stacks < threshold)
        return 1.0f;

    if (dynamic_cast<AttackAction*>(action) || dynamic_cast<CastReachTargetSpellAction*>(action))
        return 0.0f;
    if (auto* spell = dynamic_cast<CastSpellAction*>(action))
        if (spell->getThreatType() != Action::ActionThreatType::None)
            return 0.0f;

    return 1.0f;
}

float ChromaggusTimeLapseMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "chromaggus");
    if (!boss)
        return 1.0f;

    if (dynamic_cast<BwlChromaggusBreakLosAction*>(action) ||
        dynamic_cast<BwlChromaggusTimeLapseTankAction*>(action) ||
        dynamic_cast<BwlChromaggusDispelAction*>(action) ||
        dynamic_cast<BwlUseHourglassSandAction*>(action))
        return 1.0f;

    bool timeLapse = bot->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_TIMELAPSE));
    if (!timeLapse && boss->HasUnitState(UNIT_STATE_CASTING))
    {
        Spell* current = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (current && current->m_spellInfo &&
            current->m_spellInfo->Id == static_cast<uint32>(BlackwingLairSpells::SPELL_TIMELAPSE))
            timeLapse = true;
    }

    if (!timeLapse)
        return 1.0f;

    if (PlayerbotAI::IsDps(bot))
    {
        if (dynamic_cast<AttackAction*>(action))
            return 0.0f;
        if (auto* spell = dynamic_cast<CastSpellAction*>(action))
            if (spell->getThreatType() != Action::ActionThreatType::None)
                return 0.0f;
    }

    return 1.0f;
}
