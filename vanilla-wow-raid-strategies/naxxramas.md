# NAXXRAMAS — 40-MAN RAID

Raid-wide notes:
- Naxx is tuned tight: full consumables, world buffs recommended for Loatheb/Patchwerk/4HM.
- Frost resist gear for Sapphiron (casters/healers ~100-150, tanks mix); nature resist for Loatheb; shadow resist pots for Loatheb/Four Horsemen.
- [PRIEST] Mind Control is required on Razuvious and Faerlina.

## SPIDER WING

---

### boss: Anub'Rekhan
- raid: Naxxramas
- raid_size: 40
- boss_order: 1
- optional: no
- encounter_type: movement
- composition: 2 tanks, 10-12 healers
- key_abilities:
  - Impale — line of spikes toward a random player: launches and damages everyone in the line (spread)
  - Locust Swarm — every ~90s: AoE silence + heavy dot in 30 yards; boss slows himself slightly — the raid KITES him around the room's outer ledge during swarm
  - Crypt Guard adds — spawn periodically + on his death-of-add events; cleave (off-tank, kill)
  - Corpse Scarabs — spawn from corpses (AoE)
- phases: normal phase / locust-swarm kite phase, alternating
- positioning: [MT] tanks near the slime ledge; [RANGED]/[HEALER] spread 8+ yards in the middle ring; kite path = full lap around the outer ledge
- tank_rules:
  - IF Locust Swarm cast THEN [MT] backpedals along the ledge at max kite speed; [OT] holds adds; do NOT let swarm touch the raid
  - [OT] pick up Crypt Guards instantly
- healer_rules:
  - during swarm: heal the kiting tank from max range; keep the raid topped for Impales
- dps_rules:
  - priority: Crypt Guards > Corpse Scarabs > boss
  - [ALL] stop melee on boss during Locust Swarm (silence/dot range); ranged continue
- movement_rules:
  - swarm phase: entire raid stays on the opposite side of the room from the boss
  - spread vs Impale lines at all times
- threat_rules: no resets; kiting tank keeps threat by DoTs/bleeds during swarm
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: [MAGE] Frost Nova scarabs
- kill_order: Crypt Guards → scarabs → Anub'Rekhan
- enrage_timer: none
- wipe_risks: Locust Swarm clipping the raid; untanked Crypt Guards on healers
- bot_behavior_notes: swarm timer drives a raid-wide reposition routine; tank bot follows ledge waypoint loop

---

