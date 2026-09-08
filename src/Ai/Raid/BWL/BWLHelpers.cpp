/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BWLHelpers.h"
#include "AiObjectContext.h"
#include "GameObject.h"
#include "Group.h"
#include "Playerbots.h"

namespace BlackwingLairHelpers
{
    bool IsActiveSuppressionDeviceInRange(GameObject const* go, Player const* bot)
    {
        constexpr float suppressionDeviceInteractionDistance = 15.0f;
        return go &&
               go->GetEntry() == static_cast<uint32>(BlackwingLairGameObjects::GO_SUPPRESSION_DEVICE) &&
               go->GetDistance(bot) < suppressionDeviceInteractionDistance &&
               go->GetGoState() == GO_STATE_READY;
    }

    bool AreRazorgoreEggsAlive(PlayerbotAI* botAI)
    {
        GuidVector gos = botAI->GetAiObjectContext()->GetValue<GuidVector>("nearest game objects")->Get();
        for (auto const& guid : gos)
        {
            GameObject const* go = botAI->GetGameObject(guid);
            if (go && go->GetEntry() == static_cast<uint32>(BlackwingLairGameObjects::GO_BLACK_DRAGON_EGG))
                return true;
        }

        Player* bot = botAI->GetBot();
        if (!bot)
            return false;

        std::list<GameObject*> eggs;
        bot->GetGameObjectListWithEntryInGrid(eggs,
            static_cast<uint32>(BlackwingLairGameObjects::GO_BLACK_DRAGON_EGG), 200.0f);
        for (GameObject* egg : eggs)
            if (egg && egg->isSpawned() && egg->GetGoState() == GO_STATE_READY)
                return true;

        return false;
    }

    bool IsRazorgoreOffTank(Player* bot)
    {
        return PlayerbotAI::IsAssistTankOfIndex(bot, 0, true);
    }

    bool IsNonBABotNearPosition(Player const* bot, Position const& position, float distance)
    {
        Group const* group = bot->GetGroup();
        if (!group)
            return false;

        for (GroupReference const* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player const* p = gref->GetSource();
            if (!p || p == bot || !p->IsAlive() || p->GetMapId() != bot->GetMapId())
                continue;

            if (p->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_BURNING_ADRENALINE)))
                continue;

