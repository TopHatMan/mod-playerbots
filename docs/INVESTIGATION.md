# Investigation — guild presence / idle bots

**Date:** 2026-09-09
**Modules:** `mod-npc-chat-llm` (exact folder name), `mod-playerbots` (this fork)

Guild force-login lived in the LLM chat module. Login ownership is playerbots.
This pass transplants that scan into playerbots and turns force-presence off
so the calendar (`Sim.*`) can drive who is online.

## 3.1 `mod-npc-chat-llm`

| File | Function | What it does |
|---|---|---|
| `src/mod_npcchat.cpp` | `EnsureGuildBotsOnline` | **Login.** On every real-player `OnPlayerAfterUpdate`, if the player is in a guild, scan `guild_member`+`characters` every `ScanIntervalSec` and call `RandomPlayerbotMgr::AddPlayerBot(guid, 0)` for offline rndbot/addclass members. Cap `MaxLoginsPerScan`. Ordinary human alts skipped. Does **not** logout when the last real member leaves. |
| `src/mod_npcchat.cpp` | `GetGuildPresenceCfg` / `GuildPresenceNextScan` | Config + per-guild cooldown for the login scan. |
| `src/mod_npcchat.cpp` | `OnPlayerCanUseChat(..., Guild*)` → `HandleBotGuild` | **Chat only.** Player-driven guild social replies. Does not login. |
| `src/mod_npcchat.cpp` | `NpcChat.Bot.Social.Guild*` | **Chat only.** Chance/caps for guild speakers. |
| `src/mod_npcchat.cpp` | `NpcChat.Bot.GuildAmbient.*` | **Chat only**, already default 0. Autonomous guild chatter. Not login. |
| `conf/mod_npcchat.conf.dist` | `NpcChat.Bot.GuildPresence.*` | Was default **1** (on). Removed; keys now live on playerbots. |

No logout-block was found. The module pinned bots by repeatedly requesting login while a real guild member was online.

**Outcome:** login scan deleted from npc-chat. Guild *chat* (greetings/replies) stays. If nobody is online, chat is silent — the module must not login a bot to greet.

## 3.2 `mod-playerbots` presence

| Symbol / key | File | What it does |
|---|---|---|
| `AiPlayerbot.AllowGuildBots` | `PlayerbotAIConfig.cpp`, `PlayerbotMgr::AddPlayerBot` | Permit `.bot add` of a **named** guild alt. **Kept = 1.** |
| `AiPlayerbot.AllowPlayerBots` | (not present under that name) | N/A. |
| `AiPlayerbot.BotAutologin` | conf + `PlayerbotAIConfig` | Account alts on player login. Unrelated to guild roster scan. Left alone. |
| `AiPlayerbot.GuildBotMinOnline` | **does not exist** | Spec name. Real implementation was npc-chat `GuildPresence`. |
| `AiPlayerbot.MaxBotsInRealGuild` | **does not exist** | — |
| `RandomPlayerbotMgr::EnsureGuildBotsOnline` | **did not exist** | Name lived in npc-chat. Now `GuildPresenceMgr::EnsureGuildBotsOnline`. |
| `AiPlayerbot.BotActiveAlone` | `PlayerbotAI::AllowActive` | % of bots that run full AI alone. |
| `AiPlayerbot.BotActiveAloneForceWhenInGuild` | `PlayerbotAI.cpp` ~4714 | **Always active** if `IsInRealGuild()`. Default was **1**. Default now **0**. Also suppressed when `Sim.DisableGuildForcePresence = 1`. |
| `BotActiveAloneForceWhenInZone` / `IsFriend` | `PlayerbotAI::AllowActive` | Unrelated force-active rules. Left alone. |
| `PlayerbotAI::IsInRealGuild` | `PlayerbotAI.cpp` | Cache: guild has a real (non-rndbot) leader/member. Used by active-alone and level-bracket ignore. |
| `PlayerbotMgr` same-guild login | `PlayerbotHolder::AddPlayerBot` | Allows a real player to `.bot add` a **named** same-guild character. Not a roster scan. **Kept.** |
| `DisabledWithoutRealPlayer` | `RandomPlayerbotMgr::UpdateAIInternal` | Stock random mgr waits for a real player. Separate from guild presence. |
| `OrganicGuildPopulation` | `PlayerbotCommandScript.cpp` | **Manual** `.playerbots raidtime` / `.unraidtime`. Queues max-level rndbot guild members. Not automatic. Left as a GM tool. |

`GuildBotMinOnline` / `EnsureGuildBotsOnline` from the public wiki are **not** in this tree. The always-online guild bots came from npc-chat `GuildPresence.Enable = 1` plus `BotActiveAloneForceWhenInGuild = 1` (active but idle in cities).

**Outcome:**

```
AiPlayerbot.GuildPresence.Enable = 0
AiPlayerbot.BotActiveAloneForceWhenInGuild = 0
Sim.DisableGuildForcePresence = 1
```

Named `.bot add SpecificGuildAlt` still works (`AllowGuildBots = 1`).

## 3.3 Why bots do not level (code, not a live sample)

Cannot attach to a running worldserver from this pass. Code paths that produce statues:

1. **Type / master.** Guild alts added with a master wait in follow/stay. GuildPresence used `masterAccountId = 0`, so those logins are rndbots — but they still sit in capitals on `rpg`.
2. **`BotActiveAloneForceWhenInGuild = 1`** keeps them in the active-alone pool without forcing grind/quest, so city RPG continues.
3. **`RandomBotFixedLevel` / `DisableRandomLevels` / `RandomBotXPRate`.** Dist defaults: fixed-level **0**, disable-random-levels **0**, XP rate **1.0**. Live `playerbots.conf` may differ — `.sim debug` prints these.
4. Progression modules can still block XP; if `focus=level` cannot ding after grind is forced, stop and report.
5. Strategy stuck on `rpg` / stay / banker teleport (`ProbTeleToBankers`).
6. LLM guild chat does not hold movement; it only talks. Login was the problem.
7. Random mgr AI init: `AddPlayerBot` still goes through the normal login callback.

`.sim debug Name` is the live probe for master, strategies, zone, level, XP flags, idle_sec.

## Transplant

`NpcChat.Bot.GuildPresence.*` → `AiPlayerbot.GuildPresence.*` in `src/Mgr/Guild/GuildPresenceMgr.*`, hooked from `PlayerbotsPlayerScript::OnPlayerAfterUpdate`. Default **off**.
