/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_RAIDCOMMONSENSESTRATEGY_H
#define PLAYERBOTS_RAIDCOMMONSENSESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

// 5-man / 20-man / 40-man raid common sense: soulstone healers, assign brez,
// rez out of combat. Attached whenever the bot is in a group of 5+.
class RaidCommonSenseStrategy : public Strategy
{
public:
    RaidCommonSenseStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "raid"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