            if (p->GetDistance2d(position.GetPositionX(), position.GetPositionY()) < distance)
                return true;
        }

        return false;
    }

    Player* SelectRazorgoreController(Player* bot)
    {
        if (!bot)
            return nullptr;

        Group* group = bot->GetGroup();
        if (!group)
            return bot;

        Player* currentCharmer = nullptr;
        Player* preferred = nullptr;
        Player* fallback = nullptr;

        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || !member->IsAlive() || member->GetMapId() != bot->GetMapId())
                continue;
            if (member->HasAura(static_cast<uint32>(BlackwingLairSpells::SPELL_MIND_EXHAUSTION)))
                continue;

            if (Unit* charm = member->GetCharm())
                if (charm->GetEntry() == static_cast<uint32>(BlackwingLairNPCs::NPC_RAZORGORE))
                    currentCharmer = member;

            bool const preferredClass = member->getClass() == CLASS_HUNTER || member->getClass() == CLASS_PALADIN;
            bool const isTank = PlayerbotAI::IsTank(member);
            if (preferredClass && !isTank)
            {
                if (!preferred || member->GetGUID() < preferred->GetGUID())
                    preferred = member;
            }
            else if (!isTank)
            {
                if (!fallback || member->GetGUID() < fallback->GetGUID())
                    fallback = member;
            }
        }

        if (currentCharmer)
            return currentCharmer;
        if (preferred)
            return preferred;
        if (fallback)
            return fallback;
        return bot;
    }

    bool IsRazorgoreController(Player* bot)
    {
        Player* controller = SelectRazorgoreController(bot);
        return controller && controller == bot;
    }

    bool IsRazorgoreAdd(Unit const* unit)
    {
        if (!unit || !unit->IsAlive())
            return false;

        uint32 const entry = unit->GetEntry();
        return entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_MAGE) ||
               entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_WARLOCK) ||
               entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_DRAGON) ||
               entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_LEGIONAIRE);
    }

    static uint32 RazorgoreAddPriority(Unit const* unit)
    {
        if (!unit)
            return 0;
        uint32 const entry = unit->GetEntry();
        if (entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_MAGE) ||
            entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_WARLOCK))
            return 3;
        if (entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_DRAGON))
            return 2;
        if (entry == static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_LEGIONAIRE))
            return 1;
        return 0;
    }

    Unit* SelectRazorgoreAddTarget(Player* bot)
    {
        if (!bot)
            return nullptr;

        Player* controller = SelectRazorgoreController(bot);
        Unit* best = nullptr;
        uint32 bestPriority = 0;
        float bestDist = RAZORGORE_ADD_RANGE;

        std::list<Creature*> creatures;
        std::vector<uint32> const addEntries = {
            static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_MAGE),
            static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_WARLOCK),
            static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_DRAGON),
            static_cast<uint32>(BlackwingLairNPCs::NPC_BLACKWING_LEGIONAIRE)
        };
        bot->GetCreatureListWithEntryInGrid(creatures, addEntries, RAZORGORE_ADD_RANGE);

        for (Creature* unit : creatures)
        {
            if (!unit || !unit->IsAlive() || !unit->IsInCombat())
                continue;

            uint32 const priority = RazorgoreAddPriority(unit);
            if (!priority)
                continue;

            // Dragonspawn walking onto the orb platform outrank everything else.
            bool const onController = controller && unit->GetVictim() == controller;
            uint32 const effective = onController ? priority + 4 : priority;
            float const dist = bot->GetDistance(unit);
            if (effective > bestPriority || (effective == bestPriority && dist < bestDist))
            {
                bestPriority = effective;
                bestDist = dist;
                best = unit;
            }
        }

        return best;
    }

    GameObject* FindOrbOfDomination(Player* bot)
    {
        if (!bot)
            return nullptr;

        std::list<GameObject*> orbs;
        bot->GetGameObjectListWithEntryInGrid(orbs,
            static_cast<uint32>(BlackwingLairGameObjects::GO_ORB_OF_DOMINATION), 150.0f);
        GameObject* closest = nullptr;
        float best = 150.0f;
        for (GameObject* go : orbs)
        {
            if (!go || !go->isSpawned())
                continue;
            float const dist = bot->GetDistance(go);
            if (dist < best)
            {
                best = dist;
                closest = go;
            }
        }
        return closest;
    }

    GameObject* FindNearestRazorgoreEgg(Player* bot)
    {
        if (!bot)
            return nullptr;

        Unit* charm = bot->GetCharm();
        WorldObject* origin = charm ? static_cast<WorldObject*>(charm) : static_cast<WorldObject*>(bot);

        std::list<GameObject*> eggs;
        origin->GetGameObjectListWithEntryInGrid(eggs,
            static_cast<uint32>(BlackwingLairGameObjects::GO_BLACK_DRAGON_EGG), 200.0f);
        GameObject* closest = nullptr;
        float best = 200.0f;
        for (GameObject* egg : eggs)
        {
            if (!egg || !egg->isSpawned() || egg->GetGoState() != GO_STATE_READY)
                continue;
            float const dist = origin->GetDistance(egg);
            if (dist < best)
            {
                best = dist;
                closest = egg;
            }
        }
        return closest;
    }

    bool IsChromaggusBreathSpell(uint32 spellId)
    {
        return spellId == static_cast<uint32>(BlackwingLairSpells::SPELL_INCINERATE) ||
               spellId == static_cast<uint32>(BlackwingLairSpells::SPELL_TIMELAPSE) ||
               spellId == static_cast<uint32>(BlackwingLairSpells::SPELL_CORROSIVEACID) ||
               spellId == static_cast<uint32>(BlackwingLairSpells::SPELL_IGNITEFLESH) ||
               spellId == static_cast<uint32>(BlackwingLairSpells::SPELL_FROSTBURN);
    }
}
