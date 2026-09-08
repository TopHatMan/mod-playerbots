/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BWLTriggers.h"
#include "BWLHelpers.h"
#include "Playerbots.h"
#include "SpellAuras.h"

using namespace BlackwingLairHelpers;

// General

bool BwlSuppressionDeviceTrigger::IsActive()
{
    // Until MoP, only rogues could disarm suppression devices.
    // If raid cheats are enabled, any bot can disarm the devices.
    if (botAI->HasCheat(BotCheatMask::raid) || bot->IsClass(CLASS_ROGUE))
    {
        GuidVector gos = AI_VALUE(GuidVector, "nearest game objects");
        for (auto i = gos.begin(); i != gos.end(); ++i)
        {
            GameObject const* go = botAI->GetGameObject(*i);
            if (IsActiveSuppressionDeviceInRange(go, bot))
                return true;
        }
    }
    return false;
}

// Razorgore the Untamed

bool BwlRazorgoreNotMindControlledTrigger::IsActive()
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "razorgore the untamed"))
        return !boss->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_MINDCONTROL));
    return false;
}

bool BwlRazorgoreControlTrigger::IsActive()
{
    if (!AreRazorgoreEggsAlive(botAI) || !IsRazorgoreController(bot))
        return false;
    return AI_VALUE2(Unit*, "find target", "razorgore the untamed") != nullptr ||
           FindOrbOfDomination(bot) != nullptr;
}

bool BwlRazorgoreFocusAddsTrigger::IsActive()
{
    if (!AreRazorgoreEggsAlive(botAI) || IsRazorgoreController(bot))
        return false;
    return SelectRazorgoreAddTarget(bot) != nullptr;
}

bool BwlRazorgoreConflagrationTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorgore the untamed");
    if (!boss || AreRazorgoreEggsAlive(botAI))
        return false;

    Unit* victim = boss->GetVictim();
    if (!victim || victim == bot)
        return false;
    if (!victim->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_CONFLAGRATION)))
        return false;

    return bot->GetDistance2d(victim) < RAZORGORE_CONFLAG_DISTANCE;
}

// Vaelastrasz the Corrupt

bool BwlVaelastraszPositioningTrigger::IsActive()
{
    // Prevent non-tanks from rotating the boss while the tanks gain thread.
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "vaelastrasz the corrupt"))
        return boss->GetVictim() != bot;
    return false;
}

bool BwlVaelastraszBurningAdrenalineTrigger::IsActive()
{
    // No check for Vaelastrasz, because bots may still have burning adrenaline even after Vaelastrasz died.
    return bot->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BURNING_ADRENALINE));
}

bool BwlVaelastraszTankSwapTrigger::IsActive()
{
    if (!PlayerbotAI::IsTank(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "vaelastrasz the corrupt");
    if (!boss || !boss->IsAlive())
        return false;

    Unit* victim = boss->GetVictim();
    if (!victim || victim == bot)
        return false;
    if (!victim->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BURNING_ADRENALINE)))
        return false;

    return SelectNextVaelastraszTank(bot) == bot;
}

bool BwlBroodlordPositioningTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "broodlord lashlayer");
    if (!boss || boss->GetVictim() == bot)
        return false;
    return !PlayerbotAI::IsTank(bot);
}

bool BwlBroodlordBlastWaveTrigger::IsActive()
{
    if (PlayerbotAI::IsTank(bot) || !PlayerbotAI::IsRanged(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "broodlord lashlayer");
    if (!boss)
        return false;

    return bot->GetDistance2d(boss) < BROODLORD_BLASTWAVE_DISTANCE;
}

bool BwlBroodlordTankTrigger::IsActive()
{
    if (!PlayerbotAI::IsTank(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "broodlord lashlayer");
    if (!boss)
        return false;

    Unit* victim = boss->GetVictim();
    if (!victim || victim == bot)
        return false;

    Player* victimPlayer = victim->ToPlayer();
    return !victimPlayer || !PlayerbotAI::IsTank(victimPlayer);
}

bool BwlDrakePositioningTrigger::IsActive()
{
    Unit* drake = FindBwlDrake(bot);
    if (!drake || drake->GetVictim() == bot)
        return false;
    return !PlayerbotAI::IsTank(bot);
}

bool BwlDrakeWingBuffetTrigger::IsActive()
{
    if (!PlayerbotAI::IsTank(bot))
        return false;

    Unit* drake = FindBwlDrake(bot);
    if (!drake)
        return false;

    Unit* victim = drake->GetVictim();
    if (!victim || victim == bot)
        return false;

    Player* victimPlayer = victim->ToPlayer();
    if (PlayerbotAI::IsMainTank(bot))
        return true;
    return !victimPlayer || !PlayerbotAI::IsTank(victimPlayer);
}

bool BwlFiremawFlameBuffetTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "firemaw");
    if (!boss)
        return false;

    uint32 const stacks = GetFlameBuffetStacks(bot);
    uint32 const threshold = PlayerbotAI::IsTank(bot) ? 8 : static_cast<uint32>(FLAME_BUFFET_LOS_STACKS);
    if (stacks < threshold)
        return false;

    // Stay out until the debuff actually expires.
    return true;
}

bool BwlEbonrocShadowTrigger::IsActive()
{
    if (!PlayerbotAI::IsTank(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "ebonroc");
    if (!boss)
        return false;

    Unit* victim = boss->GetVictim();
    if (!victim || victim == bot)
        return false;
    if (!victim->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_SHADOW_OF_EBONROC)))
        return false;

    return SelectNextTankWithoutAura(bot, static_cast<uint32>(BlackwingLairSpells::SPELL_SHADOW_OF_EBONROC)) == bot;
}

bool BwlFlamegorFrenzyTrigger::IsActive()
{
    if (bot->getClass() != CLASS_HUNTER)
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "flamegor");
    if (!boss)
        return false;

    Aura* frenzy = boss->GetAura(static_cast<uint32>(BlackwingLairSpells::SPELL_FLAMEGOR_FRENZY));
    if (!frenzy)
        return false;

    int32 const elapsed = frenzy->GetMaxDuration() - frenzy->GetDuration();
    return elapsed >= static_cast<int32>(GetAliveHunterIndex(bot) * TRANQ_STAGGER_MS);
}

// Chromaggus

bool BwlAfflictionBronzeTrigger::IsActive()
{
    return bot->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BROOD_AFFLICTION_BRONZE));
}

// Nefarian

bool BwlWildMagicTrigger::IsActive()
{
    return bot->getClass() == CLASS_MAGE &&
        bot->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_WILD_MAGIC));
}

bool BwlNefarianFearWardTrigger::IsActive()
{
    if (bot->getClass() != CLASS_PRIEST)
        return false;

    Unit* nefarian = AI_VALUE2(Unit*, "find target", "nefarian");
    if (!nefarian || !nefarian->IsInCombat())
        return false;

    Unit* victim = nefarian->GetVictim();
    if (!victim)
        return false;

    return !botAI->HasAura("fear ward", victim);
}

// Trash

bool BwlDeathTalonWyrmguardTankTrigger::IsActive()
{
    return PlayerbotAI::IsTank(bot) && AI_VALUE2(Unit*, "find target", "death talon wyrmguard");
}

bool BwlDeathTalonWyrmguardRangedTrigger::IsActive()
{
    return PlayerbotAI::IsRanged(bot) && AI_VALUE2(Unit*, "find target", "death talon wyrmguard");
}
