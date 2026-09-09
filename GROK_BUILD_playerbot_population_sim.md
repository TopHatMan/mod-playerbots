# Grok Build Spec — Playerbot Population Simulator + Guild Presence Removal

**Date:** 2026-09-09  
**Audience:** Grok Build (implement against the live AzerothCore Playerbot tree)  
**Owner intent:** Replace random bot login/brackets with pre-planned “real player” schedules. Stop guild-always-online. Fix idle bots that sit and never level. Progressive Vanilla only.

This document is the full brief. Implement in the order given. Do not invent TBC/WotLK requirements. Do not rewrite combat AI.

---

## 0. Stack (read this first)

| Piece | Role |
|---|---|
| AzerothCore **Playerbot branch** | Core |
| `mod-playerbots` | Bot AI, random-bot mgr, alt/guild login, active-alone rules |
| `mod-npc-chat-llm` | LLM chat for bots/NPCs. Owner suspects it has a **guild presence / keep-guild-bots-online** feature. Investigate this module by exact folder name in `modules/`. |
| Individual Progression / custom progressive modules | Realm is **Progressive Lich King, currently Vanilla**. Level cap and released dungeons follow the current phase. |
| Dynamic XP module | Rates are “janked high.” Do not fight it. |
| Player-owned guild + altbots | Currently the **only bots reliably online**. They sit idle and do not level. World random bots are mostly offline. |

**Module name the owner used:** `mod-npc-chat-llm`  
If that folder does not exist, search `modules/` for close names (`mod-llm-chatter`, `mod-ollama-chat`, `mod-npc-chat`, `npc-chat-llm`) and treat the match as the LLM chat module. **State which folder you found in the first commit message / report.**

Related public cousins (for grep patterns only, do not assume file names match):

- `mod-llm-chatter` — guild chatter, login greetings; docs claim it does **not** login bots
- `mod-ollama-chat` — guild random chatter when real players are in the guild
- `mod-playerbots` — **this is the real login owner** (`GuildBotMinOnline`, `EnsureGuildBotsOnline`, `BotActiveAloneForceWhenInGuild`, `AllowGuildBots`)

---

## 1. Problems to solve

### P1 — Random login is a lottery
Stock `RandomPlayerbotMgr` uses `MinRandomBots` / `MaxRandomBots`, random in-world time (`MinRandomBotInWorldTime` / `MaxRandomBotInWorldTime`), and **level brackets**. Owner changed brackets; they are considered **bugged / tainted**. Do not depend on brackets.

Vanilla “2500 pop” means a **roster**, not 2500 concurrent full-AI clients. Concurrent cap must be configurable (start **150**).

### P2 — Only guild bots are online
When a real player in the guild is logged in, something keeps guild bots online. Owner believes **`mod-npc-chat-llm` has a guild-presence / keep-guild-bosses-online feature**. That feature must be **found and removed or disabled**. Presence must move to the new calendar.

Also investigate playerbots — it almost certainly force-logins / force-activates guild bots even if the LLM module is innocent.

### P3 — Online bots are statues
Guild/world bots sit in cities. They do not quest, grind, or ding. They look “corrupt.” Likely causes (check all):

1. Treated as **alt/guild bots (Type 2)** waiting for a master instead of solo random-bot strategies
2. `BotActiveAloneForceWhenInGuild` keeps them “active” but in follow/stay/city RPG mode
3. `RandomBotFixedLevel` / `DisableRandomLevels` / XP rate 0 for rndbots
4. Progression / level-cap module blocking XP
5. Strategy stuck on `rpg` / stay / banker teleport
6. LLM module holding them in town for chat
7. Random mgr never initialized AI on that guid

### P4 — Progressive Vanilla
No Outland, no Northrend, no unreleased instances. New characters live under the **current realm level cap**. Raid-logger persona is a dungeon rat until a vanilla raid exists.

---

## 2. Goals

1. **Investigate `mod-npc-chat-llm`** for any code that logs in guild bots, refuses to log them out, or requires N guild bots online for chat. Remove or hard-disable that. Chat may only speak to bots that are already online.
2. **Investigate `mod-playerbots`** guild presence (`EnsureGuildBotsOnline`, `GuildBotMinOnline`, `BotActiveAloneForceWhenInGuild`, `PlayerbotMgr` same-guild auto-login). Neutralize force-presence. Manual `.bot add` of a specific guild alt may remain.
3. **New population layer:** persistent fake players with personas + weekly schedules + focus. Same guid stays the same person. No class/gear reroll on a timer.
4. **While online, most vanilla bots `focus=level`.** Idle-in-capital watchdog kicks them into the world so they actually gain XP.
5. Stock Min/Max random count becomes a **safety concurrent cap only**, not the spawner.

