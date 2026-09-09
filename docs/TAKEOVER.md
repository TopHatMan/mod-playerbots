# mod-playerbots takeover plan

Living document for the `tophatman/ashbringer-exp` takeover of
`mod-playerbots` as the hub other AzerothCore bot mods call into.

Realm: AzerothCore 3.3.5a, Expansion 0 (vanilla), random-bot cap 60.
Compile from `Z:\ac\build` with ninja. Origin `liyunfan1223/mod-playerbots`
is read-only for this fork; commits stay local on `tophatman/ashbringer-exp`.

Ignore `mod-playerbots-characters`. Do not enable `mod-playerbot-world-pvp`.

---

## Why this exists

Playerbots is the AI. Dungeon-sim, pvp-life, artisans, inventory-master,
quest-helper, npc-chat-llm, ah-bot-plus, and the rest are guests. Guests
were copying headers, reserving bots in private sets, teleporting them
around, and rolling their own chat. That fights the living-world 60s feel.

Takeover means:

1. One reservation bus.
2. Bots talk (group + world), instead of silently teleporting.
3. Gear split stays: personal loot with the player, dungeon-sim without.
4. Other AIs can keep building from this file.

---

## Locked decisions

### World-PvP stays off

`mod-playerbot-world-pvp` is disabled. `mod-playerbots-pvp-life` is the PvP
director (hotspots, duels, faction campaigns, bot PvP chat). Do not run both.
If world-pvp is re-enabled by accident, its conf default is `Enable = 0`.

### Point 2 — make them talk

The old “stop teleporting / make them talk” item is talk-first:

- Bots grouped with a real player speak in **party/raid**.
- Nearby ungrouped bots **say/yell** in the world.
- Combat callouts (oom, low health, aggro, aoe, loot) use the `say`
  strategy, routed through `BotTalk`.
- Random teleports must not yank a reserved bot.
- Do not invent a second chat module. `BroadcastHelper`, `SayAction`,
  `PlayerbotTextMgr`, and `ChatReplyAction` already exist; `BotTalk` is the
  first-class entry other mods should call.

### Point 3 — gear lanes (working, do not merge)

Personal loot is working well. Keep it.

| Lane | Condition | Gear source | Owner |
| --- | --- | --- | --- |
| **Personal loot** | Prefixed support bots in a **real player's group**, in range of the kill | Independent personal roll. Never touches corpse loot, group roll, or master loot. | `mod-playerbot-dungeon-sim` `AwardGroupPersonalLoot` |
| **Dungeon-sim** | Autonomous bot party, **no real player** in the group | Dungeon-clear completion awards / sim loot | `mod-playerbot-dungeon-sim` autonomous run |
| Factory | New / addclass / maintenance | `PlayerbotFactory` BiS tables | playerbots factory only |

Helper: `BotGearLane::LaneFor(bot)`.

```
if (BotGearLane::IsGroupedWithRealPlayer(bot))
    // personal loot lane — dungeon-sim companion loot
else
    // dungeon-sim autonomous lane (or no bonus gear)
```

Do not award both lanes on the same kill. Dungeon-sim already skips
personal loot unless `HasPlayerAccountParticipant`. Keep that gate.

---

## Hub APIs (landed)

### `BotActivityMgr`

Path: `src/Mgr/Activity/BotActivityMgr.{h,cpp}`

Replaces the header-only `BotActivityRegistry.h` copies in dungeon-sim,
artisans, and world-pvp (those copies were ODR tricks that did not talk to
pvp-life’s City Life reservation, which is not even installed).

```
BotActivityMgr::TryReserve(guidLow, Owner::DungeonSim);
BotActivityMgr::Release(guidLow, Owner::DungeonSim);
BotActivityMgr::IsReserved(guidLow);
BotActivityMgr::GetOwner(guidLow);
```

Owners: `DungeonSim`, `PvPLife`, `Artisans`, `PlayerGroup`, `WorldRpg`,
`Raid`, `External`.

Same owner may refresh. Different owner must fail. Logout releases.
Timed reservations expire on the world tick.

`Owner::PlayerGroup` is claimed automatically for every bot in a group that
contains a real player (online or still listed while logged out). Dungeon-sim
`IsBotFree` already skips grouped/reserved bots; this reservation is the
second lock so pvp-life, artisans, and random teleports cannot take them
either. Config:

```
AiPlayerbot.BotActivity.PlayerGroupReserve = 1
AiPlayerbot.BotActivity.PlayerGroupReconcileSeconds = 15
```

Playerbots respects a reservation:

- `PlayerbotAI::AllowActive` denies grind / rpg / travel / out-of-party.
- `RandomPlayerbotMgr::RandomTeleport` returns immediately.
- Combat, packets, and party activity still run so a dungeon-sim or
  pvp-life bot can actually fight.

