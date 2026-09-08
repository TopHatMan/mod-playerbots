/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ONYHELPERS_H
#define PLAYERBOTS_ONYHELPERS_H

#include "Player.h"
#include "Unit.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

namespace OnyxiaHelpers
{
enum OnyxiaNPCs : uint32
{
    NPC_ONYXIA = 10184,
    NPC_ONYXIAN_WHELP = 11262,
    NPC_ONYXIA_TRIGGER = 12758
};

enum OnyxiaSpells : uint32
{
    SPELL_WINGBUFFET = 18500,
    SPELL_FLAMEBREATH = 18435,
    SPELL_FIREBALL = 18392,
    SPELL_BELLOWINGROAR = 18431,
    SPELL_ERUPTION = 17731
};

constexpr float FRONT_HALF_ANGLE = static_cast<float>(M_PI) / 4.0f;
constexpr float TAIL_HALF_ANGLE = static_cast<float>(M_PI) / 4.0f;
constexpr float MELEE_SIDE_DISTANCE = 5.5f;
constexpr float RANGED_SIDE_DISTANCE = 20.0f;
constexpr float SPREAD_DISTANCE = 8.0f;
constexpr float EGG_AVOID_RADIUS = 12.0f;
constexpr float ERUPTION_DISTANCE = 10.0f;
constexpr float TANK_WALL_TOLERANCE = 8.0f;
constexpr uint32 DEEP_BREATH_HOLD_MS = 6000;

// Head-to-the-north-wall tank camp. +X is the far wall of the lair.
constexpr float TANK_WALL_X = 18.0f;
constexpr float TANK_WALL_Y = -214.0f;
constexpr float TANK_WALL_Z = -85.5f;

inline bool IsDeepBreathSpell(uint32 spellId)
{
    switch (spellId)
    {
        case 17086:  // N to S
        case 18351:  // S to N
        case 18576:  // E to W
        case 18609:  // W to E
        case 18564:  // SE to NW
        case 18584:  // NW to SE
        case 18596:  // SW to NE
        case 18617:  // NE to SW
            return true;
        default:
            return false;
    }
}

inline bool IsOnyxiaGrounded(Unit const* boss)
{
    return boss && boss->IsInCombat() && !boss->IsFlying() && boss->GetVictim();
}

inline bool IsOnyxiaAirborne(Unit const* boss)
{
    return boss && boss->IsInCombat() && boss->IsFlying();
}

inline float FacingDiff(Unit const* boss, Player const* bot)
{
    float diff = fabs(boss->GetAngle(bot) - boss->GetOrientation());
    if (diff > static_cast<float>(M_PI))
        diff = 2.0f * static_cast<float>(M_PI) - diff;
    return diff;
}

inline bool IsInFrontCone(Unit const* boss, Player const* bot)
{
    return FacingDiff(boss, bot) < FRONT_HALF_ANGLE;
}

inline bool IsInTailCone(Unit const* boss, Player const* bot)
{
    return FacingDiff(boss, bot) > (static_cast<float>(M_PI) - TAIL_HALF_ANGLE);
}

inline bool IsInFrontOrTail(Unit const* boss, Player const* bot)
{
    return IsInFrontCone(boss, bot) || IsInTailCone(boss, bot);
}

inline bool IsNearWhelpCave(Player const* bot)
{
    Position const botPos(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
    return botPos.GetExactDist2d(-35.0f, -165.0f) <= EGG_AVOID_RADIUS ||
           botPos.GetExactDist2d(-35.0f, -260.0f) <= EGG_AVOID_RADIUS;
}
}

#endif
