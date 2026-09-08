/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ExpansionProgression.h"
#include "SharedDefines.h"
#include "World.h"

namespace ExpansionProgression
{
uint32 GetWorldExpansion()
{
    return sWorld->getIntConfig(CONFIG_EXPANSION);
}

bool IsVanilla()
{
    return GetWorldExpansion() == EXPANSION_CLASSIC;
}

bool IsTbcOrEarlier()
{
    return GetWorldExpansion() < EXPANSION_WRATH_OF_THE_LICH_KING;
}

bool IsSpellAllowed(uint32 spellId)
{
    if (!spellId)
        return false;

    uint32 const expansion = GetWorldExpansion();
    if (expansion >= EXPANSION_WRATH_OF_THE_LICH_KING)
        return true;

    // WotLK class/talent spells live in this id range.
    if (spellId >= 48000)
        return false;

    if (expansion >= EXPANSION_THE_BURNING_CRUSADE)
        return true;

    // Most TBC spells were added above ~26800. Vanilla 1.12 class spells sit below.
    if (spellId >= 26800)
        return false;

    switch (spellId)
    {
        // Warrior (TBC)
        case 469:    // Commanding Shout
        case 3411:   // Intervene
        case 20243:  // Devastate
        case 23920:  // Spell Reflection
            return false;

        // Paladin (TBC)
        case 31884:  // Avenging Wrath
            return false;

        // Hunter (TBC)
        case 34026:  // Kill Command
            return false;

        // Priest (TBC)
        case 32375:  // Mass Dispel
            return false;

        // Shaman (TBC)
        case 974:    // Earth Shield
        case 3738:   // Wrath of Air Totem
        case 24398:  // Water Shield
            return false;

        default:
            return true;
    }
}
}
