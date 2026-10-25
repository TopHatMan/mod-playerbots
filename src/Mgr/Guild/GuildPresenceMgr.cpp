/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "GuildPresenceMgr.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "RandomPlayerbotMgr.h"
#include "WorldSession.h"

#include <algorithm>
#include <ctime>
#include <map>
#include <sstream>

namespace
{
struct GuildPresenceCfg
{
    bool enable = false;
    uint32 scanIntervalSec = 10;
    uint32 maxLoginsPerScan = 20;
    bool includeAddClass = true;
};

GuildPresenceCfg cfg;
bool cfgLoaded = false;

std::map<uint32, time_t>& NextScanByGuild()
{
    static std::map<uint32, time_t> nextScan;
    return nextScan;
}

void EnsureLoaded()
{
    if (!cfgLoaded)
        GuildPresenceMgr::LoadConfig();
}

bool SuppressForcePresence()
{
    // SIM-POPULATION: calendar presence replaces "keep the guild roster warm".
    return sConfigMgr->GetOption<bool>("Sim.DisableGuildForcePresence", true);
}
} // namespace

namespace GuildPresenceMgr
{
void LoadConfig()
{
    cfg.enable = sConfigMgr->GetOption<bool>("Playerbots.GuildPresence.Enable", false);
    cfg.scanIntervalSec = std::max<uint32>(1,
        sConfigMgr->GetOption<uint32>("Playerbots.GuildPresence.ScanIntervalSec", 10));
    cfg.maxLoginsPerScan = sConfigMgr->GetOption<uint32>("Playerbots.GuildPresence.MaxLoginsPerScan", 20);
    cfg.includeAddClass = sConfigMgr->GetOption<bool>("Playerbots.GuildPresence.IncludeAddClass", true);
    cfgLoaded = true;
}

bool IsForcePresenceSuppressed()
{
    return SuppressForcePresence();
}

bool IsLoginEnabled()
{
    EnsureLoaded();
    if (SuppressForcePresence())
        return false;
    return cfg.enable && sPlayerbotAIConfig.Enabled;
}

void EnsureGuildBotsOnline(Player* realPlayer)
{
    if (!realPlayer || !realPlayer->GetSession() || realPlayer->GetSession()->IsHeadless())
        return;

    if (!IsLoginEnabled())
        return;

    uint32 const guildId = realPlayer->GetGuildId();
    if (!guildId)
        return;

    time_t const now = std::time(nullptr);
    time_t& nextScan = NextScanByGuild()[guildId];
    if (nextScan > now)
        return;
    nextScan = now + cfg.scanIntervalSec;

    // guild_member alone does not expose account ownership. Joining the
    // character row lets us distinguish configured Playerbots accounts
    // from normal player accounts while the character is offline.
    QueryResult result = CharacterDatabase.Query(
        "SELECT gm.`guid`, c.`account` "
        "FROM `guild_member` gm "
        "INNER JOIN `characters` c ON c.`guid` = gm.`guid` "
        "WHERE gm.`guildid` = {} ORDER BY gm.`guid`",
        guildId);
    if (!result)
        return;

    RandomPlayerbotMgr& randomMgr = RandomPlayerbotMgr::instance();
    uint32 requested = 0;

    do
    {
        Field* fields = result->Fetch();
        ObjectGuid::LowType const lowGuid = fields[0].Get<uint32>();
        uint32 const accountId = fields[1].Get<uint32>();
        if (!lowGuid || lowGuid == realPlayer->GetGUID().GetCounter())
            continue;

        ObjectGuid const guid = ObjectGuid::Create<HighGuid::Player>(lowGuid);

        // Connected includes characters still loading into the world. Do
        // not duplicate a login request just because IsInWorld is not set yet.
        if (ObjectAccessor::FindConnectedPlayer(guid))
            continue;

        bool eligible = PlayerbotAIConfig::instance().IsInRandomAccountList(accountId);
        if (!eligible && cfg.includeAddClass)
            eligible = randomMgr.IsAddclassBot(lowGuid);
        if (!eligible)
            continue;

        // masterAccountId=0 routes the login through RandomPlayerbotMgr.
        // These remain autonomous guild characters rather than becoming
        // personal followers of whichever real guild member triggered us.
        randomMgr.AddPlayerBot(guid, 0);
        ++requested;

        if (cfg.maxLoginsPerScan && requested >= cfg.maxLoginsPerScan)
            break;
    } while (result->NextRow());

    if (requested)
    {
        LOG_INFO("playerbots",
            "GuildPresence guild {} requested {} offline playerbot login(s) because real member {} is online.",
            guildId, requested, realPlayer->GetName());
    }
}

std::string StatusText()
{
    EnsureLoaded();
    std::ostringstream out;
    out << "GuildPresence.Enable config=" << (cfg.enable ? 1 : 0)
        << " effective=" << (IsLoginEnabled() ? 1 : 0)
        << " ScanIntervalSec=" << cfg.scanIntervalSec
        << " MaxLoginsPerScan=" << cfg.maxLoginsPerScan
        << " IncludeAddClass=" << (cfg.includeAddClass ? 1 : 0)
        << " BotActiveAloneForceWhenInGuild config="
        << (sPlayerbotAIConfig.BotActiveAloneForceWhenInGuild ? 1 : 0)
        << " suppressed=" << (IsForcePresenceSuppressed() ? 1 : 0)
        << " (mod-npc-chat-llm no longer logins bots)";
    return out.str();
}
} // namespace GuildPresenceMgr
