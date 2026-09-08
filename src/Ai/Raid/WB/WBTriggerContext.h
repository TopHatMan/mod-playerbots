/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WBTRIGGERCONTEXT_H
#define PLAYERBOTS_WBTRIGGERCONTEXT_H

#include "BossAuraTriggers.h"
#include "NamedObjectContext.h"
#include "WBTriggers.h"

class RaidWorldBossTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidWorldBossTriggerContext()
    {
        creators["wb azuregos frost resistance"] = &RaidWorldBossTriggerContext::azuregos_frost;
        creators["wb azuregos spread"] = &RaidWorldBossTriggerContext::azuregos_spread;
        creators["wb azuregos manastorm"] = &RaidWorldBossTriggerContext::azuregos_storm;
        creators["wb azuregos reflect"] = &RaidWorldBossTriggerContext::azuregos_reflect;
        creators["wb azuregos teleport"] = &RaidWorldBossTriggerContext::azuregos_teleport;
        creators["wb azuregos flank"] = &RaidWorldBossTriggerContext::azuregos_flank;
    }

private:
    static Trigger* azuregos_frost(PlayerbotAI* ai) { return new BossFrostResistanceTrigger(ai, "azuregos"); }
    static Trigger* azuregos_spread(PlayerbotAI* ai) { return new WbAzuregosSpreadTrigger(ai); }
    static Trigger* azuregos_storm(PlayerbotAI* ai) { return new WbAzuregosManastormTrigger(ai); }
    static Trigger* azuregos_reflect(PlayerbotAI* ai) { return new WbAzuregosReflectTrigger(ai); }
    static Trigger* azuregos_teleport(PlayerbotAI* ai) { return new WbAzuregosTeleportTrigger(ai); }
    static Trigger* azuregos_flank(PlayerbotAI* ai) { return new WbAzuregosFlankTrigger(ai); }
};

#endif