### `BotTalk`

Path: `src/Mgr/Text/BotTalk.{h,cpp}`

```
BotTalk::Speak(bot, Channel::Party, "oom");
BotTalk::SpeakLine(bot, Channel::Say, "talk_world_ambient");
BotTalk::Callout(bot, Callout::LowMana);
BotTalk::Tick(bot);   // ambient, rate-limited
```

Config (in `playerbots.conf.dist`):

```
AiPlayerbot.BotTalk.Enable = 1
AiPlayerbot.BotTalk.GroupChance = 18
AiPlayerbot.BotTalk.WorldChance = 8
AiPlayerbot.BotTalk.CooldownMin = 20
AiPlayerbot.BotTalk.CooldownMax = 55
AiPlayerbot.BotTalk.PreferPartyWhenGrouped = 1
```

`SayAction` prefers party/raid when `BotGearLane::IsGroupedWithRealPlayer`.
The `say` strategy is registered in `StrategyContext` and added by default
when talk is enabled. Chat replies still honor `AiPlayerbot.RandomBotTalk`.

Vanilla flavor only. No ICC / arena-queue / DK slang on this realm.

### `BotGearLane`

Path: `src/Mgr/Item/BotGearLane.{h,cpp}`

```
enum class Lane { None, PlayerGroupPersonal, AutonomousSim };
Lane LaneFor(Player* bot);
bool IsGroupedWithRealPlayer(Player* bot);
bool IsAutonomousRandomBot(Player* bot);
```

A real participant is: no playerbot AI, a selfbot, or a bot with a game
client master. Random-account bots in an all-bot group are autonomous.

---

## Module map (what may touch bots)

| Module | Role | Hub duty |
| --- | --- | --- |
| **mod-playerbots** | AI, factory, travel, raids, talk, activity bus | Hub. Everyone else calls this. |
| **mod-playerbot-dungeon-sim** | 5-man via dungeon-clear, offscreen raid sim, autonomous loot, **personal loot for player groups** | Reserve `DungeonSim`. Personal loot only if a real player is in the group. |
| **mod-dungeon-clear** | Actual dungeon AI used by dungeon-sim | No extra reservation. |
| **mod-playerbots-pvp-life** | World PvP director | Reserve `PvPLife`. Use `BotTalk` for event yells. |
| **mod-playerbot-world-pvp** | Old hotspot teleporter | **Disabled.** |
| **mod-playerbots-artisans** | Trade/general profession ads | Reserve `Artisans` (or skip reserved bots). Ads via `BotTalk` later. |
| **mod-bots-talents** | Talent UI/helper | Respect `ExpansionProgression` / `LimitTalentsExpansion`. |
| **mod-bot-inventory-master** | Inventory UI | Do not factory-replace personal-loot gear. |
| **mod-bot-quest-helper** | Quest UI | Fine. |
| **mod-multibot-bridge** | Multi-bot client bridge | Fine. |
| **mod-rndbot-buffs** | Extra buffs | Fine. |
| **mod-npc-chat-llm** | LLM NPC / optional PLAYERBOT_CHAT | If used, it is extra flavor on top of `BotTalk`, not a replacement. |
| **mod-ah-bot-plus** | Auction house | Never use bot character GUIDs as AHBot accounts. |
| **mod-open-world-party-scaling** | Open-world scaling | Fine. |
| **mod-world-boss-scale** | World-boss HP | Fine. |
| **mod-individual-progression** | Era gating | Keep `^RNDBOT.*` excluded unless we later want bots on IP. |
| **mod-azeroth-eras** | WIP attunement | Bot attunement later. |
| **mod-vanilla-naxxramas** | Vanilla Naxx content | Raid AI still lives in playerbots. |

CMake pitfall: `-DMOD_PLAYERBOTS` matches **any** folder whose name contains
`mod-playerbots`. Keep guest module names if they need the define, but do
not put playerbots core code in those folders.

---

## Remaining work (other AIs: pick a slice)

### Talk (next)

- [x] `BotTalk` API + config + ambient tick
- [x] Register and enable `say` strategy
- [x] Party/raid when grouped with the player
- [ ] More vanilla `ai_playerbot_texts` lines (pull, wipe, patrol, vendor)
- [ ] Route artisans ads and pvp-life yells through `BotTalk`
- [ ] Optional: greet nearby players without `EnableGreet` spam
- [ ] Stop treating “teleport to content” as the default RPG verb; walk, then talk

### Activity bus

