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

    triggers.push_back(new TriggerNode("wb kazzak shadow resistance", {
        NextAction("wb kazzak shadow resistance", ACTION_RAID) }));
    triggers.push_back(new TriggerNode("wb kazzak twisted", {
        NextAction("wb kazzak dispel", ACTION_EMERGENCY + 4) }));
    triggers.push_back(new TriggerNode("wb kazzak decurse", {
        NextAction("wb kazzak dispel", ACTION_RAID + 4) }));
    triggers.push_back(new TriggerNode("wb kazzak mark", {
        NextAction("wb kazzak isolate", ACTION_RAID + 5) }));
    triggers.push_back(new TriggerNode("wb kazzak spread", {
        NextAction("wb kazzak spread", ACTION_RAID + 1) }));

    triggers.push_back(new TriggerNode("wb dragon nature resistance", {
        NextAction("wb dragon nature resistance", ACTION_RAID) }));
    triggers.push_back(new TriggerNode("wb dragon fog", {
        NextAction("wb dragon move from fog", ACTION_RAID + 4) }));
    triggers.push_back(new TriggerNode("wb dragon tank swap", {
        NextAction("wb dragon tank swap", ACTION_RAID + 3) }));
    triggers.push_back(new TriggerNode("wb dragon flank", {
        NextAction("wb dragon flank", ACTION_RAID + 2) }));
    triggers.push_back(new TriggerNode("wb dragon spread", {
        NextAction("wb dragon spread", ACTION_RAID + 1) }));
    triggers.push_back(new TriggerNode("wb emeriss mushroom", {
        NextAction("wb emeriss avoid mushroom", ACTION_RAID + 4) }));
    triggers.push_back(new TriggerNode("wb emeriss dispel", {
        NextAction("wb emeriss dispel", ACTION_RAID + 2) }));
}

void RaidWorldBossStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new AzuregosMultiplier(botAI));
    multipliers.push_back(new KazzakMultiplier(botAI));
    multipliers.push_back(new EmeraldDragonMultiplier(botAI));
}
