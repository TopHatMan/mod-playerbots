/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WBACTIONS_H
#define PLAYERBOTS_WBACTIONS_H

#include "AttackAction.h"
#include "MovementActions.h"

class WbAzuregosSpreadAction : public MovementAction
{
public:
    WbAzuregosSpreadAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb azuregos spread") {}
    bool Execute(Event event) override;
};

class WbAzuregosMoveFromStormAction : public MovementAction
{
public:
    WbAzuregosMoveFromStormAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb azuregos move from storm") {}
    bool Execute(Event event) override;
};

class WbAzuregosStopForReflectAction : public Action
{
public:
    WbAzuregosStopForReflectAction(PlayerbotAI* botAI) : Action(botAI, "wb azuregos stop for reflect") {}
    bool Execute(Event event) override;
};

class WbAzuregosTankTauntAction : public AttackAction
{
public:
    WbAzuregosTankTauntAction(PlayerbotAI* botAI) : AttackAction(botAI, "wb azuregos tank taunt") {}
    bool Execute(Event event) override;
};

class WbAzuregosMoveToFlankAction : public MovementAction
{
public:
    WbAzuregosMoveToFlankAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb azuregos move to flank") {}
    bool Execute(Event event) override;
};

class WbKazzakIsolateAction : public MovementAction
{
public:
    WbKazzakIsolateAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb kazzak isolate") {}
    bool Execute(Event event) override;
};

class WbKazzakSpreadAction : public MovementAction
{
public:
    WbKazzakSpreadAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb kazzak spread") {}
    bool Execute(Event event) override;
};

class WbKazzakDispelAction : public Action
{
public:
    WbKazzakDispelAction(PlayerbotAI* botAI) : Action(botAI, "wb kazzak dispel") {}
    bool Execute(Event event) override;
};

class WbDragonFlankAction : public MovementAction
{
public:
    WbDragonFlankAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb dragon flank") {}
    bool Execute(Event event) override;
};

class WbDragonSpreadAction : public MovementAction
{
public:
    WbDragonSpreadAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb dragon spread") {}
    bool Execute(Event event) override;
};

class WbDragonTankSwapAction : public AttackAction
{
public:
    WbDragonTankSwapAction(PlayerbotAI* botAI) : AttackAction(botAI, "wb dragon tank swap") {}
    bool Execute(Event event) override;
};

class WbDragonMoveFromFogAction : public MovementAction
{
public:
    WbDragonMoveFromFogAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb dragon move from fog") {}
    bool Execute(Event event) override;
};

class WbEmerissAvoidMushroomAction : public MovementAction
{
public:
    WbEmerissAvoidMushroomAction(PlayerbotAI* botAI) : MovementAction(botAI, "wb emeriss avoid mushroom") {}
    bool Execute(Event event) override;
};

class WbEmerissDispelAction : public Action
{
public:
    WbEmerissDispelAction(PlayerbotAI* botAI) : Action(botAI, "wb emeriss dispel") {}
    bool Execute(Event event) override;
};

class WbLethonKillShadeAction : public AttackAction
{
public:
    WbLethonKillShadeAction(PlayerbotAI* botAI) : AttackAction(botAI, "wb lethon kill shade") {}
    bool Execute(Event event) override;
};

class WbTaerarKillShadeAction : public AttackAction
{
public:
    WbTaerarKillShadeAction(PlayerbotAI* botAI) : AttackAction(botAI, "wb taerar kill shade") {}
    bool Execute(Event event) override;
};

class WbTaerarTankShadeAction : public AttackAction
{
public:
    WbTaerarTankShadeAction(PlayerbotAI* botAI) : AttackAction(botAI, "wb taerar tank shade") {}
    bool Execute(Event event) override;
};

class WbTaerarAntiFearAction : public Action
{
public:
    WbTaerarAntiFearAction(PlayerbotAI* botAI) : Action(botAI, "wb taerar anti fear") {}
    bool Execute(Event event) override;
};

#endif
