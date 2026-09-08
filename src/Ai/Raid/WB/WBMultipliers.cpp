/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WBMultipliers.h"
#include "AttackAction.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "Playerbots.h"
#include "ReachTargetActions.h"
#include "WBActions.h"
#include "WBHelpers.h"

using namespace WorldBossHelpers;

float AzuregosMultiplier::GetValue(Action* action)
{
    Unit* boss = FindAzuregos(bot);
    if (!boss)
        return 1.0f;

    if (dynamic_cast<WbAzuregosMoveFromStormAction*>(action) ||
        dynamic_cast<WbAzuregosStopForReflectAction*>(action) ||
        dynamic_cast<WbAzuregosTankTauntAction*>(action) ||
        dynamic_cast<WbAzuregosMoveToFlankAction*>(action))
        return 1.0f;

    // Dead players with Mark of Frost freeze if they re-engage.
    if (bot->HasAura(SPELL_MARK_OF_FROST))
    {
        if (dynamic_cast<AttackAction*>(action))
            return 0.0f;
        if (auto* spell = dynamic_cast<CastSpellAction*>(action))
            if (spell->getThreatType() != Action::ActionThreatType::None)
                return 0.0f;
        return 1.0f;
    }

    bool vacuum = false;
    if (boss->HasUnitState(UNIT_STATE_CASTING))
    {
        Spell* current = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (current && current->m_spellInfo && current->m_spellInfo->Id == SPELL_ARCANE_VACUUM)
            vacuum = true;
    }

    // Drop dots before/during teleport, and hold until a tank has aggro again.
    Unit* victim = boss->GetVictim();
    Player* victimPlayer = victim ? victim->ToPlayer() : nullptr;
    bool tankHasAggro = victimPlayer && PlayerbotAI::IsTank(victimPlayer);
    if (vacuum || (!tankHasAggro && boss->IsInCombat() && !PlayerbotAI::IsTank(bot)))
    {
        if (dynamic_cast<CastDebuffSpellAction*>(action))
            return 0.0f;
        if (PlayerbotAI::IsDps(bot))
        {
            if (dynamic_cast<AttackAction*>(action))
                return 0.0f;
            if (auto* spell = dynamic_cast<CastSpellAction*>(action))
                if (spell->getThreatType() != Action::ActionThreatType::None)
                    return 0.0f;
        }
    }

    if (boss->HasAura(SPELL_REFLECT) && PlayerbotAI::IsRanged(bot) && PlayerbotAI::IsDps(bot))
    {
        if (auto* spell = dynamic_cast<CastSpellAction*>(action))
            if (spell->getThreatType() != Action::ActionThreatType::None)
                return 0.0f;
    }

    return 1.0f;
}

float KazzakMultiplier::GetValue(Action* action)
{
    if (!FindKazzak(bot))
        return 1.0f;

    if (dynamic_cast<WbKazzakIsolateAction*>(action) || dynamic_cast<WbKazzakDispelAction*>(action))
        return 1.0f;

    // Mark carrier runs out; don't cancel that with rotation or charge.
    if (HasMarkOfKazzak(bot))
    {
        if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<WbKazzakIsolateAction*>(action))
            return 0.0f;
        if (dynamic_cast<CastReachTargetSpellAction*>(action))
            return 0.0f;
    }

    return 1.0f;
}