- [x] Canonical `BotActivityMgr`
- [x] Honor reservations in `AllowActive` + `RandomTeleport`
- [x] Dungeon-sim / artisans / pvp-life call the hub
- [x] Player-group reservation so dungeon-sim cannot snatch a bot already grouped with you
- [ ] World-pvp copy can die (module is off)
- [ ] `.playerbots activity` GM dump (guid, owner, age)

### Gear

- [x] Document lanes; `BotGearLane` helper
- [ ] Dungeon-sim `AwardGroupPersonalLoot` calls `BotGearLane` instead of duplicating the real-player test (behavior must stay identical)
- [ ] Factory must not re-BiS a bot that just received personal loot
- [ ] Alt / addclass bots in a player group stay on the personal-loot lane

### Raid common sense (20/40)

- [x] Generic `"raid"` strategy on groups of 5+: warlocks soulstone healers (prefer real players) and announce in party/raid; druids claim a brez target and rebirth them. Functional RaidCall is not gated by BotTalk.Enable (LLM ignores bot speakers).
- [ ] Per-boss ZG / AQ / leftover MC

### Raid AI (vanilla)

Done: Core Hounds, Majordomo, Magmadar, Gehennas, Onyxia, BWL (Razorgore
through Nefarian), world bosses (Azuregos, Kazzak, emerald dragons).

Still open:

- [ ] Zul’Gurub (per-boss, `vanilla-wow-raid-strategies/zulgurub.md`)
- [ ] AQ20
- [ ] AQ40
- [ ] MC leftovers: Lucifron, Garr, Shazzrah, Sulfuron, Ragnaros
- [ ] Leave Core Hounds alone

### Class / spec

- [x] `ExpansionProgression` + 20/40/60 premade links + vanilla-first rotations
- [ ] Live `playerbots.conf` must copy new `PremadeSpecLink` 20/40 lines from dist
- [ ] Factory refresh to unlearn illegal spells on existing 60s
- [ ] Per-class pass: hunters ammo/pet, paladin seals/judgement, warrior stance dancing

### Living world (later, separate commits)

- [x] Full world at server start: SmartPopulation OFF, Min 1000 / Max 3600 log in before anyone connects
- [x] Guild force-login transplanted out of `mod-npc-chat-llm` into `GuildPresenceMgr` (default off)
- [x] Population calendar `Sim.*` (`SimPopulationMgr`) — roster + weekly windows + idle kick
- [ ] AH: bots as customers, never as AHBot GUIDs
- [ ] One open-world director (RPG + grind + pvp-life + dungeon-sim) on top of `BotActivityMgr`
- [ ] Performance: `BotActiveAlone`, visibility, less cross-map teleport

### PvP combat AI

pvp-life can move bodies. Playerbots still needs to **win** outdoor PvP:
target enemy players, use CC, drink, not grind the nearest boar mid-fight.
That is a later slice (`AttackEnemyPlayersStrategy` / `pvp` strat).

---

## How to implement a slice

1. Read this file and `AGENTS.md`.
2. Do not edit Core Hounds. Do not enable world-pvp. Do not merge gear lanes.
3. Call hub APIs; do not copy them.
4. Keep vanilla constraints (`ExpansionProgression`).
5. One concern per commit. Do not stage the dirty smart-pop/AH files unless
   that **is** the commit.
6. For raid bosses: imperative if/then, yards, roles, kill order, then
   `bot_behavior_notes` in `vanilla-wow-raid-strategies/SCHEMA.md`.

---

## Ideas parked (not started)

- Bot mail / auction as customers of `mod-ah-bot-plus`.
- Guild chat as a social layer (already partly in `BroadcastHelper`).
- First-class 5-man strategies inside playerbots so dungeon-sim is a
  dispatcher, not an AI.
- Era tables for loot/spells/reps instead of WotLK DB + denylist.
- `mod-npc-chat-llm` only for named NPCs; bots keep `BotTalk` so they do
  not all sound like the same model.
- World-boss outdoor `worldboss` strat is zone-attached; keep it on combat
  re-apply.
- CMake: consider renaming guest folders that match `mod-playerbots*` if
  the `MOD_PLAYERBOTS` define causes grief.

---

## File index for this takeover

| File | Purpose |
| --- | --- |
| `AGENTS.md` | Short contract for other AIs |
| `docs/TAKEOVER.md` | This plan |
| `src/Mgr/Activity/BotActivityMgr.*` | Reservation bus |
| `src/Mgr/Text/BotTalk.*` | First-class speech |
| `src/Mgr/Item/BotGearLane.*` | Personal vs autonomous gear |
| `data/sql/playerbots/updates/2026_09_08_00_ai_playerbot_talk_texts.sql` | Extra talk lines |
| `conf/playerbots.conf.dist` | `AiPlayerbot.BotTalk.*`, `AiPlayerbot.BotActivity.PlayerGroup*`, gear-lane comments |
