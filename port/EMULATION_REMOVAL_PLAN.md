# Plan: removing emulation from the active port

**Latest user direction (2026-10-06):** start `fa18_native` from the existing
`port/game/` sources, initially intro -> menu. This supersedes the incremental
adapter-removal strategy below. The native target omits CPU, translation, glue
and chipset objects. Current milestone details are in `native/README.md`.
Old runtime percentages below describe the reference runners and do not measure
the native intro/menu milestone or whole-game completion.

Native continuation milestones (functional scope, not instruction counts):

- Complete: source splash -> credits -> pilot entry -> main menu.
- Complete: native menu navigation, mode banners, source-gated mission list,
  flight-log summary, Escape return, reset/save/reload. Source owners and seven
  settled-screen comparisons: `../analysis/native_menu_milestone.md`.
- Connected, partial: full C08F26 bootstrap including native record updates
  and context refresh, Free Flight
  C0FECE delayed scene selection, C101FC/C10228 viewport transition and C10678
  messages. Initial pose/camera and gate banks match a source checkpoint.
  The record/context slice now repeats while setup permits it; active record
  dispatch and its cell matrix match focused original memory results.
  Later full record state still differs; see the latest scoped estimate below.
  See `../analysis/native_flight_start_milestone.md` and
  `../analysis/native_bootstrap_records_milestone.md` for bounded evidence.
- Connected: C1072E message completion, source typed-code acknowledgement,
  location and aircraft input, C10B90 recorder/root refresh, camera-origin
  normalization, and source P pause/resume. The root becomes aircraft kind
  $11 through its original constructor. See
  `../analysis/native_setup_selection_milestone.md`.
- Connected: source view matrices/projection, horizon selection, normal/wide
  terrain packets, polygon clipping and direct host line/fill/compositing.
  The real native setup preview now shows terrain. 160 polygon cases plus
  complete horizon/map submissions match original plane buffers at location
  and aircraft-selection checkpoints. See
  `../analysis/native_terrain_preview_milestone.md`. Roughly 80% of Free Flight
  startup wiring (previously 70%); this excludes whole-game completion.
- Connected: scene placement traversal, C1EE14 static/flat model commands,
  C096BC/C096CA ground descriptors, fixed matrix mark and direct host circles.
  Three native setup checkpoints exercise 246/10,334/17,005 model calls;
  reached descriptor returns, vertices and planes match focused original
  comparisons. See `../analysis/native_scene_objects_milestone.md`.
  Roughly 85% of Free Flight startup wiring (previously 80%), with aircraft
  record drawing, cockpit/HUD and active controls still pending.
- Connected: C1ED4C/C1F000 aircraft descriptors/hulls, compact/extended derived
  vertices, C1CCBC followup placements, C279D0 grid and C1518C setup controls.
  Three checkpoints execute 290/22,484/31,567 descriptors; descriptor drawing,
  record caches and complete parents match focused original non-stack RAM.
  See `../analysis/native_aircraft_rendering_milestone.md`. Roughly 90% of
  Free Flight startup wiring (previously 85%), excluding active-flight acceptance.
- Connected: C12098 view controls before the record pass, C1B27E flight input,
  C13D84 indexed aircraft controls and C149BE root motion with normalization,
  attenuation, region probe, source clock requests and touchdown tones. Seven
  checkpoints match original C12098/C1C63E non-stack RAM, including positive
  throttle speed/motion and arrow press/ramp/release. Aircraft selection now
  completes into C10DAE without P. See
  `../analysis/native_flight_controls_milestone.md`. Roughly 95% of Free Flight
  startup wiring (previously 90%); grounded motion is demonstrated, takeoff
  and recorded active-flight acceptance are not.
- Connected: nineteen direct cockpit/HUD instrument and panel owners, with
  native plane copy/mask/fill and compass shifts. Three checkpoints exercise
  780/1307/1707 HUD frames; 95 original-instruction cases at each checkpoint
  match every non-stack RAM byte. See `../analysis/native_hud_milestone.md`.
  Roughly 97% of Free Flight startup wiring (previously 95%), excluding full
  frame/timing and active-flight acceptance.
- Connected: source C11B44 notification cadence, C11BFC cockpit warnings,
  C31226 postflight renderer and C322EE message line. The C32CEE final text
  sequence now follows flight work. Three checkpoints each pass 115
  HUD/message cases. Startup estimate remains about 97%; game update counter
  and timer-driven cadence are the next active-flight dependency.
- Connected: C25312 resumable timer polling, C2548A sampling, source game
  counter/pause gates, periodic page clearing and C082B8 redraw requests.
  The disk update-rate table controls timing; a pending poll does not repeat
  physics, drawing or C32CEE text. C28996 regions and reached C28E28 zone checks
  now run. Seven view/record checkpoints, 36 timer/readout cases, two periodic
  record passes and focused yielding/pause checks pass. See
  `../analysis/native_clock_milestone.md`. Roughly 98% of startup wiring
  (previously 97%); whole-frame ownership and recorded-flight parity remain open.
- Connected: typed C12950 control/sound actions, with ordinary arguments and
  locals, existing audio consumers, C131BE magnitude and C133B2 step behavior.
  The native run executes 2454 active/inactive calls; 720 source cases match
  non-stack RAM and 1469 exact sound requests. Seven view/record checks and
  link omission pass. See `../analysis/native_control_actions_milestone.md`.
  Startup estimate remains about 98%; native sample loading/output remains open.
- Connected: shared C0F048-C0F124 scene order in the playable native runner.
  Position-bias gating, flagged-only control drawing and both range-dependent
  orders now follow the source owner. 1024 complete parent-contract cases,
  three renderer checkpoints, setup/pause/resume and twelve reference tests
  pass. See `../analysis/native_scene_ordering_milestone.md`. This batch is
  complete; startup estimate remains about 98%, excluding full-flight parity.
