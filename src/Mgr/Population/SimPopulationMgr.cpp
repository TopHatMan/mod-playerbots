/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "SimPopulationMgr.h"
#include "BotActivityMgr.h"
#include "CharacterCache.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "GuildPresenceMgr.h"
#include "Log.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "PlayerbotsDatabase.h"
#include "Random.h"
#include "RandomPlayerbotMgr.h"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{
struct SimCfg
{
    bool enable = false;
    uint32 rosterTarget = 10000;
    uint32 targetOnline = 150;
    uint32 minimumOnline = 120;
    uint32 concurrentCap = 150;
    uint32 startupStaggerSec = 600;
    uint32 tickSec = 20;
    uint32 idleKickSec = 120;
    bool countAltbotsAgainstCap = false;
    std::string defaultFocus = "level";
    bool allowChurn = false;
    uint32 realmMaxLevel = 60;
    bool disableStockRandomMgr = true;
    bool disableGuildForcePresence = true;
};

struct SimAccount
{
    uint32 simId = 0;
    std::string persona;
    std::string status;
    int32 timezoneOffsetMin = 0;
};

struct SimChar
{
    uint32 guid = 0;
    uint32 simId = 0;
    std::string role;
    std::string focus;
    std::string status;
};

struct SimSched
{
    uint32 simId = 0;
    uint8 dowMask = 0;
    int32 loginTodMin = 0;
    int32 durationMin = 0;
    int32 jitterMin = 25;
    std::string kind;
};

SimCfg cfg;
bool cfgLoaded = false;
bool rosterLoaded = false;
bool loggedEmpty = false;
uint32 tickAccumMs = 0;
time_t worldStart = 0;
time_t nextAutoSeedCheck = 0;
uint32 constexpr AutoSeedRetrySec = 300;

std::unordered_map<uint32, SimAccount> accounts; // simId
std::unordered_map<uint32, SimChar> chars;       // guid
std::unordered_map<uint32, std::vector<SimSched>> schedules; // simId
std::unordered_map<uint32, time_t> idleSince;
std::unordered_map<uint32, time_t> pendingLogout;
std::unordered_map<uint32, time_t> floorUntil;
std::unordered_map<uint32, time_t> lastFocusApply;
std::unordered_set<uint32> sessionLogged; // guid with open actual_login

void EnsureLoaded()
{
    if (!cfgLoaded)
        SimPopulationMgr::LoadConfig();
}

tm LocalNow(time_t t)
{
    tm local{};
#if defined(_WIN32)
    localtime_s(&local, &t);
#else
    localtime_r(&t, &local);
#endif
    return local;
}

int KindPriority(std::string const& kind)
{
    if (kind == "raid")
        return 4;
    if (kind == "play")
        return 3;
    if (kind == "ah")
        return 2;
    if (kind == "quick")
        return 1;
    return 0;
}

int JitterOffset(uint32 guid, int dow, int32 jitterMin)
{
    if (jitterMin <= 0)
        return 0;
    uint32 h = guid * 1103515245u + uint32(dow) * 12345u;
    return int(h % uint32(2 * jitterMin + 1)) - int(jitterMin);
}

bool InWindow(int nowMin, int startMin, int durationMin)
{
    int endMin = startMin + durationMin;
    if (endMin <= 1440)
        return nowMin >= startMin && nowMin < endMin;
    return nowMin >= startMin || nowMin < (endMin - 1440);
}

bool IsAltbotProtected(Player* bot)
{
    if (!bot)
        return false;
    PlayerbotAI* ai = GET_PLAYERBOT_AI(bot);
    if (ai && ai->HasGameClientMaster())
        return true;
    if (Group* group = bot->GetGroup())
    {
        for (Group::MemberSlot const& slot : group->GetMemberSlots())
        {
            Player* member = ObjectAccessor::FindConnectedPlayer(slot.guid);
            if (member && IsRealPlayer(member))
                return true;
        }
    }
    return false;
}

bool IsSimCharActive(SimChar const& ch)
{
    if (ch.status != "active")
        return false;
    auto it = accounts.find(ch.simId);
    if (it == accounts.end())
        return false;
    return it->second.status == "active";
}

