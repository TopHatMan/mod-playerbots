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

#endif
