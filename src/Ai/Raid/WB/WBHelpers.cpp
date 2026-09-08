/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WBHelpers.h"
#include "EncounterHelpers.h"
#include "Playerbots.h"
#include "SpellAuras.h"

namespace WorldBossHelpers
{
static Unit* FindAliveCreature(Player* bot, uint32 entry, float range = 150.0f)
{
    if (!bot)
        return nullptr;
    Creature* creature = bot->FindNearestCreature(entry, range, true);
    return (creature && creature->IsAlive()) ? creature : nullptr;
}

Unit* FindAzuregos(Player* bot)
{
    return FindAliveCreature(bot, NPC_AZUREGOS);
}

Unit* FindKazzak(Player* bot)
{
    if (Unit* kazzak = FindAliveCreature(bot, NPC_LORD_KAZZAK))
        return kazzak;
    return FindAliveCreature(bot, NPC_DOOMLORD_KAZZAK);
}

Unit* FindEmeraldDragon(Player* bot)
{
    static uint32 const dragons[] = { NPC_EMERISS, NPC_LETHON, NPC_TAERAR, NPC_YSONDRE };
    for (uint32 entry : dragons)
        if (Unit* dragon = FindAliveCreature(bot, entry))
            return dragon;
    return nullptr;
}

Unit* FindWorldBoss(Player* bot)
{
    if (Unit* boss = FindAzuregos(bot))
        return boss;
    if (Unit* boss = FindKazzak(bot))
        return boss;
    return FindEmeraldDragon(bot);
}

bool HasMarkOfKazzak(Unit const* unit)
{
    if (!unit)
        return false;
    return unit->HasAura(SPELL_MARK_OF_KAZZAK) || unit->HasAura(SPELL_MARK_OF_KAZZAK_TBC);
}

uint32 GetNoxiousBreathStacks(Unit const* unit)
{
    if (!unit)
        return 0;
    if (Aura const* aura = unit->GetAura(SPELL_NOXIOUS_BREATH))
        return aura->GetStackAmount();
    return 0;
}

Player* SelectNextTankWithoutAura(Player* bot, uint32 auraId)
{
    if (!bot)
        return nullptr;

    std::vector<Player*> chain;
    if (Player* mt = EncounterHelpers::GetGroupMainTank(bot))
        chain.push_back(mt);
    for (uint8 i = 0; i < 4; ++i)
    {
        if (Player* ot = EncounterHelpers::GetGroupAssistTank(bot, i))
        {
            bool already = false;
            for (Player* existing : chain)
                if (existing == ot)
                    already = true;
            if (!already)
                chain.push_back(ot);
        }
    }

    for (Player* tank : chain)
    {
        if (!tank || !tank->IsAlive() || tank->GetMapId() != bot->GetMapId())
            continue;
        if (auraId && tank->HasAura(auraId))
            continue;
        return tank;
    }
    return nullptr;
}

bool IsEmeraldDragon(Unit const* unit)
{
    if (!unit)
        return false;
    uint32 const entry = unit->GetEntry();
    return entry == NPC_EMERISS || entry == NPC_LETHON || entry == NPC_TAERAR || entry == NPC_YSONDRE;
}
}
