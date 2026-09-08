/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WBActions.h"
#include "EncounterHelpers.h"
#include "Playerbots.h"
#include "WBHelpers.h"

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
