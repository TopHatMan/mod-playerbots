/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ONYTRIGGERS_H
#define PLAYERBOTS_ONYTRIGGERS_H

#include "PlayerbotAI.h"
#include "Trigger.h"

class OnyxiaDeepBreathTrigger : public Trigger
{
public:
    OnyxiaDeepBreathTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class OnyxiaNearTailTrigger : public Trigger
{
public:
    OnyxiaNearTailTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class RaidOnyxiaFireballSplashTrigger : public Trigger
{
public:
    RaidOnyxiaFireballSplashTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class RaidOnyxiaWhelpsSpawnTrigger : public Trigger
{
public:
    RaidOnyxiaWhelpsSpawnTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class OnyxiaAvoidEggsTrigger : public Trigger
{
public:
    OnyxiaAvoidEggsTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class OnyxiaTankWallTrigger : public Trigger
{
public:
    OnyxiaTankWallTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class OnyxiaBellowingRoarTrigger : public Trigger
{
public:
    OnyxiaBellowingRoarTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class OnyxiaLavaTrigger : public Trigger
{
public:
    OnyxiaLavaTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

class OnyxiaTankTauntTrigger : public Trigger
{
public:
    OnyxiaTankTauntTrigger(PlayerbotAI* botAI);
    bool IsActive() override;
};

#endif
