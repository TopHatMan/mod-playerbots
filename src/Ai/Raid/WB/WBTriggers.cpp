/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WBTriggers.h"
#include "EncounterHelpers.h"
#include "Playerbots.h"
#include "WBHelpers.h"

using namespace WorldBossHelpers;

bool WbAzuregosSpreadTrigger::IsActive()
{
    Unit* boss = FindAzuregos(bot);
    if (!boss || !boss->IsInCombat() || PlayerbotAI::IsTank(bot))
        return false;

    GuidVector nearby = AI_VALUE(GuidVector, "nearest friendly players");
    for (ObjectGuid guid : nearby)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit != bot && unit->IsAlive() && bot->GetDistance(unit) < SPREAD_DISTANCE)
            return true;
    }
    return false;
}

bool WbAzuregosManastormTrigger::IsActive()
{
    Unit* boss = FindAzuregos(bot);
    if (!boss || !boss->IsInCombat())
        return false;

    if (bot->HasAura(SPELL_MANA_STORM))
        return true;

    return !EncounterHelpers::GetDynamicObjectPositions(bot, 10.0f, SPELL_MANA_STORM).empty();
}

bool WbAzuregosReflectTrigger::IsActive()
{
    if (!PlayerbotAI::IsRanged(bot) || !PlayerbotAI::IsDps(bot))
        return false;
    Unit* boss = FindAzuregos(bot);
    return boss && boss->HasAura(SPELL_REFLECT);
}

bool WbAzuregosTeleportTrigger::IsActive()
{
    if (!PlayerbotAI::IsTank(bot))
        return false;

    Unit* boss = FindAzuregos(bot);
    if (!boss || !boss->IsInCombat())
        return false;

    if (boss->HasUnitState(UNIT_STATE_CASTING))
    {
        Spell* current = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (current && current->m_spellInfo && current->m_spellInfo->Id == SPELL_ARCANE_VACUUM)
            return true;
    }

    Unit* victim = boss->GetVictim();
    if (!victim || victim == bot)
        return false;
    Player* victimPlayer = victim->ToPlayer();
    return !victimPlayer || !PlayerbotAI::IsTank(victimPlayer);
}

bool WbAzuregosFlankTrigger::IsActive()
{
    Unit* boss = FindAzuregos(bot);
    if (!boss || !boss->IsInCombat() || PlayerbotAI::IsTank(bot) || boss->GetVictim() == bot)
        return false;

    float diff = fabs(boss->GetAngle(bot) - boss->GetOrientation());
    if (diff > static_cast<float>(M_PI))
        diff = 2.0f * static_cast<float>(M_PI) - diff;
    return diff < static_cast<float>(M_PI) / 4.0f || diff > 3.0f * static_cast<float>(M_PI) / 4.0f;
}
