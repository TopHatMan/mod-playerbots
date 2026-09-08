/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WBTRIGGERS_H
#define PLAYERBOTS_WBTRIGGERS_H

#include "Trigger.h"

class WbAzuregosSpreadTrigger : public Trigger
{
public:
    WbAzuregosSpreadTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb azuregos spread") {}
    bool IsActive() override;
};

class WbAzuregosManastormTrigger : public Trigger
{
public:
    WbAzuregosManastormTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb azuregos manastorm") {}
    bool IsActive() override;
};

class WbAzuregosReflectTrigger : public Trigger
{
public:
    WbAzuregosReflectTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb azuregos reflect") {}
    bool IsActive() override;
};

class WbAzuregosTeleportTrigger : public Trigger
{
public:
    WbAzuregosTeleportTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb azuregos teleport") {}
    bool IsActive() override;
};

class WbAzuregosFlankTrigger : public Trigger
{
public:
    WbAzuregosFlankTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb azuregos flank") {}
    bool IsActive() override;
};

class WbKazzakMarkTrigger : public Trigger
{
public:
    WbKazzakMarkTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb kazzak mark") {}
    bool IsActive() override;
};

class WbKazzakSpreadTrigger : public Trigger
{
public:
    WbKazzakSpreadTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb kazzak spread") {}
    bool IsActive() override;
};

class WbKazzakTwistedTrigger : public Trigger
{
public:
    WbKazzakTwistedTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb kazzak twisted") {}
    bool IsActive() override;
};

class WbKazzakDecurseTrigger : public Trigger
{
public:
    WbKazzakDecurseTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb kazzak decurse") {}
    bool IsActive() override;
};

#endif
