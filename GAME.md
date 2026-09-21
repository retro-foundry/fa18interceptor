from pathlib import Path

content = r"""# F/A-18 Interceptor (Amiga, 1988) — Reverse-Engineering Dossier

## Purpose

This document is an agent-facing research handoff for reverse engineering **F/A-18 Interceptor**, developed by Intellisoft and published by Electronic Arts for the Commodore Amiga in 1988.

The goal is not merely to describe the game. The goal is to identify:

- reliable historical facts that help recover the architecture;
- known executable/disk/runtime properties;
- public patch code that provides hard addresses into the original program;
- behavioural quirks that can be turned into black-box experiments;
- likely subsystem boundaries;
- useful comparison targets;
- and a practical order of attack for producing a high-quality disassembly/decompilation.

### Evidence labels

Use these labels in all reverse-engineering notes:

- **CONFIRMED-BINARY** — verified directly in the original executable or runtime trace.
- **CONFIRMED-PATCH** — verified in the public WHDLoad patch source.
- **DOCUMENTED** — stated in the original manual or contemporary developer/review material.
- **HISTORICAL-REPORT** — old user/developer report, cheat sheet, Usenet post, etc.
- **HYPOTHESIS** — plausible interpretation that still needs binary/runtime verification.

Do not silently promote a historical report or hypothesis to a binary fact.

---

# 1. Executive conclusions

These are the most important facts to know before starting.

1. **The main game is an ordinary AmigaDOS executable loaded through `LoadSeg`, not a custom trackloader game.**  
   The current public WHDLoad slave opens DOS/graphics libraries, assigns logical devices, and loads a program named `f-18 interceptor` with `LoadSeg`.

2. **The executable is deeply segmented.**  
   The WHDLoad patch explicitly walks to **segment index 71** and accesses data at offset **+357** inside it. Therefore there are at least 72 linked LoadSeg segments in the executable used by the patch. Treat the Hunk layout as first-class information rather than flattening it immediately.

3. **The executable may be packed. Decrunch before comparing addresses.**  
   The WHDLoad installer checks the executable size; if it is below 200,000 bytes it invokes `xfdDecrunch`. Preserve both packed and unpacked hashes.

4. **The game is mostly 68000 assembly, but not entirely.**  
   A contemporary 1988 Amiga Computing article says roughly **90% of the program was written in assembler**, with **menus and setup routines written in C**.

5. **Bob Dinnerman developed it using Electronic Arts' Artist Workstation (AWS).**  
   A contemporary COMPUTE! article describes the EA AWS as an IBM AT/compatible with 2.5 MB RAM, connected to the target machine through an EA proprietary cable, with EA editors and a symbolic debugger. That history makes the large number of executable segments/modules less surprising.

6. **The renderer really did use the Amiga blitter for 3D work.**  
   A separate 1988 Amiga Computing technical feature says some of Interceptor's 3D-solid algorithms were designed around the blitter's **line drawing and area filling**, while Dinnerman also wrote his own polygon creation/movement routines. Do not reduce this to the generic claim that the blitter was only used to clear the screen.

7. **The game contains CPU-speed-dependent timing.**  
   WHDLoad v2.0 added explicit speed regulation because text and other behaviour became too fast on accelerated Amigas. The patch also replaces a CPU busy loop, adds a sound delay, and synchronizes the game using VBL counts.

8. **The modern WHDLoad patch is a superb reverse-engineering oracle.**  
   It exposes named fixes for:
   - CPU-dependent loop;
   - code protection;
   - two access faults;
   - keyboard handling;
   - sound timing;
   - splash-screen timing;
   - game speed regulation;
   - VBL/input handling.

9. **The protection is ordinary executable code, not exotic disk protection.**  
   Historical 1988 patches modified four `ANDI.W #$000F,Dn`-looking instruction patterns so the codewheel answer could always be `0000`. Modern WHDLoad skips the protection completely.

10. **The game has hard-coded `DF0:` paths.**  
    Contemporary reports and old hard-drive installation instructions confirm that support files are looked up using explicit `DF0:` strings. A classic workaround replaces them with a logical device such as `F18:` and assigns that device to a directory.

11. **Flight physics and game speed may be partially frame-coupled.**  
    The WHDLoad speed regulator waits for a minimum of four VBL increments during normal in-game execution. If this call corresponds one-to-one with simulation/render ticks, that suggests approximately **12.5 Hz on PAL** and **15 Hz on NTSC**. This is an inference, not yet proof. Verify immediately.

12. **Early PC JetFighter is a valuable comparative oracle.**  
    The original programmer Bob Dinnerman also created JetFighter. Paul Grace later recalled that PC libraries even contained Amiga OS strings. Contemporary material described JetFighter as essentially the PC development of Interceptor. Use it to compare high-level algorithms, tables, mission/camera/radar concepts and strings, but do not assume binary code identity.

---

# 2. Provenance and development history

## 2.1 Core credits

Known Amiga credits:

- **Robert "Bob" Dinnerman** — design and programming
- **Moses Ma** — design assistance
- **Greg Johnson** — graphic design
- **David Warhol** — sounds/music
- **Richard Hilleman** — producer
- **Paul Grace** — assistant/associate producer
- **Steven E. Hayes** — technical director
- **Simon Roberts** — packaging
- **Chester Chen** — manual illustrations

The exact wording varies slightly between credit databases and period material.

## 2.2 Bob Dinnerman's background

Contemporary material states:

- Interceptor was Dinnerman's first commercial home-computer game.
- The Amiga was his first home computer.
- He previously worked for Bally/Midway.
- He programmed **Discs of Tron**.
- Some of Interceptor's flight-algorithm work developed from work on a Bally flying game that was planned and then cancelled.
- Development took roughly a year and a half.

This background matters because it explains why the code may look more like arcade/simulation code than a conventional Amiga game engine.

## 2.3 Language split

Contemporary Amiga Computing reporting says:

- approximately 90% assembler;
- menus and setup routines in C.

### Reverse-engineering implication

Do not assume one compiler-generated idiom across the whole binary.

The menu/setup code may show recognizable 1980s C compiler patterns:

- conventional stack frames;
- switch jump tables;
- library-call wrappers;
- small helper functions;
- compiler-specific register save conventions.

The core simulation/renderer may be much more hand-written.

A useful early task is to classify every segment as:

- likely hand-written assembly;
- likely C-generated code;
- data;
- BSS;
- mixed/unknown.

---

# 3. Electronic Arts Artist Workstation development environment

A 1988 COMPUTE! article describes EA's contemporary Artist Workstation:

- IBM AT or compatible;
- 2.5 MB RAM;
- proprietary cable to the target computer;
- EA internal music, sound and graphics editors;
- symbolic debugger;
- code written on the workstation and executed/debugged on target hardware.

Interceptor was explicitly reported as having been developed on EA's AWS.

### Why this is useful

This creates several possibilities to investigate:

- many Hunk segments may reflect source modules produced by the AWS build system;
- development-time symbols may once have existed even if stripped from retail;
- common EA Amiga titles of the same period may share startup/runtime/library glue;
- debug/string conventions may resemble other AWS-built EA games.

Do a comparative binary survey of 1987-1989 EA Amiga executables if time allows.

---

# 4. Platform and memory targets

## Original documented requirements

Original manual:

- Amiga 500 / 1000 / 2000;
- at least **512 KiB** RAM;
- 1 MiB recommended;
- 1 MiB enables richer sound and the full demonstration sequence;
- on a 512 KiB machine, disconnecting an external drive could leave more memory for sound.

The game is an OCS-era title.

## WHDLoad environment

The current WHDLoad installation reports:

- 512 KiB Chip RAM;
- 576 KiB expansion allocation in the complete slave environment;
- Kickstart 1.3 image required by the WHDLoad emulation setup.

The patch source itself defines:

- `CHIPMEMSIZE = $80000`
- `FASTMEMSIZE = $50000`

The difference between the advertised expansion amount and the patch's `$50000` fast-memory allocation is consistent with additional Kickstart/emulation requirements; do not confuse WHDLoad's requirements with the exact original retail-game memory map.

### Essential experiment

Run the same original executable under:

1. 512 KiB configuration;
2. 1 MiB configuration.

Trace:

- `AllocMem`;
- file opens;
- audio/sample loads;
- demo-script branches;
- buffer allocations.

This should expose the memory-capability gate very quickly.

---

# 5. Disk and executable architecture

## 5.1 Ordinary AmigaDOS installation

Available evidence indicates a normal AmigaDOS disk rather than a custom trackloader:

- the game has normal AmigaDOS files/directories;
- old users edited `S/startup-sequence`;
- the main file is called `F-18 Interceptor`;
- WHDLoad copies ordinary files;
- WHDLoad uses DOS `LoadSeg` to launch the main executable.

## 5.2 Hard-coded device paths

Contemporary 1988 users complained that the game looked for sound, picture and other supporting resources using hard-coded paths beginning with:

`DF0:`

The classic hard-drive workaround was:

- replace every `DF0:` string with a same-length logical device such as `F18:`;
- copy files to a hard-drive directory;
- `ASSIGN F18: <directory>`.

The modern WHDLoad slave likewise creates assignments for `DF0` and `F18`.

### Reverse-engineering use

Search the decrunched executable for every occurrence of:

- `DF0:`
- `F18:`
- file extensions;
- resource names;
- `graphics.library`
- `dos.library`

Then cross-reference every string to recover the resource loader graph.

## 5.3 Main executable name

The public WHDLoad source defines its program name as:

`f-18 interceptor`

Case is not significant to normal AmigaDOS use, but preserve original directory entry spelling in project notes.

## 5.4 Packed executable

The WHDLoad installer:

- checks the installed main executable's size;
- if it is below 200,000 bytes, runs `xfdDecrunch` on it.

This is a very important warning.

### Rule

Never compare:

- patch offsets;
- historical crack offsets;
- disassembler output;

until the executable variant and packing state are known.

Preserve:

- original disk image hash;
- original packed file hash;
- unpacked/decrunched file hash.

## 5.5 Segmented Hunk executable

The WHDLoad loader stores the `LoadSeg` segment list and later calls an internal helper with value **71**.

That helper follows the BCPL/AmigaDOS segment links and returns the address of segment index 71. The patch then adds **357 bytes** and treats that location as an in-game/text-mode state byte.

Therefore:

**CONFIRMED-PATCH: the executable targeted by WHDLoad contains at least 72 linked segments.**

### First static-analysis deliverable

Generate a table like:

| Index | Hunk type | File offset | Runtime size | Relocs | Suspected role |
|---:|---|---:|---:|---:|---|
| 0 | CODE | | | | startup |
| ... | | | | | |
| 71 | ? | | | | contains mode byte at +357 |

Preserve segment boundaries in the disassembly.

---

# 6. Public WHDLoad source — exact reverse-engineering landmarks

The current public source lives in:

`jotd666/whdload_slaves`

Path:

`F/F-18Interceptor/usr/source/F-18InterceptorHD.s`

At the time this dossier was prepared, the GitHub blob SHA observed for that file was:

`3100bc7375685acfa34b92520198ff2900065282`

This is **WHDLoad patch/slave source, not the original F/A-18 Interceptor source code**.

That distinction is essential.

## 6.1 Patch landmarks

The following values are from the current patch list.

Treat them as **WHDLoad `resload_PatchSeg` logical offsets** until you have proved how each maps into your exact Hunk/file version. They are not automatically raw executable file offsets.

| Logical offset | Patch purpose | Suggested provisional label |
|---:|---|---|
| `$006B4` | wait on intro picture when BUTTONWAIT enabled | `intro_picture_wait_site` |
| `$008E2` | replaces CPU-dependent empty/busy loop | `busy_delay_site` |
| `$01590` | keyboard event hook | `keyboard_event_site` |
| `$01A78` | 6-byte NOP related to protection | `protection_call_site` |
| `$08286` | speed regulation | `game_speed_wait_site` |
| `$092E6` | VBL/input hook | `vbl_input_site` |
| `$0FD7C` | access-fault fix #1 | `pointer_fault_site_1` |
| `$11468` | access-fault fix #2 | `pointer_fault_site_2` |
| `$19737` | protection-passed flag forced to 1 | `protection_flag_1` |
| `$3A964` | protection-passed flag forced to 1 | `protection_flag_2` |
| `$451C8` | sound timing/delay hook | `sound_delay_site` |

There is also an older comment in the patch source describing a protection change around raw-looking offset `1CE70`, changing an immediate value from `$FF` to `$01`. Treat that comment as version-specific historical information until matched against your executable.

## 6.2 In-game state byte

The slave:

1. obtains segment index 71;
2. adds 357;
3. stores the resulting address;
4. speed-regulation code reads a byte there.

Patch comments/logic distinguish:

- zero: text is being displayed;
- non-zero: normal in-game state.

### High-value task

Identify the original references to `segment71 + 357`.

The surrounding data may be a state structure containing:

- game mode;
- message state;
- mission state;
- view state;
- timing flags.

This is one of the best initial anchors in the whole executable.

---

# 7. Timing model

Timing deserves very early attention because incorrect timing will contaminate every physics observation.

## 7.1 CPU-dependent busy loop

The WHDLoad source replaces a loop that its author describes as effectively something like a C busy-wait.

The replacement waits through raster vertical-position changes and skips the remainder of the original CPU loop.

The patch author's comments say the original delay appears:

- on the title;
- after the welcome message;
- and in a few similar places.

This is strong evidence of elapsed-time behaviour originally implemented as raw CPU iteration rather than VBL timing.

## 7.2 VBL-based speed regulation

Normal regulated gameplay in the WHDLoad v2 patch uses a VBL counter.

The VBL hook increments the counter.

The speed-regulation routine normally waits for a minimum count of:

`4`

before proceeding.

### Important inference

If one call to this regulator corresponds to one complete simulation/render update:

- PAL 50 Hz / 4 = **12.5 updates/sec**
- NTSC 60 Hz / 4 = **15 updates/sec**

This may be:

- renderer cadence;
- simulation cadence;
- or just the cadence of one part of the loop.

**Do not assume which. Measure it.**

## 7.3 Text-mode timing

The patch handles text differently from in-game updates so mission text remains readable on fast machines.

This means there are at least two timing regimes:

- normal in-game;
- text/menu/display.

Recover that state machine before rewriting timing.

## 7.4 Sound delay

A separate patch at logical `$451C8` introduces a raster-based delay.

This is an excellent anchor into the sound-update path.

## 7.5 Required timing experiments

Run at:

- stock 68000 speed;
- accelerated 68020/68030 equivalent;
- fast 68040/68060 equivalent;
- PAL;
- NTSC.

Log per second:

- main-loop iterations;
- world integration calls;
- renderer calls;
- VBLs;
- input polls;
- audio-update calls.

Then determine which quantities remain invariant.

---

# 8. Graphics / renderer

## 8.1 Contemporary evidence

A 1988 technical article on Amiga 3D graphics states that some of Interceptor's 3D-solid algorithms were designed around the Amiga blitter's:

- line-drawing capability;
- area-fill capability.

It also says Dinnerman wrote his own routines for creating and moving polygons/solids.

A separate contemporary review explicitly praises:

- fast 3D solids;
- sampled sound;
- use of the blitter.

### Consequence

The renderer should be investigated as a likely hybrid:

- 68000 transforms/project/clipping;
- custom polygon/solid routines;
- blitter line/fill assistance for at least some rasterization operations;
- possible CPU work for small shapes/HUD.

Do not assume all polygon fill is blitter-driven, and do not assume it is all CPU-driven.

## 8.2 Modern WHDLoad graphical bug clue

The WHDLoad ReadMe reports that on faster machines the third-person heading/speed/height indicators can show graphical corruption and speculates that they may be:

- drawing into the currently displayed buffer;
- or missing a blitter wait.

This is not proven, but it provides a good breakpoint target.

## 8.3 Graphics analysis plan

Instrument:

- all writes to custom-chip range `$DFF000-$DFF1FE`;
- especially blitter registers;
- bitplane pointers;
- display-window/fetch registers;
- copper list pointers;
- DMA enable;
- palette writes.

For each frame, capture:

- BLTCON0/1;
- BLTA/B/C/D pointers;
- BLTSIZE;
- modulos;
- masks;
- blitter busy/wait points.

Also trace `graphics.library` calls.

### Questions to answer

- What is the gameplay display resolution?
- How many bitplanes are active?
- Is there one framebuffer or multiple?
- Is there full double buffering, partial buffering, or dirty-region rendering?
- Are HUD/text and 3D scene drawn by different paths?
- Is the cockpit a bitmap/background with 3D window composited into it?
- Which polygon stages use blitter line/fill?
- How are clipping and near-plane handling implemented?
- Is back-face culling used?
- How is depth ordering performed?
- Are object faces stored already ordered or sorted dynamically?
- Is the world a flat mesh, object list, spatial grid, or hand-scripted landmark set?

## 8.4 Useful visual-state probes

Change only one thing at a time and diff memory/custom-register traces:

- cockpit vs external view;
- tower view;
- forward vs left/right/rear view;
- map;
- HUD on/off;
- zoom levels;
- parachute camera;
- missile camera if available;
- demo camera.

Camera systems are likely a cleaner entry point than immediately attacking the 3D math.

---

# 9. Input subsystem

## 9.1 Documented controls

Key systems include:

- F1-F10 throttle;
- repeated F10 enables afterburner;
- `=` / `-` incremental throttle;
- Backspace brakes/airbrake;
- comma/period rudder;
- cursor keys pitch/roll;
- G gear;
- A hook;
- H HUD;
- J ECM according to the full original manual;
- C chaff;
- F flare;
- M map;
- R radar range;
- T target;
- Return weapon;
- Space fire;
- Shift-F rescue pod;
- Shift-E eject;
- P pause;
- Esc restart;
- Shift-Esc quit to menu;
- numeric keypad camera/view controls;
- brackets zoom.

A later fan key sheet lists `K` for ECM, but the full manual says `J`. Trust the original manual first; verify runtime raw keycode.

## 9.2 Raw keycode landmarks from WHDLoad joypad patch

The modern slave synthesizes original keyboard events using these raw keycodes:

| CD32 input | Raw keycode | Intended game action |
|---|---:|---|
| Play | `$19` | pause/play-related key |
| Blue | `$44` | select weapon / Return |
| Yellow | `$14` | target select / T |
| Green | `$23` | flare / F |
| Reverse | `$0B` | thrust down |
| Forward | `$0C` | thrust up |

This gives hard constants for locating the original keyboard dispatch path.

The keyboard patch site is at logical `$01590`.

### Suggested first input experiment

Set breakpoints on the original handler and press one documented key at a time.

Build:

`raw keycode -> event -> state variable -> consumer function`

This quickly labels a large part of the main state structure.

---

# 10. Aircraft, physics and likely constants

## 10.1 Aircraft

Player-accessible:

- F/A-18 Hornet
- F-16 Falcon

Carrier start forces the F/A-18.

The cockpit/control scheme is largely shared.

## 10.2 Useful documented thresholds

Manual/historical observations provide concrete values useful for correlating memory state:

- ordinary takeoff rotation roughly **200 knots**;
- carrier qualification approach roughly **175 knots**;
- training demo uses around **80% thrust**;
- training may exceed **14,000 ft** and roughly **650 knots**;
- radar ranges: **2 / 10 / 40 miles**.

These are not necessarily internal constants in these exact units, but they are excellent calibration points.

## 10.3 Artificial world ceiling

Old detailed hints describe an exact ceiling at:

**40,960 ft**

Decimal 40960 equals:

`0xA000`

This is a powerful search target, but `$A000` can occur for unrelated reasons.

### Experiment

Climb under controlled conditions while watching candidate altitude variables.

Identify:

- raw altitude;
- displayed altitude conversion;
- clamp/compare instruction;
- vertical velocity response at ceiling.

Then check whether world X/Y boundaries use related magnitudes.

## 10.4 World boundaries

Historical hints describe hard North/South/East/West boundaries analogous to the altitude ceiling.

This suggests explicit coordinate clamps rather than an unbounded world.

Recover:

- coordinate origin;
- map scaling;
- units per mile;
- boundary constants;
- heading coordinate convention.

## 10.5 Collision oddities as probes

Historical detailed player notes report:

- the jet can sometimes "skip" on land/water if attitude is favorable;
- otherwise it can bog down to a stop while thrust remains unchanged;
- bridges may be visually present but not collide;
- the downed pilot and rescue pod are not normal collidable geometry;
- low-speed building contact may be survivable whereas faster impact crashes;
- the carrier can support the aircraft even with much of the aircraft extending beyond the deck;
- bizarre ejection/camera/player-object edge cases are possible.

These are not proof of the collision implementation, but they strongly suggest experiments for distinguishing:

- visual geometry;
- collision geometry;
- object bounding regions;
- terrain/surface tests;
- speed/impact thresholds;
- player-controlled-object pointer;
- camera-target pointer.

---

# 11. Camera system

The game supports:

- cockpit forward;
- look left/right/rear/up/down;
- multiple external compass views;
- tower view;
- map;
- zoom;
- parachuting views.

Historical notes indicate that many camera controls continue to work while the pilot is parachuting.

There are reports of an ejection edge case in which the camera/player state can become detached from a replacement aircraft.

### Reverse-engineering value

This makes the camera system an excellent way to identify several independent references:

- `controlled_object`
- `camera_target`
- `view_mode`
- `cockpit_enabled`
- `zoom`
- `ejected/pilot_object`

Do not assume all of these are the same state variable.

---

# 12. Radar/HUD/combat subsystem

## 12.1 Weapons

Core weapons:

- AIM-120 AMRAAM
- AIM-9 Sidewinder
- M61 cannon

Also:

- ECM
- chaff
- flare
- rescue pod in relevant mission
- ejection

Manual and secondary sources differ on some ammunition-count details. Prefer runtime state over modern summaries.

## 12.2 Radar ranges

Selectable radar ranges:

- 2 miles
- 10 miles
- 40 miles

These values provide an easy route to locating:

- radar-range index;
- lookup table;
- radar scale calculation.

## 12.3 Historical HUD/radar details

Detailed old hints claim:

- the HUD target-range marker changes around the clock as range closes;
- radar target-marker width encodes whether a target is above or below the player.

These should be easy to verify visually and then map to rendering conditionals.

---

# 13. Game modes and mission architecture

Main progression:

1. Demonstration Flight
2. Free Flight
3. Training — demo of maneuvers
4. Training — practice maneuvers
5. Qualification
6. Next Active Advanced Mission
7. Selectable completed missions
8. Flight log

There are six combat missions.

This gives a strong expectation that the program contains a top-level scenario/mode dispatch table.

### Find early

- menu option -> mode ID;
- mode ID -> setup routine;
- setup -> object spawns;
- objective update;
- completion/failure;
- return-to-menu.

Do not decompile six missions independently until this shared framework is understood.

---

# 14. Demonstration and training modes as scripted-engine oracles

## Demonstration Flight

The demo is fully scripted:

- starts at USS Enterprise;
- aircraft launches;
- follows a tour;
- lands at Alameda;
- repeats;
- user normally has no flight control.

This is ideal for finding:

- deterministic path/script data;
- camera scripting;
- throttle/gear scripted events;
- waypoint system.

### Experiment

Record memory every frame through a complete demo.

Search for low-entropy tables consumed sequentially.

## Training demonstration

The training demo contains seven known maneuvers and a deterministic sequence.

Training practice adds an instructor F-16.

This may expose:

- AI/autopilot control interface;
- desired heading/pitch/roll values;
- maneuver scripts;
- waypoint/follow-target logic.

These modes may be easier to reverse than hostile combat AI because they are structured and repeatable.

---

# 15. Hidden/free-flight starts

Multiple old sources describe undocumented free-flight entries.

Evidence conflicts:

- one old Usenet cheat says choose Free Flight and press **0** to reach a remote location around 34N/117W, described as an unfinished Edwards AFB area;
- GameFAQs lists **6, 7, 8, 9** as extra free-flight entries;
- a 1993 cheat compendium says **5 through 9**.

Do not choose one claim and discard the others.

### High-value test

Instrument the free-flight menu dispatch and press:

`0, 1, 2, 3, 4, 5, 6, 7, 8, 9`

For every value record:

- accepted/rejected;
- scenario ID;
- aircraft type;
- world X/Y/Z;
- heading;
- runway/carrier/object setup;
- script pointer.

This may reveal abandoned locations/scenarios still in the binary.

---

# 16. Six combat missions

The documented/historically known missions include:

1. **Visual Confirmation**
2. **Emergency Defense Operation / Air Force One defense**
3. **Intercept stolen F-16s**
4. **Search and Rescue**
5. **Intercept Incoming Cruise Missile**
6. **Carrier Sub / Shadow Sub mission**

The first missions are documented in the original manual; later missions were deliberately not fully spoiled there.

## Mission 4 — rescue

Useful implementation probes:

- downed pilot object;
- 2-mile radar-range use;
- low-altitude proximity;
- rescue-pod drop;
- completion radius/condition.

The pilot and pod reportedly have unusual/non-standard collision behavior.

## Mission 5 — cruise missile

Useful because it contains a very distinctive object type:

- high speed;
- very low altitude, reported around 200 ft;
- time-critical target.

Trace its object update function to identify how non-aircraft movers fit into the object system.

## Mission 6 — Shadow Sub

This mission has decades of contradictory player lore.

One WHDLoad-distributed hints file says its author contacted Electronic Arts and was told to:

- approach low;
- use AMRAAM;
- hit the conning tower;
- look for smoke;
- destroy enemy aircraft;
- rearm/refuel;
- return as needed.

Other historical/modern descriptions say the carrier may be logically destroyed without a visible explosion, and some players concluded only the patrolling aircraft actually mattered.

### Do not solve this from lore.

Trace the code.

Locate:

- mission ID 6 setup;
- carrier/sub object type;
- conning-tower hit/damage flag;
- smoke-state transition;
- aircraft/enemy count;
- return/landing state;
- completion predicate.

Produce a truth table for all relevant objective flags.

This is an ideal place where reverse engineering can settle a 38-year-old ambiguity.

---

# 17. Difficulty / replay behavior

Contemporary Usenet discussion reported that missions can vary on repeat attempts, for example:

- rescue-pilot location changes;
- cruise-missile direction can vary;
- stolen F-16 behavior can change;
- enemies may become harder.

The manual also indicates replayed/completed missions may become tougher.

### Investigate

Look for:

- completion mask;
- mission retry count;
- mission success count;
- difficulty scalar;
- random seed;
- enemy skill index.

Capture initial mission state from:

- first attempt;
- failed retry;
- first success;
- replay after completion.

Diff snapshots.

---

# 18. Flight log / persistence

The original manual describes a persistent flight log.

It tracks information such as:

- missions started/completed;
- weapons fired/hits;
- flight time;
- crashes;
- missile hits suffered;
- callsign/progression.

A supplied `R`/Rookie pilot on a write-enabled disk begins with the first three missions credited as complete.

Saving occurs through the flight-log UI.

## This is one of the easiest data formats to recover

Use a disposable copy of the disk.

Procedure:

1. Hash every disk file.
2. Save an untouched Rookie log.
3. Change callsign only; save; diff.
4. Change one mission-start count; save; diff.
5. Change one completion flag; save; diff.
6. Fire exactly one weapon; save; diff.
7. Create exactly one crash; save; diff.
8. Compare changes.

Recover:

- file name;
- record size;
- callsign encoding;
- counters;
- mission bitmasks;
- checksum, if any.

Then use persistence-field references to find the in-memory pilot/log structure.

---

# 19. Copy protection

## 19.1 Original protection

The retail game uses a physical rotating "Flight Computer" code wheel.

The manual explains a four-character challenge/countercode process and allows three attempts.

OpenRetro preserves scans of the code wheel and manual.

## 19.2 1988 public patch

A July 1988 Usenet/public-domain patch modified the executable so the valid response becomes `0000`.

Published instructions searched for:

- `0240000F` — three occurrences
- `0241000F` — one occurrence

and replaced the final mask with zero.

Decoded as 68000, these byte patterns strongly resemble:

- `ANDI.W #$000F,D0`
- `ANDI.W #$000F,D1`

changing the immediate mask to zero.

This decoding is a **HYPOTHESIS until checked against the exact executable**, but it is highly plausible.

A public-domain `f18fix.c` wrote zero bytes at raw file offsets:

- `0x1CD8D`
- `0x1CD9D`
- `0x1CDAD`
- `0x1CDBB`

Those raw offsets are version/packing dependent.

## 19.3 WHDLoad protection landmarks

Modern WHDLoad additionally:

- forces one protection-pass byte at logical `$19737` to 1;
- forces another at `$3A964` to 1;
- NOPs six bytes at `$01A78`.

These sites should triangulate the protection state machine.

## 19.4 Unit A crack clue

The WHDLoad patch explicitly remains compatible with a well-known Unit A crack.

It installs a handler for **TRAP #0** that returns zero.

This strongly suggests that cracked executable contains or depends on a `TRAP #0` path.

### Use crack variants only as comparison material

For an original-code reconstruction:

- disassemble the original;
- diff cracks against it;
- label crack modifications;
- do not let crack-specific code become canonical.

---

# 20. Two WHDLoad access-fault fixes

The patch contains two fixes for access faults.

Both repair pointer/address values by manipulating their high bits using a value derived from the expansion-memory address.

This is extremely interesting.

Possible explanations include:

- original code assuming 24-bit-era address properties;
- packed pointer representation;
- top-byte metadata;
- a pointer calculation that only happened to work in original memory placement.

### Required work

At both logical fault sites:

- recover the original instructions;
- trace the full pointer provenance;
- identify the structure being indexed;
- determine what the top byte originally represented;
- compare behaviour under original A500-like memory placement.

Do not simply preserve the WHDLoad fix in a decompilation without understanding why it was needed.

---

# 21. Sound system

Confirmed from contemporary reporting:

- sampled sound is a major feature;
- additional/richer sound appears with more RAM;
- external views alter the perceived engine sound in contemporary descriptions.

WHDLoad also patches a specific sound-delay site.

## Investigate

Determine whether runtime audio uses:

- `audio.device`;
- direct Paula DMA programming;
- both;
- preloaded raw samples;
- streamed/resource-loaded samples;
- synthesized/looped engine layers.

Trace:

- `$DFF0A0-$DFF0DA` audio channel registers;
- DMA enable changes;
- sample pointer/length changes;
- period/volume modulation.

Look for a state-dependent mixer driven by:

- thrust;
- afterburner;
- camera inside/outside;
- gear;
- stall/wind;
- missile warning;
- cannon.

Because 1 MiB changes sound content, file-open traces under 512 KiB and 1 MiB may reveal sample filenames/formats immediately.

---

# 22. JetFighter relationship

This is a major comparative lead.

## Evidence

- Bob Dinnerman programmed/designed both.
- Contemporary reporting described the subsequent PC game as an Interceptor-related development.
- Paul Grace's modern first-person account says EA's PC-version negotiations failed and the work later appeared as **JetFighter**.
- Grace recalls that PC libraries retained Amiga OS-related strings, suggesting a genuine code/data lineage rather than a purely conceptual sequel.

## How to use JetFighter

With a legally obtained early DOS JetFighter build:

Extract and compare:

- strings;
- lookup tables;
- trig tables;
- aircraft constants;
- waypoint data;
- mission-state concepts;
- camera/view indexing;
- radar scaling;
- AI states;
- weapon tables.

Do not expect identical machine code:

- Amiga: 68000 + custom chips;
- DOS JetFighter: x86 + EGA/PC hardware.

The value is semantic correspondence.

If a table in the Amiga binary is mysterious but an analogous DOS table sits near readable strings or symbols, it may unlock the Amiga meaning.

---

# 23. Renderer/world hypotheses to test

These are intentionally hypotheses, not facts.

## HYPOTHESIS A — object-list/painter rendering

Given the era, filled solids and relatively sparse world, a back-to-front object/face renderer is plausible.

Test by:

- changing camera;
- tracking face submission order;
- identifying sort keys;
- looking for depth compares/sorts.

## HYPOTHESIS B — coarse collision separate from render geometry

Bridge/pilot/pod quirks and carrier-edge behaviour suggest collision geometry may be much simpler than visual meshes.

Test by locating:

- object collision class;
- terrain/surface height test;
- building hit tests;
- carrier deck bounds;
- speed threshold.

## HYPOTHESIS C — shared scripted control path for demo/training/AI

Demo flight, instructor aircraft and combat AI may all feed a common aircraft-control state rather than separate physics.

Find the aircraft integration function first, then identify who writes desired control inputs.

## HYPOTHESIS D — fixed-point world state

Almost certain for performance reasons, but the exact format is unknown.

Recover empirically rather than assuming 16.16.

Correlate memory with:

- 1 ft altitude change;
- 1 degree heading;
- 1 knot;
- known map distances.

---

# 24. Behavioural bugs that are useful reverse-engineering tools

Do not "fix" these before understanding them.

They can expose architecture.

## Surface skipping / bogging

Can identify terrain-contact and crash thresholds.

## 40,960 ft ceiling

Can identify altitude variable and clamp.

## Hard N/S/E/W boundaries

Can identify world coordinates and map extent.

## Water landing

Can reveal that crash/ground logic is not simply terrain type == water.

## Carrier-edge support

Can reveal deck collision bounds.

## Ejection/replacement-aircraft state oddity

Can separate:

- player object;
- pilot/parachute object;
- camera target;
- cockpit mode.

## Demo external-view plane popping

WHDLoad notes this is more common in demo mode on fast machines.

Potentially reveals synchronization between:

- object update;
- draw list;
- buffer flip.

## Third-person indicator corruption

Potential blitter/display-buffer race.

---

# 25. Suggested static-analysis workflow

## Phase 0 — preserve exact inputs

For every disk/executable variant:

- SHA-256 the disk image;
- list every AmigaDOS file, size, date and hash;
- classify original/cracked/repacked;
- preserve original untouched copy;
- preserve packed main executable;
- generate and hash decrunched executable.

Create:

`variants.md`

Do not merge findings across variants without saying which binary they came from.

## Phase 1 — parse Hunks first

Before broad disassembly:

- enumerate HUNK_HEADER/HUNK_CODE/HUNK_DATA/HUNK_BSS;
- enumerate relocations;
- inspect symbol/debug hunks if any;
- assign stable segment IDs;
- generate runtime relocation mapping.

Output:

`hunks.json`

and:

`hunks.md`

## Phase 2 — map WHDLoad landmarks

For each patch logical offset:

- map to segment;
- map to local segment offset;
- map to raw decrunched file offset;
- disassemble ±128 bytes;
- identify callers and callees;
- assign provisional function names.

Output:

`whdload_landmarks.md`

## Phase 3 — string and OS-call pass

Find:

- `DF0:`;
- library names;
- resource filenames;
- mission text;
- UI text;
- callsign/log strings.

Resolve Exec/DOS/Graphics LVO calls.

Label functions such as:

- open resource;
- read resource;
- save log;
- AllocMem;
- WaitBOVP;
- screen setup;
- keyboard input.

## Phase 4 — classify assembly vs C

Use:

- prologue/epilogue patterns;
- register-save conventions;
- stack locals;
- switch tables;
- repeated helper idioms.

Mark suspected C-generated segments.

## Phase 5 — build call graph

Do not immediately translate to C.

First produce:

- function boundaries;
- call graph;
- global cross-references;
- segment ownership;
- side effects.

---

# 26. Suggested dynamic-analysis workflow

Use WinUAE/FS-UAE debugger or another emulator capable of:

- instruction breakpoints;
- memory watchpoints;
- custom-chip register logging;
- deterministic savestates.

## Experiment A — input-state matrix

Snapshot at stable free flight.

Toggle exactly one control.

Diff RAM.

Repeat for:

- throttle each level;
- afterburner;
- gear;
- hook;
- brake;
- HUD;
- ECM;
- chaff;
- flare;
- radar range;
- target;
- weapon;
- each view;
- zoom;
- map.

Goal: identify main player/cockpit state structure.

## Experiment B — coordinates

Keep plane stationary/on runway.

Record memory.

Move known distances/headings.

Find candidate X/Y/Z.

Use map and altitude readout to solve scale.

## Experiment C — flight integration

Watch candidate:

- position;
- velocity;
- attitude;
- angular velocity;
- G;
- thrust.

Single-step one game tick.

Recover state-update order.

## Experiment D — scripted demo

The demo is deterministic.

Trace one complete loop and identify:

- script pointer;
- waypoint index;
- target speed/heading/altitude;
- event opcodes if any.

## Experiment E — mission setup diffs

Create savestates immediately after setup of every mode/mission.

Diff to find:

- scenario ID;
- object list;
- mission flags;
- spawn data;
- AI skill.

## Experiment F — renderer register trace

Log blitter register writes for one frame in:

- cockpit;
- external;
- map;
- tower.

Cluster operations by call-site PC.

This can turn opaque graphics code into named passes.

---

# 27. Persistence experiment in detail

Because the disk log is writable, use it as a controlled reverse-engineering channel.

Recommended sequence:

1. Copy pristine disk.
2. Record full filesystem hashes.
3. Save Rookie log unchanged.
4. Change only callsign.
5. Save.
6. Diff.
7. Reset.
8. Start exactly one mission.
9. Save.
10. Diff.
11. Fire exactly one missile.
12. Save.
13. Diff.
14. Score one hit.
15. Save.
16. Diff.
17. Complete one mission.
18. Save.
19. Diff.

Once format is known, search RAM for exact serialized fields to find live state.

---

# 28. Mission-state reconstruction strategy

For each mission create a state-machine document.

Example schema:

```text
MissionState
  mission_id
  phase
  objective_flags
  failure_flags
  enemies_remaining
  player_returned
  player_landed
  special_object_state
  timer
  rng_seed
  