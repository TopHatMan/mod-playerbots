# AGENTS.md

mod-playerbots is an AzerothCore module that adds AI-controlled player bots to a
World of Warcraft 3.3.5a (WotLK) server. It is C++20, built as part of the
AzerothCore CMake tree, and backed by MySQL.

Design philosophy: **stability, performance, and predictability over behavioral
realism.** Every action, trigger, and value runs per bot, per tick, across
thousands of bots. Default behaviour must be cheap; expensive behaviour is opt-in.

The module is its own repository, checked out at `modules/mod-playerbots/` inside
an AzerothCore fork. The fork's `AGENTS.md` and `.agents/docs/` apply as a base;
for paths under this module, this file and `.agents/docs/` win where they differ.
The fork's SQL placement, `--author` attribution, and e2e rules do not apply here.

## Agent rules

- **Do not configure or build unless explicitly asked.** Builds are slow and rarely
  needed for code changes. Verify without a build instead: run
  `python apps/codestyle/codestyle-cpp.py` from the module root (the CI check),
  `clang-format --dry-run --Werror <file>` on touched files, and grep the creator
  tables for every new action, trigger, or strategy name.
- Only build inside the custom core,
  [mod-playerbots/azerothcore-wotlk](https://github.com/mod-playerbots/azerothcore-wotlk); upstream
  AzerothCore will not compile this module. Branches pair up: module `test-staging` builds against core `test-staging`, module `master` against core `Playerbot`.
- PRs target `test-staging`, never `master`; `check_pr_source.yml` enforces it.
- **Never edit SQL under `data/sql/*/base/`, `data/sql/playerbots/create/`, or
  `data/sql/playerbots/archive/`.** Those are snapshots maintainers regenerate. New
  SQL is a dated file under the matching `data/sql/<db>/updates/` directory.
- Formatting follows `.clang-format` and `.editorconfig`: UTF-8, LF, 4-space indent
  (tabs forbidden), max 120 columns, trailing newline, no trailing whitespace.
- Credit upstream authors: ported GPLv2 code keeps its notice and gets a
  `Co-authored-by:` trailer per original author.
- Planning docs go in `.agents/plans/<task-slug>/` (gitignored), named
  `<task-slug>.<TYPE>.md` (`PLAN`, `REQUIREMENTS`, `ANALYSIS`, …).

## Mandatory reading per task

Read the matching doc(s) BEFORE starting the task:

- Writing or modifying C++ → `.agents/docs/cpp-guidelines.md` and
  `.agents/docs/cpp-playerbots.md`
- Touching a strategy, action, trigger, value, multiplier, or `AiFactory.cpp` →
  also `.agents/docs/ai-engine.md`
- Creating or modifying SQL, or adding bot chat text → `.agents/docs/sql-guidelines.md`
- Reviewing a changeset or PR → `.agents/docs/code-review.md`
- Self-reviewing, or opening or updating a PR → also `.agents/docs/self-review-rules.md`
- Writing a commit, or opening or updating a PR → `.agents/docs/pull-requests.md`;
  the `pr-title` skill proposes titles, the body is human-authored
- Capturing a lesson or adding/updating agent docs → `.agents/docs/README.md`

## Repository layout

- `src/Bot/` — bot runtime: `PlayerbotAI` (per-bot tick), `PlayerbotMgr`,
  `RandomPlayerbotMgr`, `Engine/` (Strategy-Trigger-Action engine, `Multiplier`,
  action queue), `Factory/` (bot creation and gearing), `Cmd/`, `Debug/`.
- `src/Ai/Base/` — shared strategies, actions, triggers, values used by every class.
- `src/Ai/Class/<Class>/` — per-class rotations, one directory per class.
- `src/Ai/Dungeon/`, `src/Ai/Raid/` — encounter-specific strategies and multipliers.
- `src/Ai/World/` — RPG, questing, travel, and other out-of-combat behaviour.
- `src/Mgr/` — managers: guild, item, movement, security, talent, text, travel.
- `src/Script/` — AzerothCore script hooks, including `WorldThr/` (world-thread
  processor for cross-thread operations).
- `src/Db/` — the module's own database pool.
- `data/sql/playerbots/` — the `acore_playerbots` database; `data/sql/characters/`
  and `data/sql/world/` — updates the module applies to the core databases.
- `conf/playerbots.conf.dist` — every configuration option, with defaults and
  comments. New options are documented here.
- `apps/codestyle/codestyle-cpp.py` — the codestyle checker CI runs.
- `.github/workflows/` — build matrix (Linux, macOS, Windows), C++ codestyle +
  cppcheck, clang-format (advisory), PR source-branch enforcement.

## Key constraints

- **Triggers must be O(1).** They run every tick for every active strategy; no scans
  of the quest log, inventory, nearby units, or group members without a cached value.
- **Actions gate expensive work** behind `isUseful()` / `isPossible()`.
- **No synchronous database queries on map threads.** Bot AI runs on map threads; a
  query blocks every bot on that map and a crash takes the map down. Factory and
  login paths on the world thread are the exception.
- **Shared code must not branch on strategy names**, and one action does one job.
- **Bot chat is translatable** via `GetBotTextOrDefault` plus a translation SQL update.
- **Expensive or behaviour-changing features are opt-in** via `playerbots.conf.dist`.
  Shared-code changes that ship alongside a gated feature are reviewed as if the
  feature were off, because for most servers it will be.

## Persisting lessons

When a user correction reveals a lesson that generalizes, offer to persist it into
these docs (placement per `.agents/docs/README.md`).

## Branch contract: tophatman/ashbringer-exp

This section is the working agreement for humans and other AIs editing
`mod-playerbots` on branch `tophatman/ashbringer-exp`. Where it disagrees with
the WotLK defaults above, this branch wins.

Read `docs/TAKEOVER.md` before changing living-world, talk, gear, PvP, or
activity-reservation code. That document is the plan; this file is the
do-not-break list.

## This module is the hub

Sibling mods under `Z:\ac\modules\` must **call into** playerbots. They must
not grow a second copy of:

- bot activity reservation (`BotActivityMgr`)
- bot speech (`BotTalk`)
- “is this bot grouped with a real player?” (`BotGearLane`)

Canonical headers:

- `src/Mgr/Activity/BotActivityMgr.h`
- `src/Mgr/Text/BotTalk.h`
- `src/Mgr/Item/BotGearLane.h`

`Playerbots.h` includes all three.

## Realm lock

- Expansion is Classic. Cap is 60. `RandomBotMaxLevel` is 60.
- Vanilla 1–60 talent/spell gate lives in `src/Util/ExpansionProgression.*`.
- Do not teach or rotate TBC/WotLK spells while Expansion < WotLK.
- Do not dump leftover talent points into WotLK rows.

## Gear lanes (do not merge these)

| Lane | Who | Where gear comes from |
| --- | --- | --- |
| **Personal loot** | Bots grouped **with a real player** | `mod-playerbot-dungeon-sim` companion loot. Keep it. It works. |
| **Dungeon-sim** | Bots running **without** a real player | Autonomous dungeon-sim / dungeon-clear completion awards. |
| Factory BiS | Fresh/addclass bots | `PlayerbotFactory` only. Not a living-world gear source. |

Personal loot = bots with the player get gear.
Dungeon-sim = bots without the player get gear.

Bots grouped with a real player are reserved as `Owner::PlayerGroup`.
Dungeon-sim / pvp-life / artisans must not snatch them.

## PvP

- `mod-playerbot-world-pvp` is **disabled**. Leave it off.
- `mod-playerbots-pvp-life` is the PvP director. It must reserve bots through
  `BotActivityMgr` with `Owner::PvPLife`.

## Do not mix leftover work

Uncommitted smart-pop / AH / living-world leftovers in:

- `conf/playerbots.conf.dist`
- `src/PlayerbotAIConfig.cpp` / `.h`
- `src/Bot/RandomPlayerbotMgr.cpp`
- RPG sell/travel files

must not be folded into raid, class, talk, or activity commits. Touch them
only for the specific hunk you are landing.

## Raid AI

- Leave Core Hounds (`e8b34dc0`) alone. It is correct.
- Remaining vanilla raid pipeline: ZG, AQ20, AQ40, leftover MC (Lucifron,
  Garr, Shazzrah, Sulfuron, Ragnaros).
- Boss notes live in `vanilla-wow-raid-strategies/` and follow `SCHEMA.md`.

## Talk

Bots must **not** speak unsolicited. `BotTalk` stays off so it does not
trigger `mod-npc-chat-llm`. Player-driven whispers to bots are LLM RP.
Use `role` / `role tank|heal|dps` to set vanilla PvE spec and strategies.

## Style

- Match surrounding playerbots code: file header, include guards
  `PLAYERBOTS_*_H`, no `using namespace` in headers.
- Distances, roles, and kill order in raid code are imperative if/then.
- Do not invent WotLK slang, spells, or gear on a 60 realm.
