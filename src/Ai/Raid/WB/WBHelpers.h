/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WBHELPERS_H
#define PLAYERBOTS_WBHELPERS_H

#include "Player.h"
#include "PlayerbotAI.h"

namespace WorldBossHelpers
{
enum WorldBossNPCs : uint32
{
    NPC_AZUREGOS = 6109,
    NPC_LORD_KAZZAK = 12397,
    NPC_DOOMLORD_KAZZAK = 18728,
    NPC_YSONDRE = 14887,
    NPC_LETHON = 14888,
    NPC_EMERISS = 14889,
    NPC_TAERAR = 14890,
    NPC_DREAM_FOG = 15224,
    NPC_DEMENTED_DRUID = 15260,
    NPC_SPIRIT_SHADE = 15261,
    NPC_SHADE_OF_TAERAR = 15302
};

enum WorldBossSpells : uint32
{
    // Azuregos
    SPELL_MARK_OF_FROST = 23182,
    SPELL_MANA_STORM = 21097,
    SPELL_REFLECT = 22067,
    SPELL_ARCANE_VACUUM = 21147,

    // Kazzak (vanilla + TBC doomlord ids)
    SPELL_MARK_OF_KAZZAK = 21056,
    SPELL_MARK_OF_KAZZAK_TBC = 32960,
    SPELL_TWISTED_REFLECTION = 21063,

    // Emerald dragons (shared)
    SPELL_TAIL_SWEEP = 15847,
    SPELL_SLEEP = 24778,
    SPELL_NOXIOUS_BREATH = 24818,
    SPELL_MARK_OF_NATURE = 25040,

    // Emeriss
    SPELL_CORRUPTION_OF_EARTH = 24910,
    SPELL_VOLATILE_INFECTION = 24928,

    // Lethon
    SPELL_DRAW_SPIRIT = 24811,

    // Taerar
    SPELL_BELLOWING_ROAR = 22686,
    SPELL_SHADE = 24313,

    // Ysondre
    SPELL_LIGHTNING_WAVE = 24819
};

constexpr float SPREAD_DISTANCE = 8.0f;
constexpr float KAZZAK_SPREAD_DISTANCE = 10.0f;
constexpr float KAZZAK_MARK_ISOLATE_DISTANCE = 30.0f;
constexpr float MUSHROOM_CORPSE_DISTANCE = 10.0f;
constexpr float FOG_DISTANCE = 8.0f;
constexpr uint32 NOXIOUS_BREATH_SWAP_STACKS = 2;

Unit* FindAzuregos(Player* bot);
Unit* FindKazzak(Player* bot);
Unit* FindEmeraldDragon(Player* bot);
Unit* FindWorldBoss(Player* bot);
bool HasMarkOfKazzak(Unit const* unit);
uint32 GetNoxiousBreathStacks(Unit const* unit);
Player* SelectNextTankWithoutAura(Player* bot, uint32 auraId);
bool IsEmeraldDragon(Unit const* unit);
}

#endif
