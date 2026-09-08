/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "OnyStrategy.h"
#include "AttackAction.h"
#include "EncounterHelpers.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "OnyActions.h"
#include "OnyHelpers.h"
#include "Playerbots.h"

using namespace OnyxiaHelpers;

void RaidOnyxiaStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "ony fire resistance", { NextAction("ony fire resistance", ACTION_RAID) }));

    // P1 / P3: sides only (breath + tail), MT against the north wall.
    triggers.push_back(new TriggerNode(
        "ony near tail", { NextAction("ony move to side", ACTION_RAID + 2) }));
    triggers.push_back(new TriggerNode(
        "ony tank wall", { NextAction("ony tank wall", ACTION_RAID + 2) }));
    triggers.push_back(new TriggerNode(
        "ony avoid eggs", { NextAction("ony avoid eggs move", ACTION_EMERGENCY + 5) }));

    // P2: Deep Breath to a side wall and hold, spread for fireballs, melee on whelps.
    triggers.push_back(new TriggerNode(
        "ony deep breath warning", { NextAction("ony move to safe zone", ACTION_EMERGENCY + 5) }));
    triggers.push_back(new TriggerNode(
        "ony fireball splash incoming", { NextAction("ony spread out", ACTION_EMERGENCY + 2) }));
    triggers.push_back(new TriggerNode(
        "ony whelps spawn", { NextAction("ony kill whelps", ACTION_RAID + 1) }));

    // P3: roar fear counters, lava cracks, threat rebuild after Wing Buffet / fear.
    triggers.push_back(new TriggerNode(
        "ony bellowing roar", { NextAction("ony anti fear", ACTION_EMERGENCY + 3) }));
    triggers.push_back(new TriggerNode(
        "ony lava", { NextAction("ony move from lava", ACTION_RAID + 4) }));
    triggers.push_back(new TriggerNode(
        "ony tank taunt", { NextAction("ony tank taunt", ACTION_RAID + 3) }));
}

void RaidOnyxiaStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new OnyxiaMultiplier(botAI));
}

float OnyxiaMultiplier::GetValue(Action* action)
{
    if (dynamic_cast<RaidOnyxiaMoveToSafeZoneAction*>(action) ||
        dynamic_cast<OnyxiaAvoidEggsAction*>(action) ||
        dynamic_cast<OnyxiaMoveFromLavaAction*>(action) ||
        dynamic_cast<OnyxiaAntiFearAction*>(action))
        return 1.0f;

    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return 1.0f;

    if (boss->HasUnitState(UNIT_STATE_CASTING))
    {
        Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (currentSpell && currentSpell->m_spellInfo && IsDeepBreathSpell(currentSpell->m_spellInfo->Id))
        {
            if (dynamic_cast<MovementAction*>(action) || dynamic_cast<AttackAction*>(action))
                return 0.0f;
            if (auto* spell = dynamic_cast<CastSpellAction*>(action))
                if (spell->getThreatType() != Action::ActionThreatType::None)
                    return 0.0f;
        }
    }

    // P1: no cooldown dump so dps cannot pass the tank (no taunt recovery if they pull).
    if (IsOnyxiaGrounded(boss) && boss->GetHealthPct() > 65.0f && PlayerbotAI::IsDps(bot) &&
        EncounterHelpers::IsDpsCooldownAction(bot, action))
        return 0.0f;

    return 1.0f;
}
