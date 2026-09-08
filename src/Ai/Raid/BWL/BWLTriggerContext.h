/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BWLTRIGGERCONTEXT_H
#define PLAYERBOTS_BWLTRIGGERCONTEXT_H

#include "BWLTriggers.h"
#include "BossAuraTriggers.h"
#include "NamedObjectContext.h"

class RaidBwlTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidBwlTriggerContext()
    {
        creators["bwl suppression device"] = &RaidBwlTriggerContext::bwl_suppression_device;

        creators["bwl razorgore fire resistance"] = &RaidBwlTriggerContext::bwl_razorgore_fire_resistance_trigger;
        creators["bwl razorgore not mind controlled"] = &RaidBwlTriggerContext::bwl_razorgore_not_mind_controlled;
        creators["bwl razorgore control"] = &RaidBwlTriggerContext::bwl_razorgore_control;
        creators["bwl razorgore focus adds"] = &RaidBwlTriggerContext::bwl_razorgore_focus_adds;
        creators["bwl razorgore conflagration"] = &RaidBwlTriggerContext::bwl_razorgore_conflagration;

        creators["bwl vaelastrasz fire resistance"] = &RaidBwlTriggerContext::bwl_vaelastrasz_fire_resistance_trigger;
        creators["bwl vaelastrasz positioning"] = &RaidBwlTriggerContext::bwl_vaelastrasz_positioning;
        creators["bwl vaelastrasz burning adrenaline"] = &RaidBwlTriggerContext::bwl_vaelastrasz_burning_adrenaline;
        creators["bwl vaelastrasz tank swap"] = &RaidBwlTriggerContext::bwl_vaelastrasz_tank_swap;

        creators["bwl broodlord fire resistance"] = &RaidBwlTriggerContext::bwl_broodlord_fire_resistance_trigger;
        creators["bwl broodlord positioning"] = &RaidBwlTriggerContext::bwl_broodlord_positioning;
        creators["bwl broodlord blast wave"] = &RaidBwlTriggerContext::bwl_broodlord_blast_wave;
        creators["bwl broodlord tank"] = &RaidBwlTriggerContext::bwl_broodlord_tank;

        creators["bwl firemaw fire resistance"] = &RaidBwlTriggerContext::bwl_firemaw_fire_resistance_trigger;
        creators["bwl flamegor fire resistance"] = &RaidBwlTriggerContext::bwl_flamegor_fire_resistance_trigger;
        creators["bwl drake positioning"] = &RaidBwlTriggerContext::bwl_drake_positioning;
        creators["bwl drake wing buffet"] = &RaidBwlTriggerContext::bwl_drake_wing_buffet;
        creators["bwl firemaw flame buffet"] = &RaidBwlTriggerContext::bwl_firemaw_flame_buffet;
        creators["bwl ebonroc shadow"] = &RaidBwlTriggerContext::bwl_ebonroc_shadow;
        creators["bwl flamegor frenzy"] = &RaidBwlTriggerContext::bwl_flamegor_frenzy;

        creators["bwl affliction bronze"] = &RaidBwlTriggerContext::bwl_affliction_bronze;
        creators["bwl wild magic"] = &RaidBwlTriggerContext::bwl_wild_magic;
        creators["bwl nefarian fear ward"] = &RaidBwlTriggerContext::bwl_nefarian_fear_ward;
        creators["bwl death talon wyrmguard tank"] = &RaidBwlTriggerContext::bwl_death_talon_wyrmguard_tank;
        creators["bwl death talon wyrmguard ranged"] = &RaidBwlTriggerContext::bwl_death_talon_wyrmguard_ranged;
    }

private:
    static Trigger* bwl_suppression_device(PlayerbotAI* ai) { return new BwlSuppressionDeviceTrigger(ai); }
    static Trigger* bwl_razorgore_fire_resistance_trigger(PlayerbotAI* ai) { return new BossFireResistanceTrigger(ai, "razorgore the untamed"); }
    static Trigger* bwl_razorgore_not_mind_controlled(PlayerbotAI* ai) { return new BwlRazorgoreNotMindControlledTrigger(ai); }
    static Trigger* bwl_razorgore_control(PlayerbotAI* ai) { return new BwlRazorgoreControlTrigger(ai); }
    static Trigger* bwl_razorgore_focus_adds(PlayerbotAI* ai) { return new BwlRazorgoreFocusAddsTrigger(ai); }
    static Trigger* bwl_razorgore_conflagration(PlayerbotAI* ai) { return new BwlRazorgoreConflagrationTrigger(ai); }
    static Trigger* bwl_vaelastrasz_fire_resistance_trigger(PlayerbotAI* ai) { return new BossFireResistanceTrigger(ai, "vaelastrasz the corrupt"); }
    static Trigger* bwl_vaelastrasz_positioning(PlayerbotAI* ai) { return new BwlVaelastraszPositioningTrigger(ai); }
    static Trigger* bwl_vaelastrasz_burning_adrenaline(PlayerbotAI* ai) { return new BwlVaelastraszBurningAdrenalineTrigger(ai); }
    static Trigger* bwl_vaelastrasz_tank_swap(PlayerbotAI* ai) { return new BwlVaelastraszTankSwapTrigger(ai); }
    static Trigger* bwl_broodlord_fire_resistance_trigger(PlayerbotAI* ai) { return new BossFireResistanceTrigger(ai, "broodlord lashlayer"); }
    static Trigger* bwl_broodlord_positioning(PlayerbotAI* ai) { return new BwlBroodlordPositioningTrigger(ai); }
    static Trigger* bwl_broodlord_blast_wave(PlayerbotAI* ai) { return new BwlBroodlordBlastWaveTrigger(ai); }
    static Trigger* bwl_broodlord_tank(PlayerbotAI* ai) { return new BwlBroodlordTankTrigger(ai); }
    static Trigger* bwl_firemaw_fire_resistance_trigger(PlayerbotAI* ai) { return new BossFireResistanceTrigger(ai, "firemaw"); }
    static Trigger* bwl_flamegor_fire_resistance_trigger(PlayerbotAI* ai) { return new BossFireResistanceTrigger(ai, "flamegor"); }
    static Trigger* bwl_drake_positioning(PlayerbotAI* ai) { return new BwlDrakePositioningTrigger(ai); }
    static Trigger* bwl_drake_wing_buffet(PlayerbotAI* ai) { return new BwlDrakeWingBuffetTrigger(ai); }
    static Trigger* bwl_firemaw_flame_buffet(PlayerbotAI* ai) { return new BwlFiremawFlameBuffetTrigger(ai); }
    static Trigger* bwl_ebonroc_shadow(PlayerbotAI* ai) { return new BwlEbonrocShadowTrigger(ai); }
    static Trigger* bwl_flamegor_frenzy(PlayerbotAI* ai) { return new BwlFlamegorFrenzyTrigger(ai); }
    static Trigger* bwl_affliction_bronze(PlayerbotAI* ai) { return new BwlAfflictionBronzeTrigger(ai); }
    static Trigger* bwl_wild_magic(PlayerbotAI* ai) { return new BwlWildMagicTrigger(ai); }
    static Trigger* bwl_nefarian_fear_ward(PlayerbotAI* ai) { return new BwlNefarianFearWardTrigger(ai); }
    static Trigger* bwl_death_talon_wyrmguard_tank(PlayerbotAI* ai) { return new BwlDeathTalonWyrmguardTankTrigger(ai); }
    static Trigger* bwl_death_talon_wyrmguard_ranged(PlayerbotAI* ai) { return new BwlDeathTalonWyrmguardRangedTrigger(ai); }
};

#endif