**Non-goals (v1):** new combat AI, LLM rewrite, forced BGs, 2500 concurrent bots, deleting characters on “quit,” Journey/level-gate challenge mode (separate project).

---

## 3. Phase 0 — Investigate (do this before writing tables)

Produce a short `INVESTIGATION.md` next to the code (or a comment block in the PR) listing:

### 3.1 `mod-npc-chat-llm` (exact name; search variants)

Grep the module for:

```
guild
GuildBot
EnsureGuild
MinOnline
keep.*online
presence
LoginBot
AddRandomBot
autologin
always online
roster
```

For each hit, write: file, function, whether it **logins**, **blocks logout**, or **only chats**.

**Required outcome:** If the module logins or pins guild bots, add a master switch:

```
NpcChatLlm.GuildPresence.Enable = 0
```

Default **0**. When 0: no login, no logout-block, no “keep N guild bots warm.” Guild chat features may still run against whoever is already online. If nobody is online, chat is silent.

Do not delete guild *chat* (greetings, replies) unless it is inseparable from login. Prefer a config kill-switch.

### 3.2 `mod-playerbots` presence

Grep and read:

| Symbol / key | Why |
|---|---|
| `AiPlayerbot.AllowGuildBots` | Permit inviting guild alts |
| `AiPlayerbot.AllowPlayerBots` | |
| `AiPlayerbot.BotAutologin` | Account alts on player login |
| `AiPlayerbot.GuildBotMinOnline` | Keep N guild bots online when a real member is in |
| `AiPlayerbot.MaxBotsInRealGuild` | |
| `RandomPlayerbotMgr::EnsureGuildBotsOnline` | Force-login loop (~30s) |
| `AiPlayerbot.BotActiveAlone` | % of bots that run full AI alone |
| `AiPlayerbot.BotActiveAloneForceWhenInGuild` | Wiki: ALWAYS ACTIVE if in a guild with a real player |
| `AiPlayerbot.BotActiveAloneForceWhenInZone` | |
| `AiPlayerbot.BotActiveAloneForceWhenIsFriend` | |
| `PlayerbotAI::IsInRealGuild` | |
| `PlayerbotMgr` same-guild login path | Type 2 alt/guild bots |
| `DisabledWithoutRealPlayer` | |

Wiki active-alone order (applied even if `BotActiveAlone` is low):

1. Not active during startup grace  
2. Always active in BG/instance  
3. Always active in combat (solo)  
4. Always active if zone has a real player  
5. **Always active if in a guild that has a real player**  
6. Always active if real player is master  
7. Always active if grouped with real leader  
8–12. BG queue, LFG, 300yd player, friend list, bot cluster  

**Required outcome for v1:**

```
AiPlayerbot.BotActiveAloneForceWhenInGuild = 0
AiPlayerbot.GuildBotMinOnline = 0
```

No-op `EnsureGuildBotsOnline` when min is 0 (should already return early; verify).  
Do **not** auto-login the whole guild on player login.  
`AllowGuildBots = 1` may stay so a player can still add a **named** guild alt.

### 3.3 Why bots do not level

For one online idle guild bot and one rndbot (if any), log:

- Has master? Grouped with real player?
- Current strategies (combat + noncombat)
- Zone / map
- Level, XP, `RandomBotXPRate`, fixed-level flags
- Idle seconds
- Account type (rndbot account prefix vs player account)
- Guild id / `IsInRealGuild`

If `level` focus cannot gain XP even after forcing quest/grind strategy, **stop and report**. A calendar on statues is useless.

---

## 4. Phase 1 — Calendar on existing rndbot GUIDs

No new accounts yet. Pick existing random-bot characters, stamp persona + schedule, login/logout those GUIDs on a clock.

### 4.1 Design rules

- **Roster** (config) can be 1500–2500 names.  
- **Concurrent cap** default 150, hard safety 300.  
- Stagger logins after restart (`Sim.StartupStaggerSec = 600`).  
- Jitter every window so 400 evening cores do not hit 19:00:00.  
- Do not rerandomize class/gear/name.  
- Do not use level-bracket balancer.  
- Guild/alt bots a real player explicitly added this session are **not** mass-logged-out by the sim.  
- Player-owned altbots do **not** count against the concurrent cap (config `Sim.CountAltbotsAgainstCap = 0`).

### 4.2 Tables (playerbots DB or dedicated schema)

Do not put sim state only in `characters` extras that a rndbot reset wipes. Prefer `pb_sim_*`.

