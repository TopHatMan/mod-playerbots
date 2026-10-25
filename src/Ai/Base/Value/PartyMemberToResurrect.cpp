/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PartyMemberToResurrect.h"
#include "Corpse.h"
#include "Group.h"
#include "Playerbots.h"

class IsTargetOfResurrectSpell : public SpellEntryPredicate
{
public:
    bool Check(SpellInfo const* spellInfo) override
    {
        for (uint8 i = 0; i < 3; ++i)
        {
            if (spellInfo->Effects[i].Effect == SPELL_EFFECT_RESURRECT ||
                spellInfo->Effects[i].Effect == SPELL_EFFECT_RESURRECT_NEW ||
                spellInfo->Effects[i].Effect == SPELL_EFFECT_SELF_RESURRECT)
                return true;
        }

        return false;
    }
};

Unit* PartyMemberToResurrect::Calculate()
{
    Group* group = bot->GetGroup();
    if (!group)
        return nullptr;

    IsTargetOfResurrectSpell alreadyCasting;
    Player* bestReal = nullptr;
    Player* bestOther = nullptr;
    float bestRealDist = 99999.f;
    float bestOtherDist = 99999.f;
    float const maxDist = std::max(sPlayerbotAIConfig.SpellDistance * 3.0f, 80.0f);

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* player = ref->GetSource();
        if (!player || player == bot || player->IsAlive() || player->isResurrectRequested())
            continue;
        if (player->IsGameMaster() || player->GetMapId() != bot->GetMapId())
            continue;

        float dist = bot->GetDistance(player);
        if (Corpse* corpse = player->GetCorpse())
        {
            float const corpseDist = bot->GetDistance(corpse);
            if (corpseDist < dist)
                dist = corpseDist;
        }
        if (dist > maxDist)
            continue;
        if (IsTargetOfSpellCast(player, alreadyCasting))
            continue;

        bool const realParticipant = IsRealPlayer(player) ||
            (GET_PLAYERBOT_AI(player) && GET_PLAYERBOT_AI(player)->HasGameClientMaster());
        if (realParticipant)
        {
            if (dist < bestRealDist)
            {
                bestRealDist = dist;
                bestReal = player;
            }
        }
        else if (dist < bestOtherDist)
        {
            bestOtherDist = dist;
            bestOther = player;
        }
    }

    return bestReal ? static_cast<Unit*>(bestReal) : bestOther;
}