bool WantedNow(SimChar const& ch, time_t now, std::string* outKind, int* outStart, int* outEnd)
{
    if (!IsSimCharActive(ch))
        return false;

    tm local = LocalNow(now);
    int dow = local.tm_wday; // 0=Sun
    int nowMin = local.tm_hour * 60 + local.tm_min;
    auto ait = accounts.find(ch.simId);
    int32 const timezoneOffset = ait != accounts.end() ? ait->second.timezoneOffsetMin : 0;
    int localTotal = nowMin + timezoneOffset;
    int localDayOffset = 0;
    while (localTotal < 0)
    {
        localTotal += 1440;
        --localDayOffset;
    }
    while (localTotal >= 1440)
    {
        localTotal -= 1440;
        ++localDayOffset;
    }
    int localDow = (dow + localDayOffset + 7) % 7;
    uint8 bit = uint8(1u << localDow);

    auto sit = schedules.find(ch.simId);
    if (sit == schedules.end())
        return false;

    int bestPri = -1;
    for (SimSched const& sch : sit->second)
    {
        if (!(sch.dowMask & bit))
            continue;
        int jitter = JitterOffset(ch.guid, localDow, sch.jitterMin);
        int start = sch.loginTodMin + jitter;
        while (start < 0)
            start += 1440;
        start %= 1440;
        if (!InWindow(nowMin, start, sch.durationMin))
            continue;
        int pri = KindPriority(sch.kind);
        if (pri > bestPri)
        {
            bestPri = pri;
            if (outKind)
                *outKind = sch.kind;
            if (outStart)
                *outStart = start;
            if (outEnd)
                *outEnd = start + sch.durationMin;
        }
    }
    return bestPri >= 0;
}

void LoadRoster()
{
    accounts.clear();
    chars.clear();
    schedules.clear();
    floorUntil.clear();

    if (QueryResult accRes = PlayerbotsDatabase.Query(
            "SELECT sim_id, persona, status FROM pb_sim_account"))
    {
        do
        {
            Field* f = accRes->Fetch();
            SimAccount a;
            a.simId = f[0].Get<uint32>();
            a.persona = f[1].Get<std::string>();
            a.status = f[2].Get<std::string>();
            accounts[a.simId] = a;
        } while (accRes->NextRow());
    }

    if (QueryResult tzRes = PlayerbotsDatabase.Query(
            "SELECT sim_id, timezone_offset_min FROM pb_sim_account_timezone"))
    {
        do
        {
            Field* f = tzRes->Fetch();
            auto it = accounts.find(f[0].Get<uint32>());
            if (it != accounts.end())
                it->second.timezoneOffsetMin = f[1].Get<int32>();
        } while (tzRes->NextRow());
    }

    if (QueryResult chRes = PlayerbotsDatabase.Query(
            "SELECT guid, sim_id, role, focus, status FROM pb_sim_character"))
    {
        do
        {
            Field* f = chRes->Fetch();
            SimChar c;
            c.guid = f[0].Get<uint32>();
            c.simId = f[1].Get<uint32>();
            c.role = f[2].Get<std::string>();
            c.focus = f[3].Get<std::string>();
            c.status = f[4].Get<std::string>();
            chars[c.guid] = c;
        } while (chRes->NextRow());
    }

    if (QueryResult schRes = PlayerbotsDatabase.Query(
            "SELECT sim_id, dow_mask, login_tod_min, duration_min, jitter_min, kind FROM pb_sim_schedule"))
    {
        do
        {
            Field* f = schRes->Fetch();
            SimSched s;
            s.simId = f[0].Get<uint32>();
            s.dowMask = f[1].Get<uint8>();
            s.loginTodMin = f[2].Get<int32>();
            s.durationMin = f[3].Get<int32>();
            s.jitterMin = f[4].Get<int32>();
            s.kind = f[5].Get<std::string>();
            schedules[s.simId].push_back(s);
        } while (schRes->NextRow());
    }

    rosterLoaded = true;
    uint32 scheduleRows = 0;
    for (auto const& kv : schedules)
        scheduleRows += uint32(kv.second.size());
    LOG_INFO("playerbots", "SimPopulation loaded {} accounts, {} characters, {} schedule rows.",
        uint32(accounts.size()), uint32(chars.size()), scheduleRows);
}

char const* PickPersona(uint32 index, uint32 total)
{
    // Mix from the spec, applied in order so a 2000 roster matches the table.
    if (!total)
        return "evening";
    double p = double(index) / double(total);
    if (p < 0.35) return "evening";
    if (p < 0.50) return "morning";
    if (p < 0.60) return "afternoon";
    if (p < 0.68) return "night";
    if (p < 0.80) return "weekend";
    if (p < 0.92) return "raid_logger";
    if (p < 0.97) return "banker";
    return "pvper";
}

char const* DefaultFocusFor(std::string const& persona)
{
    if (persona == "banker")
        return "gold";
    if (persona == "pvper")
        return "pvp";
    if (persona == "raid_logger")
        return "dungeon";
    if (persona == "morning" && roll_chance_i(40))
        return "gold";
    if (persona == "night" && roll_chance_i(40))
        return "pvp";
    return cfg.defaultFocus.c_str();
}

int32 PickTimezoneOffset(uint32 index, uint32 total)
{
    static constexpr int32 offsets[] = {-360, -300, -240, -180, -120, -60, 0, 60, 120, 180};
    if (!total)
        return 0;
    return offsets[index % (sizeof(offsets) / sizeof(offsets[0]))];
}

