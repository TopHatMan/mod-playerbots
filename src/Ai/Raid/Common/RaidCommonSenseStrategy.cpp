/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RaidCommonSenseStrategy.h"
#include "Playerbots.h"

void RaidCommonSenseStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("often", { NextAction("raid soulstone healer", 42.0f) }));
    triggers.push_back(new TriggerNode("often", { NextAction("raid assign brez", 41.0f) }));
    triggers.push_back(new TriggerNode("combat party member dead", { NextAction("raid rebirth", 90.0f) }));
    triggers.push_back(new TriggerNode("party member dead", { NextAction("raid rebirth", 50.0f) }));
}
