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

        creators["wb kazzak shadow resistance"] = &RaidWorldBossTriggerContext::kazzak_shadow;
        creators["wb kazzak mark"] = &RaidWorldBossTriggerContext::kazzak_mark;
        creators["wb kazzak spread"] = &RaidWorldBossTriggerContext::kazzak_spread;
        creators["wb kazzak twisted"] = &RaidWorldBossTriggerContext::kazzak_twisted;
        creators["wb kazzak decurse"] = &RaidWorldBossTriggerContext::kazzak_decurse;

        creators["wb dragon nature resistance"] = &RaidWorldBossTriggerContext::dragon_nature;
        creators["wb dragon flank"] = &RaidWorldBossTriggerContext::dragon_flank;
        creators["wb dragon spread"] = &RaidWorldBossTriggerContext::dragon_spread;
        creators["wb dragon tank swap"] = &RaidWorldBossTriggerContext::dragon_swap;
        creators["wb dragon fog"] = &RaidWorldBossTriggerContext::dragon_fog;
        creators["wb emeriss mushroom"] = &RaidWorldBossTriggerContext::emeriss_mushroom;
        creators["wb emeriss dispel"] = &RaidWorldBossTriggerContext::emeriss_dispel;
        creators["wb lethon shade"] = &RaidWorldBossTriggerContext::lethon_shade;
        creators["wb lethon nature resistance"] = &RaidWorldBossTriggerContext::lethon_nature;
        creators["wb taerar nature resistance"] = &RaidWorldBossTriggerContext::taerar_nature;
        creators["wb taerar shade"] = &RaidWorldBossTriggerContext::taerar_shade;
        creators["wb taerar tank shade"] = &RaidWorldBossTriggerContext::taerar_tank_shade;
        creators["wb taerar fear"] = &RaidWorldBossTriggerContext::taerar_fear;
    }

private:
    static Trigger* azuregos_frost(PlayerbotAI* ai) { return new BossFrostResistanceTrigger(ai, "azuregos"); }
    static Trigger* azuregos_spread(PlayerbotAI* ai) { return new WbAzuregosSpreadTrigger(ai); }
    static Trigger* azuregos_storm(PlayerbotAI* ai) { return new WbAzuregosManastormTrigger(ai); }
    static Trigger* azuregos_reflect(PlayerbotAI* ai) { return new WbAzuregosReflectTrigger(ai); }
    static Trigger* azuregos_teleport(PlayerbotAI* ai) { return new WbAzuregosTeleportTrigger(ai); }
    static Trigger* azuregos_flank(PlayerbotAI* ai) { return new WbAzuregosFlankTrigger(ai); }
    static Trigger* kazzak_shadow(PlayerbotAI* ai) { return new BossShadowResistanceTrigger(ai, "lord kazzak"); }
    static Trigger* kazzak_mark(PlayerbotAI* ai) { return new WbKazzakMarkTrigger(ai); }
    static Trigger* kazzak_spread(PlayerbotAI* ai) { return new WbKazzakSpreadTrigger(ai); }
    static Trigger* kazzak_twisted(PlayerbotAI* ai) { return new WbKazzakTwistedTrigger(ai); }
    static Trigger* kazzak_decurse(PlayerbotAI* ai) { return new WbKazzakDecurseTrigger(ai); }
    static Trigger* dragon_nature(PlayerbotAI* ai) { return new BossNatureResistanceTrigger(ai, "emeriss"); }
    static Trigger* dragon_flank(PlayerbotAI* ai) { return new WbDragonFlankTrigger(ai); }
    static Trigger* dragon_spread(PlayerbotAI* ai) { return new WbDragonSpreadTrigger(ai); }
    static Trigger* dragon_swap(PlayerbotAI* ai) { return new WbDragonTankSwapTrigger(ai); }
    static Trigger* dragon_fog(PlayerbotAI* ai) { return new WbDragonFogTrigger(ai); }
    static Trigger* emeriss_mushroom(PlayerbotAI* ai) { return new WbEmerissMushroomTrigger(ai); }
    static Trigger* emeriss_dispel(PlayerbotAI* ai) { return new WbEmerissDispelTrigger(ai); }
    static Trigger* lethon_shade(PlayerbotAI* ai) { return new WbLethonShadeTrigger(ai); }
    static Trigger* lethon_nature(PlayerbotAI* ai) { return new BossNatureResistanceTrigger(ai, "lethon"); }
    static Trigger* taerar_nature(PlayerbotAI* ai) { return new BossNatureResistanceTrigger(ai, "taerar"); }
    static Trigger* taerar_shade(PlayerbotAI* ai) { return new WbTaerarShadeTrigger(ai); }
    static Trigger* taerar_tank_shade(PlayerbotAI* ai) { return new WbTaerarTankShadeTrigger(ai); }
    static Trigger* taerar_fear(PlayerbotAI* ai) { return new WbTaerarFearTrigger(ai); }
};

#endif