### boss: Grand Widow Faerlina
- raid: Naxxramas
- raid_size: 40
- boss_order: 2
- optional: no
- encounter_type: add-control
- composition: 2 tanks; 2 [PRIEST] with Mind Control; nature resist helps
- key_abilities:
  - Frenzy — +150% damage enrage; only removed by sacrificing a Naxxramas Worshipper via Mind Control (Widow's Embrace) — also suppresses Poison Bolt Volley for 30s
  - Poison Bolt Volley — raid-wide nature dot (dispel/NR)
  - Rain of Fire — ground AoE (move)
  - adds: 4 Naxxramas Worshippers (KEEP for MC sacrifices) + 2 Naxxramas Followers (kill)
- phases: frenzy cycle every ~60-80s → worshipper sacrifice
- positioning: [MT] tanks Faerlina at the back of her platform; worshippers held/sheeped nearby but OFF the boss; [RANGED] spread
- tank_rules:
  - [OT] hold the 4 Worshippers (or [MAGE] sheep some) near the boss for quick MC access
- healer_rules:
  - dispel Poison Bolt Volley; watch tank during unsuppressed Frenzy windows
- dps_rules:
  - kill the 2 Followers at pull; DO NOT kill Worshippers
  - [PRIEST] rotation: MC a Worshipper → walk it to Faerlina → sacrifice (Widow's Embrace) the instant she Frenzies
  - kill Faerlina after adds controlled
- movement_rules:
  - IF Rain of Fire under you THEN move
- threat_rules: none
- interrupt_priority: none
- dispel_priority:
  - Poison Bolt Volley (poison) — [DRUID]/[SHAMAN]/[PALADIN]
- cc_assignments: [PRIEST] Mind Control Worshippers for sacrifice rotation; [MAGE] Polymorph backup
- kill_order: Naxxramas Follower x2 → Faerlina (Worshippers sacrificed one per Frenzy, never dps'd)
- enrage_timer: Frenzy is the recurring soft-enrage; missing a sacrifice usually wipes
- wipe_risks: worshipper accidentally killed (no sacrifice = frenzy wipe); late MC sacrifice
- bot_behavior_notes:
  - dps bots: Worshippers blacklisted from targeting
  - priest bots: MC-sacrifice routine on Frenzy emote, sequenced priest 1 → 2 → 3 → 4

---

### boss: Maexxna
- raid: Naxxramas
- raid_size: 40
- boss_order: 3
- optional: no
- encounter_type: tank-and-spank
- composition: 1-2 tanks; heavy tank healing; poison/disease dispels
- key_abilities:
  - Web Spray — every 40s: ENTIRE RAID stunned 8s + damage; tank must be shielded + HoT'd BEFORE each spray
  - Web Wrap — wraps 3 players into cocoons on the walls (destroy cocoons to free them; wrapped players take ticking damage)
  - Necrotic Poison — deadly poison on the tank (reduces healing 90%... dispel INSTANTLY)
  - Enrage — at 30%: massive damage
  - Spider adds — small spiders spawn periodically (kill)
- phases: sub-30% enrage burn
- positioning: [MT] tanks her at the web center facing outward; [RANGED] spread near walls to reach cocoons fast
- tank_rules:
  - [MT] use a defensive cooldown for every Web Spray window (Last Stand/Shield Wall rotation)
- healer_rules:
  - [PRIEST] shield [MT] + HoTs BEFORE every 40s Web Spray (the tank gets zero heals for 8s)
  - dispel Necrotic Poison off the tank instantly (poison)
- dps_rules:
  - [RANGED] kill Web Wrap cocoons within seconds of spawning
  - kill spider adds; burn hard at 30% (enrage) — save cooldowns
- movement_rules:
  - cocooned players: call out; nearest ranged swaps target
- threat_rules: none
- interrupt_priority: none
- dispel_priority:
  - Necrotic Poison (poison) — [DRUID]/[SHAMAN]/[PALADIN] — tank only, INSTANT
- cc_assignments: none
- kill_order: cocoons → spiders → Maexxna
- enrage_timer: 30% soft enrage
- wipe_risks: tank dying inside Web Spray (no shield/HoTs); Necrotic Poison left on tank
- bot_behavior_notes: healer bots: pre-spray routine on 40s timer (shield + HoTs on MT); ranged bots: cocoon = priority target

## PLAGUE WING

---

### boss: Noth the Plaguebringer
- raid: Naxxramas
- raid_size: 40
- boss_order: 4
- optional: no
- encounter_type: add-control
- composition: 1-2 tanks; 3+ decursers ([MAGE]/[DRUID] — mandatory, this fight is a decurse check)
- key_abilities:
  - Curse of the Plaguebringer — curse on ~10 players; if it expires un-decursed it becomes Wrath of the Plaguebringer (massive AoE shadow damage spreading to others) — DECURSE WITHIN ~10 SECONDS OR WIPE
  - Cripple — slows (curse — decurse)
  - Blink — teleports + full threat wipe
  - Skeleton waves — summoned adds each ground phase; bigger waves while he is on the balcony
  - Balcony phase — every ~110s he teleports away for ~70s; adds keep coming
- phases: ground (tank + adds) / balcony (adds only), 3 cycles then he stops returning... (3 balcony trips max, then permanent ground)
- positioning: [MT] tanks center; [RANGED] spread; decursers assigned to raid groups
- tank_rules:
  - IF he Blinks THEN re-taunt immediately; [OT] rounds up skeletons
- healer_rules:
  - heal through blink transitions; watch decurse-focused mages (they'll be busy)
- dps_rules:
  - [MAGE]/[DRUID] DECURSE FIRST, dps second — every curse, every time, all 10 targets
  - AoE skeletons; dps Noth between waves
- movement_rules:
  - none special
- threat_rules: Blink = full threat wipe; hold dps after each blink
- interrupt_priority: none
- dispel_priority:
  - Curse of the Plaguebringer (curse) — [MAGE]/[DRUID] — ALL targets within seconds; absolute top priority
  - Cripple (curse) — after Plaguebringer curses are clear
- cc_assignments: [MAGE] Frost Nova skeleton packs
- kill_order: skeletons → Noth
- enrage_timer: none
- wipe_risks: a single missed Plaguebringer curse chain-wiping the raid
- bot_behavior_notes: decurse bots: this fight sets decurse priority ABOVE all damage and above non-emergency healing; assign fixed raid-group coverage per decurser

---

### boss: Heigan the Unclean
- raid: Naxxramas
- raid_size: 40
- boss_order: 5
- optional: no
- encounter_type: movement
- composition: 1 tank; disease dispel ([PRIEST]/[SHAMAN]/[PALADIN])
- key_abilities:
  - Eruption — the floor erupts in sections in a repeating sequence (1→2→3→4→3→2→1...); the "dance": stand only in the safe section
  - Decrepit Fever — disease: -50% max HP + damage (dispel)
  - P2 (every ~90s): Heigan teleports to his platform and Eruption speeds up — whole raid dances; he also channels mana burn... (P1 melee burn aura near him — [MELEE] watch HP)
- phases: P1 ground (he's tanked on the dance floor, slow eruptions) / P2 platform (fast eruptions, raid dances)
- positioning: dance floor divided in 4 sections; [MT] tanks him in section 1-2 area; raid moves as a group through safe sections
- tank_rules:
  - [MT] dance WITH the pattern while holding him; keep him faced away
- healer_rules:
  - dispel Decrepit Fever fast; heal eruption grazes
- dps_rules:
  - dance first, dps second; [RANGED] dps while moving; [MELEE] mind the P1 proximity burn aura — dip in/out
- movement_rules:
  - follow the section sequence; NEVER lag a section behind; IF unsure THEN move to the NEXT section early
- threat_rules: none
- interrupt_priority: none
- dispel_priority:
  - Decrepit Fever (disease) — [PRIEST]/[SHAMAN]/[PALADIN] — fast
- cc_assignments: none
- kill_order: Heigan
- enrage_timer: none
- wipe_risks: dancers dying to eruptions; fever stacking on lagging players
- bot_behavior_notes: waypoint sequence per phase (P1 slow loop, P2 fast loop); movement priority > all combat actions

---

### boss: Loatheb
- raid: Naxxramas
- raid_size: 40
- boss_order: 6
- optional: no
- encounter_type: dps-race
- composition: 1 tank; EVERY healer on a strict 1-heal-per-minute rotation; nature/shadow consumables (Greater Shadow Protection, healthstones, bandages)
- key_abilities:
  - Corrupted Mind — healers may cast only ONE healing spell per 60 seconds (heal rotation: healer 1 → 2 → 3...)
  - Inevitable Doom — after 2 minutes: stacking raid-wide shadow dot applied every 30s, damage ramps each application (undispellable — use shadow prot pots, healthstones, bandages)
  - Fungal Bloom — Spore adds spawn: killing a Spore gives a 90s debuff to 5 nearby players: +50% crit and abilities generate NO THREAT (rotation: groups take turns getting it)
  - Poison Aura — nature dot on melee (nature resist)
- phases: after 2:00 Inevitable Doom begins; fight is a dps race against doom ramping
- positioning: [MT] tanks Loatheb near a spore spawn point; raid groups rotate to spores; [MELEE] behind
- tank_rules:
  - [MT] defensives as Doom ramps; threat is trivial (spore debuff removes dps threat)
- healer_rules:
  - STRICT rotation: your single heal per minute goes to the tank (assigned order); IF your turn THEN land a max-rank heal, then go back to dps/wanding
  - everyone self-heals: bandage, healthstone, pot on Doom ticks
- dps_rules:
  - kill Spores in the assigned group order (stand next to the spore when it dies to get the buff)
  - with the spore buff: go ALL OUT (no threat); without it: still dps hard
- movement_rules:
  - rotate to spore spawn when your group is called
- threat_rules: spore debuff = zero threat; before your first spore, watch threat
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: none
- kill_order: Spores (by rotation) → Loatheb
- enrage_timer: Inevitable Doom ramp = soft enrage (~7-8 min practical limit)
- wipe_risks: healer rotation collapse (two heals in one window wastes the minute); slow spore rotation; Doom out-racing dps
- bot_behavior_notes:
  - healer bots: heal-cast lockout per bot (60s), strict ordered rotation list
  - dps bots: spore-buff acquisition routine beats boss dps when their group is up

## MILITARY WING

---

### boss: Instructor Razuvious
- raid: Naxxramas
- raid_size: 40
- boss_order: 7
- optional: no
- encounter_type: gimmick
- composition: 2 [PRIEST] with Mind Control (mandatory — no priest, no kill); 1 [MT] for understudy pickup
- key_abilities:
  - Death Knight Understudy x4 — priest Mind Control turns them into your tanks: they have Taunt and Bone Barrier (75% damage reduction); rotate 2 MC'd understudies tanking the boss
  - Disrupting Shout — massive AoE that burns mana (mana users: stay at max range/out of LoS when possible)
  - Jagged Knife — dot on a random player (heal through)
  - Unbalancing Strike — huge melee hit on the tank (only understudies can survive it long-term)
- phases: none; understudy MC tank rotation until he dies
- positioning: [PRIEST] MC understudy #1 → taunt Razuvious → tank him at the center; second [PRIEST] ready with understudy #2; [MT] holds the 2 spare understudies; [CASTER]/[HEALER] at max range (Disrupting Shout range management)
- tank_rules:
  - tanks do NOT tank Razuvious (his Unbalancing Strike wrecks player tanks); tanks hold spare understudies
  - [PRIEST] IF current understudy's Bone Barrier expires or MC is ending THEN next priest MCs a fresh understudy and taunts
- healer_rules:
  - heal the MC'd understudies (they are your tanks); heal Jagged Knife targets
- dps_rules:
  - dps Razuvious only after an MC'd understudy has solid threat; never hit the understudies
- movement_rules:
  - mana users: max range; move out before each Disrupting Shout if positioned close
- threat_rules: threat lives on the MC'd understudy; dps throttle at each understudy swap
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: [PRIEST] Mind Control x2 on understudies (rotation); [MAGE] sheep spare understudies in emergencies
- kill_order: Razuvious → understudies (after)
- enrage_timer: none
- wipe_risks: MC gap (unmitigated Unbalancing Strike); understudy killed by dps (target discipline)
- bot_behavior_notes: priest bots run a scripted MC-tank rotation; dps bots blacklist understudies

---

### boss: Gothik the Harvester
- raid: Naxxramas
- raid_size: 40
- boss_order: 8
- optional: no
- encounter_type: add-control
- composition: raid SPLIT IN TWO — "live side" (melee-heavy) and "undead/dead side" (tank/healer/ranged balanced); gates lock when pulled
- key_abilities:
  - Live side adds: Unrelenting Trainee (weak melee), Unrelenting Death Knight (hard melee), Unrelenting Rider (charge)
  - Dead side: killed live-side adds RESPAWN as undead (Spectral Trainee: arcane explosion; Spectral Deathknight: hard melee; Spectral Rider + Spectral Horse)
  - kill-rate mechanic: the faster live side kills, the more dead side is flooded — BALANCE the kill rate
  - P2 (4:30): Gothik himself teleports between sides — Harvest Soul (stacking -10% stats debuff); gates open
- phases: P1 (4:30 split-side add waves) → P2 (Gothik active, gates open)
- positioning: live side: left half, kill adds at the gate; dead side: right half, [MT] picks up spectral spawns at their spawn point; do not stand on the gate line
- tank_rules:
  - live side: [OT] holds Death Knights/Riders; dead side: [MT] holds spectral spawns, kill in order
- healer_rules:
  - split healers between sides (more on dead side); heavy AoE healing on dead side
- dps_rules:
  - live side kill order: Death Knight > Rider > Trainee — but THROTTLE: only kill as fast as dead side can handle (call out)
  - dead side kill order: Spectral Trainee (arcane explosion = kill first) > Rider > Deathknight > Horse
  - P2: all dps Gothik
- movement_rules:
  - P2: [ALL] converge on Gothik's current side
- threat_rules: adds spawn with proximity aggro; tanks body-pull spawns
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: [MAGE] Frost Nova trainees; [WARLOCK] Banish spectral overflow (undead)
- kill_order: P1: per-side priorities above; P2: Gothik
- enrage_timer: none (P2 Harvest Soul ramp is the timer)
- wipe_risks: live side killing too fast → dead side flooded; spectral trainees exploding on dead-side healers
- bot_behavior_notes: pre-assign every bot to live/dead side; live-side bots use a kill-rate throttle (hold dps on command); dead-side bots prioritize Spectral Trainees

---

### boss: The Four Horsemen
- raid: Naxxramas
- raid_size: 40
- boss_order: 9
- optional: no
- encounter_type: council+swap
- composition: 4-8 tanks (4 geared primary + backup); 12 healers; shadow/fire resist pots; full raid split into 4 groups
- key_abilities:
  - Marks — each horseman applies a stacking Mark debuff (Korth'azz/Blaumeux/Zeliek/Rivendare) to nearby players; 4+ stacks = death; groups ROTATE between horsemen to let marks expire (~75s duration... standard: swap at 3 stacks)
  - Thane Korth'azz (front-left): Meteor — heavy fire damage split among players in 8 yards (group up on him)
  - Lady Blaumeux (back-left): Void Zone — ground shadow AoE (move); tanked by a ranged/tank hybrid at range? (standard: [WARLOCK]/tank with healers, she casts Shadow Bolt at range)
  - Sir Zeliek (back-right): Holy Wrath — chains to nearby players (spread); tanked at range similarly
  - Baron Rivendare (front-right): melee + Unholy Shadow aura
  - Shield Wall — at 50% and 20% each horseman takes 75% less damage for 20s
- phases: none; rotation fight: groups cycle horsemen on mark count
- positioning: four corners: Korth'azz pulled front-left, Rivendare front-right, Blaumeux back-left, Zeliek back-right; each group assigned a corner rotation sequence (clockwise)
- tank_rules:
  - each horseman: [MT] + backup tank; IF your mark stacks hit 3 THEN your group rotates to the next corner per the sequence
  - taunt handoffs during rotation must be instant (an untanked horseman casts/one-shots)
- healer_rules:
  - healers rotate WITH their group; keep 2+ healers on each active tank corner
- dps_rules:
  - burn Korth'azz first (grouped for Meteor), then Rivendare, then Blaumeux, then Zeliek
  - IF Void Zone under you THEN move instantly
  - at Zeliek: spread 8 yards (Holy Wrath chains)
- movement_rules:
  - rotation: whole group moves together to the next corner on the mark call; never rotate alone
- threat_rules: taunt handoffs are the only threat events; dps hold 2s on arrival at a new corner
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: none
- kill_order: Korth'azz → Rivendare → Blaumeux → Zeliek
- enrage_timer: none (mark stacking is the timer — rotation must be clean)
- wipe_risks: 4+ marks on a group (missed rotation); taunt handoff failure; Zeliek Holy Wrath chains through a stacked group
- bot_behavior_notes:
  - per-bot corner rotation script keyed on mark count (rotate at 3)
  - group cohesion: bots move as a unit with their group, not individually

## CONSTRUCT WING

---

### boss: Patchwerk
- raid: Naxxramas
- raid_size: 40
- boss_order: 10
- optional: no
- encounter_type: dps-race
- composition: 1 [MT] + 2-3 off-tanks (Hateful Strike soaks — high HP/armor); 12 healers with assigned soak-tank targets; ~7-minute enrage
- key_abilities:
  - Hateful Strike — every ~1.2s: massive hit on the highest-HP melee-range target that is NOT the top threat (i.e., an off-tank); off-tanks must stay topped off and higher HP than dps
  - Berserk — 7-minute hard enrage
  - Frenzy — sub-5%: minor enrage
- phases: none; pure throughput check
- positioning: [MT] + 3 [OT] all in melee range stacked; [MELEE] dip into melee only if their HP stays below the OTs' HP (or stay out of Hateful rotation per strategy); [RANGED]/[HEALER] at range
- tank_rules:
  - [MT] holds aggro; [OT] do not pass [MT] on threat but stay top HP among non-MT melee
  - all tanks: defensive cooldowns as assigned; spam threat
- healer_rules:
  - assigned healing: 2-3 healers per off-tank, spam max-rank heals non-stop (Hateful hits ~7-9k)
  - IF your assigned soak drops below ~40% THEN immediate max heal; there is no time to triage elsewhere
- dps_rules:
  - maximum sustained dps from second 5; this is a pure gear/consumable check
  - [MELEE] keep HP below off-tanks (do not get healed above them — a melee with top HP eats a Hateful and dies)
- movement_rules: none (stand and deliver)
- threat_rules: [MT] must stay top threat; [OT] second; dps third — threat discipline mandatory
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: none
- kill_order: Patchwerk
- enrage_timer: 7 minutes hard enrage
- wipe_risks: healer OOM/missed Hateful heal → soak dies → melee start dying; dps below enrage threshold
- bot_behavior_notes: healer bots: fixed assignment + spam-heal loop, no cross-triage; melee bots: HP-management rule (stay below soak-tank HP)

---

### boss: Grobbulus
- raid: Naxxramas
- raid_size: 40
- boss_order: 11
- optional: no
- encounter_type: movement
- composition: 1 [MT] + 1 [OT]; poison dispels
- key_abilities:
  - Mutating Injection — dot on a random player: on expiry/dispel drops a growing poison cloud AT THE PLAYER'S FEET (run to the wall/edge before it expires — NEVER dispel early... dispel only when positioned)
  - Slime Spray — frontal cone: spawns Fallout Slime adds for each player hit (tank faces boss away; [OT] kills slimes)
  - Poison clouds — existing clouds grow and persist; the room slowly fills (kite the boss along the outer wall)
- phases: none; the arena fills with clouds over time (soft timer)
- positioning: [MT] kites Grobbulus slowly along the outer wall; raid stays in the clean middle; designated cloud-drop spot = the wall behind the kite path
- tank_rules:
  - [MT] kite steadily along the wall (new clouds drop behind you); face boss away from raid
  - [OT] intercept Fallout Slimes
- healer_rules:
  - IF Mutating Injection target is positioned at the wall THEN dispel; otherwise heal through until they arrive
- dps_rules:
  - [OT]+assigned dps kill Fallout Slimes fast (they pulse AoE)
  - IF Mutating Injection on you THEN run to the drop spot, wait for expiry, return
- movement_rules:
  - never cross the boss's frontal cone; cloud carriers move along the outer path, not through the raid
- threat_rules: none
- interrupt_priority: none
- dispel_priority:
  - Mutating Injection (poison) — dispel ONLY when the carrier is at the drop spot
- cc_assignments: none
- kill_order: Fallout Slimes → Grobbulus
- enrage_timer: room-fills-with-clouds soft timer
- wipe_risks: cloud dropped in the raid; slime adds overwhelming
- bot_behavior_notes: injection-carrier bots: path to wall waypoint, hold until debuff expires, return; dispel bots: position-gated dispel rule

---

### boss: Gluth
- raid: Naxxramas
- raid_size: 40
- boss_order: 12
- optional: no
- encounter_type: add-control
- composition: 2 tanks; 2+ [HUNTER] tranq; [MAGE]/[WARRIOR] zombie-kiting crew; heavy AoE
- key_abilities:
  - Zombie Chow — spawn from the 3 grates continuously; they slowly walk to Gluth; if one reaches him he heals ~5%; they hit cloth extremely hard (kite them: Frost Trap, Frost Nova, Piercing Howl, kiting rotation)
  - Decimate — every ~105s: EVERYONE (raid + zombies) dropped to 5% HP; afterwards zombies ignore players and beeline to Gluth — AoE them down BEFORE they arrive
  - Frenzy — Tranquilizing Shot
  - Mortal Wound — stacking on tank (tank swap)
  - Terrifying Roar — fear
- phases: Decimate cycle every ~105s is the rhythm
- positioning: [MT]/[OT] tank Gluth near the door; [MAGE]+kiters patrol the grates; [RANGED]/[HEALER] mid-room spread; fear counters on tanks
- tank_rules:
  - swap on Mortal Wound stacks; [WARRIOR] stance-dance fears
  - [HUNTER] tranq every Frenzy
- healer_rules:
  - after Decimate: heal the TANKS first (they are at 5% with zombies loose), then raid
- dps_rules:
  - between Decimates: slow zombies (do not kill near Gluth — they just respawn anyway; keep them slowed/rooted)
  - IF Decimate hits THEN [ALL] switch to zombies: Frost Nova, AoE, execute — kill ALL before any reach Gluth
  - [MAGE] kite packs with Blizzard/nova rotation; [HUNTER] Frost Trap the grate exits
- movement_rules:
  - zombie kiters: circular kiting path away from the boss
- threat_rules: fear causes tank threat scrambles; re-taunt after Roar
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: [MAGE] Frost Nova + [HUNTER] Frost Trap on zombies; [WARRIOR] Piercing Howl
- kill_order: zombies (post-Decimate: ALL of them) → Gluth
- enrage_timer: none
- wipe_risks: zombie reaching Gluth post-Decimate (heal cascade); untranq'd Frenzy during Decimate window
- bot_behavior_notes: Decimate = global target-swap trigger: all dps bots switch to nearest zombie until count = 0

---

### boss: Thaddius
- raid: Naxxramas
- raid_size: 40
- boss_order: 13
- optional: no
- encounter_type: gimmick
- composition: 2 tanks; 2 balanced split groups for the platforms; ~5-minute enrage on Thaddius
- key_abilities:
  - P1: Feugen & Stalagg (two platforms): Magnetic Pull — every ~30s they SWAP TANKS by pulling them across the gap; both must die within 5s of each other or they resurrect; Static Field/chain lightning
  - The Jump — after P1, jump the gap to Thaddius's platform (missed jump = slime + run back)
  - P2 Thaddius: Polarity Shift — every ~30s each player gets a charge (+ or -): same-charge players stacked together gain +damage per stack; touching an OPPOSITE charge = arcane explosion damage to the raid (separate by charge: e.g., + left, - right)
  - Ball Lightning — if nobody in melee range, he chain-nukes the raid (tank must be in melee)
  - Chain Lightning — unavoidable raid damage
  - Berserk — 5 minutes
- phases: P1 (two mini-bosses on platforms) → P2 (Thaddius)
- positioning: P1: raid splits evenly between the two platforms; P2: melee/ranged stack in TWO groups by charge (plus-group left, minus-group right), swapping sides on every Polarity Shift
- tank_rules:
  - P1: [MT] on Feugen, [OT] on Stalagg; after Magnetic Pull, each tank tanks the boss that arrived at their platform
  - P2: [MT] never leave melee range (Ball Lightning); taunt is normal
- healer_rules:
  - P2: heal your charge-group; heavy Chain Lightning raid damage
- dps_rules:
  - P1: balance dps so both mini-bosses die within 5s (call the execute)
  - P2: IF Polarity Shift THEN check your charge: SAME charge → stay/stack with your group; CHANGED → run to the other side before touching anyone opposite
  - stacked charges multiply damage — groups stack tight on their side
- movement_rules:
  - THE JUMP: run+jump the platform gap; bots must path to jump point and jump
  - charge changes: immediate side swap, no exceptions
- threat_rules: P2 threat normal; dps race vs 5-min enrage
- interrupt_priority: none
- dispel_priority: none
- cc_assignments: none
- kill_order: P1: Feugen + Stalagg (simultaneous) → P2: Thaddius
- enrage_timer: 5 minutes on Thaddius
- wipe_risks: opposite-charge contact (arcane explosions); missed platform jump; staggered P1 kills (resurrect loop)
- bot_behavior_notes:
  - polarity state machine per bot: charge attribute → side assignment; immediate relocation on change
  - jump routine: waypoint + jump action
  - P1: dps throttle to sync kills within 5s

## FROSTWYRM LAIR

---

### boss: Sapphiron
- raid: Naxxramas
- raid_size: 40
- boss_order: 14
- optional: no
- encounter_type: movement
- composition: 1-2 tanks; frost resist gear on the raid (100-150+); decursers; heavy raid healing
- key_abilities:
  - Frost Aura — constant raid-wide frost dot (frost resist + heal through)
  - Life Drain — curse: drains HP and heals Sapphiron (DECURSE fast)
  - Blizzard — moving ground blizzard (move)
  - Air phase (every ~45s): flies up; Icebolt — freezes 5 random players into ice blocks (spread before it lands; the frozen take splash damage if stacked); then Frost Breath — room-wide lethal blast EXCEPT behind the ice blocks (hide behind a block, LoS the breath)
  - Cleave/Tail Sweep — standard dragon
- phases: ground phase / air phase (icebolt + breath), alternating
- positioning: ground: [MT] tanks Sapphiron against the wall; [MELEE] sides; [RANGED] spread 10 yards (icebolt splash); air: spread for icebolts, then collapse behind a block
- tank_rules:
  - standard dragon tanking; watch cleave
- healer_rules:
  - constant raid healing vs Frost Aura; burst the iceblocked players after breath
  - [MAGE]/[DRUID] decurse Life Drain instantly (it heals the boss)
- dps_rules:
  - ground: dps; air phase: spread, then run behind the nearest ice block BEFORE Frost Breath fires
  - do NOT break the ice blocks early (they are your cover)
- movement_rules:
  - IF air phase begins THEN spread 10 yards
  - IF ice blocks are out THEN move behind one with clear LoS-block to Sapphiron; wait for the breath, then spread
  - avoid Blizzards
- threat_rules: none
- interrupt_priority: none
- dispel_priority:
  - Life Drain (curse) — [MAGE]/[DRUID] — immediate, all targets
- cc_assignments: none
- kill_order: Sapphiron
- enrage_timer: none (healer mana + frost aura = soft timer)
- wipe_risks: players caught outside ice-block cover during Frost Breath; Life Drain left uncursed healing the boss
- bot_behavior_notes:
  - air-phase routine: spread → identify nearest ice block → path behind it → hold → resume
  - never attack ice blocks (target blacklist)

---

### boss: Kel'Thuzad
- raid: Naxxramas
- raid_size: 40
- boss_order: 15
- optional: no (final boss of Naxxramas)
- encounter_type: gimmick
- composition: 2-3 tanks; 2+ interrupters ([ROGUE]/[WARRIOR] rotation on the boss); 10-12 healers
- key_abilities:
  - P1 (waves, ~5 min): Soldiers of the Frozen Wastes (skeletons — kill; they explode on touch... slow), Soul Weavers (banshees — kill fast: knockback), Unstoppable Abominations (tank them)
  - P2 (Kel'Thuzad active): Frost Bolt (single-target nuke on tank — INTERRUPTABLE, must be kicked), Frostbolt Volley (unavoidable raid-wide — heal through), Chains of Kel'Thuzad — mind-controls 5 players (usually including tanks): CC the MC'd, threat drops, [OT] picks up boss), Frost Blast — freezes a random player AND anyone within 10 yards: dot that will kill without fast healing (SPREAD 10 yards), Shadow Fissure — red circle on the ground: MOVE (one-shot), Mana Detonation — burns mana of a random mana user + AoE around them (mana users spread/stay away from others)
  - P3 (40%): 5x Guardian of Icecrown adds — they grow stronger over time (Blood Tap); standard: tanks/CC hold them (they can be shackled? No — they are beetles... [DRUID] hibernate? No. Standard: kill or perma-CC via... in practice: [PALADIN] stuns, [WARRIOR] taunt-kite; kill them or finish KT fast)
- phases: P1 (add waves) → P2 (KT active) → P3 at 40% (adds + KT)
- positioning: P1: raid in the middle circle, tanks at the room edge picking up abominations; P2/P3: raid spread 10+ yards around the room (Frost Blast), melee stacked at boss's back loosely with 10-yard sub-groups
- tank_rules:
  - P1: [OT] tanks abominations; P2: [MT] on KT; IF [MT] is Chained (MC) THEN [OT] taunts immediately
  - P3: spare tanks pick up Guardians
- healer_rules:
  - Frostbolt Volley = raid AoE heal windows; Frost Blast targets need INSTANT heals (they tick to death in ~4s)
  - P1: efficient healing (long phase)
- dps_rules:
  - P1 kill order: Soul Weavers > Soldiers > Abominations (tanked last)
  - P2: interrupt rotation on Frost Bolt (never let one land on the tank)
  - [MELEE] IF Chains MCs melee THEN [MAGE] sheep / [WARLOCK] fear the MC'd players
  - P3: burn Kel'Thuzad; kill or control Guardians per assignment
- movement_rules:
  - IF Shadow Fissure under you THEN move out NOW (one-shot)
  - IF Frost Blast: pre-spread 10 yards so only the target is frozen
  - Mana Detonation target: move away from other players
- threat_rules: Chains of Kel'Thuzad = MC + threat wipe on MC'd tanks; [OT] must cover instantly
- interrupt_priority:
  - Frost Bolt (single-target) — kick every cast ([ROGUE]/[WARRIOR] rotation)
- dispel_priority: none critical
- cc_assignments: [MAGE] Polymorph / [WARLOCK] Fear on Chained players; P3: stuns on Guardians
- kill_order: P1: Soul Weavers → Soldiers → Abominations; P2/P3: Kel'Thuzad (Guardians controlled/killed per call)
- enrage_timer: none (Guardian ramp in P3 is the soft timer)
- wipe_risks: Frost Bolt landing on tank (interrupt rotation failure); chained Frost Blast on a stacked raid; Shadow Fissure deaths; P3 Guardian overwhelm
- bot_behavior_notes:
  - interrupt rotation: sequenced kick bots with cooldown tracking
  - strict 10-yard spacing enforcement for all bots in P2/P3
  - MC response: nearest mage bot sheeps MC'd targets automatically
  - Shadow Fissure = top-priority ground-effect dodge
