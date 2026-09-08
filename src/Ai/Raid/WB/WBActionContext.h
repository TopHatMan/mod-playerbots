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

        creators["wb dragon nature resistance"] = &RaidWorldBossActionContext::dragon_nature;
        creators["wb dragon flank"] = &RaidWorldBossActionContext::dragon_flank;
        creators["wb dragon spread"] = &RaidWorldBossActionContext::dragon_spread;
        creators["wb dragon tank swap"] = &RaidWorldBossActionContext::dragon_swap;
        creators["wb dragon move from fog"] = &RaidWorldBossActionContext::dragon_fog;
        creators["wb emeriss avoid mushroom"] = &RaidWorldBossActionContext::emeriss_mushroom;
        creators["wb emeriss dispel"] = &RaidWorldBossActionContext::emeriss_dispel;
        creators["wb lethon kill shade"] = &RaidWorldBossActionContext::lethon_shade;
        creators["wb lethon nature resistance"] = &RaidWorldBossActionContext::lethon_nature;
        creators["wb taerar nature resistance"] = &RaidWorldBossActionContext::taerar_nature;
        creators["wb taerar kill shade"] = &RaidWorldBossActionContext::taerar_kill_shade;
        creators["wb taerar tank shade"] = &RaidWorldBossActionContext::taerar_tank_shade;
        creators["wb taerar anti fear"] = &RaidWorldBossActionContext::taerar_anti_fear;
        creators["wb ysondre nature resistance"] = &RaidWorldBossActionContext::ysondre_nature;
        creators["wb ysondre kill druid"] = &RaidWorldBossActionContext::ysondre_druid;
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
    static Action* dragon_nature(PlayerbotAI* ai) { return new BossNatureResistanceAction(ai, "emeriss"); }
    static Action* dragon_flank(PlayerbotAI* ai) { return new WbDragonFlankAction(ai); }
    static Action* dragon_spread(PlayerbotAI* ai) { return new WbDragonSpreadAction(ai); }
    static Action* dragon_swap(PlayerbotAI* ai) { return new WbDragonTankSwapAction(ai); }
    static Action* dragon_fog(PlayerbotAI* ai) { return new WbDragonMoveFromFogAction(ai); }
    static Action* emeriss_mushroom(PlayerbotAI* ai) { return new WbEmerissAvoidMushroomAction(ai); }
    static Action* emeriss_dispel(PlayerbotAI* ai) { return new WbEmerissDispelAction(ai); }
    static Action* lethon_shade(PlayerbotAI* ai) { return new WbLethonKillShadeAction(ai); }
    static Action* lethon_nature(PlayerbotAI* ai) { return new BossNatureResistanceAction(ai, "lethon"); }
    static Action* taerar_nature(PlayerbotAI* ai) { return new BossNatureResistanceAction(ai, "taerar"); }
    static Action* taerar_kill_shade(PlayerbotAI* ai) { return new WbTaerarKillShadeAction(ai); }
    static Action* taerar_tank_shade(PlayerbotAI* ai) { return new WbTaerarTankShadeAction(ai); }
    static Action* taerar_anti_fear(PlayerbotAI* ai) { return new WbTaerarAntiFearAction(ai); }
    static Action* ysondre_nature(PlayerbotAI* ai) { return new BossNatureResistanceAction(ai, "ysondre"); }
    static Action* ysondre_druid(PlayerbotAI* ai) { return new WbYsondreKillDruidAction(ai); }
};

#endif
