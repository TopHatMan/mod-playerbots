/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BWLHELPERS_H
#define PLAYERBOTS_BWLHELPERS_H

#include "Player.h"
#include "PlayerbotAI.h"

class GameObject;

namespace BlackwingLairHelpers
{
    enum class BlackwingLairSpells : uint32
    {
        // General
        SPELL_ONYXIA_SCALE_CLOAK = 22683,

        // Razorgore the Untamed
        SPELL_MINDCONTROL = 19832,
        SPELL_EGG_DESTROY = 19873,
        SPELL_MIND_EXHAUSTION = 23958,
        SPELL_CONFLAGRATION = 23023,

        // Vaelastrasz the Corrupt
        SPELL_BURNING_ADRENALINE = 18173,

        // Broodlord Lashlayer
        SPELL_BROODLORD_BLASTWAVE = 23331,
        SPELL_BROODLORD_KNOCKBACK = 25778,
        SPELL_BROODLORD_MORTALSTRIKE = 24573,

        // Drakes
        SPELL_SHADOWFLAME = 22539,
        SPELL_WINGBUFFET = 23339,
        SPELL_FLAMEBUFFET = 23341,
        SPELL_SHADOW_OF_EBONROC = 23340,
        SPELL_FLAMEGOR_FRENZY = 23342,

        // Chromaggus
        SPELL_BROOD_AFFLICTION_BLUE = 23153,
        SPELL_BROOD_AFFLICTION_BLACK = 23154,
        SPELL_BROOD_AFFLICTION_RED = 23155,
        SPELL_BROOD_AFFLICTION_BRONZE = 23170,
        SPELL_BROOD_AFFLICTION_GREEN = 23169,
        SPELL_CHROMATIC_MUTATION = 23174,
        SPELL_HOURGLASS_SAND = 23645,
        SPELL_CHROMAGGUS_FRENZY = 23128,
        SPELL_INCINERATE = 23308,
        SPELL_TIMELAPSE = 23310,
        SPELL_CORROSIVEACID = 23313,
        SPELL_IGNITEFLESH = 23315,
        SPELL_FROSTBURN = 23187,

        // Nefarian
        SPELL_WILD_MAGIC = 23410,
        SPELL_NEF_WARRIOR = 23397,
        SPELL_NEF_DRUID = 23398,
        SPELL_NEF_PRIEST = 23401,
        SPELL_NEF_PALADIN = 23418,
        SPELL_NEF_SHAMAN = 23425,
        SPELL_NEF_WARLOCK = 23427,
        SPELL_NEF_HUNTER = 23436,
        SPELL_NEF_ROGUE = 23414,
        SPELL_NEF_VEIL_OF_SHADOW = 22687,
        SPELL_NEF_BELLOWING_ROAR = 22686
    };

    enum class BlackwingLairGameObjects : uint32
    {
        GO_SUPPRESSION_DEVICE = 179784,
        GO_BLACK_DRAGON_EGG = 177807,
        GO_ORB_OF_DOMINATION = 177808
    };

    enum class BlackwingLairNPCs : uint32
    {
        NPC_RAZORGORE = 12435,
        NPC_BLACKWING_DRAGON = 12422,
        NPC_BLACKWING_LEGIONAIRE = 12416,
        NPC_BLACKWING_MAGE = 12420,
        NPC_BLACKWING_WARLOCK = 12459,
        NPC_VAELASTRAZ = 13020,
        NPC_BROODLORD = 12017,
        NPC_FIREMAW = 11983,
        NPC_EBONROC = 14601,
        NPC_FLAMEGOR = 11981,
        NPC_CHROMAGGUS = 14020,
        NPC_NEFARIAN = 11583,
        NPC_BONE_CONSTRUCT = 14605,
        NPC_TOTEM_C_FIRE_NOVA = 14662,
        NPC_TOTEM_C_STONESKIN = 14663,
        NPC_TOTEM_C_HEALING = 14664,
        NPC_TOTEM_C_WINDFURY = 14666,
        NPC_DEATH_TALON_WYRMGUARD = 12460,
        NPC_INFERNAL = 89,
        NPC_BRONZE_DRAKONID = 14263,
        NPC_BLUE_DRAKONID = 14261,
        NPC_RED_DRAKONID = 14264,
        NPC_GREEN_DRAKONID = 14262,
        NPC_BLACK_DRAKONID = 14265,
        NPC_CHROMATIC_DRAKONID = 14302
    };

    constexpr float RAZORGORE_ADD_RANGE = 80.0f;
    constexpr float RAZORGORE_CONFLAG_DISTANCE = 10.0f;
    constexpr float FLAME_BUFFET_LOS_STACKS = 5;
    constexpr float BROODLORD_BLASTWAVE_DISTANCE = 12.0f;
    constexpr uint32 TRANQ_STAGGER_MS = 1500;

    bool IsActiveSuppressionDeviceInRange(GameObject const* go, Player const* bot);
    bool AreRazorgoreEggsAlive(PlayerbotAI* botAI);
    bool IsRazorgoreOffTank(Player* bot);
    bool IsNonBABotNearPosition(Player const* bot, Position const& position, float distance);

    Player* SelectRazorgoreController(Player* bot);
    bool IsRazorgoreController(Player* bot);
    bool IsRazorgoreAdd(Unit const* unit);
    Unit* SelectRazorgoreAddTarget(Player* bot);
    GameObject* FindOrbOfDomination(Player* bot);
    GameObject* FindNearestRazorgoreEgg(Player* bot);
    Player* SelectNextTankWithoutAura(Player* bot, uint32 auraId);
    Player* SelectNextVaelastraszTank(Player* bot);
    Unit* FindBwlDrake(Player* bot);
    uint32 GetFlameBuffetStacks(Unit const* unit);
    uint32 GetAliveHunterIndex(Player* bot);
    bool IsChromaggusBreathSpell(uint32 spellId);
    bool IsNefarianClassCallAura(Player const* bot);
    Unit* SelectNefarianAddTarget(Player* bot);
    bool IsNefarianTotem(Unit const* unit);
    bool IsNefarianDrakonid(Unit const* unit);
}

#endif
