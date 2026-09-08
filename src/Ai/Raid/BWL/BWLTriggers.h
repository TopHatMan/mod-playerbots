/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BWLTRIGGERS_H
#define PLAYERBOTS_BWLTRIGGERS_H

#include "Trigger.h"

// General

class BwlSuppressionDeviceTrigger : public Trigger
{
public:
    BwlSuppressionDeviceTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl suppression device") {}
    bool IsActive() override;
};

// Razorgore the Untamed

class BwlRazorgoreNotMindControlledTrigger : public Trigger
{
public:
    BwlRazorgoreNotMindControlledTrigger(PlayerbotAI* botAI, std::string const& name = "bwl razorgore not mind controlled") : Trigger(botAI, name) {}
    bool IsActive() override;
};

class BwlRazorgoreControlTrigger : public Trigger
{
public:
    BwlRazorgoreControlTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl razorgore control") {}
    bool IsActive() override;
};

class BwlRazorgoreFocusAddsTrigger : public Trigger
{
public:
    BwlRazorgoreFocusAddsTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl razorgore focus adds") {}
    bool IsActive() override;
};

class BwlRazorgoreConflagrationTrigger : public Trigger
{
public:
    BwlRazorgoreConflagrationTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl razorgore conflagration") {}
    bool IsActive() override;
};

// Vaelastrasz the Corrupt

class BwlVaelastraszPositioningTrigger : public Trigger
{
public:
    BwlVaelastraszPositioningTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl vaelastrasz positioning") {}
    bool IsActive() override;
};

class BwlVaelastraszBurningAdrenalineTrigger : public Trigger
{
public:
    BwlVaelastraszBurningAdrenalineTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl vaelastrasz burning adrenaline") {}
    bool IsActive() override;
};

class BwlVaelastraszTankSwapTrigger : public Trigger
{
public:
    BwlVaelastraszTankSwapTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl vaelastrasz tank swap") {}
    bool IsActive() override;
};

class BwlBroodlordPositioningTrigger : public Trigger
{
public:
    BwlBroodlordPositioningTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl broodlord positioning") {}
    bool IsActive() override;
};

class BwlBroodlordBlastWaveTrigger : public Trigger
{
public:
    BwlBroodlordBlastWaveTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl broodlord blast wave") {}
    bool IsActive() override;
};

class BwlBroodlordTankTrigger : public Trigger
{
public:
    BwlBroodlordTankTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl broodlord tank") {}
    bool IsActive() override;
};

class BwlDrakePositioningTrigger : public Trigger
{
public:
    BwlDrakePositioningTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl drake positioning") {}
    bool IsActive() override;
};

class BwlDrakeWingBuffetTrigger : public Trigger
{
public:
    BwlDrakeWingBuffetTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl drake wing buffet") {}
    bool IsActive() override;
};

class BwlFiremawFlameBuffetTrigger : public Trigger
{
public:
    BwlFiremawFlameBuffetTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl firemaw flame buffet") {}
    bool IsActive() override;
};

class BwlEbonrocShadowTrigger : public Trigger
{
public:
    BwlEbonrocShadowTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl ebonroc shadow") {}
    bool IsActive() override;
};

class BwlFlamegorFrenzyTrigger : public Trigger
{
public:
    BwlFlamegorFrenzyTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl flamegor frenzy") {}
    bool IsActive() override;
};

// Chromaggus

class BwlAfflictionBronzeTrigger : public Trigger
{
public:
    BwlAfflictionBronzeTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl affliction bronze") {}
    bool IsActive() override;
};

class BwlChromaggusBreathTrigger : public Trigger
{
public:
    BwlChromaggusBreathTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl chromaggus breath") {}
    bool IsActive() override;
};

class BwlChromaggusFrenzyTrigger : public Trigger
{
public:
    BwlChromaggusFrenzyTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl chromaggus frenzy") {}
    bool IsActive() override;
};

class BwlChromaggusMutationTrigger : public Trigger
{
public:
    BwlChromaggusMutationTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl chromaggus mutation") {}
    bool IsActive() override;
};

class BwlChromaggusDispelTrigger : public Trigger
{
public:
    BwlChromaggusDispelTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl chromaggus dispel") {}
    bool IsActive() override;
};

class BwlChromaggusTimeLapseTrigger : public Trigger
{
public:
    BwlChromaggusTimeLapseTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl chromaggus time lapse") {}
    bool IsActive() override;
};

// Nefarian

class BwlWildMagicTrigger : public Trigger
{
public:
    BwlWildMagicTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl wild magic") {}
    bool IsActive() override;
};

class BwlNefarianFearWardTrigger : public Trigger
{
public:
    BwlNefarianFearWardTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl nefarian fear ward") {}
    bool IsActive() override;
};

// Trash

class BwlDeathTalonWyrmguardTankTrigger : public Trigger
{
public:
    BwlDeathTalonWyrmguardTankTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl death talon wyrmguard tank") {}
    bool IsActive() override;
};

class BwlDeathTalonWyrmguardRangedTrigger : public Trigger
{
public:
    BwlDeathTalonWyrmguardRangedTrigger(PlayerbotAI* botAI) : Trigger(botAI, "bwl death talon wyrmguard ranged") {}
    bool IsActive() override;
};

#endif