void InsertSchedule(uint32 simId, uint8 dowMask, int32 loginMin, int32 durationMin, int32 jitterMin, char const* kind)
{
    PlayerbotsDatabase.DirectExecute(
        "INSERT INTO pb_sim_schedule (sim_id, dow_mask, login_tod_min, duration_min, jitter_min, kind) "
        "VALUES ({}, {}, {}, {}, {}, '{}')",
        simId, uint32(dowMask), loginMin, durationMin, jitterMin, kind);
}

void SeedSchedules(uint32 simId, std::string const& persona)
{
    // bit0=Sun .. bit6=Sat
    uint8 const weekdays = 0x3E; // Mon-Fri
    uint8 const monThu = 0x1E;
    uint8 const fri = 0x20;
    uint8 const sat = 0x40;
    uint8 const sun = 0x01;
    uint8 const all = 0x7F;

    if (persona == "evening")
    {
        InsertSchedule(simId, monThu, 19 * 60, int32(urand(240, 360)), 25, "play");
        InsertSchedule(simId, fri, 20 * 60, int32(urand(300, 420)), 25, "play");
    }
    else if (persona == "morning")
        InsertSchedule(simId, weekdays, 6 * 60, int32(urand(120, 240)), 20, "play");
    else if (persona == "afternoon")
        InsertSchedule(simId, weekdays, 14 * 60, 4 * 60, 20, "play");
    else if (persona == "night")
        InsertSchedule(simId, all, 23 * 60, 4 * 60, 20, "play");
    else if (persona == "weekend")
    {
        InsertSchedule(simId, fri, 20 * 60, 6 * 60, 25, "play");
        InsertSchedule(simId, uint8(sat | sun), 12 * 60, 8 * 60, 30, "play");
    }
    else if (persona == "raid_logger")
    {
        InsertSchedule(simId, 0x04, 20 * 60, 90, 15, "play");           // Tue midweek
        InsertSchedule(simId, uint8(0x10 | sun), 19 * 60 + 20, 3 * 60, 15, "raid"); // Thu+Sun
    }
    else if (persona == "banker")
    {
        InsertSchedule(simId, all, 12 * 60, 45, 15, "ah");
        InsertSchedule(simId, all, 20 * 60, 45, 15, "ah");
    }
    else if (persona == "pvper")
        InsertSchedule(simId, weekdays | fri | sat, 19 * 60, 3 * 60, 25, "play");
    else
        InsertSchedule(simId, weekdays, 19 * 60, 4 * 60, 25, "play");
}

uint32 CountOnlineSim(bool includeAltbots)
{
    uint32 n = 0;
    for (auto const& kv : chars)
    {
        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(kv.first);
        Player* bot = ObjectAccessor::FindConnectedPlayer(guid);
        if (!bot || !bot->IsInWorld())
            continue;
        if (!includeAltbots && IsAltbotProtected(bot))
            continue;
        ++n;
    }
    return n;
}

void RecordSession(uint32 guid, char const* denied)
{
    time_t now = time(nullptr);
    tm local = LocalNow(now);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
        local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
        local.tm_hour, local.tm_min, local.tm_sec);

    if (denied)
    {
        PlayerbotsDatabase.Execute(
            "INSERT INTO pb_sim_session (guid, planned_login, planned_logout, denied_reason) "
            "VALUES ({}, '{}', '{}', '{}')",
            guid, buf, buf, denied);
        return;
    }

    PlayerbotsDatabase.Execute(
        "INSERT INTO pb_sim_session (guid, planned_login, planned_logout, actual_login) "
        "VALUES ({}, '{}', '{}', '{}')",
        guid, buf, buf, buf);
    sessionLogged.insert(guid);
}

void RecordLogout(uint32 guid)
{
    if (!sessionLogged.count(guid))
        return;
    time_t now = time(nullptr);
    tm local = LocalNow(now);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
        local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
        local.tm_hour, local.tm_min, local.tm_sec);
    PlayerbotsDatabase.Execute(
        "UPDATE pb_sim_session SET actual_logout = '{}' WHERE guid = {} AND actual_logout IS NULL "
        "ORDER BY id DESC LIMIT 1",
        buf, guid);
    sessionLogged.erase(guid);
}

void ApplyFocus(Player* bot, std::string const& focus)
{
    if (!bot)
        return;
    PlayerbotAI* ai = GET_PLAYERBOT_AI(bot);
    if (!ai)
        return;

    if (focus == "level")
        ai->ChangeStrategy("+quest,+grind,-stay,-follow,-passive", BOT_STATE_NON_COMBAT);
    else if (focus == "dungeon")
        ai->ChangeStrategy("+quest,+grind,-stay", BOT_STATE_NON_COMBAT);
    else if (focus == "pvp")
        ai->ChangeStrategy("+pvp,+grind,-stay", BOT_STATE_NON_COMBAT);
    else if (focus == "raid")
        ai->ChangeStrategy("+rpg,-grind", BOT_STATE_NON_COMBAT);
    else if (focus == "gold")
        ai->ChangeStrategy("+rpg,-grind", BOT_STATE_NON_COMBAT);
    else if (focus == "explore")
        ai->ChangeStrategy("+rpg,-stay", BOT_STATE_NON_COMBAT);
    else if (focus == "social")
        ai->ChangeStrategy("+rpg,-grind,-follow", BOT_STATE_NON_COMBAT);

    lastFocusApply[bot->GetGUID().GetCounter()] = time(nullptr);
}

