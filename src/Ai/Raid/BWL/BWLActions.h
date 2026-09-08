/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BWLACTIONS_H
#define PLAYERBOTS_BWLACTIONS_H

#include "Action.h"
#include "AttackAction.h"
#include "MovementActions.h"

// General

class BwlOnyxiaScaleCloakAuraCheckAction : public Action
{
public:
    BwlOnyxiaScaleCloakAuraCheckAction(PlayerbotAI* botAI) : Action(botAI, "bwl onyxia scale cloak aura check") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class BwlTurnOffSuppressionDeviceAction : public Action
{
public:
    BwlTurnOffSuppressionDeviceAction(PlayerbotAI* botAI) : Action(botAI, "bwl turn off suppression device") {}
    bool Execute(Event event) override;
};

// Razorgore the Untamed

class BwlRazorgoreAvoidAoeAction : public MovementAction
{
public:
    BwlRazorgoreAvoidAoeAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl razorgore avoid aoe") {}
    bool Execute(Event event) override;
};

class BwlRazorgoreMarkBossAction : public AttackAction
{
public:
    BwlRazorgoreMarkBossAction(PlayerbotAI* botAI) : AttackAction(botAI, "bwl razorgore mark boss") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class BwlRazorgoreControlAction : public MovementAction
{
public:
    BwlRazorgoreControlAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl razorgore control") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class BwlRazorgoreFocusAddsAction : public AttackAction
{
public:
    BwlRazorgoreFocusAddsAction(PlayerbotAI* botAI) : AttackAction(botAI, "bwl razorgore focus adds") {}
    bool Execute(Event event) override;
};

class BwlRazorgoreAvoidConflagrationAction : public MovementAction
{
public:
    BwlRazorgoreAvoidConflagrationAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl razorgore avoid conflagration") {}
    bool Execute(Event event) override;
};

// Vaelastrasz the Corrupt

class BwlVaelastraszMoveAwayAction : public MovementAction
{
public:
    BwlVaelastraszMoveAwayAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl vaelastrasz move away") {}
    bool Execute(Event event) override;

private:
    bool CalculateFleeDirection(Unit const* boss, float& fleeX, float& fleeY) const;
    bool MoveAlongFleeDirection(Unit const* boss, float fleeX, float fleeY);
};

class BwlVaelastraszTankSwapAction : public AttackAction
{
public:
    BwlVaelastraszTankSwapAction(PlayerbotAI* botAI) : AttackAction(botAI, "bwl vaelastrasz tank swap") {}
    bool Execute(Event event) override;
};

class BwlBroodlordAvoidBlastWaveAction : public MovementAction
{
public:
    BwlBroodlordAvoidBlastWaveAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl broodlord avoid blast wave") {}
    bool Execute(Event event) override;
};

class BwlBroodlordTankAction : public AttackAction
{
public:
    BwlBroodlordTankAction(PlayerbotAI* botAI) : AttackAction(botAI, "bwl broodlord tank") {}
    bool Execute(Event event) override;
};

class BwlDrakeWingBuffetAction : public AttackAction
{
public:
    BwlDrakeWingBuffetAction(PlayerbotAI* botAI) : AttackAction(botAI, "bwl drake wing buffet") {}
    bool Execute(Event event) override;
};

class BwlFiremawBreakLosAction : public MovementAction
{
public:
    BwlFiremawBreakLosAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl firemaw break los") {}
    bool Execute(Event event) override;
};

class BwlEbonrocTauntAction : public AttackAction
{
public:
    BwlEbonrocTauntAction(PlayerbotAI* botAI) : AttackAction(botAI, "bwl ebonroc taunt") {}
    bool Execute(Event event) override;
};

class BwlFlamegorTranqAction : public Action
{
public:
    BwlFlamegorTranqAction(PlayerbotAI* botAI) : Action(botAI, "bwl flamegor tranq") {}
    bool Execute(Event event) override;
};

// Chromaggus

class BwlUseHourglassSandAction : public Action
{
public:
    BwlUseHourglassSandAction(PlayerbotAI* botAI) : Action(botAI, "bwl use hourglass sand") {}
    bool Execute(Event event) override;
};

class BwlChromaggusBreakLosAction : public MovementAction
{
public:
    BwlChromaggusBreakLosAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl chromaggus break los") {}
    bool Execute(Event event) override;
};

class BwlChromaggusFrenzyTranqAction : public Action
{
public:
    BwlChromaggusFrenzyTranqAction(PlayerbotAI* botAI) : Action(botAI, "bwl chromaggus tranq") {}
    bool Execute(Event event) override;
};

class BwlChromaggusSheepAction : public Action
{
public:
    BwlChromaggusSheepAction(PlayerbotAI* botAI) : Action(botAI, "bwl chromaggus sheep") {}
    bool Execute(Event event) override;
};

class BwlChromaggusDispelAction : public Action
{
public:
    BwlChromaggusDispelAction(PlayerbotAI* botAI) : Action(botAI, "bwl chromaggus dispel") {}
    bool Execute(Event event) override;
};

class BwlChromaggusTimeLapseTankAction : public AttackAction
{
public:
    BwlChromaggusTimeLapseTankAction(PlayerbotAI* botAI) : AttackAction(botAI, "bwl chromaggus time lapse tank") {}
    bool Execute(Event event) override;
};

class BwlNefarianFearWardAction : public Action
{
public:
    BwlNefarianFearWardAction(PlayerbotAI* botAI) : Action(botAI, "bwl nefarian fear ward") {}
    bool Execute(Event event) override;
};

// Trash

class BwlDeathTalonWyrmguardTankMoveAwayAction : public MovementAction
{
public:
    BwlDeathTalonWyrmguardTankMoveAwayAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl death talon wyrmguard tank move away") {}
    Unit* GetTarget() override;
    bool isUseful() override;
    bool Execute(Event event) override;
};

class BwlDeathTalonWyrmguardRangedMoveAwayAction : public MovementAction
{
public:
    BwlDeathTalonWyrmguardRangedMoveAwayAction(PlayerbotAI* botAI) : MovementAction(botAI, "bwl death talon wyrmguard ranged move away") {}
    Unit* GetTarget() override;
    bool Execute(Event event) override;
};

#endif
