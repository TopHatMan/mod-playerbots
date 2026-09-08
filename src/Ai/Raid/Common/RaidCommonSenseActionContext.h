/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_RAIDCOMMONSENSEACTIONCONTEXT_H
#define PLAYERBOTS_RAIDCOMMONSENSEACTIONCONTEXT_H

#include "NamedObjectContext.h"
#include "RaidCommonSenseActions.h"

class RaidCommonSenseActionContext : public NamedObjectContext<Action>
{
public:
    RaidCommonSenseActionContext()
    {
        creators["raid soulstone healer"] = &RaidCommonSenseActionContext::raid_soulstone_healer;
        creators["raid assign brez"] = &RaidCommonSenseActionContext::raid_assign_brez;
        creators["raid rebirth"] = &RaidCommonSenseActionContext::raid_rebirth;
    }

private:
    static Action* raid_soulstone_healer(PlayerbotAI* botAI) { return new RaidSoulstoneHealerAction(botAI); }
    static Action* raid_assign_brez(PlayerbotAI* botAI) { return new RaidAssignBrezAction(botAI); }
    static Action* raid_rebirth(PlayerbotAI* botAI) { return new RaidRebirthAction(botAI); }
};

#endif
