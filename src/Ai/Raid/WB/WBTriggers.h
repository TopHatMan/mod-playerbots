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

class WbDragonFlankTrigger : public Trigger
{
public:
    WbDragonFlankTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb dragon flank") {}
    bool IsActive() override;
};

class WbDragonSpreadTrigger : public Trigger
{
public:
    WbDragonSpreadTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb dragon spread") {}
    bool IsActive() override;
};

class WbDragonTankSwapTrigger : public Trigger
{
public:
    WbDragonTankSwapTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb dragon tank swap") {}
    bool IsActive() override;
};

class WbDragonFogTrigger : public Trigger
{
public:
    WbDragonFogTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb dragon fog") {}
    bool IsActive() override;
};

class WbEmerissMushroomTrigger : public Trigger
{
public:
    WbEmerissMushroomTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb emeriss mushroom") {}
    bool IsActive() override;
};

class WbEmerissDispelTrigger : public Trigger
{
public:
    WbEmerissDispelTrigger(PlayerbotAI* botAI) : Trigger(botAI, "wb emeriss dispel") {}
    bool IsActive() override;
};

#endif