bool InCityOrInn(Player* bot)
{
    if (!bot)
        return false;
    if (bot->HasPlayerFlag(PLAYER_FLAGS_RESTING))
        return true;
    switch (bot->GetZoneId())
    {
        case 1519: // Stormwind
        case 1537: // Ironforge
        case 1657: // Darnassus
        case 1637: // Orgrimmar
        case 1638: // Thunder Bluff
        case 1497: // Undercity
            return true;
        default:
            return false;
    }
}

void IdleWatchdog(time_t now)
{
    if (!cfg.idleKickSec)
        return;

    for (auto const& kv : chars)
    {
        if (kv.second.focus != "level" || !IsSimCharActive(kv.second))
            continue;

        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(kv.first);
        Player* bot = ObjectAccessor::FindPlayer(guid);
        if (!bot || !bot->IsInWorld())
        {
            idleSince.erase(kv.first);
            continue;
        }
        if (IsAltbotProtected(bot) || bot->InBattlegroundQueue() || bot->InBattleground())
        {
            idleSince.erase(kv.first);
            continue;
        }
        if (BotActivityMgr::BlocksWorldWander(bot))
        {
            idleSince.erase(kv.first);
            continue;
        }
        if (!InCityOrInn(bot))
        {
            idleSince.erase(kv.first);
            continue;
        }

        time_t& since = idleSince[kv.first];
        if (!since)
            since = now;
        if (uint32(now - since) < cfg.idleKickSec)
            continue;

        ApplyFocus(bot, "level");
        sRandomPlayerbotMgr.RandomTeleportGrindForLevel(bot);
        since = now;
        LOG_DEBUG("playerbots", "SimPopulation idle-kick {} (guid {}) out of city/inn.",
            bot->GetName(), kv.first);
    }
}

void TickLoginLogout(time_t now)
{
    uint32 uptime = uint32(now - worldStart);
    bool staggering = cfg.startupStaggerSec && uptime < cfg.startupStaggerSec;

    std::vector<std::pair<int, uint32>> wanted; // (-priority, guid)
    std::unordered_set<uint32> wantedSet;

    for (auto const& kv : chars)
    {
        std::string kind;
        if (!WantedNow(kv.second, now, &kind, nullptr, nullptr))
            continue;
        wantedSet.insert(kv.first);
        wanted.push_back({-KindPriority(kind), kv.first});
    }

    // A calendar describes preferred play times, but it must not make the
    // realm look empty between overlapping persona windows. Keep a stable,
    // deterministic reserve online until the next floor refresh. These are
    // still real roster characters and retain their persona; this is only a
    // presence backstop when the scheduled pool is too small.
    for (auto it = floorUntil.begin(); it != floorUntil.end();)
    {
        if (it->second <= now)
            it = floorUntil.erase(it);
        else
            ++it;
    }

    for (auto const& kv : floorUntil)
    {
        if (!wantedSet.count(kv.first))
        {
            wantedSet.insert(kv.first);
            wanted.push_back({0, kv.first});
        }
    }

    uint32 onlineBeforeFloor = CountOnlineSim(cfg.countAltbotsAgainstCap);
    uint32 floor = std::min(cfg.minimumOnline, cfg.concurrentCap);
    if (onlineBeforeFloor < floor)
    {
        uint32 needed = floor - onlineBeforeFloor;
        for (auto const& kv : chars)
        {
            if (!needed || wantedSet.count(kv.first) || !IsSimCharActive(kv.second))
                continue;

            ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(kv.first);
            if (ObjectAccessor::FindConnectedPlayer(guid) || sRandomPlayerbotMgr.GetPlayerBot(guid))
                continue;

            // Keep the reserve for a meaningful interval so it does not
            // oscillate in and out on every 20-second manager tick.
            floorUntil[kv.first] = now + 15 * 60;
            wantedSet.insert(kv.first);
            wanted.push_back({-1, kv.first});
            --needed;
        }
    }
    std::sort(wanted.begin(), wanted.end());

    // Logout not-wanted (30s grace in combat / instance).
    for (auto const& kv : chars)
    {
        if (wantedSet.count(kv.first))
        {
            pendingLogout.erase(kv.first);
            continue;
        }
        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(kv.first);
        Player* bot = sRandomPlayerbotMgr.GetPlayerBot(guid);
        if (!bot)
        {
            pendingLogout.erase(kv.first);
            continue;
        }
        if (IsAltbotProtected(bot))
            continue;

        bool grace = bot->IsInCombat();
        if (Map* map = bot->GetMap())
            grace = grace || map->Instanceable();
        if (grace)
        {
            time_t& when = pendingLogout[kv.first];
            if (!when)
                when = now + 30;
            if (now < when)
                continue;
        }
        sRandomPlayerbotMgr.LogoutPlayerBot(guid);
        RecordLogout(kv.first);
        pendingLogout.erase(kv.first);
        idleSince.erase(kv.first);
    }

    uint32 online = CountOnlineSim(cfg.countAltbotsAgainstCap);
    uint32 loginsThisTick = 0;
    uint32 const loginBatch = 8;

    for (auto const& item : wanted)
    {
        uint32 guidLow = item.second;
        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        if (ObjectAccessor::FindConnectedPlayer(guid) || sRandomPlayerbotMgr.GetPlayerBot(guid))
            continue;

        if (staggering)
        {
            uint32 delay = guidLow % std::max(1u, cfg.startupStaggerSec);
            if (uptime < delay)
            {
                RecordSession(guidLow, "restart_stagger");
                continue;
            }
        }

        uint32 desiredOnline = std::min(cfg.targetOnline, cfg.concurrentCap);
        if (online >= desiredOnline)
        {
            RecordSession(guidLow, "cap");
            continue;
        }
        if (loginsThisTick >= loginBatch)
            break;

        sRandomPlayerbotMgr.AddPlayerBot(guid, 0);
        RecordSession(guidLow, nullptr);
        ++online;
        ++loginsThisTick;
    }

    // Apply focus shortly after login (AI may not exist on the same tick).
    for (uint32 guidLow : wantedSet)
    {
        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindPlayer(guid);
        if (!bot || !bot->IsInWorld() || !GET_PLAYERBOT_AI(bot))
            continue;
        time_t last = lastFocusApply[guidLow];
        if (last && now - last < 60)
            continue;
        auto it = chars.find(guidLow);
        if (it != chars.end())
            ApplyFocus(bot, it->second.focus);
    }
}

