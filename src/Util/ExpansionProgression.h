/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_EXPANSIONPROGRESSION_H
#define PLAYERBOTS_EXPANSIONPROGRESSION_H

#include "Common.h"

// Progressive-server helpers: when the world Expansion is Classic (or TBC),
// bots must not learn or cast later-expansion class spells, and should spend
// talent points like a 1-60 (or 1-70) character rather than a WotLK 80.
namespace ExpansionProgression
{
uint32 GetWorldExpansion();
bool IsVanilla();
bool IsTbcOrEarlier();

// Trainer/class spells that did not exist (or were not baseline) in the
// current world expansion. Used both when teaching factory spells and when
// deciding whether a rotation may cast them.
bool IsSpellAllowed(uint32 spellId);
}

#endif
