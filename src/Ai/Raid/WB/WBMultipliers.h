/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_WBMULTIPLIERS_H
#define PLAYERBOTS_WBMULTIPLIERS_H

#include "Multiplier.h"

class AzuregosMultiplier : public Multiplier
{
public:
    AzuregosMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "azuregos multiplier") {}
    float GetValue(Action* action) override;
};

class KazzakMultiplier : public Multiplier
{
public:
    KazzakMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "kazzak multiplier") {}
    float GetValue(Action* action) override;
};

class EmeraldDragonMultiplier : public Multiplier
{
public:
    EmeraldDragonMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "emerald dragon multiplier") {}
    float GetValue(Action* action) override;
};

#endif