- Connected: reached C1F584-C1F6F8 model strip interpolation and C25704
  dynamics warning messages. 96 strip cases and an actual 7150-tick pullback
  checkpoint pass, including positive strip rendering and view/record parity.
  See `../analysis/native_model_strips_milestone.md`. Batch complete; startup
  wiring remains roughly 98%, with takeoff unproven.
- Connected: original-image terrain visibility lookup for negative/wrapped
  indices and exact X selector-word behavior. 384 source cases and the actual
  repaired tick-7294 map buffers pass. The 7150 checkpoint now explicitly checks
  takeoff (ground flag clears, height increases, bookkeeping set), with source
  view/record and rendering comparisons. See
  `../analysis/native_map_visibility_milestone.md`. Rough startup wiring now
  99% (previously 98%); recorded-flight acceptance remains open.
- Connected: postflight voice cleanup, event pair and source message/redraw
  children; existing completion/message callbacks are bound. The real 8200
  probe reaches C11788, and three original entry/wait/release cases pass. See
  `../analysis/native_postflight_entry_milestone.md`. Batch complete; startup
  wiring remains roughly 99%, excluding full reset/recorded-flight acceptance.
- Connected: resumable C1612C display publication/palette activity and C2F558
  page selection. Four PAL waits decrement activity once without repeating
  gameplay; the native pullback now resets through C11788 and resumes C10DAE.
  128 resumable source cases, 1024 blocking CPU/RAM contracts and three
  postflight source cases pass, as do the affected native checks and twelve
  reference tests. See `../analysis/native_outer_display_milestone.md`.
  Display/reset milestone 100% for this demonstrated path; startup remains
  roughly 99%, excluding complete frame timing and recorded-run acceptance.
- Connected: source C0F3C4 pending-input owner and C1AD74/C1AC28 dispatchers.
  Host key presses/releases queue through source polling, recorder draining,
  command publication and clearing before record updates. 144 original-RAM
  cases, four native wait/press/release checkpoints, affected gameplay checks,
  frontend/menu and twelve reference tests pass. See
  `../analysis/native_input_milestone.md`. Keyboard/source-owner milestone
  100%; physical gameport acquisition, modifier timing and countermeasure
  inherited arguments remain open. No whole-game percentage is inferred.
- Connected: qualification (digit 5 / source mode 9) now passes the banner,
  constructs the carrier scene, consumes the briefing and enters C10DAE.
  F10 throttle/pullback demonstrates short takeoff. C207FE carrier surface
  gating and C1FF0A face-result accumulation are connected; C0A2F0 landing
  scheduling follows source gates. Four actual checkpoints, three sets of
  original startup/record/render comparisons, affected regressions and twelve
  reference tests pass. See `../analysis/native_qualification_milestone.md`.
  Qualification startup/short-takeoff milestone 100%; complete landing,
  outcomes and recorded-run acceptance remain open.
- Connected: sealed raw keyboard recordings by update iteration, anchored at
  the main menu after native cold startup. Same-update ordering, metadata
  independence, bounded crash/carrier startup and suspension counters pass.
  Replay delivery milestone 100%; original cadence/full recorded parity open.
  See `../analysis/native_loop_replay_milestone.md`.
- Connected: C17F8C collision sound and C06C02 release fault. The sealed crash
  prefix reaches two source postflight resets at iteration 2150; original
  view/record and collision-child comparisons pass. Bounded collision/reset
  milestone 100%; loaded samples/full recorded parity remain open. See
  `../analysis/native_collision_milestone.md`.
- Connected: C0F920/C08F26 sequence return, source renderer-bank clearing and
  return/reselection at the native menu. The entire sealed crash input reaches
  its three-crash/menu outcome. Four original reset cases and qualification /
  frontend regressions pass. Sequence-return milestone 100%; one of three
  recorded scenarios functionally complete, full frame parity still open.
  See `../analysis/native_sequence_return_milestone.md`.
- Connected: original consumed-key export at C1AD74 and native
  `FA18_GAME_INPUT_V1` replay. Injection and consumption are now distinguished;
  turn/hook state matches an original checkpoint through iteration 5508,
  apart from two existing startup differences. Input-alignment milestone 100%;
  carrier touchdown reaches the missing C083E2 mission reset. See
  `../analysis/native_game_input_milestone.md`.
- Connected: C149BE -> C083E2 carrier touchdown mission reset. Eight original
  reset cases and actual landing/contact/motion state through iteration 6288
  pass; the old helper's wrong player-phase address is corrected. Touchdown
  milestone 100%; success result callback C11078 remains open. See
  `../analysis/native_carrier_touchdown_milestone.md`.
- Next: connect C11078/C110A4 qualification results, carrier-success/demo outcomes,
  complete C0EFD4 frame ownership, remaining HUD/end-of-frame owners,
  remaining active-record children (C1C63E/C22C80), and flight acceptance.
  Current native endpoint is `scene-setup`.
  The earlier supposed crash banner was original disk text, CRACKED BY A-HA.
- Still open: demo/remaining modes, active-flight exit/restart, outcomes/progression,
  audio, original timing and typed game state. The scene-setup endpoint
  does not establish recorded-flight acceptance or whole-game completion.

Use focused changed-path checks; full sealed replays are reserved for meaningful
gameplay acceptance checkpoints. Copper fade remains excluded.

Scope: the playable runners `fa18_recomp` and `fa18_romfree` built by
`recomp/CMakeLists.txt` and `../scripts/build_recomp.py`. Written 2026-10-06
against the tree at `9c062b80`. Read `../CURRENT_PORT_HANDOFF.md` and
`../AGENTS.md` first; this plan is subordinate to the user's latest instructions.

