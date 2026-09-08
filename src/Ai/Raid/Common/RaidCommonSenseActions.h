/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_RAIDCOMMONSENSEACTIONS_H
#define PLAYERBOTS_RAIDCOMMONSENSEACTIONS_H

#include "Action.h"

class PlayerbotAI;
class Unit;

class RaidSoulstoneHealerAction : public Action
{
public:
    RaidSoulstoneHealerAction(PlayerbotAI* botAI) : Action(botAI, "raid soulstone healer") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RaidAssignBrezAction : public Action
{
public:
    RaidAssignBrezAction(PlayerbotAI* botAI) : Action(botAI, "raid assign brez") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RaidRebirthAction : public Action
{
public:
    RaidRebirthAction(PlayerbotAI* botAI) : Action(botAI, "raid rebirth") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
