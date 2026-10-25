/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RoleAction.h"
#include "AiFactory.h"
#include "Event.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"
#include "Playerbots.h"
#include "SharedDefines.h"

#include <algorithm>
#include <cctype>

namespace
{
std::string Lower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string RoleName(Player* bot, PlayerbotAI* ai)
{
    if (ai->IsTank(bot, true))
        return "tank";
    if (ai->IsHeal(bot, true))
        return "heal";
    return "dps";
}

std::string CurrentSpecName(Player* bot)
{
    uint8 const cls = bot->getClass();
    std::string const current = Lower(AiFactory::GetPlayerSpecName(bot));
    for (int specNo = 0; specNo < MAX_SPECNO; ++specNo)
    {
        std::string const name = sPlayerbotAIConfig.PremadeSpecName[cls][specNo];
        if (name.empty())
            continue;
        if (Lower(name).find(current) != std::string::npos)
            return name;
    }
    return current.empty() ? "unknown" : current;
}

std::string StrategyList(PlayerbotAI* ai, BotState state)
{
    std::vector<std::string> names = ai->GetStrategies(state);
    std::ostringstream out;
    bool first = true;
    for (std::string const& name : names)
    {
        if (!first)
            out << ", ";
        first = false;
        out << name;
    }
    return out.str();
}
} // namespace

bool RoleAction::Execute(Event event)
{
    std::string param = Lower(event.getParam());
    while (!param.empty() && param.front() == ' ')
        param.erase(param.begin());

    std::ostringstream out;
    if (param.empty() || param == "?" || param == "status")
        out << CurrentReport();
    else if (param == "help")
    {
        out << "role                - show spec, role, and strategies\n"
            << "role tank|heal|dps  - apply vanilla PvE spec + strategies\n"
            << "role prot pve       - apply that premade spec (talents spec <name>)\n"
            << "talents spec list   - list premade names for this class\n"
            << "Filter: @tank role tank   @heal role heal   @dps role dps";
    }
    else
        out << ApplyRole(param);

    botAI->TellMaster(out);
    return true;
}

std::string RoleAction::CurrentReport()
{
    std::ostringstream out;
    out << bot->GetName() << " lvl" << uint32(bot->GetLevel()) << " "
        << RoleName(bot, botAI) << " | " << CurrentSpecName(bot) << "\n";
    out << "combat: " << StrategyList(botAI, BOT_STATE_COMBAT) << "\n";
    out << "nc: " << StrategyList(botAI, BOT_STATE_NON_COMBAT);
    return out.str();
}

std::string RoleAction::PickPveSpecName(uint8 cls, std::string const& role) const
{
    if (cls == CLASS_DEATH_KNIGHT)
        return "";

    char const* needle = nullptr;
    if (role == "tank")
    {
        switch (cls)
        {
            case CLASS_WARRIOR:
            case CLASS_PALADIN:
                needle = "prot pve";
                break;
            case CLASS_DRUID:
                needle = "bear pve";
                break;
            default:
                return "";
        }
    }
    else if (role == "heal" || role == "healer")
    {
        switch (cls)
        {
            case CLASS_PRIEST:
                needle = "holy pve";
                break;
            case CLASS_PALADIN:
                needle = "holy pve";
                break;
            case CLASS_SHAMAN:
                needle = "resto pve";
                break;
            case CLASS_DRUID:
                needle = "resto pve";
                break;
            default:
                return "";
        }
    }
    else if (role == "dps")
    {
        switch (cls)
        {
            case CLASS_WARRIOR:
                needle = "arms pve";
                break;
            case CLASS_PALADIN:
                needle = "ret pve";
                break;
            case CLASS_HUNTER:
                needle = "bm pve";
                break;
            case CLASS_ROGUE:
                needle = "combat pve";
                break;
            case CLASS_PRIEST:
                needle = "shadow pve";
                break;
            case CLASS_SHAMAN:
                needle = "enh pve";
                break;
            case CLASS_MAGE:
                needle = "frost pve";
                break;
            case CLASS_WARLOCK:
                needle = "affli pve";
                break;
            case CLASS_DRUID:
                needle = "cat pve";
                break;
            default:
                return "";
        }
    }

    if (!needle)
        return "";

    for (int specNo = 0; specNo < MAX_SPECNO; ++specNo)
    {
        if (sPlayerbotAIConfig.PremadeSpecName[cls][specNo] == needle)
            return needle;
    }
    return needle;
}

std::string RoleAction::ApplyRole(std::string param)
{
    if (bot->getClass() == CLASS_DEATH_KNIGHT)
        return "Death Knights are disabled on this vanilla realm.";

    std::string specName;
    if (param == "tank" || param == "heal" || param == "healer" || param == "dps")
        specName = PickPveSpecName(bot->getClass(), param);
    else
        specName = param;

    if (specName.empty())
        return "This class has no vanilla PvE spec for that role. Try: role help";

    uint8 const cls = bot->getClass();
    int found = -1;
    for (int specNo = 0; specNo < MAX_SPECNO; ++specNo)
    {
        std::string const name = sPlayerbotAIConfig.PremadeSpecName[cls][specNo];
        if (name.empty())
            continue;
        if (Lower(name) == Lower(specName) || Lower(name).find(Lower(specName)) != std::string::npos)
        {
            found = specNo;
            specName = name;
            break;
        }
    }

    if (found < 0)
        return "Unknown spec '" + specName + "'. Whisper: talents spec list";

    PlayerbotFactory::InitTalentsBySpecNo(bot, found, true);
    botAI->ResetStrategies();

    std::ostringstream out;
    out << "Set " << specName << " (" << RoleName(bot, botAI) << "). "
        << "combat: " << StrategyList(botAI, BOT_STATE_COMBAT);
    return out.str();
}
