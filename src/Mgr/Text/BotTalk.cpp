/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BotTalk.h"
#include "BotGearLane.h"
#include "Config.h"
#include "Group.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "Random.h"
#include "SharedDefines.h"

#include <algorithm>
#include <ctime>
#include <initializer_list>
#include <iterator>
#include <mutex>
#include <unordered_map>

namespace
{
struct TalkConfig
{
    bool enable = true;
    uint32 groupChance = 18;
    uint32 worldChance = 8;
    uint32 cooldownMin = 20;
    uint32 cooldownMax = 55;
    bool preferPartyWhenGrouped = true;
};

TalkConfig cfg;
bool cfgLoaded = false;

std::mutex lastLock;
std::unordered_map<uint32, time_t> lastSpoke;

uint32 LanguageFor(Player* bot)
{
    return bot && bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH;
}

std::string Pick(std::initializer_list<char const*> lines)
{
    if (!lines.size())
        return {};
    auto it = lines.begin();
    std::advance(it, urand(0, uint32(lines.size() - 1)));
    return *it;
}

std::string FallbackLine(std::string const& key)
{
    if (key == "critical health" || key == "talk_critical_health")
        return Pick({ "I'm dying!", "Heal! Heal!", "Almost dead!" });
    if (key == "low health" || key == "talk_low_health")
        return Pick({ "Need heals.", "I'm hurt.", "Heals on me." });
    if (key == "low mana" || key == "talk_low_mana")
        return Pick({ "Oom.", "Need mana.", "Drinking after this." });
    if (key == "taunt" || key == "talk_taunt")
        return Pick({ "On me!", "Got aggro.", "Taunting." });
    if (key == "aoe" || key == "talk_aoe")
        return Pick({ "Adds!", "Lots of them.", "AoE them down." });
    if (key == "loot" || key == "talk_loot")
        return Pick({ "Loot up.", "Need on that.", "Grab the corpse." });
    if (key == "talk_group_ambient")
        return Pick({
            "Ready.",
            "On you.",
            "Let's go.",
            "I need a drink after this.",
            "Watch the patrol.",
            "Pull when you're ready.",
            "I've got the next one.",
            "Buffs are up."
        });
    if (key == "talk_world_ambient")
        return Pick({
            "Anyone for a run?",
            "Looking for a group.",
            "Good hunting.",
            "Need a tank for Deadmines.",
            "WTS greens, cheap.",
            "This zone is packed.",
            "Heading to the inn.",
            "Where is the flight master?"
        });
    if (key == "talk_pull")
        return Pick({ "Pulling.", "Incoming.", "On the pull." });
    if (key == "talk_incoming")
        return Pick({ "Incoming!", "Adds on me!", "Help on this one!" });
    return {};
}

time_t LastSpoke(uint32 guidLow)
{
    std::lock_guard<std::mutex> g(lastLock);
    auto it = lastSpoke.find(guidLow);
    return it == lastSpoke.end() ? 0 : it->second;
}

void MarkSpoke(uint32 guidLow)
{
    std::lock_guard<std::mutex> g(lastLock);
    lastSpoke[guidLow] = time(nullptr);
}

bool OnCooldown(Player* bot)
{
    if (!bot)
        return true;
    time_t const last = LastSpoke(bot->GetGUID().GetCounter());
    if (!last)
        return false;
    uint32 const wait = urand(cfg.cooldownMin, std::max(cfg.cooldownMin, cfg.cooldownMax));
    return (time(nullptr) - last) < time_t(wait);
}

std::string ResolveLine(std::string const& key, std::map<std::string, std::string> const& placeholders)
{
    std::string text;
    if (PlayerbotTextMgr::instance().GetBotText(key, text, placeholders) && !text.empty())
        return text;
    return FallbackLine(key);
}
} // namespace

