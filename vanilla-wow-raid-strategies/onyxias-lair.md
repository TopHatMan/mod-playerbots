# ONYXIA'S LAIR — 40-MAN RAID

Raid-wide notes:
- Onyxia Scale Cloak NOT required here (that is BWL/Nefarian), but fire resist helps in P2.
- [WARRIOR] tanks must be able to stance-dance Berserker Rage; Alliance bring [PRIEST] Fear Ward, Horde bring [SHAMAN] Tremor Totem.

---

### boss: Onyxia
- raid: Onyxia's Lair
- raid_size: 40
- boss_order: 1
- optional: no
- encounter_type: gimmick
- composition: 1 [MT] + 1-2 off-tanks; 10-12 healers; at least 1 fear counter per tank group
- key_abilities:
  - Flame Breath — frontal cone fire damage (never stand in front)
  - Cleave — frontal
  - Tail Sweep — rear knockback; can throw players into the whelp caves (spawns whelps)
  - Wing Buffet — frontal knockback that reduces tank threat
  - Fireball (P2) — heavy fireball on random players while airborne
  - Deep Breath (P2) — telegraphed: "Onyxia takes in a deep breath..." then a massive flame line across a random section of the room; lethal if not avoided
  - Bellowing Roar (P3) — raid-wide fear every ~30s; feared players take damage from erupting lava cracks
  - Whelps (P2 and cave accidents) — waves of Onyxian Whelps from both caves
- phases:
  - P1 (100-65%): ground; tank-and-spank, slow controlled dps
  - P2 (65-40%): airborne; whelps + fireballs + Deep Breath
  - P3 (40-0%): ground; fear phase with lava cracks; burn
- positioning:
  - P1/P3: [MT] tanks Onyxia against the north wall, head to the wall; [MELEE] at max melee range on her SIDES (between front-leg and hind-leg line — safe from breath and tail); [RANGED]/[HEALER] spread loosely at range, never behind the tail
  - P2: [ALL] spread 8+ yards; [MT]/[MELEE] group whelps for AoE; track Deep Breath direction
- tank_rules:
  - [MT] position boss to the wall at pull; after every Wing Buffet expect threat loss — taunt and rebuild
  - [WARRIOR] P3: Berserker Rage every Bellowing Roar (or rely on Fear Ward/Tremor)
  - [OT] P2: pick up whelps, drag to AoE point
- healer_rules:
  - P2: heavy raid healing on fireball targets
  - P3: feared healers are the main risk; position so fear paths do not cross lava cracks; top the tank immediately after each fear
- dps_rules:
  - P1: slow, sustained dps; never pass the tank on threat (no taunt recovery if you pull)
  - P2: [MELEE] AoE whelps; [RANGED] nuke Onyxia; [ALL] IF "Onyxia takes in a deep breath" THEN move perpendicular out of the flame line (run to the sides of the room, away from her facing line)
  - P3: resume positions; burn; watch threat after each fear
- movement_rules:
  - IF Deep Breath emote THEN move to the side walls immediately — never run along her facing axis
  - IF feared toward lava cracks THEN strafe on recovery; healers pre-shield before Roar
  - NEVER stand near the whelp caves (Tail Sweep into a cave = extra whelp wave)
- threat_rules: Wing Buffet reduces tank threat; P3 fears scramble threat; dps throttle after each event
- interrupt_priority: none
- dispel_priority: none (Engulfing Flames dot in P2: heal through)
- cc_assignments: none
- kill_order: Onyxia (P2 whelps die to AoE as they spawn)
- enrage_timer: none
- wipe_risks: Deep Breath hitting a stacked raid; tank feared during P3 with no stance-dance; tail-swipe whelp floods
- bot_behavior_notes:
  - bots need a Deep Breath reaction: on emote, path to nearest side wall, hold 6s
  - melee bots: positional constraint "side arc only" (roughly 90-270 degrees off head)
  - P3: fear-dispel/stance routines fire on Bellowing Roar cast