```sql
CREATE TABLE IF NOT EXISTS pb_sim_account (
  sim_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  persona ENUM('morning','afternoon','evening','night','weekend','raid_logger','banker','pvper') NOT NULL,
  status ENUM('active','paused','quit') NOT NULL DEFAULT 'active',
  quit_at DATETIME NULL,
  return_at DATETIME NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (sim_id)
);

CREATE TABLE IF NOT EXISTS pb_sim_character (
  guid INT UNSIGNED NOT NULL,
  sim_id INT UNSIGNED NOT NULL,
  role ENUM('main','alt','bank') NOT NULL DEFAULT 'main',
  focus ENUM('level','dungeon','pvp','raid','gold','explore','social') NOT NULL DEFAULT 'level',
  focus_until DATETIME NULL,
  status ENUM('active','paused','retired') NOT NULL DEFAULT 'active',
  notes VARCHAR(128) NULL,
  PRIMARY KEY (guid),
  KEY (sim_id)
);

CREATE TABLE IF NOT EXISTS pb_sim_schedule (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  sim_id INT UNSIGNED NOT NULL,
  dow_mask TINYINT UNSIGNED NOT NULL,  -- bit0=Sun .. bit6=Sat
  login_tod_min SMALLINT UNSIGNED NOT NULL, -- minutes from midnight, realm time
  duration_min SMALLINT UNSIGNED NOT NULL,
  jitter_min SMALLINT UNSIGNED NOT NULL DEFAULT 25,
  kind ENUM('play','raid','ah','quick') NOT NULL DEFAULT 'play',
  PRIMARY KEY (id),
  KEY (sim_id)
);

CREATE TABLE IF NOT EXISTS pb_sim_session (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  guid INT UNSIGNED NOT NULL,
  planned_login DATETIME NOT NULL,
  planned_logout DATETIME NOT NULL,
  actual_login DATETIME NULL,
  actual_logout DATETIME NULL,
  denied_reason ENUM('cap','restart_stagger','already_online') NULL,
  PRIMARY KEY (id),
  KEY (guid)
);
```

### 4.3 Persona mix when seeding

| persona | % of roster | default week | default focus |
|---|---|---|---|
| evening | 35 | Mon–Thu ~19:00 for 4–6h; Fri later/longer | level |
| morning | 15 | 06:00–10:00, 2–4h weekdays | level or gold |
| afternoon | 10 | 14:00–18:00 | level |
| night | 8 | 23:00–03:00 | level or pvp |
| weekend | 12 | Fri night + Sat/Sun long | level |
| raid_logger | 12 | raid_start-40m to raid_end+15m; optional midweek 90m | dungeon until a vanilla raid is live |
| banker | 5 | 1–2 short city sessions / day | gold |
| pvper | 3 | evenings | pvp |

Vanilla seed level stamp (under current cap, **not** brackets):

- 40% levels 1–19  
- 30% 20–39  
- 20% 40–49  
- 10% at or near cap (50–60 only if cap allows)

They must be allowed to **gain XP after seed**. Document that these playerbots.conf flags fight the feature and should be off for sim characters:

- `AiPlayerbot.RandomBotFixedLevel = 0`
- `AiPlayerbot.DisableRandomLevels = 0` (if present)
- Do not leave rndbot XP rate at 0

### 4.4 Manager tick (every 15–30s)

1. Realm now + concurrent cap.  
2. `wanted[]` = active sim characters whose window (login + jitter → login + duration) contains now.  
3. Priority: `raid` > `play` > `ah` > `quick`.  
4. Online but not wanted → queue logout (prefer 30s grace if in combat / instance).  
5. Wanted but offline → queue login until cap. Overflow writes `denied_reason=cap`.  
6. After worldserver start, spread first wave over `Sim.StartupStaggerSec`.  
7. After login, apply focus → existing playerbot strategies.  
8. If `Sim.DisableStockRandomMgr = 1`, stock Min/MaxRandomBots must **not** add/remove bots. Those keys become unused spawners.

Login must use the existing specific-GUID login path (whatever `LoginBot` / `AddRandomBot(guid)` equivalent exists). Do not ask the count lottery for “any bot.”

### 4.5 Focus → existing AI (do not write a new brain)

| focus | behavior |
|---|---|
| level | quest + grind in current vanilla band; accept dungeon groups; **leave cities** if idle > `Sim.IdleKickSec` |
| dungeon | live near a spawned vanilla instance stone / LFG if already enabled |
| pvp | world PvP; BG only if `RandomBotJoinBG` is already on; never required |
| raid | city/repair; move to raid only if that vanilla raid exists |
| gold | vendor / AH / gather |
| explore | travel / flight paths |
| social | inn / capital, low movement |

**Idle watchdog (required):** if `focus=level` and bot is in a capital or inn for > 120s and not in a group/queue/repair with a real master, force quest/grind and send them toward a vanilla zone appropriate for their level.

### 4.6 Config (`sim_population.conf` or a section in playerbots.conf)