Frame-by-frame comparisons ignore Copper fade, as the user confirmed on
2026-10-06. Fade-only palette/brightness differences do not fail acceptance.
Geometry, drawing order, other rendering differences and gameplay state still
need to match. Strict RGB hashes remain useful diagnostics but cannot by
themselves reject a run under this comparison policy.

"Emulation removed" means one thing only, and it is binary per subsystem: the
shipped executable links none of

1. **Musashi** — the 68000 interpreter and its register file;
2. **the generated translation** — `recomp/generated/recomp_0*.c`,
   `recomp_static_deferred.c`, `recomp_table.c`, which execute original
   instructions through Musashi opcode handlers over a shared register file;
3. **guest memory** — the big-endian 68000 image and `machine/bus.c`;
4. **the chipset model** — `machine/blitter.c`, Copper, `display.c`, CIAs,
   interrupt model.

Everything below is ordered so that each phase removes a named dependency from
a path the runner actually executes, and so that progress is measured rather
than asserted.

## Where the dependency actually is today

Traced from the runner entry:

`recomp/recomp_main.c` parses arguments, boots either a UAE savestate
(`fa18_machine_load_state`) or, under `FA18_ROMFREE_MAIN`, the original ADF via
`romfree/profile.c` -> `amiga/` OFS/Hunk loading -> an explicit process handoff
that places hunks in guest RAM and sets the guest PC. The frame loop is
`fa18_machine_run_frame()` plus `fa18_loop_frame()` (`recomp_main.c:144-145`,
`:218-219`, `:530-531`). Inside a frame the machine steps Musashi; the recomp
hook dispatches translated routines (`recomp/recomp_runtime.c`), and a routine
whose address appears in `game/glue/ports.c` runs recreated C through its glue
instead. Chipset work is serviced only at instruction boundaries in
`fa18_machine_service()`, which is what keeps the two execution engines on one
timeline.

So **every** recreated C routine today is still reached because an emulated PC
arrived at its original address, operates on guest memory through
`game/memory.h` (`rd_*`/`wr_*` on `gaddr`), and drives output by writing
emulated custom registers. That is the shape of the problem: the C exists, but
the emulator is still the thing that calls it, holds its data, and renders for
it.

Counted baseline at `9c062b80` (historical; not a current reconstruction total):

| Quantity | Count | Source |
| --- | ---: | --- |
| Recreated routines registered as ports | 614 | `game/glue/ports.c` rows |
| Recreated `game/*.c` modules | 146 | file count |
| Glue translation units still required | 247 | `game/glue/*.c` |
| Translated routines / instructions | 624 / 34,309 | `recomp/generated/recomp_manifest.json` |
| Statically recompiled deferred entries | 85 (44 original ADF, 41 reference wrappers) | `recomp_deferred.json` |
| `rd_*`/`wr_*` guest-memory sites in `game/` outside glue | 7,858 | grep |
| `rd_*`/`wr_*` sites inside `game/glue/` | 1,130 | grep |
| `custom_write*` sites in `game/` | 175 | grep |
| `wait_blitter()` sites in `game/` | 23 | grep |
| Registered OS service rows in `os/` | 38 | grep |
| Active CTests | 11 | `recomp/CMakeLists.txt` (4) + `amiga/CMakeLists.txt` (7) |
| Sealed native recordings | 3 | `captures/native/` (demo01, qual_carrier_success, qual_fail_crashes) |

`recomp_deferred.json` still records `cpu_and_chipset_required: true`,
`readable_decompilation_complete: false` and
`whole_original_call_graph_complete: false`. The last of those is the honest
bound on any denominator below: code never discovered cannot be counted.

## Measuring progress

The user wants a percentage. A defensible one has to be measured from the
running game, has to be impossible to raise by writing disconnected code, and
has to reach 100% exactly when a subsystem becomes deletable. The design below
satisfies that; it replaces, and must not be confused with, the 614/699
readable-entry count, which measures routine reconstruction only.

### The headline number: emulated-work share removed

For each scenario in a fixed suite, run the same scenario twice and count the
original work still executed by an emulation engine:

```
removed(scenario) = 1 - emulated_instructions_ON / emulated_instructions_OFF
```

`emulated_instructions` is the schema-2 sum of `interpreted`, `generated`,
`residual` and `adapter`. The last category counts each original opcode fetched
by the hand-written CPU instruction adapters through `step_begin`; source PC,
register-file and bus semantics remain emulation even without an opcode-table
call. Merely scheduling a retained C frame fetches no opcode and is excluded.
OFF is the same scenario with `--ports off`. The old schema omitted adapters;
its percentages are superseded rather than comparable progress measurements.
The deletion gate still requires a build without CPU objects: a zero executed
count alone cannot prove CPU/register independence or whole-game coverage.

**Report the minimum across the suite, never the mean**, together with the
per-scenario table. The suite is the three sealed native recordings plus the
two ADF-only launcher checks (MSVC and GNU); suite coverage is itself a stated
limit of the measurement, and undiscovered code enters the denominator only
when some scenario exercises it.

### The four axis meters

The headline covers the CPU axis only. Three subsystems remain after it reaches
100%, so report four axes side by side, each 0-100%, each with its own
denominator:

| Axis | Metric | 100% means |
| --- | --- | --- |
| **A. CPU** | all four schema-2 instruction categories, as above | no original instruction executes; CPU omission build passes |
| **B. Memory** | guest-bus accesses per frame issued from recreated C and its glue, against the `--ports off` baseline; plus converted `rd_*`/`wr_*` sites (0 of 8,988 today) | game state lives in C objects; `bus.c` unreferenced |
| **C. Chipset/IO** | chipset operations per frame actually serviced (blitter ops, Copper instructions, bitplane fetches, CIA/interrupt events) against the baseline | native drawing, audio and timing; `machine/` unreferenced |
| **D. OS/boot** | service dispatches per scenario reaching guest-resident OS wrappers or `m68k` state, against the baseline; plus the boot handoff | native entry, no guest PC, no hunk placement into guest RAM |

