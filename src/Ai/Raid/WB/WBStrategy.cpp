/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WBStrategy.h"
#include "WBMultipliers.h"

void RaidWorldBossStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("wb azuregos frost resistance", {
        NextAction("wb azuregos frost resistance", ACTION_RAID) }));
    triggers.push_back(new TriggerNode("wb azuregos manastorm", {
        NextAction("wb azuregos move from storm", ACTION_RAID + 4) }));
    triggers.push_back(new TriggerNode("wb azuregos reflect", {
        NextAction("wb azuregos stop for reflect", ACTION_RAID + 4) }));
    triggers.push_back(new TriggerNode("wb azuregos teleport", {
        NextAction("wb azuregos tank taunt", ACTION_RAID + 5) }));
    triggers.push_back(new TriggerNode("wb azuregos flank", {
        NextAction("wb azuregos move to flank", ACTION_RAID + 2) }));
    triggers.push_back(new TriggerNode("wb azuregos spread", {
        NextAction("wb azuregos spread", ACTION_RAID + 1) }));
}

void RaidWorldBossStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new AzuregosMultiplier(botAI));
}
