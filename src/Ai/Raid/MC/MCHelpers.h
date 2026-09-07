/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MCHELPERS_H
#define PLAYERBOTS_MCHELPERS_H

#include "Common.h"
#include <vector>

class Creature;
class Player;
class Unit;

namespace MoltenCoreHelpers
{
enum MoltenCoreNPCs
{
    // Golemagg
    NPC_CORE_RAGER = 11672,

    // Majordomo Executus
    NPC_MAJORDOMO_EXECUTUS = 12018,
    NPC_FLAMEWAKER_HEALER = 11663,
    NPC_FLAMEWAKER_ELITE = 11664,

    // Core Hound (trash)
    NPC_CORE_HOUND = 11671,
};
enum MoltenCoreSpells
{
    // Baron Geddon
    SPELL_INFERNO = 19695,
    SPELL_LIVING_BOMB = 20475,

    // Golemagg
    SPELL_GOLEMAGGS_TRUST = 20553,
    SPELL_MAGMA_SPLASH = 13880,

    // Core Hound Play Dead. Nearby living hounds reignite smoldering ones.
    SPELL_PLAY_DEAD = 19822,
};

constexpr uint32 MAGMA_SPLASH_BACK_OFF_STACKS = 20;
constexpr float MAGMA_SPLASH_BACK_OFF_DISTANCE = 12.0f;

// Matches the script's GetCreaturesWithEntryInRange check: any living,
// non-smoldering Core Hound within 80 yards will full-heal a Play Dead hound.
constexpr float CORE_HOUND_PACK_RANGE = 80.0f;

// Classic pack execute: drip every fighting hound to this percent, then AoE
// so they all enter Play Dead together. 10% leaves a small buffer for queued
// casts without starting the burn so early that the pack desyncs.
constexpr float CORE_HOUND_EXECUTE_HEALTH_PCT = 10.0f;

// Below this, only a few DPS stay on a hound and new DoTs are blocked so a
// 40-player raid cannot Play Dead it before the rest of the pack is ready.
constexpr float CORE_HOUND_LOW_HEALTH_PCT = 25.0f;
constexpr float CORE_HOUND_BALANCE_BAND_PCT = 5.0f;
constexpr uint32 CORE_HOUND_MAX_DPS_PER_HOUND_HIGH = 6;
constexpr uint32 CORE_HOUND_MAX_DPS_PER_HOUND_LOW = 3;

// Shazzrah's Arcane Explosion radius
constexpr float ARCANE_EXPLOSION_DISTANCE = 26.0f;

struct CoreHoundPackState
{
    std::vector<Creature*> fighting;
    std::vector<Creature*> smoldering;

    // Two or more still fighting, or at least one smoldering while others live.
    // A lone leftover hound with no Play Dead auras is just killed normally.
    bool HasPack() const { return fighting.size() >= 2 || (!smoldering.empty() && !fighting.empty()); }

    // Planned execute (every fighting hound at or below 10%) or emergency
    // burn after someone already smoldered — remaining hounds must die first.
    bool IsBurnPhase() const;
};

CoreHoundPackState GetCoreHoundPackState(Player* bot);
Creature* SelectCoreHoundBalanceTarget(Player* bot);
bool IsFightingCoreHound(Unit const* unit);
}

#endif
