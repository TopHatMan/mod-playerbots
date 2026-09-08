/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ONYTRIGGERCONTEXT_H
#define PLAYERBOTS_ONYTRIGGERCONTEXT_H

#include "BossAuraTriggers.h"
#include "NamedObjectContext.h"
#include "OnyTriggers.h"

class RaidOnyxiaTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidOnyxiaTriggerContext()
    {
        creators["ony fire resistance"] = &RaidOnyxiaTriggerContext::fire_resistance;
        creators["ony near tail"] = &RaidOnyxiaTriggerContext::near_tail;
        creators["ony deep breath warning"] = &RaidOnyxiaTriggerContext::deep_breath;
        creators["ony fireball splash incoming"] = &RaidOnyxiaTriggerContext::fireball_splash;
        creators["ony whelps spawn"] = &RaidOnyxiaTriggerContext::whelps_spawn;
        creators["ony avoid eggs"] = &RaidOnyxiaTriggerContext::avoid_eggs;
        creators["ony tank wall"] = &RaidOnyxiaTriggerContext::tank_wall;
        creators["ony bellowing roar"] = &RaidOnyxiaTriggerContext::bellowing_roar;
        creators["ony lava"] = &RaidOnyxiaTriggerContext::lava;
        creators["ony tank taunt"] = &RaidOnyxiaTriggerContext::tank_taunt;
    }

private:
    static Trigger* fire_resistance(PlayerbotAI* ai) { return new BossFireResistanceTrigger(ai, "onyxia"); }
    static Trigger* near_tail(PlayerbotAI* ai) { return new OnyxiaNearTailTrigger(ai); }
    static Trigger* deep_breath(PlayerbotAI* ai) { return new OnyxiaDeepBreathTrigger(ai); }
    static Trigger* fireball_splash(PlayerbotAI* ai) { return new RaidOnyxiaFireballSplashTrigger(ai); }
    static Trigger* whelps_spawn(PlayerbotAI* ai) { return new RaidOnyxiaWhelpsSpawnTrigger(ai); }
    static Trigger* avoid_eggs(PlayerbotAI* ai) { return new OnyxiaAvoidEggsTrigger(ai); }
    static Trigger* tank_wall(PlayerbotAI* ai) { return new OnyxiaTankWallTrigger(ai); }
    static Trigger* bellowing_roar(PlayerbotAI* ai) { return new OnyxiaBellowingRoarTrigger(ai); }
    static Trigger* lava(PlayerbotAI* ai) { return new OnyxiaLavaTrigger(ai); }
    static Trigger* tank_taunt(PlayerbotAI* ai) { return new OnyxiaTankTauntTrigger(ai); }
};

#endif
