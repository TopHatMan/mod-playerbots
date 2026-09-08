/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WBACTIONCONTEXT_H
#define PLAYERBOTS_WBACTIONCONTEXT_H

#include "Action.h"
#include "BossAuraActions.h"
#include "NamedObjectContext.h"
#include "WBActions.h"

class RaidWorldBossActionContext : public NamedObjectContext<Action>
{
public:
    RaidWorldBossActionContext()
    {
        creators["wb azuregos frost resistance"] = &RaidWorldBossActionContext::azuregos_frost;
        creators["wb azuregos spread"] = &RaidWorldBossActionContext::azuregos_spread;
        creators["wb azuregos move from storm"] = &RaidWorldBossActionContext::azuregos_storm;
        creators["wb azuregos stop for reflect"] = &RaidWorldBossActionContext::azuregos_reflect;
        creators["wb azuregos tank taunt"] = &RaidWorldBossActionContext::azuregos_taunt;
        creators["wb azuregos move to flank"] = &RaidWorldBossActionContext::azuregos_flank;

        creators["wb kazzak shadow resistance"] = &RaidWorldBossActionContext::kazzak_shadow;
        creators["wb kazzak isolate"] = &RaidWorldBossActionContext::kazzak_isolate;
        creators["wb kazzak spread"] = &RaidWorldBossActionContext::kazzak_spread;
        creators["wb kazzak dispel"] = &RaidWorldBossActionContext::kazzak_dispel;
    }

private:
    static Action* azuregos_frost(PlayerbotAI* ai) { return new BossFrostResistanceAction(ai, "azuregos"); }
    static Action* azuregos_spread(PlayerbotAI* ai) { return new WbAzuregosSpreadAction(ai); }
    static Action* azuregos_storm(PlayerbotAI* ai) { return new WbAzuregosMoveFromStormAction(ai); }
    static Action* azuregos_reflect(PlayerbotAI* ai) { return new WbAzuregosStopForReflectAction(ai); }
    static Action* azuregos_taunt(PlayerbotAI* ai) { return new WbAzuregosTankTauntAction(ai); }
    static Action* azuregos_flank(PlayerbotAI* ai) { return new WbAzuregosMoveToFlankAction(ai); }
    static Action* kazzak_shadow(PlayerbotAI* ai) { return new BossShadowResistanceAction(ai, "lord kazzak"); }
    static Action* kazzak_isolate(PlayerbotAI* ai) { return new WbKazzakIsolateAction(ai); }
    static Action* kazzak_spread(PlayerbotAI* ai) { return new WbKazzakSpreadAction(ai); }
    static Action* kazzak_dispel(PlayerbotAI* ai) { return new WbKazzakDispelAction(ai); }
};

#endif