### The gate that keeps the percentages honest

Alongside the meters, report **deletable subsystems: n/4** — the number of the
four subsystems above that the runner can actually be linked without, proved by
building a configuration that omits those objects and passing the suite.
Today this is **0/4**, and it stays 0/4 until a phase completes end to end. A
percentage reported without this gate beside it is not a progress report.

### Building the meter (Phase 0 deliverable)

- Add cheap counters behind the existing `--profile` JSON: per-engine executed
  instruction counts (extend `FA18RecompStats`, which already carries
  `interpreted_game`, `generated_cycles`, `dispatches`); bus accesses split by
  originating engine in `machine/bus.c`; chipset operation counts in
  `machine/machine.c`, `blitter.c`, `display.c`; service dispatch counts in
  `os/service_dispatch_adapter.c`.
- Add `tools/recomp/emulation_meter.py`: runs the suite OFF and ON, joins the
  profile JSONs, writes `analysis/emulation_removal_meter.json` and a markdown
  table, and prints the four axes, the per-scenario minimum and the n/4 gate.
- Record the baseline in `CURRENT_PORT_HANDOFF.md` and commit it. Every later
  phase reports the meter delta it produced; a batch that moves no axis is
  reported as moving no axis.
- While doing this: `tools/recomp/inventory.py` still globs `port/*.h`, which no
  longer exist after `e70a7014`. Fix or retire it rather than feeding stale
  output into the meter.

## Phases

Each phase ends with: the suite passing, the meter delta recorded, and a commit.
Within a phase, work in small batches that are exercised in the runner before
the next batch starts, per `AGENTS.md`.

### Phase 0 — Call graph, meter, baseline

Produce the written startup/frame call graph from `recomp_main.c` through
`fa18_machine_run_frame` to the registered owners, as `CURRENT_PORT_HANDOFF.md`
requires, and the meter above. No behaviour changes.
Exit: baseline numbers published; 0/4 gate stated.

### Phase 1 — Native call graph (axis A)

First connected batch complete: `C2D408` calls `C1342C` directly through the
game owner, with compatibility values published at the parent CPU boundary.
The leaf's CPU adapter/registration are removed; the original generated OFF
implementation remains. All five scenarios exercise the direct edge and retain
identical pre-change outputs/counters. This removes a shared-register child
dependency, but moves no measured axis: raw CPU **38.4011%**, delta **0.0000 pp**,
memory/chipset/boot **0%**, deletion **0/4**. Combined parity remains failing,
so Phase 1's exit gate is still open. Evidence:
`../analysis/emulation_removal_meter_after_native_side.json/.md`.

The seven CPU helper adapters below C1342C are also retired after known-call
ownership validation, parent source checks and unchanged results in all five
scenarios. There are now eight direct C entries and 606 readable CPU registry
rows; reconstruction remains 614 entries. Raw percentage and all cutover axes
are unchanged. Removing 36 unused adapter access sites is not memory cutover.
Evidence: `../analysis/emulation_removal_meter_after_matrix_leaves.json/.md`.
Following the user's instruction, use targeted comparisons and bounded runner
checks for routine batches; reserve full replays for substantial changes,
milestone acceptance and unresolved failures.

C2DD4E and its C2DE96/C2DEA2 helpers are now also C-owned without CPU adapters.
The bounded 800-frame parent probe executes 69 depth calls and preserves prior
outputs/counters; source comparisons and both builds pass. There are 11 direct
C entries plus 603 CPU registrations. No full replay was repeated: reuse the
preceding **38.4011%** raw minimum, with zero observed bounded counter delta,
cutover axes **0%**, deletion **0/4**. The parent's source timing mismatch still
requires work. Evidence: `../analysis/emulation_removal_matrix_depth_batch.json/.md`.

The matrix-side owner also passes its selected record explicitly to six C
helpers, removing their guest CURRENT_RECORD lookups. Three bounded probes
and parent source comparisons preserve behavior and instruction/device counts;
native guest reads decrease by 56/0/106. No full replay was repeated. Raw CPU
estimate remains **38.4011%** from the last full suite; cutover axes **0%**,
deletion **0/4**. Removing six lookup sites is not conversion of state to C
objects. Evidence: `../analysis/emulation_removal_record_arguments_batch.json/.md`.

The live flight parent's C25D9E matrix call now selects the game C owner with
ordinary copied arguments instead of dispatching a C2D408 CPU entry. Its CPU
entry adapter/registry are removed; outer CPU timing/result publication and
the existing 9,500-cycle atomic timing debt remain. Three bounded 800-frame
parent and combined before/after pairs preserve RGB, indices and final RAM;
combined dispatches decrease by 68/15/75. Both builds, twelve CTests, the DMA
parent oracle and argument/IRQ/reset boundary fixture pass. There are twelve
C-owned entries and 602 readable CPU registry rows. Bounded instruction delta
is zero; cached full raw CPU **38.4011%**, accepted percentage unavailable,
memory/chipset/boot cutover **0%**, deletion **0/4**. Combined source parity
still fails at demo/carrier/crash 565/446/263; Phase 1 is incomplete. Evidence:
`../analysis/emulation_removal_matrix_dispatch_batch.json/.md`.

