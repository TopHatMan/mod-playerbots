/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ONYACTIONCONTEXT_H
#define PLAYERBOTS_ONYACTIONCONTEXT_H

#include "Action.h"
#include "BossAuraActions.h"
#include "NamedObjectContext.h"
#include "OnyActions.h"

class RaidOnyxiaActionContext : public NamedObjectContext<Action>
{
public:
    RaidOnyxiaActionContext()
    {
        creators["ony fire resistance"] = &RaidOnyxiaActionContext::fire_resistance;
        creators["ony move to side"] = &RaidOnyxiaActionContext::move_to_side;
        creators["ony spread out"] = &RaidOnyxiaActionContext::spread_out;
        creators["ony move to safe zone"] = &RaidOnyxiaActionContext::move_to_safe_zone;
        creators["ony kill whelps"] = &RaidOnyxiaActionContext::kill_whelps;
        creators["ony avoid eggs move"] = &RaidOnyxiaActionContext::avoid_eggs;
        creators["ony tank wall"] = &RaidOnyxiaActionContext::tank_wall;
        creators["ony anti fear"] = &RaidOnyxiaActionContext::anti_fear;
        creators["ony move from lava"] = &RaidOnyxiaActionContext::move_from_lava;
        creators["ony tank taunt"] = &RaidOnyxiaActionContext::tank_taunt;
    }

private:
    static Action* fire_resistance(PlayerbotAI* ai) { return new BossFireResistanceAction(ai, "onyxia"); }
    static Action* move_to_side(PlayerbotAI* ai) { return new RaidOnyxiaMoveToSideAction(ai); }
    static Action* spread_out(PlayerbotAI* ai) { return new RaidOnyxiaSpreadOutAction(ai); }
    static Action* move_to_safe_zone(PlayerbotAI* ai) { return new RaidOnyxiaMoveToSafeZoneAction(ai); }
    static Action* kill_whelps(PlayerbotAI* ai) { return new RaidOnyxiaKillWhelpsAction(ai); }
    static Action* avoid_eggs(PlayerbotAI* ai) { return new OnyxiaAvoidEggsAction(ai); }
    static Action* tank_wall(PlayerbotAI* ai) { return new OnyxiaTankWallAction(ai); }
    static Action* anti_fear(PlayerbotAI* ai) { return new OnyxiaAntiFearAction(ai); }
    static Action* move_from_lava(PlayerbotAI* ai) { return new OnyxiaMoveFromLavaAction(ai); }
    static Action* tank_taunt(PlayerbotAI* ai) { return new OnyxiaTankTauntAction(ai); }
};

#endif
