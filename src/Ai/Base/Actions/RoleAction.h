/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROLEACTION_H
#define PLAYERBOTS_ROLEACTION_H

#include "Action.h"

class PlayerbotAI;

// Whisper or party: "role", "role tank", "role heal", "role dps", "role prot pve"
class RoleAction : public Action
{
public:
    RoleAction(PlayerbotAI* botAI) : Action(botAI, "role") {}

    bool Execute(Event event) override;

private:
    std::string CurrentReport();
    std::string ApplyRole(std::string param);
    std::string PickPveSpecName(uint8 cls, std::string const& role) const;
};

#endif