The live input and indexed-record phases now also call game C owners, retiring
C1B27E/C13D84 plus nine exclusive helper CPU adapters. Six paired bounded
800-frame captures preserve drawing/RAM and instruction/device counts;
combined dispatches decrease 110/0/104. Both builds, twelve CTests, GNU profiling
and the DMA/boundary oracle pass. All 309 sandbox source-parent calls match;
shadow's 104 incomplete calls remain incomplete, equal to baseline. There are
23 C-owned entries and 591 readable CPU registry rows. Bounded CPU-work delta
is **0.0000 pp**; cached raw CPU **38.4011%**, other cutover axes **0%**, gate
**0/4**, accepted percentage unavailable. Outer CPU/timing and guest-state
dependencies remain. Next actual CPU-work owner: C2C392, 626 original generated
instructions, called 27 times in the bounded demo. Evidence:
`../analysis/emulation_removal_flight_calls_batch.json/.md`.

The C25C6A call now selects a native forty-arm C2C392 autopilot. Its complete
original owner totals **987 instructions**, including **361 cold instructions**
missed by the previous generated body. A retained C frame owns response limits
and continuation phases; existing runtime children remain original CPU work.
49,600 component cases match all registers/PC/SR/RAM (864/987 instructions
observed); 12,400 production-continuation cases match (841/987); both cover all
361 cold instructions. Unknown/modified action targets retain the exact source
transfer. All builds, twelve CTests, GNU profiling and DMA/boundary checks pass.
The 800-frame demo has 27 native calls, zero C2C392 CPU entries and **2,076 fewer
aggregate CPU instructions**: raw work avoided **69.8779% (+0.0397 pp)** for
that bounded demo only. Carrier/crash counters and outputs are unchanged.
First non-fade source differences are **619/446/263**; parity remains open.
Parent instruction/event timing is unmodeled. Counts: 24 C-owned entries,
591 readable CPU rows, 84 deferred entries, 691 direct opcode bindings.
Cached full raw CPU **38.4011%** and bounded three-recording minimum
**48.0797%** stay unchanged; accepted percentage unavailable. Other cutover
axes **0%**, deletion **0/4**. No full replay suite repeated. Evidence:
`../analysis/emulation_removal_autopilot_batch.json/.md`. Next: remaining
normalization/steering CPU child calls, then C28E28 in the live flight parent.

C25BA6 now selects the native C28E28 zone-exit owner through a retained C
continuation; its CPU entry/registry/guard and sole wrapper file are retired.
16,384 production-continuation cases match all registers/PC/SR/RAM, covering
79/79 original instructions. Both compilers/runners, twelve CTests, GNU
profiling and an MSVC/GNU demo pass. Bounded native calls are 2/0/2; aggregate
CPU instructions unchanged. Demo/carrier frames and all final RAMs match the
preceding build, but crash rendering has a **185-pixel non-fade regression**
starting at frame **556**. Source first differences remain 619/446/263.
Component correctness passes; rendering acceptance stays open, with parent
instruction/event timing unmodeled. Counts 25 C-owned entries / 590 readable
CPU rows / 84 deferred entries. Batch CPU-work delta **0.0000 pp**, bounded
demo raw CPU **69.8779%**, full minimum **38.4011%** cached; accepted percentage
unavailable, other cutover axes **0%**, deletion **0/4**. No full replay suite
repeated. Evidence: `../analysis/emulation_removal_zone_exit_batch.json/.md`.

Remaining C routines call each other by returning into the dispatcher. Give each
recreated routine a direct C entry point and let a C caller call its C callee
directly, keeping glue only for callers that are still generated code.
`PORT.md` already prescribes the end state: when every caller of a routine is
C, its glue is deleted.

- Start at the leaves of the already-C call graph, where both caller and callee
  are registered ports and neither takes an interrupt nor touches hardware
  inside the call — those are exactly the calls `shadow` already compares, so
  the oracle exists.
- Work outward toward the main loop. The main loop itself is the last step:
  once the frame body is C calling C, the dispatcher is reached only for the
  remaining generated entries.
- Measured by: dispatches per frame, glue files deleted, axis A.

Exit: the frame body is native, and the only emulated instructions left are the
deferred entries and undiscovered code (Phase 5).

Native C2C392 now calls six steering game functions directly, returning C
state instead of dispatching their CPU entries. The production continuation
matches 49,600 source cases; six constructed real-runner frame pairs exercise
every child and match source RAM/RGB/indices at the original return boundary.
The probes mask IRQs; parent timing and sealed source parity remain open.
All three bounded 800-frame recording outputs/profiles are unchanged. Both
builds, twelve CTests, profiling and MSVC/GNU demo comparisons pass. Remaining
original callers require the steering CPU adapters; inventory stays 25 C-owned,
590 CPU rows, 84 deferred. Batch delta **0.0000 pp**, bounded demo **69.8779%**,
cached full raw minimum **38.4011%**; accepted share unavailable, other cutover
axes **0%**, deletion **0/4**. No full suite repeated. Evidence:
`../analysis/emulation_removal_steering_calls_batch.json/.md`.

Native C2C392 also calls vector normalization/magnitude directly in game C.
12,400 source cases match complete state with held events (396 calls each,
zero CPU-entry dispatch); a constructed real-runner frame exercises both
edges and matches source RAM/RGB/indices. Normalization adapters remain for
other callers. Both builds, twelve CTests, profiling and bounded recording
comparisons pass. Those prefixes do not exercise this normalization path:
delta **0.0000 pp**, bounded demo **69.8779%**, cached full raw **38.4011%**.
Accepted share unavailable, other axes **0%**, deletion **0/4**, counts unchanged.
No full suite repeated. Outer flight timing/ownership remains work. Evidence:
`../analysis/emulation_removal_normalization_batch.json/.md`.

