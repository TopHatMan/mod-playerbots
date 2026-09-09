# Vanilla WoW Raid Strategy Compendium (Patch 1.12 / Classic Era)

## PURPOSE
Machine-parseable raid strategy data for every 20-man and 40-man raid encounter in
vanilla World of Warcraft (patch 1.12 ruleset, which is also the Classic Era ruleset),
plus the six outdoor world bosses. Written as behavioral rules suitable for driving
playerbots: imperative directives, explicit if/then responses, numeric distances,
prioritized target/interrupt/dispel lists.

## FILE INDEX
- `molten-core.md`        — 40-man, 10 bosses
- `onyxias-lair.md`       — 40-man, 1 boss
- `blackwing-lair.md`     — 40-man, 8 bosses
- `zulgurub.md`           — 20-man, 10 encounters (8 core + 2 optional)
- `ruins-of-ahnqiraj.md`  — 20-man, 6 bosses
- `temple-of-ahnqiraj.md` — 40-man, 9 bosses
- `naxxramas.md`          — 40-man, 15 bosses
- `world-bosses.md`       — 6 outdoor bosses (40-man tuning)

## ENTRY SCHEMA
Every boss entry uses `### boss: <Name>` followed by these fields in this order:

- `raid`: raid name
- `raid_size`: 20 | 40
- `boss_order`: typical kill order index within the raid
- `optional`: yes | no
- `encounter_type`: tank-and-spank | add-control | council | dps-race | movement | gimmick | council+swap
- `composition`: tanks/healers/dps counts, required classes, required resistance or gear
- `key_abilities`: list of `name — effect` (concise, factual)
- `phases`: `trigger: behavior change`
- `positioning`: where tanks/melee/ranged/healers stand, in yards
- `tank_rules` / `healer_rules` / `dps_rules`: imperative directives per role
- `movement_rules`: explicit if/then movement responses
- `threat_rules`: aggro-specific directives (drops, wipes, taunt-immunity)
- `interrupt_priority`: ordered list of spells that must be kicked
- `dispel_priority`: ordered list of `debuff — who dispels — whom`
- `cc_assignments`: what gets polymorphed/banished/feared/etc.
- `kill_order`: ordered target list
- `enrage_timer`: hard/soft enrage if any
- `wipe_risks`: top failure conditions
- `bot_behavior_notes`: simplifications and explicit bot directives (stack points, stop-attack triggers, channel behavior)

## CONVENTIONS
- All distances are in yards. All times in seconds unless noted.
- Role tags: [ALL] [TANK] [OT] [HEALER] [DPS] [MELEE] [RANGED] [CASTER] plus class tags
  ([PRIEST] [MAGE] [WARLOCK] [HUNTER] [ROGUE] [DRUID] [PALADIN] [SHAMAN] [WARRIOR]).
- `IF <condition> THEN <action>` is the standard reactive rule form.
- "LoS" = break line of sight. "Spread N" = maintain N yards from every other raid member.
- Faction note: Alliance = paladin blessings, Horde = shaman totems; both are listed where relevant.
- Strategies assume competent execution, no world buffs required unless stated, and the
  standard Classic-era (2019+) understanding of mechanics.