```
Sim.Enable = 1
Sim.RosterTarget = 2000
Sim.ConcurrentCap = 150
Sim.StartupStaggerSec = 600
Sim.TickSec = 20
Sim.IdleKickSec = 120
Sim.CountAltbotsAgainstCap = 0
Sim.DefaultFocus = level
Sim.AllowChurn = 0
Sim.RealmMaxLevel = 60
Sim.DisableStockRandomMgr = 1
Sim.DisableGuildForcePresence = 1
```

When `Sim.DisableGuildForcePresence = 1`, enforce the playerbots / npc-chat-llm kill switches from Phase 0 even if someone left old keys on.

---

## 5. Phase 2 — Seed / generate

Fill roster to `Sim.RosterTarget` from existing rndbot characters first. Create new rndbot accounts only if the pool is short. New mains only in **1–currentCap**. No 61–80. No TBC races/zones if those areas are not in the vanilla phase.

---

## 6. Phase 3 — Churn (optional, off by default)

Weekly job when `Sim.AllowChurn = 1`:

- 1–2% of actives: `status=quit`, `return_at = now + 14–90 days`. Stop scheduling. **Do not delete the character.**
- Due returns get a new schedule; maybe an alt.
- New blood: N new mains in the current cap band.

---

## 7. Commands (GM)

```
.sim status          roster / active / online / cap / by persona / by focus
.sim clock           next 10 planned logins
.sim debug Name      persona, focus, window, strategy, zone, level, xp flags, idle_sec, master Y/N, login reason
.sim pause Name
.sim resume Name
.sim seed N
.sim focus Name level
.sim guildpresence   print whether npc-chat-llm + playerbots force-presence is on
```

`.sim debug` is mandatory for the idle-bot problem.

---

## 8. Acceptance tests

### Guild presence

1. Real player in a large bot guild logs in. **Zero** extra guild bots log in solely because of guild membership.  
2. `GuildBotMinOnline = 0` and `BotActiveAloneForceWhenInGuild = 0`.  
3. `mod-npc-chat-llm` guild presence / keep-online is off. Chat still works if a guild bot happens to be online. If none are online, chat is silent — module must **not** login a bot to greet.  
4. `.bot add SpecificGuildAlt` still works (`AllowGuildBots`).

### Calendar / world

5. After restart, online count climbs over ~10 minutes, not 10 seconds.  
6. 09:00 realm time is clearly thinner than 20:30.  
7. Same named hunter appears two consecutive evenings (stable guid).  
8. A `focus=level` bot at ~12 is **not** still in Stormwind/Org 10 minutes later; XP increased.  
9. No bot created above current progressive cap.  
10. Guild/alt bots a human added this session are not mass-logged-out.  
11. `Sim.Enable = 0` returns spawn control to stock random mgr without crash.

### Investigation deliverable

12. `INVESTIGATION.md` lists every guild-presence hit in `mod-npc-chat-llm` and `mod-playerbots` with file:function and login-vs-chat.

---

## 9. Suggested layout

Prefer a **sidecar module** `mod-sim-population` so upstream `mod-playerbots` can still update.

```
modules/mod-sim-population/
  conf/sim_population.conf.dist
  src/SimPopulationMgr.cpp
  src/SimPopulationMgr.h
  src/SimCommands.cpp
  data/sql/...
  INVESTIGATION.md
```

It may call playerbots login APIs. It must not fork the entire playerbots repo unless a hook is impossible.

If a tiny patch inside `mod-playerbots` is required (early return in `EnsureGuildBotsOnline`, honor `BotActiveAloneForceWhenInGuild = 0`), keep that patch small and commented `SIM-POPULATION`.

---

## 10. Implementation order (do not skip)

1. Grep + `INVESTIGATION.md` for `mod-npc-chat-llm` and playerbots guild presence.  
2. Disable force-presence (conf + early returns).  
3. Confirm a lone bot can gain XP with quest/grind forced.  
4. Tables + seed from existing GUIDs + tick login/logout + cap + stagger.  
5. Idle watchdog.  
6. Commands.  
7. Only then seed-to-target and optional churn.

---

## 11. Out of scope (do not implement in this pass)

Journey / Gate Points / half-XP challenge mode (Hogger + home-zone explore at 10, dungeons required from 20, PvP optional HKs). That is a different module. Do not mix it into the population mgr.

---

## 12. One-paragraph summary

On a vanilla progressive AzerothCore Playerbot realm, stock random-bot count and level brackets are treated as broken. Guild bots stay online and idle because something in `mod-npc-chat-llm` and/or `mod-playerbots` (`EnsureGuildBotsOnline`, `GuildBotMinOnline`, `BotActiveAloneForceWhenInGuild`) treats “in my guild” as “always logged in, always active, wait for master.” Find that, turn it off. Chat modules must not login bots. Then drive world presence from a persistent persona + weekly calendar with a ~150 concurrent cap, and force `focus=level` so bots leave capitals and actually ding. Same character stays the same person. No TBC content.