Latest connected batch: C25B66 now schedules retained game C dynamics and
composes its five native child owners directly. Its 554 CPU instruction cases
are deleted (42.3547% of the shared family); 754 other cases are unchanged.
A frame-end return fix prevents the interpreter consuming a pending C return.
16,384 held-child cases match full state and all 554 boundaries; six focused
runner frame pairs pass. GNU/MSVC both runners, twelve CTests, profiling and
an MSVC/GNU bounded demo pass. Original-child live matrix fixtures still fail
case 2. Three 800-frame probes complete but non-fade changes versus the previous
commit begin at 591/446/263 (39,478/2,989/187,594 pixels); final RAM differs.
These parity gates and unmodeled parent event timing remain open. No full replay
repeated. Bounded demo raw CPU **69.8706%**, delta **-0.0073 pp**;
full raw **38.4011%** cached, accepted share unavailable; other axes **0%**,
deletion **0/4**, counts 25/590/84/691. Raw counters exclude CPU-style port
steps, so they do not measure this body deletion or whole-plan completion.
Evidence: `../analysis/emulation_removal_flight_parent_batch.json/.md`.
Continue outer C22 record-loop C ownership and direct parent integration.

Latest connected batch: C22C80 now runs a retained game `ControlRecordsFrame`
and calls the retained record-dynamics C owner directly. All 226 outer CPU
instruction cases are deleted; the sibling C1C63E's 112 cases are unchanged.
Together with the previous batch, **780/780 cases of these two targeted CPU
bodies (100%) are removed**. Thin entry adapters, guest memory and other
children's runtime boundaries remain, so this is not whole-plan completion.
4,096 original-child core cases match all state. 4,096 production continuation
cases match registers/PC/SR and RAM outside old CPU stack scratch C7FD00..C7FEFF;
strict all-RAM fails case 0 at C7FED5. IRQ/event timing is held in those proofs.
GNU/MSVC both runners, twelve CTests, profiling and the MSVC/GNU demo pass.
Three 800-frame recording probes reach 129/30/150 direct C22C80 -> C25B66 calls
and zero C25B66 CPU-entry dispatch. Non-fade comparisons versus the previous
batch fail at 346/446/263; final RAM differs. Timing/parity remain open. No full
replay repeated. Bounded demo raw **69.8515%**, delta **-0.0191 pp**;
full **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw counters exclude CPU-style port steps and cannot represent whole-plan
completion. Counts remain 25/590/84/691. Evidence:
`../analysis/emulation_removal_record_loop_batch.json/.md`.
Next: C1C63E parent ownership and native root/control/render children; preserve
explicit timing, non-fade, stack and matrix acceptance debt.

Latest connected batch: native C1C63E retains game `RecordUpdateStageFrame`
and calls the C22C80 record loop directly. All 112 update-stage CPU cases are
deleted; the shared C22C80/C1C63E instruction body is now **100% removed**.
This connected chain has removed 892 instruction cases, retaining thin entries,
guest data and other original children. 4,096 original-child core cases match
all state; 4,096 production cases match registers/PC/SR and RAM outside old
CPU stack scratch. Strict all-RAM still fails (case 0 at C7FED1). Both compilers/
runners, twelve CTests and profiling pass. The MSVC/GNU demo matches. Bounded
recordings reach 43/15/75 native stage -> record-loop calls, with zero C22C80
CPU entry dispatch. Previous-build non-fade/index differences start at
381/446/263 (64/1,588/12,536 pixels); final RAM differs. Timing/parity stay open.
No full replay repeated. Bounded raw demo **69.8507%**, delta **-0.0008 pp**;
full raw **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw meter excludes CPU-style adapters; repair that measurement before using it
for further progress estimates. Inventory unchanged. Evidence:
`../analysis/emulation_removal_update_stage_batch.json/.md`.

Latest measurement correction (schema 2): source-instruction adapters are now
counted at `step_begin`, where they fetch/decode an original opcode. Retained C
entry scheduling is excluded. The old schema omitted that work and overstated
CPU removal: cached **38.4011%** and old bounded **69.8507%** are superseded.
The corrected **800-frame, three-recording** raw CPU minimum is **0.4470%**;
per scenario demo **2.5530%**, carrier **0.4470%**, crash **2.4463%**. This is a
partial probe, not a full-suite result or completion percentage. The corrected
full-suite minimum is unmeasured. Accepted share unavailable: source comparisons
still fail. Memory/chipset/boot cutover **0%**, subsystem deletion **0/4**.
No gameplay behavior changed in this meter batch; old demo RGB/index/RAM and
all old profile fields match, MSVC/GNU profiles match, twelve CTests and profiling
invariance pass. No full replay repeated. Evidence:
`../analysis/emulation_removal_adapter_meter_probe.json/.md`; the canonical
`../analysis/emulation_removal_meter.json/.md` now contain this schema-2 partial
probe. Historical schema-1 full evidence remains in git/build history and must
not be used as the current estimate. Continue native game/parent ownership and
report future deltas using schema 2. The connected chain still removes 892
instruction cases, with explicit parity/timing/stack/matrix debt below.

### Phase 2 — Memory cutover (axis B, PORT.md stage F)

Replace `gaddr` plus `rd_*`/`wr_*` with real C structs and pointers, as
`game/memory.h` anticipates.

- Per memory region, prove with the Phase 0 counters that no generated or
  interpreted code reads or writes it in any suite scenario; only then convert
  it. Regions still shared with generated code convert last.
- Convert region by region, not file by file: the unit of safety is the data,
  not the module.
- Keep word-exact arithmetic; the conversion must not change behaviour, and
  live RGB/RAM comparison remains the gate.

Exit: the guest image holds only what still-generated code uses, and axis B
states that remainder exactly.

### Phase 3 — Native IO backend (axis C, PORT.md stage F)

Replace chipset-mediated output with direct host output behind the interfaces
`game/hardware.h` already funnels everything through: `custom_write()`,
`custom_write_ptr()`, `wait_blitter()` — 175 and 23 sites, a small and already
centralised surface.

