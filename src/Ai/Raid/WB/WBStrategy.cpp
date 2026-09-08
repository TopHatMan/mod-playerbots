/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WBStrategy.h"
#include "Playerbots.h"
#include "WBHelpers.h"
#include "WBMultipliers.h"

using namespace WorldBossHelpers;

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
    triggers.push_back(new TriggerNode("wb lethon nature resistance", {
        NextAction("wb lethon nature resistance", ACTION_RAID) }));
    triggers.push_back(new TriggerNode("wb lethon shade", {
        NextAction("wb lethon kill shade", ACTION_RAID + 3) }));
    triggers.push_back(new TriggerNode("wb taerar nature resistance", {
        NextAction("wb taerar nature resistance", ACTION_RAID) }));
    triggers.push_back(new TriggerNode("wb taerar fear", {
        NextAction("wb taerar anti fear", ACTION_EMERGENCY + 3) }));
    triggers.push_back(new TriggerNode("wb taerar tank shade", {
        NextAction("wb taerar tank shade", ACTION_RAID + 3) }));
    triggers.push_back(new TriggerNode("wb taerar shade", {
        NextAction("wb taerar kill shade", ACTION_RAID + 3) }));
}

void RaidWorldBossStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new AzuregosMultiplier(botAI));
    multipliers.push_back(new KazzakMultiplier(botAI));
    multipliers.push_back(new EmeraldDragonMultiplier(botAI));
}

void RaidWorldBossStrategy::AppendTargetExclusions(GuidSet& exclusions, TargetValueExclusionType type)
{
    if (type != TargetValueExclusionType::Dps)
        return;

    Player* bot = botAI->GetBot();
    Unit* dragon = FindEmeraldDragon(bot);
    if (!dragon)
        return;

    if (dragon->GetEntry() == NPC_LETHON)
    {
        Creature* shade = bot->FindNearestCreature(NPC_SPIRIT_SHADE, 80.0f, true);
        if (shade && shade->IsAlive())
            exclusions.insert(dragon->GetGUID());
    }
    else if (dragon->GetEntry() == NPC_TAERAR)
    {
        Creature* shade = bot->FindNearestCreature(NPC_SHADE_OF_TAERAR, 80.0f, true);
        if (shade && shade->IsAlive())
            exclusions.insert(dragon->GetGUID());
    }
}