namespace BotTalk
{
void LoadConfig()
{
    cfg.enable = sConfigMgr->GetOption<bool>("Playerbots.BotTalk.Enable", false);
    cfg.groupChance = sConfigMgr->GetOption<uint32>("Playerbots.BotTalk.GroupChance", 18);
    cfg.worldChance = sConfigMgr->GetOption<uint32>("Playerbots.BotTalk.WorldChance", 8);
    cfg.cooldownMin = sConfigMgr->GetOption<uint32>("Playerbots.BotTalk.CooldownMin", 20);
    cfg.cooldownMax = sConfigMgr->GetOption<uint32>("Playerbots.BotTalk.CooldownMax", 55);
    cfg.preferPartyWhenGrouped = sConfigMgr->GetOption<bool>("Playerbots.BotTalk.PreferPartyWhenGrouped", true);
    if (cfg.cooldownMax < cfg.cooldownMin)
        cfg.cooldownMax = cfg.cooldownMin;
    cfgLoaded = true;
}

bool IsEnabled()
{
    if (!cfgLoaded)
        LoadConfig();
    return cfg.enable;
}

bool ShouldUseParty(Player* bot)
{
    if (!cfgLoaded)
        LoadConfig();
    if (!cfg.preferPartyWhenGrouped || !bot)
        return false;
    return BotGearLane::IsGroupedWithRealPlayer(bot) && bot->GetGroup();
}

Channel PreferredGroupChannel(Player* bot)
{
    if (!bot || !bot->GetGroup())
        return Channel::Say;
    if (bot->GetGroup()->isRaidGroup())
        return Channel::Raid;
    return Channel::Party;
}

bool RaidCall(Player* bot, std::string_view text)
{
    if (!bot || text.empty())
        return false;
    return Speak(bot, PreferredGroupChannel(bot), text);
}

bool Speak(Player* bot, Channel channel, std::string_view text, Player* whisperTo)
{
    if (!bot || text.empty())
        return false;

    PlayerbotAI* ai = GET_PLAYERBOT_AI(bot);
    if (!ai)
        return false;

    std::string msg(text);
    if (msg.rfind("/y ", 0) == 0)
    {
        channel = Channel::Yell;
        msg = msg.substr(3);
    }
    else if (msg.rfind("/s ", 0) == 0)
    {
        channel = Channel::Say;
        msg = msg.substr(3);
    }

    bool ok = false;
    switch (channel)
    {
        case Channel::Yell:
            ok = ai->Yell(msg);
            break;
        case Channel::Party:
            ok = ai->SayToParty(msg);
            break;
        case Channel::Raid:
            ok = ai->SayToRaid(msg);
            break;
        case Channel::Guild:
            ok = ai->SayToGuild(msg);
            break;
        case Channel::World:
            ok = ai->SayToWorld(msg);
            break;
        case Channel::Whisper:
            if (whisperTo)
                ok = ai->Whisper(msg, whisperTo->GetName());
            break;
        case Channel::Say:
        default:
            ok = ai->Say(msg);
            break;
    }

    if (ok)
        MarkSpoke(bot->GetGUID().GetCounter());
    return ok;
}

bool SpeakLine(Player* bot, Channel channel, std::string const& textKey,
               std::map<std::string, std::string> const& placeholders)
{
    std::string const line = ResolveLine(textKey, placeholders);
    if (line.empty())
        return false;
    return Speak(bot, channel, line);
}

bool SayCallout(Player* bot, Callout callout)
{
    if (!IsEnabled() || !bot)
        return false;

    Channel channel = ShouldUseParty(bot) ? PreferredGroupChannel(bot) : Channel::Say;
    char const* key = "talk_group_ambient";
    switch (callout)
    {
        case Callout::CriticalHealth:
            key = "critical health";
            break;
        case Callout::LowHealth:
            key = "low health";
            break;
        case Callout::LowMana:
            key = "low mana";
            break;
        case Callout::Taunt:
            key = "taunt";
            break;
        case Callout::Aoe:
            key = "aoe";
            break;
        case Callout::Loot:
            key = "loot";
            break;
        case Callout::Pull:
            key = "talk_pull";
            break;
        case Callout::Incoming:
            key = "talk_incoming";
            channel = Channel::Yell;
            break;
    }
    return SpeakLine(bot, channel, key);
}

bool Tick(Player* bot, PlayerbotAI* botAI)
{
    if (!IsEnabled() || !bot || !botAI)
        return false;
    if (!bot->IsAlive() || bot->IsBeingTeleported() || bot->IsInFlight())
        return false;
    if (botAI->HasStrategy("silent", BotState::BOT_STATE_NON_COMBAT))
        return false;
    if (OnCooldown(bot))
        return false;

    // Combat callouts are the say strategy's job.
    if (bot->IsInCombat())
        return false;

    if (ShouldUseParty(bot))
    {
        if (cfg.groupChance && urand(1, 100) <= cfg.groupChance)
            return SpeakLine(bot, PreferredGroupChannel(bot), "talk_group_ambient");
        return false;
    }

    if (!botAI->HasPlayerNearby())
        return false;

    if (cfg.worldChance && urand(1, 100) <= cfg.worldChance)
        return SpeakLine(bot, Channel::Say, "talk_world_ambient");

    return false;
}
} // namespace BotTalk