- Reimplement the drawing those writes express (bitplane targets, blits, Copper
  colour and display setup) as direct framebuffer operations, keeping the
  existing RGB444 frame as the comparison surface so the sealed recordings stay
  the oracle.
- Audio (Paula) is currently not modelled at all; it is new work here, not a
  removal, and must be reported as such rather than counted on axis C.
- Frame timing moves from CIA/beam events to the host pacer already present in
  `recomp/frame_pacer.h`.

Exit: a scenario renders identically with the `machine/` chipset services unused.

### Phase 4 — Boot and OS (axis D)

- Replace the ADF -> Hunk -> guest-RAM -> guest-PC handoff in `romfree/` with a
  native entry that calls the recreated start-up directly. `amiga/` disk, OFS
  and Hunk loading is host code and stays.
- Reduce the 38 service rows to those the native game still calls; services that
  exist only because guest code made a library call disappear with their caller,
  so most of this follows Phases 1-3 rather than leading them.
- Save/load of the original configuration file must keep working against the
  same on-disk bytes.

Exit: no guest PC, no guest stack, no supervisor state.

### Phase 5 — Residual original code (closes axis A)

- The 44 deferred entries backed by original ADF code are real game behaviour
  and need readable decompilation; the 41 reference wrappers should be shown
  unreachable in the native build and dropped, not reimplemented.
- `whole_original_call_graph_complete` is false: before claiming completion, run
  the suite with the interpreter made fatal rather than permissive, so any
  undiscovered path fails loudly instead of falling back silently.
- Then delete Musashi, `recomp/generated/`, the dispatcher and the glue
  directory.

### Phase 6 — Deletion and acceptance

Remove the four subsystems from the build, not merely from the call path — the
gate is a link without them. Acceptance is the whole game, not the suite:
startup, menu, every mission mode, mission outcomes and progression, carrier
success, flight teardown, save/load. The handoff lists mission outcomes and
progression, carrier success and active-flight teardown as unverified even under
emulation; they must be verified before any completion claim.

## Ordering and why

Phase 1 precedes Phase 2 because a native call graph is what makes "no generated
code touches this region" provable. Phase 2 precedes Phase 3 because the drawing
code must own its own data before it can own its own output. Phase 4 trails
because most OS services die with their guest callers. Phase 5 is last because
the deferred entries are the expensive, low-information residue and should not
block the structural work.

## What this plan deliberately does not do

- It does not estimate a completion date. Any estimate must come with its scope,
  assumptions and measured basis, per `AGENTS.md`.
- It does not reuse the 614/699 figure as a progress percentage.
- It does not create a parallel implementation outside `game/`, and adds no C
  sources or headers at the top level of `port/`.
- It does not treat a passing standalone component test as runtime progress;
  only the meter and the suite move the number.

## Execution record — 2026-10-06

Phase 0 call graph and measurement deliverables are published in
`../analysis/emulation_removal_call_graph.md` and
`../analysis/emulation_removal_meter.json/.md`. The runner's existing profile
now includes all executed interpreter/generated/residual opcode instructions,
including three OS RTE helpers, bus API origin/page counts, chipset operations,
service dispatches and native port calls/steps. The inventory header glob now
uses the active `game/` directory.

The full five-scenario baseline's raw minimum CPU-work share removed is
**38.3921%**. Every scenario fails RGB parity and the three native ON final RAM
seals fail; ADF ON/OFF iteration counts differ. The accepted CPU share is
therefore unset. Memory conversion, native chipset and native boot remain
**0%**; deletion gate **0/4**; batch removal delta **0 percentage points**.

Both active toolchains build, all twelve active CTests pass, both full ADF-only
launcher checks pass, and profiling is invisible to RGB/RAM/stdout/stderr in
OFF/ON/interpreter runner checks on both toolchains. An independent unchanged
`9c062b80` build reproduces the full crash replay and ADF startup failures byte
for byte. This validates the instrumentation while retaining existing combined
ON timing debt. Phase 0's suite acceptance prerequisite remains open, as do
Phases 1-6. No emulation subsystem was removed or declared complete.

RAM-page counters observe the guest API. Direct DMA and host compatibility
memory accesses are separate and their absence from those counters must not
be used to claim exclusive native ownership in Phase 2.

### Stores-icon timing prerequisite

The runner's `C30A00` / `C30AE2` stores-icon entries previously executed the
existing C owners with fixed 900/500-cycle charges. That coupled drawing to
the wrong beam timeline: their isolated ON replay first differed at frame
424 (34,144 pixels). The active registry now calls source-derived instruction
steps in `game/glue/glue_hud_stores_step.c`, including the shared early return.
This is a temporary CPU adapter repair, not a native-call-graph cutover.

Both GNU/MSVC runners build; all twelve active CTests pass. The active opcode
oracle matches 90 instructions / 2,880 cases including bus contention. Parent
and independent child shadow/sandbox probes match. The full isolated live gate
matches all three recordings' RGB streams and sealed final RAM. Combined ON
now matches demo01 through frame 564 under strict RGB comparison; frame 565
must be assessed with the Copper-fade exclusion before it is treated as a
rendering failure.

`../analysis/emulation_removal_meter_after_hud_stores.json/.md` records the full
five-scenario rerun: raw minimum CPU work removed **38.4011%**, a **+0.0090
percentage-point** observation. Its strict RGB parity result is diagnostic
under the user's fade exclusion; final-RAM/iteration gates remain open.
Memory/native-chipset/native-boot cutover remain **0%**, deletion gate **0/4**.
No phase or emulation subsystem is completed by this repair.

### Copper-fade comparison implementation

Both runners now expose optional `--index8` output from the existing display
model. The shared comparison checks those selected indices even during black
frames. An RGB difference is excluded only when its unchanged index is 0-15
and both colours belong to that index's original fade sequence at C08510;
upper-palette and unrelated colour changes remain failures. Unequal frame
counts are reported as parity failures, with the common prefix compared.
The meter, live gate, timing probe and comparison image report share this rule.
The historical strict RGB baseline remains in git; the after-stores report now
records the fade-aware results.