SimChar const* FindByName(std::string const& name, uint32* guidOut = nullptr)
{
    ObjectGuid guid = sCharacterCache->GetCharacterGuidByName(name);
    if (guid.IsEmpty())
        return nullptr;
    uint32 low = guid.GetCounter();
    auto it = chars.find(low);
    if (it == chars.end())
        return nullptr;
    if (guidOut)
        *guidOut = low;
    return &it->second;
}

std::string JoinStrategies(PlayerbotAI* ai)
{
    if (!ai)
        return "-";
    std::ostringstream out;
    out << "co[";
    bool first = true;
    for (std::string const& s : ai->GetStrategies(BOT_STATE_COMBAT))
    {
        if (!first)
            out << ',';
        first = false;
        out << s;
    }
    out << "] nc[";
    first = true;
    for (std::string const& s : ai->GetStrategies(BOT_STATE_NON_COMBAT))
    {
        if (!first)
            out << ',';
        first = false;
        out << s;
    }
    out << ']';
    return out.str();
}
} // namespace

namespace SimPopulationMgr
{
void LoadConfig()
{
    cfg.enable = sConfigMgr->GetOption<bool>("Sim.Enable", true);
    cfg.rosterTarget = sConfigMgr->GetOption<uint32>("Sim.RosterTarget", 10000);
    cfg.targetOnline = sConfigMgr->GetOption<uint32>("Sim.TargetOnline", 150);
    cfg.minimumOnline = sConfigMgr->GetOption<uint32>("Sim.MinimumOnline", 120);
    cfg.concurrentCap = sConfigMgr->GetOption<uint32>("Sim.ConcurrentCap", 150);
    if (cfg.concurrentCap > 300)
        cfg.concurrentCap = 300;
    cfg.targetOnline = std::min(cfg.targetOnline, cfg.concurrentCap);
    cfg.minimumOnline = std::min(cfg.minimumOnline, cfg.targetOnline);
    cfg.startupStaggerSec = sConfigMgr->GetOption<uint32>("Sim.StartupStaggerSec", 600);
    cfg.tickSec = std::max<uint32>(5, sConfigMgr->GetOption<uint32>("Sim.TickSec", 20));
    cfg.idleKickSec = sConfigMgr->GetOption<uint32>("Sim.IdleKickSec", 120);
    cfg.countAltbotsAgainstCap = sConfigMgr->GetOption<bool>("Sim.CountAltbotsAgainstCap", false);
    cfg.defaultFocus = sConfigMgr->GetOption<std::string>("Sim.DefaultFocus", "level");
    cfg.allowChurn = sConfigMgr->GetOption<bool>("Sim.AllowChurn", false);
    cfg.realmMaxLevel = sConfigMgr->GetOption<uint32>("Sim.RealmMaxLevel", 60);
    cfg.disableStockRandomMgr = sConfigMgr->GetOption<bool>("Sim.DisableStockRandomMgr", true);
    cfg.disableGuildForcePresence = sConfigMgr->GetOption<bool>("Sim.DisableGuildForcePresence", true);
    cfgLoaded = true;
    worldStart = time(nullptr);
    (void)cfg.allowChurn;
}

void Update(uint32 diff)
{
    EnsureLoaded();
    if (!cfg.enable)
        return;

    if (!rosterLoaded)
        LoadRoster();

    tickAccumMs += diff;
    if (tickAccumMs < cfg.tickSec * 1000)
        return;
    tickAccumMs = 0;

    time_t now = time(nullptr);

    // Auto-seed from existing rndbot accounts so the calendar takes over with
    // no manual `.sim seed` step. Retried on a throttle so a roster that is
    // short of RosterTarget keeps topping up as more rndbot characters exist.
    if (chars.size() < cfg.rosterTarget && now >= nextAutoSeedCheck)
    {
        nextAutoSeedCheck = now + AutoSeedRetrySec;
        uint32 const wanted = cfg.rosterTarget - uint32(chars.size());
        uint32 const seeded = Seed(wanted);
        if (seeded)
        {
            loggedEmpty = false;
            LOG_INFO("playerbots",
                "SimPopulation auto-seeded {} character(s) from existing rndbot accounts (roster {} / target {}).",
                seeded, uint32(chars.size()), cfg.rosterTarget);
        }
    }

    if (chars.empty())
    {
        if (!loggedEmpty)
        {
            LOG_INFO("playerbots",
                "SimPopulation enabled but no eligible existing rndbot characters were found to auto-seed "
                "(need characters on AiPlayerbot random/addclass accounts, level <= Sim.RealmMaxLevel). "
                "Will keep retrying; `.sim seed N` still works manually too.");
            loggedEmpty = true;
        }
        return;
    }

    TickLoginLogout(now);
    IdleWatchdog(now);
}

bool IsEnabled()
{
    EnsureLoaded();
    return cfg.enable;
}

bool DisableStockRandomMgr()
{
    EnsureLoaded();
    return cfg.enable && cfg.disableStockRandomMgr;
}

bool DisableGuildForcePresence()
{
    EnsureLoaded();
    return cfg.disableGuildForcePresence;
}

bool OwnsGuid(uint32 guidLow)
{
    return chars.find(guidLow) != chars.end();
}

uint32 Seed(uint32 count)
{
    EnsureLoaded();
    if (!count)
        return 0;
    if (!rosterLoaded)
        LoadRoster();

    std::vector<uint32> const& accountsList = sPlayerbotAIConfig.RandomBotAccounts;
    if (accountsList.empty())
    {
        LOG_ERROR("playerbots", "SimPopulation seed: no random-bot accounts configured.");
        return 0;
    }

    std::ostringstream inList;
    for (size_t i = 0; i < accountsList.size(); ++i)
    {
        if (i)
            inList << ',';
        inList << accountsList[i];
    }

    QueryResult result = CharacterDatabase.Query(
        "SELECT guid, account, name, level FROM characters "
        "WHERE account IN ({}) AND level <= {} ORDER BY guid",
        inList.str(), cfg.realmMaxLevel);
    if (!result)
        return 0;

    struct Cand { uint32 guid; uint32 level; };
    std::vector<Cand> pool;
    do
    {
        Field* f = result->Fetch();
        uint32 guid = f[0].Get<uint32>();
        if (chars.count(guid))
            continue;
        uint32 level = f[3].Get<uint32>();
        if (level < 1 || level > cfg.realmMaxLevel)
            continue;
        pool.push_back({guid, level});
    } while (result->NextRow());

    uint32 take = std::min<uint32>(count, uint32(pool.size()));
    uint32 stamped = 0;
    for (uint32 i = 0; i < take; ++i)
    {
        char const* persona = PickPersona(uint32(chars.size()) + i, std::max(take, cfg.rosterTarget));
        char const* focus = DefaultFocusFor(persona);

        PlayerbotsDatabase.DirectExecute(
            "INSERT INTO pb_sim_account (persona, status) VALUES ('{}', 'active')", persona);

        uint32 simId = 0;
        if (QueryResult idRes = PlayerbotsDatabase.Query("SELECT LAST_INSERT_ID()"))
            simId = idRes->Fetch()[0].Get<uint32>();
        if (!simId)
            break;

        PlayerbotsDatabase.DirectExecute(
            "INSERT INTO pb_sim_character (guid, sim_id, role, focus, status) "
            "VALUES ({}, {}, 'main', '{}', 'active')",
            pool[i].guid, simId, focus);
        PlayerbotsDatabase.DirectExecute(
            "INSERT INTO pb_sim_account_timezone (sim_id, timezone_offset_min) VALUES ({}, {})",
            simId, PickTimezoneOffset(uint32(chars.size()) + i, std::max(take, cfg.rosterTarget)));
        SeedSchedules(simId, persona);
        ++stamped;
    }

    LoadRoster();
    loggedEmpty = false;
    LOG_INFO("playerbots", "SimPopulation seeded {} characters (requested {}).", stamped, count);
    return stamped;
}

std::string StatusText()
{
    EnsureLoaded();
    if (!rosterLoaded)
        LoadRoster();

    std::unordered_map<std::string, uint32> personaCount;
    std::unordered_map<std::string, uint32> focusCount;
    uint32 active = 0;
    for (auto const& kv : chars)
    {
        if (!IsSimCharActive(kv.second))
            continue;
        ++active;
        auto ait = accounts.find(kv.second.simId);
        if (ait != accounts.end())
            ++personaCount[ait->second.persona];
        ++focusCount[kv.second.focus];
    }

    uint32 online = CountOnlineSim(true);
    uint32 onlineCap = CountOnlineSim(cfg.countAltbotsAgainstCap);

    std::ostringstream out;
    out << "Sim.Enable=" << (cfg.enable ? 1 : 0)
        << " roster=" << chars.size() << "/" << cfg.rosterTarget
        << " active=" << active
        << " online=" << online
        << " target=" << cfg.targetOnline << " floor=" << cfg.minimumOnline
        << " capCounted=" << onlineCap << "/" << cfg.concurrentCap
        << " stagger=" << cfg.startupStaggerSec
        << " stockMgr=" << (DisableStockRandomMgr() ? "disabled" : "on")
        << "\n personas:";
    for (auto const& kv : personaCount)
        out << ' ' << kv.first << '=' << kv.second;
    out << "\n focus:";
    for (auto const& kv : focusCount)
        out << ' ' << kv.first << '=' << kv.second;
    if (chars.size() < cfg.rosterTarget)
        out << "\n WARN: roster is below target; only existing eligible rndbot/addclass characters can be rotated."
            << " Seed more characters or lower Sim.RosterTarget.";
    if (chars.size() < cfg.minimumOnline)
        out << "\n WARN: roster is smaller than Sim.MinimumOnline; the online floor cannot be maintained.";
    if (sPlayerbotAIConfig.RandomBotFixedLevel)
        out << "\n WARN: RandomBotFixedLevel=1 (fights leveling)";
    if (sPlayerbotAIConfig.DisableRandomLevels)
        out << "\n WARN: DisableRandomLevels=1";
    if (sPlayerbotAIConfig.RandomBotXPRate <= 0.0f)
        out << "\n WARN: RandomBotXPRate<=0";
    return out.str();
}

std::string ClockText()
{
    EnsureLoaded();
    if (!rosterLoaded)
        LoadRoster();

    time_t now = time(nullptr);
    tm local = LocalNow(now);
    int nowMin = local.tm_hour * 60 + local.tm_min;

    struct Hit { int wait; uint32 guid; int start; std::string kind; std::string name; };
    std::vector<Hit> hits;

    for (auto const& kv : chars)
    {
        if (!IsSimCharActive(kv.second))
            continue;
        auto sit = schedules.find(kv.second.simId);
        if (sit == schedules.end())
            continue;
        for (int ahead = 0; ahead < 2; ++ahead)
        {
            int dow = (local.tm_wday + ahead) % 7;
            uint8 bit = uint8(1u << dow);
            for (SimSched const& sch : sit->second)
            {
                if (!(sch.dowMask & bit))
                    continue;
                int start = sch.loginTodMin + JitterOffset(kv.first, dow, sch.jitterMin);
                while (start < 0)
                    start += 1440;
                start %= 1440;
                int wait = start - nowMin + ahead * 1440;
                if (wait < 0)
                    wait += 1440;
                std::string name;
                sCharacterCache->GetCharacterNameByGuid(ObjectGuid::Create<HighGuid::Player>(kv.first), name);
                hits.push_back({wait, kv.first, start, sch.kind, name});
            }
        }
    }
    std::sort(hits.begin(), hits.end(), [](Hit const& a, Hit const& b) { return a.wait < b.wait; });

    std::ostringstream out;
    out << "realm " << local.tm_hour << ':' << (local.tm_min < 10 ? "0" : "") << local.tm_min
        << " next 10 planned logins:\n";
    uint32 shown = 0;
    for (Hit const& h : hits)
    {
        if (shown >= 10)
            break;
        int hh = (h.start / 60) % 24;
        int mm = h.start % 60;
        out << "  +" << h.wait << "m  " << hh << ':' << (mm < 10 ? "0" : "") << mm
            << "  " << h.kind << "  " << (h.name.empty() ? "?" : h.name)
            << " guid=" << h.guid << '\n';
        ++shown;
    }
    if (!shown)
        out << "  (none)\n";
    return out.str();
}

std::string DebugText(std::string const& name)
{
    EnsureLoaded();
    if (!rosterLoaded)
        LoadRoster();

    uint32 guidLow = 0;
    SimChar const* ch = FindByName(name, &guidLow);
    if (!ch)
        return "not a sim character (or unknown name): " + name;

    auto ait = accounts.find(ch->simId);
    std::string persona = ait != accounts.end() ? ait->second.persona : "?";
    std::string accStatus = ait != accounts.end() ? ait->second.status : "?";

    time_t now = time(nullptr);
    std::string kind;
    int start = 0;
    int end = 0;
    bool wanted = WantedNow(*ch, now, &kind, &start, &end);

    ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(guidLow);
    Player* bot = ObjectAccessor::FindPlayer(guid);
    PlayerbotAI* ai = bot ? GET_PLAYERBOT_AI(bot) : nullptr;

    std::ostringstream out;
    out << name << " guid=" << guidLow << " sim_id=" << ch->simId
        << " persona=" << persona << " acc=" << accStatus
        << " role=" << ch->role << " focus=" << ch->focus
        << " char=" << ch->status << '\n';
    out << " window now=" << (wanted ? "YES" : "no");
    if (wanted)
        out << " kind=" << kind << " " << (start / 60) << ':' << (start % 60)
            << '-' << ((end / 60) % 24) << ':' << (end % 60);
    out << " login=" << (wanted ? "schedule" : "not-in-window") << '\n';

    if (!bot)
    {
        out << " online=N master=N idle_sec=- zone=- level=- xp_flags "
            << "FixedLevel=" << (sPlayerbotAIConfig.RandomBotFixedLevel ? 1 : 0)
            << " DisableRandomLevels=" << (sPlayerbotAIConfig.DisableRandomLevels ? 1 : 0)
            << " XPRate=" << sPlayerbotAIConfig.RandomBotXPRate << '\n';
        out << " strategies=-";
        return out.str();
    }

    bool master = ai && ai->HasGameClientMaster();
    time_t idle = 0;
    auto iit = idleSince.find(guidLow);
    if (iit != idleSince.end() && iit->second)
        idle = now - iit->second;

    out << " online=Y master=" << (master ? "Y" : "N")
        << " idle_sec=" << idle
        << " zone=" << bot->GetZoneId() << " map=" << bot->GetMapId()
        << " level=" << uint32(bot->GetLevel())
        << " xp=" << bot->GetUInt32Value(PLAYER_XP)
        << " resting=" << (bot->HasPlayerFlag(PLAYER_FLAGS_RESTING) ? 1 : 0)
        << " FixedLevel=" << (sPlayerbotAIConfig.RandomBotFixedLevel ? 1 : 0)
        << " DisableRandomLevels=" << (sPlayerbotAIConfig.DisableRandomLevels ? 1 : 0)
        << " XPRate=" << sPlayerbotAIConfig.RandomBotXPRate << '\n';
    out << " strategies=" << JoinStrategies(ai);
    return out.str();
}

std::string GuildPresenceText()
{
    std::ostringstream out;
    out << GuildPresenceMgr::StatusText()
        << "\n Sim.DisableGuildForcePresence=" << (DisableGuildForcePresence() ? 1 : 0);
    return out.str();
}

bool Pause(std::string const& name)
{
    uint32 guidLow = 0;
    SimChar const* ch = FindByName(name, &guidLow);
    if (!ch)
        return false;
    PlayerbotsDatabase.DirectExecute(
        "UPDATE pb_sim_character SET status='paused' WHERE guid={}", guidLow);
    LoadRoster();
    return true;
}

bool Resume(std::string const& name)
{
    uint32 guidLow = 0;
    SimChar const* ch = FindByName(name, &guidLow);
    if (!ch)
        return false;
    PlayerbotsDatabase.DirectExecute(
        "UPDATE pb_sim_character SET status='active' WHERE guid={}", guidLow);
    PlayerbotsDatabase.DirectExecute(
        "UPDATE pb_sim_account SET status='active' WHERE sim_id={}", ch->simId);
    LoadRoster();
    return true;
}

bool SetFocus(std::string const& name, std::string const& focus)
{
    static std::unordered_set<std::string> const allowed =
        {"level", "dungeon", "pvp", "raid", "gold", "explore", "social"};
    if (!allowed.count(focus))
        return false;
    uint32 guidLow = 0;
    SimChar const* ch = FindByName(name, &guidLow);
    if (!ch)
        return false;
    PlayerbotsDatabase.DirectExecute(
        "UPDATE pb_sim_character SET focus='{}' WHERE guid={}", focus, guidLow);
    LoadRoster();
    if (Player* bot = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(guidLow)))
        ApplyFocus(bot, focus);
    return true;
}
} // namespace SimPopulationMgr