The full suite excludes 5,171,713 fade pixels in demo01, 2,061,205 in the carrier
run, and 626,291 in each ADF run. Combined ON still fails drawing/state parity:
the first differing indices are demo01 frame 316 (64 pixels), carrier frame
374 (55,901), crashes frame 213 (1,905), and both ADF runs frame 1460 (26).
Demo01 frame 565 changes indices too; it is not a fade-only difference.
Different native replay frame counts and final RAM seals remain visible.

Both toolchains build, all twelve CTests pass, diagnostic capture preserves
RGB/RAM/stdout/stderr in OFF/ON/interpreter checks on both toolchains, and the
three full isolated stores replays pass the new gate. Policy regression cases
cover allowed fade, unrelated/upper-palette colours, equal-RGB index changes,
black-frame drawing changes and unequal lengths. The full native captures'
RGB/RAM hashes, all profile counters and runner stats match the pre-diagnostic
batch; their completed captures were reused after repairing length handling.
This batch changes no removal axis: raw CPU **38.4011%**, memory/chipset/boot
**0%**, deletion gate **0/4**, removal delta **0 percentage points**.

### Cockpit message timing prerequisite

The real C0EFD4 frame-update step calls C11BFC at C0F12C. Its fixed
3,000-cycle charge independently reproduces the demo frame-316 index mismatch.
The live registry now uses source-timed message boundaries. All 256 original
instructions / 8,192 DMA oracle cases match. Isolated 800-frame demo/carrier/
crash drawing and RAM match OFF; all 42/15/74 shadow and sandbox calls pass.
Both toolchain builds and all twelve CTests pass.

Combined ON still differs at frame 316; a second 320-frame subdivision isolates
C30EAA. This repair removes a timing error, not a CPU/guest-bus dependency.
No full suite rerun: last measured raw CPU **38.4011%** is cached, new full-suite
delta unmeasured; accepted CPU share unset; memory/chipset/boot **0%**, gate
**0/4**. Evidence: `../analysis/emulation_removal_message_timing_batch.json/.md`.

### Image blit timing prerequisite

C30EAA's fixed image charge is replaced by source boundaries for clipping,
blitter waits and four ordered submissions, reusing the C30F46 timing tail.
All 62 instructions / 1,984 DMA cases match; three isolated 800-frame drawing/
RAM comparisons pass. Shadow has zero mismatches (1/3/7 incomplete); sandbox
passes all calls. Both builds, twelve CTests and GNU profiling checks pass.
Message-plus-image timing matches demo through 800. ALL still differs at 316;
a fresh short subdivision over all 603 entries isolates C25482.

This repairs integration timing, not CPU independence. No full suite rerun:
cached raw CPU **38.4011%**, new full-suite delta unmeasured, accepted CPU share
unset; memory/chipset/boot **0%**, gate **0/4**. Evidence:
`../analysis/emulation_removal_image_timing_batch.json/.md`.

### Signed-byte timer timing prerequisite

C25482's fixed 30-cycle adapter independently reproduced demo frame 316. It
now preserves original signed-byte test, decrement and return boundaries.
All four instructions / 128 DMA cases match; three isolated 800-frame drawing/
RAM comparisons match OFF and all 27/15/36 shadow/sandbox calls pass. Both
builds, twelve CTests and GNU profiling checks pass. Message/image/timer group
matches demo through 800. ALL still differs at 316; neither complete registry
half reproduces it alone now. Minimize the remaining interaction next.

This is a temporary CPU timing repair. No full suite rerun: cached raw CPU
**38.4011%**, new delta unmeasured; accepted CPU share unset; memory/chipset/boot
**0%**, gate **0/4**. Evidence:
`../analysis/emulation_removal_timer_timing_batch.json/.md`.

### Compass/selection timing interaction

The C310AA/C12242 pair independently reproduced demo frame 316, while either
entry alone passed that bounded interval. Source-timed multiply/divide/shift
and selection cleanup boundaries now replace both fixed charges. All 39
instructions / 1,248 DMA cases match; three paired 800-frame drawing/RAM probes
and every shadow/sandbox call pass. Both builds, twelve CTests, GNU profiling
checks and an 800-frame MSVC/GNU paired demo pass.

Combined demo first non-fade difference moves from 316 to **565** (683 pixels),
with Copper fade excluded. Carrier/crash still differ at 374/213. A minimized
220-frame crash pair **C265E8,C2D408** reproduces 213; each alone passes that
interval. Reused isolated matrix captures first differ at 584/446/263.
Address original event timing without restoring retired CPU children or tuning
average fees. This is integration repair, not native frame completion.

No full suite rerun: cached raw CPU **38.4011%**, new delta unmeasured; accepted
CPU share unset; memory/chipset/boot **0%**, gate **0/4**. Evidence:
`../analysis/emulation_removal_selection_timing_batch.json/.md`.

### Flagged-slot timing prerequisite

Latest validated prerequisite replaces C265E8's fixed scan charge with
source timing. All 65 instructions / 2,080 DMA cases and isolated 800-frame
comparisons pass; both builds, twelve CTests and GNU profiling checks pass.
Combined carrier moves 374 -> **446**, crash 213 -> **263**, demo stays 565.
No full rerun: cached raw CPU **38.4011%**, new delta unmeasured; native
memory/chipset/boot **0%**, gate **0/4**. This removes a timing error, not a
CPU dependency. Evidence: `../analysis/emulation_removal_slot_timing_batch.json/.md`.
The user's concern about slow removal progress is valid: prioritize connected
native call ownership and deletion next, rather than further adapter expansion.
