# Native port handoff — 2026-10-06

## Objective and user constraints

Latest connected HUD marker return (2026-10-07): line sizes and defined
clipped-start X deltas now reach first depleted recorder flare/chaff commands.
Other rejected starts preserve the preceding result. 79 full bodies and 96
input parents match original RAM/drawing, including twelve fresh Free Flight
marker frames. Two snapshots each pass 128 line-return/RAM cases; focused HUD
checks pass 492 RAM cases and 252 returns. Ten affected checks and all builds
pass. See `analysis/native_hud_marker_input_return.md`. Active grid/aircraft
marker composition, remaining HUD/command contracts and full acceptance stay open.

Latest small-text fault return (2026-10-07): the original release fault hook
is RTS, so an odd destination preserves the selected glyph after setting $46.
The native renderer now retains that result through its existing drawing skip.
480 HUD non-stack RAM cases and 237 defined returns match, including 27 odd
fault returns across cockpit/context views. Six affected checks and all builds
pass. See `analysis/native_small_text_fault_return.md`. Full port remains open.

Latest connected scene-label return (2026-10-07): scene row-Z loads, matrix
products and position-number text results now reach first depleted recorder
flare/chaff commands. Terminator and alternate skipped-row exits match source;
skipped labels preserve the context speed/altitude/heading HUD result. The suite
matches 67 complete bodies and 84 recorder input parents, including twelve
label frames. 256 label returns/non-stack RAM cases and 450 HUD RAM cases with
207 defined returns pass. Ten affected checks and all native/reference builds
pass. See `analysis/native_scene_label_input_return.md`. Active grid/marker,
remaining HUD/command contracts and whole-game acceptance remain open.

Latest connected debug return (2026-10-07): four-plane text character/glyph
results now reach first depleted recorder flare/chaff commands through the
real numeric overlay. Inactive overlays preserve the preceding result.
Fifty-five complete bodies and 72 recorder input parents match original
compared RAM/drawing, including twelve one-/three-field debug frames.
256 gated debug returns match original source, with clipping and odd-window
selection cases. Ten affected checks and all native/reference builds pass.
See `analysis/native_debug_text_input_return.md`. Scene-label, marker, earlier
HUD and intervening-command returns and whole-game acceptance remain open.

Latest connected HUD return (2026-10-07): source bar destinations, small-text
character/glyph results and periodic cockpit redraw pass counts now reach the
first depleted recorder flare/chaff command. Forty-three actual flight bodies
and 60 recorder input parents match original compared RAM/drawing, including
twelve new HUD/redraw cases. Focused checks match 405 HUD RAM cases and 117
defined returns with clipped and skipped paths. Nine affected checks pass;
Release/Debug and both reference runners build. Selected unfinished drawing
and intervening-command returns remain explicit missing contracts. See
`analysis/native_hud_input_returns.md`. The complete-port goal remains active.

Latest connected drawing return (2026-10-07): periodic C2F582 page clears
publish their actual pattern for the next first depleted recorder command.
Twelve clears reached by the normal game counter and twelve following input
parents match original independent returns. The complete suite matches 31
flight bodies and 48 recorder input parents, including drawing bytes. Selected
labels/debug overlays invalidate this return; other drawing/command producers
remain explicit missing contracts. See `analysis/native_page_clear_input_return.md`.

Latest connected recorder return (2026-10-07): C32CEE's defined message/glyph
byte now reaches the next first depleted pending flare/chaff command. Nineteen
actual flight bodies and 36 recorder input parents match original compared
RAM/drawing; twelve first-depleted cases verify independently derived original
frame returns and both release-bit states. Inactive message frames do not assign
a new value. The real message owner passes 8,192 CPU/SR/RAM component calls.
Other drawing-derived and intervening-command returns remain unfinished.
See `analysis/native_message_input_carry_milestone.md`. The goal stays active.

Latest user controls report (2026-10-07): rudder and numeric keypad external
views did not work. Native host input now shares the reference physical Amiga
mapper; typed-text translation is no longer used to identify physical keys.
Comma/period and SDL/legacy keypad press/release events are connected. A normal
Free Flight test drives the SDL event queue through the playable host handler;
26 input/stage intervals and 52 bodies match original compared RAM/display,
including opposite rudder control-axis motion, releases and external/cockpit
view changes with either Num Lock flag. See
`analysis/native_host_keys_milestone.md`. Full goal remains unfinished.

Latest user direction (2026-10-07): clear generated output and prevent renewed
disk growth. Workspace 60.501 -> 3.719 GiB; 56.782 GiB reclaimed. Build/CTest
cleanup now uses a 4 GiB budget, native comparisons discard passing RAM, and
manual native captures default to 512 MiB. Active Visual Studio builds, source,
media and recordings are preserved. Original recent windows are verified
`.dat.gz` files, directly readable by the window checker. See
`analysis/workspace_artifact_retention.md` and AGENTS.md retention rules.

The active goal is **get gameplay going and matching the recorded runs**.
It is still active and unfulfilled. The user requested this file to transfer
context; this is not a request to cancel or mark the gameplay goal complete.

Latest startup correction (2026-10-07): cold native entry now invokes C08EE4/
C08EB8 before C08F26. All 16 initial cores and 9,376 complete record cores
across 586 independent demo boundaries match, correcting the player region
flag/countdown gap. Takeoff drawing passes 223/223; later drawing stays 83/363
while player/camera/complete cores pass 363/363. Thirty-two cold source parents,
disk-backed demo contracts, eight runtime gates and Free Flight/postflight
comparisons pass. See `analysis/native_scene_startup_milestone.md`. Full goal
remains active.

- Latest user clarification: exact Amiga frame timing is not a completion
  requirement; this is a native port. Preserve gameplay physics, rules, input
  behavior and source-defined timers, while allowing different rendering and
  presentation cadence. Compare equivalent gameplay states/events rather than
  requiring identical wall-clock timestamps or frame numbers. Intro, loading
  and preflight may also run faster.
- Accepted target: modern hardware completes each rendered frame within a
  20 ms budget and presents every frame without skipping. Do not reproduce
  Amiga missed frames or rendering delays. This is the intended pacing policy,
  not a measured performance guarantee or permission to change physics/timers.
- Ignore **Copper fade** when comparing frames. Do not silently mask HUD pixels,
  loosen physics/input comparisons or invent another exclusion.
- Commit completed, validated batches as work progresses. Give progress estimates
  with a concrete scope and evidence. Routine counts and emulator instruction
  percentages do not measure native gameplay completeness.
- Avoid frequent expensive full original replays. Reuse retained original
  captures and run focused affected-path checks; repeat broad checks only for a
  new change, failure or unresolved concern.
- Original source/disk behavior is authoritative. Do not supply captured game
  states, pixels or fitted clocks as native behavior; controlled test inputs
  belong in validation entries only.
- Preserve `.vscode/`, sealed recordings and unrelated changes. Do not edit
  `scripts/check_native_build.py`, `scripts/native_frame_count.py` or
  `port/native_data_allowlist.txt`.

Read `AGENTS.md`, `CURRENT_PORT_HANDOFF.md`, `PORT.md` and `port/README.md`.
Their historical restrictions against a new runner were explicitly superseded
by the user's native-port direction. Older milestone notes can contain now-fixed
TODOs; current code and the newest evidence take precedence.

## Current checkout and architecture

Branch: `coverage-accounting`. The latest implementation batch fixes the reported
demo outside-view clipping and the coarse matrix clamp; use `git log -1`
for its commit. Prior scene implementation:
**`d5203c52`**. Preserve untracked `.vscode/`.
No build/test process is pending and no user answer or approval is pending.

The playable native runner is **`fa18_native`**:

- Entry, replay, captures and build: `port/native/`.
- Connected game composition: `port/game/native/`, reusing domain C in
  `port/game/`. Shared disk/loading facilities remain in `port/amiga/`.
- `port/native/CMakeLists.txt` is included by `port/recomp/CMakeLists.txt`.
  `fa18_native_runtime` is an object library shared by the actual executable
  and native integration tests, not a separate gameplay implementation.
- The checked native link omits CPU, generated translations, glue, bus and
  chipset objects. Storage still uses original addresses; typed state migration
  and whole-game coverage remain unfinished.
- `fa18_recomp` and `fa18_romfree` are original-behavior reference tools.
  Do not restore the deleted top-level `fa18_port` or add C/H files under `port/`.

Normal build and launch:

```powershell
python scripts/build_native.py
build/native/fa18_native.exe --adf local/media/fa18.adf
```

The build script configures `BUILD_TESTING=OFF`. To build integration tests:

```powershell
cmake -S port/recomp -B build/native-cmake -DFA18_NATIVE_ONLY=ON -DBUILD_TESTING=ON
cmake --build build/native-cmake --config Release --target fa18_native fa18_native_scene_exit_test fa18_native_record_expiry_test --parallel 8
Copy-Item build/native-cmake/native/Release/fa18_native.exe build/native/fa18_native.exe
```

Configure from **`port/recomp`**, not `port/native`. GNU reference/oracle builds
through `scripts/build_recomp.py` share `build/recomp/obj`; do not run those
builds in parallel. Independent MSVC build directories can run concurrently.

## What runs and what parity actually proves

Latest independent later-demo assessment: source 2401..2763/native 2364..2726
at ticks 222..584 matches all 363 named player/camera/phase/control boundaries.
Named motion/rates/pose/matrices and complete record cores match all 5,808
record instances after the cold-start default/level correction. All 363
boundaries have all sixteen cores matching. Together with takeoff, complete
cores match 9,376/9,376 instances across 586 independently aligned boundaries.
Strict two-page drawing passes 83/363 and still exits 1. At tick 508, the
source view-hold timer expires and requests a cockpit redraw earlier in elapsed
time; same-state/clock frame-body and original expiry-parent checks pass. No
mask, fitted timer or game behavior change was introduced. See
`analysis/native_later_demo_assessment.md`; compressed source captures are
retained under `build/native-flight/demo-later-review/`. Independent complete
sequences and seconds-driven drawing assessment remain open.

Native intro, credits, callsign editing, menu/mission/log selection and config
save/reload work. Demonstration, Free Flight and carrier qualification execute
native input, record dynamics, terrain/models, cockpit/HUD, display, messages
and audio. The original cockpit assets `pix/inst5` and `pix/frnt5` are loaded
and drawn. The earlier missing-cockpit issue was real and has been fixed.
SDL stereo output and `--wav PATH` use native PCM; bit-exact original audio
fetch/filter/interrupt timing has not been established.

The three recordings have functional outcomes: **3/3**. The historical strict
recorded-frame sequence acceptance remains **0/3**, but exact Amiga frame timing
is no longer a completion gate. No revised complete sequence assessment has
been performed yet. Do not relabel old failures as new passes or report a
branch's 100% completion as whole-game parity.

Current independent comparison evidence:

| Scope | Result |
| --- | --- |
| Five selected demo/carrier checkpoints | Both 320x200 four-plane pages, phase/controls and named player motion/pose/matrices match |
| Consecutive demo window, source 2401..2528 / native 2364..2491 | Player motion/pose/matrices, phase and controls match 128/128 |
| Same window, both complete drawing pages | 53/128 match, **41.4% of this window** |
| Five retained demo boundaries, ticks 222/272/273/322/349 | Named motion/rates/orientation/matrices agree for all 16 flight records; sampled evidence only |
| Original frame-body composition | Eight previously sampled ordinary bodies passed; latest active/crash/map rechecks pass |

Frame-body composition compares original code on the native body's starting
state and clock interval. It proves assembled behavior given those inputs,
not independent original task/display pacing or a complete recording.
Its scratch, asynchronous voice and blitter-busy exclusions are explicit in
`analysis/native_frame_body_milestone.md` and the oracle. Independent checkpoints
compare a narrower named state scope; record flags/counters and full RAM parity
remain separate requirements.

## Main unresolved gameplay-frame difference

The first independent window failure is **source update 2452 / native update
2415, game tick 273**. Twenty differing bytes in plane 3 at y=192 belong to the
target-info line: original **HDG:166**, native still **ALT:5110**. Later drawing
differences remain in that information-line region; the last two boundaries
agree again. This is a real HUD difference, not Copper fade.

The source path is C25312 / `begin_main_loop_timers()` -> C25482 / `tick_timer()`
on INFO_REQUEST (`C45886`) -> `message_line.c:info_line()` -> INFO_PAGE (`C459C4`).
It cycles ALT/HDG/SPD when sampled seconds advance. Across the retained window
the original timer advances **18,000 ms**, native **8,560 ms**.

Both select C2502E rate-table entry 15, **67 ms**, and use the same timer
arithmetic. No wrong index or arithmetic bug has been found. Native approaches
that rate limit; original rendering takes longer. `native/clock.c` samples
whole 20 ms PAL ticks; `native/display.c` currently yields WaitBOVP until the
next whole host PAL boundary. Original viewport/task/sub-PAL pacing remains
unverified, but reproducing that pacing is not required. A seconds-driven HUD
can show a different page at the same update count when elapsed time differs;
verify its timer and page transitions at equivalent elapsed game time before
calling this a gameplay defect. Do not add a guessed multiplier, fitted delay
or captured clock to reproduce the original machine's rendering cost.

C16D04 also publishes fractional time at C45AF6. C28782/C28CAE can use its low
word/byte to derive geometry offsets at C45B18/C45B1A. This is a possible later
clock dependency, but it does not cause this retained window's HUD mismatch.

**Next priority:** finish unsupported gameplay commands/modes and assess later
gameplay against equivalent source states/events using retained evidence first.
Check physics, collisions, AI, mission outcomes and source-defined timer behavior;
do not spend implementation effort reproducing Amiga rendering delays. Keep
strict frame comparisons as diagnostics, and separately classify differences
caused only by elapsed-time/presentation alignment. Do not blanket-mask HUD
differences. The two latest commits fixed genuine aborts; the unchanged 53/128
strict drawing result is diagnostic evidence, not a completion percentage.

## Latest completed commits

### Demo attached camera and extended matrix limits — 2026-10-07

The reported clipping just after demo takeoff came from passing an already
rotated vertex into C1F2EE's attached-camera projection. The original reloads
the unrotated model vertex at A1-6; native composition now does the same.
The barrel roll also occurs in the original playback and is preserved.
Source/native camera position and plane pose already matched. All 223
consecutive takeoff boundaries now match both complete drawing pages, camera
state and named player motion/pose/matrices (previously 153 drawing matches).
The independent comparator now includes named camera state; mutation tests
reject incorrect observer/camera matrices. Ten actual frame-body cases pass,
including three new outside-takeoff/roll/bank regressions.

Extended modes five through eight also exposed C2E048's signed-long coarse
clamp: native narrowed before comparing. The clamp now precedes narrowing.
576 complete matrix cases and all seven actual record checkpoints pass,
including the formerly failing mode-seven body. Normal probes match 232
input/stage intervals and 726 sampled bodies, including active postflight
dynamics. Mode five's player stays stationary under these inputs while other
aircraft fly; its probe requires observed non-player aircraft movement.
All eleven selected runtime gates, the final renderer regressions, full
native/reference builds and final frontend/link check pass. The public native
executable is refreshed. See `analysis/native_outside_camera_milestone.md`.
Artifacts are in `build/native-flight/demo-takeoff-review/` and
`build/native-flight/combat-probe/clamp-5` through `clamp-8`.

This accepts the compared takeoff segment and connected mission samples,
not complete independent flights or mission success. Remaining whole-game,
typed-state, audio and performance requirements stay active; the later
seconds-driven HUD comparison remains separately unaccepted.

### Keyboard flare/chaff and connected control effects — 2026-10-07

Real Free Flight F/C now consume stock, post source messages and reach C1518C's
launch/update/draw children through the actual scene. C159AE/C15AD4/C257EC
derive launch direction; existing collision/ground owners and source
C2CCA0/C2CD28/C2CD94 drawing are connected. Source C1AE02/C1AE08 defines the
keyboard depleted-stock carry byte; successful pending commands save their
own event. SHIFT-F mode 6 calls source sound 6.

Validation: 688 complete input-parent cases; 192 complete control-effect-parent
cases including visible lines/layers; four actual keyboard-driven native bodies
covering flare/chaff launch and early ground-contact expiry. All four bodies
match original compared gameplay state and every display byte. Active/crash/map,
queued input, three native integration tests, frontend/link omission and twelve
reference host/loading checks pass. No full original replay repeated. Component
and runtime evidence are distinguished in
`analysis/native_countermeasures_milestone.md`.

Build `fa18_native_countermeasures_test` with the native runner, then run
`python tools/native/check_countermeasures.py`. Reports/logs persist under
`build/native-flight/countermeasure-check/`; passing raw RAM is temporary.
First depleted recorder commands with an unclaimed queue remain unsupported.
Full gameplay goal remains active.

Follow-up collision connection: C266AE now consumes source C26CC0/C26D8A
typed returns; the component child reuses C27456's existing face-plane owner.
Expanded C1518C source comparisons pass 256 cases, reaching 256 component,
32 face and 128 plane child calls (32 hits / 220 misses). Four controlled
C1518C parents using actual native runtime objects also match original RAM
and display: two face hits, two component misses. Those exports bracket only
the control parent; they are separate from the four keyboard-driven full frame
bodies, which still pass. Motion-helper contract/real-child regressions and
frontend/native omission checks pass. See the milestone's follow-up section.

Recorder $FD follow-up: its inherited selection is dead to publication
(C1BCEE -> C1BEDA -> C1C23C), so function keys now proceed without aborting.
Complete input comparisons pass 848 cases, including 160 new $FD combinations;
two controlled actual native C0F3C4 parents match original compared RAM.
The `frame.fd.N` files bracket only input, separate from complete frame bodies.
Native menu/function-key/pilot-log regression checks pass.

Claimed-recorder follow-up (2026-10-07): C1C23C tests KEY_TAKEN before inspecting
the event release bit or touching the queue. Depleted flare/chaff commands can
therefore finish once a prior command has claimed input without reconstructing
their dead inherited event. Stock/messages and modifier clears retain their
original owners. All 2,512 complete input parents match original compared RAM,
including 1,536 new cases covering every stock byte for each effect and recorder
modes 1/2/3 with nonzero original incoming D4. Twenty-four controlled C0F3C4
parents using the playable runtime also match, including twelve sequences
where a successful flare claims input before depleted chaff. These parents
bracket input only; four ordinary keyboard-driven full bodies still match
gameplay and every display byte. No comparison exclusions changed. A first
depleted command with an unclaimed queue still requires its actual producer;
no carry value is guessed for that case.

### `d5203c52` — C0DA38 alternate selection and enclosing-frame exit

Actual call chain: frontend -> flight -> scene -> UPDATE_BUFFERS -> C0D730.
Bit $2000 selects C0DA38. It writes the four viewport corners at CORNER_RECORDS,
tests a signed threshold against 14400 and returns accept/reject. **Both**
outcomes unlink C0EFD4's frame on this path, skipping the remaining scene,
HUD, timers, counter and C32CEE final message; outer display still proceeds.
C0DA38 itself is not a page-presentation service.

`display_records.c:prepare_full_display_selection()` shares the existing
ordinary rectangle behavior. `submit_update_display_buffers()` now returns
`UpdateSequenceResult` so `owner_finished` is propagated. Flight returns
NATIVE_FLIGHT_OWNER_EXIT; frontend skips the final message and resumes display.

Validation: 18 original signed-threshold/mode/direct/enclosing-call cases;
native disk/input-backed frontend integration; complete original C0EFEA-to-exit
prefix, **5,253 instructions, zero compared gameplay/display differences**.
The controlled fixture is demo update 2365, tick 223, PAL 5189..5189.
It does not prove a particular player input naturally activates this branch.
See `analysis/native_scene_exit_milestone.md`.

### `311d09b8` — C22ADE destroyed-record expiry and target cleanup

C22AC0's reached destruction branch now sets timer +$4C to 15, clears $0200,
sets $0400, and calls existing C09DD0 cleanup before descriptor drawing.
Only a matching selected record is cleared; WARNING_CAUSES is masked with
$FFFFBDFF and C25704 posts **TARGET DESTROYED**, $4016. The historical
CONTEXT_PUBLISH_SELECTION_TONE hook name is misleading: it posts a message,
not an immediate tone. C22C70 is an empty source render hook. Repeated rendering
does not restart expiry or repost the message.

Twenty controlled source descriptor cases and a disk/input-backed native scene
test pass. The test derives the visible record from the actual render list;
the cockpit need not render the player's own exterior model. This verifies
rendering/cleanup, not a complete input-driven kill or collision sequence.
See `analysis/native_record_expiry_milestone.md`; its older remaining-C0DA38
statement is superseded by `d5203c52`.

### Prior comparison/input commits

- `e2cd4eab`: independent comparisons at equal **pre-input** boundaries and
  source-defined draw/display page roles. Previous key-release and physical
  buffer comparisons produced false mismatches. Verifier mutation checks keep
  HUD, input, motion, publication and wrong-frame comparisons strict.
- `85d302bf`: retained consecutive 128-update original window and native capture
  ranges; isolated the real HUD clock mismatch.
- `2f216caf`: complete source PAL mouse/button/input callback connection,
  including source wrap/clamps/bounds and waits. Existing audio, HUD, cockpit,
  stores, labels and frame-tail milestones are listed in the current handoff.

## Capture alignment and retained artifacts

Source dumps at C0EFD4 are **before input/stage**. Native `.entry.dat` observes
that same phase. `.before.dat` is later, at C0EFEA, after input/stage; do not
compare it to a source pre-input dump across a key edge. `.after.dat` normally
ends at C0F3C0. C0DA38 exits instead report `frame_owner_exit: true`; their
`.after.dat` is at the owner return. The normal body checker rejects this
alternate boundary; the oracle supports explicit `owner-exit` fixtures.

C2F558 selects the drawing table and C1612C publishes then swaps DRAW_PAGE.
Compare draw/display roles, not physical buffer numbering. Carrier source
draw page 1 corresponds to native page 0. Validate PAGE_PLANE_TABLE against
each run's own DRAW_PAGE; this is not permission to search for similar frames.

Native `--frame-capture FIRST+COUNT PREFIX` produces
`PREFIX.ITERATION.entry.dat`, `.before.dat`, `.after.dat`. For a single capture,
the iteration is omitted from filenames. Reference `FA18_LOOP_DUMP=FIRST+COUNT:PREFIX`
produces `PREFIX.ITERATION.dat`; its single form uses the supplied path.

Useful retained files under `build/native-flight/`:

- `gameplay-window-source.2401.dat` through `.2528.dat`: original pre-input
  boundaries. Their one bounded original prefix agrees with the previously
  retained final RAM/registers; no need to repeat it casually.
- `gameplay-window-comparison/native.2364.entry.dat` through `.2491.entry.dat`,
  corresponding body exports and `comparison.json`.
- `reference-demo2401.dat`: accepted against native entry 2364, tick 222.
  `reference-demo-initial.dat` and `reference-demo6000frames.dat` are additional
  reference evidence; the latter ends mid-phase and needs phase-aware handling.
- `reference-carrier5101.dat`, `reference-carrier5501.dat`,
  `reference-carrier6001.dat`, `reference-carrier-approach.dat`: accepted native
  entries 5101/5501/6001/6289, ticks 412/812/1312/1600.
- **`carrier-game-input.fa18in`**: corrected carrier consumed-key input,
  554 events, end update 8038. Earlier carrier input variants were wrong.
- `scene-exit-check/`: current controlled native `.before/.after` captures,
  original `.source.dat` and `capture.json` for the alternate-exit proof.
- `input-callback-aligned-frame2364.before.dat` and `.after.dat`: prior normal
  body fixture. **`input-callback-aligned2364.dat` is not a matching pre-input
  checkpoint**; it is a different frame phase.
- `sample-freeflight6100.png` and `.wav`: actual cockpit/audio review artifacts.

Sealed demo input: `captures/native/demo01/input.fa18in`, end 4892.
Sealed crash input: `captures/native/qual_fail_crashes/input.fa18in`, end 3082.
Original `captures/native/*/state.bin` files belong only to reference/oracle
loading; native starts from disk/input.

RAM exports contain 1 MiB (512 KiB chip + 512 KiB slow), sometimes followed by
72 original register bytes. Address offset is `a` below $80000, otherwise
`a - $C00000 + $80000`. Compare phases before interpreting any byte difference.

## Useful commands and latest validation

These are available checks, not instructions to rerun every check on resumption.

```powershell
# Reuse independent original checkpoints; starts native from disk/input.
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-demo2401.dat --iteration 2364
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-carrier-approach.dat --input build/native-flight/carrier-game-input.fa18in --iteration 6289

# Known HUD difference: currently exits 1 and retains every failure.
python tools/native/check_gameplay_window.py --source-prefix build/native-flight/gameplay-window-source --source-first 2401 --native-first 2364 --count 128 --out build/native-flight/gameplay-window-comparison

# Strictness using an existing accepted pair; no game run.
python tools/native/check_gameplay_comparison.py --source build/native-flight/gameplay-window-source.2401.dat --native build/native-flight/gameplay-window-comparison/native.2364.entry.dat

# Focused original/native alternate-exit proof; GNU builds sequentially.
python tools/native/check_scene_exit.py
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_(scene_exit|record_expiry)$'

# Ordinary assembled frame bodies and native/frontend omission gate.
python tools/native/check_frame_body.py --case active --case crash-flight --case map
python tools/native/check_frontend.py --runner build/native/fa18_native.exe --map build/native-cmake/native/fa18_native.map
```

Latest validated batch also rebuilt MSVC `fa18_recomp` and passed twelve
reference host/loader tests:

```powershell
ctest --test-dir build/recomp-cmake -C Release --output-on-failure -R '^(fa18_host_|fa18_clean_machine|fa18_emulation_meter|amiga_)'
```

Logs: `build/native-flight/scene-exit-check.log`, `scene-exit-frame-check.log`,
`scene-exit-frontend-check.log`, `scene-exit-reference-build.log`.
No full original replay was repeated in the last two implementation batches.

## Other unfinished scope

`native/menu.c:carried_selection()` still aborts for a first depleted pending
recorder countermeasure when KEY_TAKEN is zero: its inherited selection needs
the real source caller contract. Keyboard flare/chaff, successful pending
commands, claimed-queue depleted commands and $FD function keys are connected.
Do not invent a carry for the
remaining paths. Control-effect component/face children are now connected;
complete component-hit/weapon-kill sequences remain unverified.
Other command/dynamics/model dispatches retain explicit missing-child failures.
Some menu modes only reach their banner; native flight currently enables modes
1, 2, 3, 4, 5, 6, 7, 9, 125 and 127. Digit 3's mode 2
now reaches its source prompts, record-4 playback, flight failure and menu
return. 32 actual input/stage intervals and 21 sampled bodies match original
instructions' compared RAM/display. The transition preserves the source
saved-pointer sort choice; filled circles retain the original fixed address
offsets. Further mode-2 streams, resets and complete outcomes remain open.
Native page allocation now retains the original descending 320x200 plane
layout, independently confirmed in retained original demo/carrier RAM.
See `analysis/native_mode_two_milestone.md`;
`python tools/native/check_mode_two.py` reuses the actual runtime objects.

Digit 4's mode 125 now connects its source restore setup and reaches sustained
flight. Model command $120 retains C1FEE4's clock-controlled 18-byte skip;
the native record loop dispatches mode 125 to source scheduler C0A334.
The normal disk/input run reaches 2,211 scene/HUD frames, Escape/menu return
and re-entry. 55 actual input/stage intervals and 35 bodies match compared
original RAM/display. Mode-2 source regression and seven affected native
integration tests pass. No new comparison exclusions were added.
See `analysis/native_mode_125_milestone.md` and run
`python tools/native/check_mode_two.py --mode 125 --out build/native-flight/restore-check/original-check`.
Complete mode-125 outcomes and sequence acceptance remain open.

Next-mission digit 7 now uses the original pilot-log field to select mode 6
and runs briefing/context/aircraft setup into sustained flight. The normal
disk/input run reaches 401 scene/HUD frames. 38 actual input/stage intervals
and 27 sampled bodies match compared original RAM/display. The source
C0FECE transition sorts all lists from its saved A4=C29662 byte, independent
of the request mask; mode-2's already preserved choice remains intact.
C24FA4 message lookup, C29368 origin selection, setup engine/noise and
C0A15C's scheduler are connected. Eight affected native integration tests,
mode-2/mode-125 source regressions and current frontend checks pass.
See `analysis/native_mode_six_milestone.md`; run
`python tools/native/check_mode_two.py --mode 6 --out build/native-flight/mission-six/original-check`.
Mode-6 completion/outcome children, cancellations and variants remain open.
Mission-list F1 now runs normal mode 3 (recorder mode 0) through briefing,
source aircraft selection and flight. Both aircraft choices reach 969
scene/HUD frames and each passes 43 actual input/stage intervals / 29 sampled
bodies against compared original RAM/display. Source draw commands $118
(C1FE68 workspace selector) and $11C (C0CFB6 circle stream) are connected.
65,588 selector cases and 24 complete circle-stream parents match original
instructions; the transition preserves saved A4=C296EE's nonzero sort byte.
The model oracle now verifies the actual descending 320x200 plane layout,
replacing its obsolete ascending 256-row assumption. Ten affected native
integration tests and current frontend/frame-body/reference checks pass.
See `analysis/native_mode_three_milestone.md`; run the shared checker with
`--mode 3`, and with `--mode 3 --aircraft 2` for the second choice, using
separate `--out` directories. Mission completion/combat and variants remain open.
Mission-list F2 now runs normal mode 4 through briefing/context setup into
flight, reaching 2,507 scene/HUD frames. 39 actual input/stage intervals and
27 sampled bodies match compared original RAM/display. The native record
loop calls readable C09EC4 through its source mode-4 scheduler; C0FECE retains
saved A4=C296E4's nonzero sort byte. Thirteen affected native integration tests
and current frontend checks pass. No comparison exclusions were added.
See `analysis/native_mode_four_milestone.md`; run the shared checker with
`--mode 4 --out build/native-flight/mission-four/original-check`.
Complete mode-4 outcomes, combat and other variants remain open.
Mission-list F3 now runs normal mode 5 through briefing/context setup into
flight, reaching 2,312 scene/HUD frames. 39 actual input/stage intervals and
27 sampled bodies match compared original RAM/display. The native record
loop calls readable C0A002 through its source mode-5 scheduler; C0FECE retains
saved A4=C2968A's nonzero sort byte. Seven affected native integration tests
and current frontend checks pass. No comparison exclusions were added.
See `analysis/native_mode_five_milestone.md`; run the shared checker with
`--mode 5 --out build/native-flight/mission-five/original-check`.
Complete mode-5 outcomes, record-restoration branches and combat remain open.
Mission-list F5 now runs source mode 7 using an eligible saved-pilot fixture,
with 2,289 scene/HUD frames. The original disk pilot's availability byte at
offset $18 is zero and still rejects F5. Tests change only that saved-pilot
flag, then reopen through the normal loader before input. 42 actual input/stage
intervals and 29 sampled bodies, including C11078/C110A4, match compared original
RAM/display. Native C0A1E0 calls C1BEE8's readable record-view publication with
the source record-word event and STREAM_MODE; 256 complete publication parents
pass. Model command $FC/C21FA4 passes 64 complete parallelogram-tail cases.
C0FECE preserves saved A4=C296DA's nonzero sort byte. C110A4 uses typed host
locals, leaving gameplay RAM untouched by its private frame; the older result
oracle's six-byte scratch exclusion is removed and 39 result/restart parents
pass. No new comparison exclusions were added. Direct standalone placement
drivers on mode-7 snapshots fail at draw command $C3 in their controlled repeated
expiry fixture; original execution also fails there. That fixture comparison
remains unaccepted, alongside complete outcomes/combat and other variants.
See `analysis/native_mode_seven_milestone.md`; run the shared checker with
`--mode 7 --out build/native-flight/mission-seven/original-check`.
Mission-list F6 now runs source mode 8 with an eligible saved pilot, reaching
2,458 scene/HUD frames. The disk's availability byte at offset $19 remains zero
and still rejects F6. Only the saved-pilot fixture changes that byte, then reloads
through the normal loader before input. Native C0A364's existing readable
scheduler is connected; C0FECE preserves saved A4=C29702's nonzero sort choice.
38 actual input/stage intervals and 27 sampled bodies match compared original
RAM/display with zero differences and no new comparison exclusions. Eleven
affected native integration tests, including current frontend and menu, pass.
See `analysis/native_mode_eight_milestone.md`; run the shared checker with
`--mode 8 --out build/native-flight/mission-eight/original-check`.
Every numbered mission entry now has a connected sampled route. Complete mission
outcomes, reached combat/weapon children and other input/state variants remain
unfinished; startup gates do not measure whole-game completeness.
Normal mode-8 Shift-E now reaches source ejection/failure and returns to the
menu after 739 scene/HUD frames. 46 actual input/stage intervals and 43 sampled
bodies match compared original RAM/display. The reached flag publication,
programmed sound, action-record clone/view/matrix, C22B1A lifetime stream and
draw commands $E0/$100 are connected. 64 additional input parents and 128
additional geometry parents pass. No new comparison exclusions were added.
18 affected native integration tests, three default setup-model comparisons,
reference build and 12 host/loading checks pass.
See `analysis/native_ejection_milestone.md`; run the shared checker with
`--mode 8 --eject --out build/native-flight/ejection/original-check`.
Other input/state variants, successful outcomes and combat remain unfinished.

Whole gameplay sequences under the revised timing scope, complete
record/flag/counter state and post-result behavior remain unverified. Exact
audio fidelity is also unverified. The native link's omission
of emulation objects is real evidence for this runner's current connected scope,
not proof that every original game path has been ported. The gameplay goal must
remain active until its full requested end state is implemented and verified.

Normal mode-8 Return/Space now runs three firing probes: source weapon high
nibbles $30/$20 consume their respective missile stocks, and $10 consumes gun
ammunition. Draw $98/C21E08 has connected readable geometry. The gun path also
preserves C2CCA0's test/clear through the point child's returned plane-word
destination, fixing a one-pixel original-body mismatch. 138 input/stage intervals
and 112 sampled bodies match compared original RAM/display with zero differences
and no new exclusions. Geometry/point components pass 256/64 complete parents.
Actual runner probes reach 2,458 scene/HUD frames. All 21 affected native tests
pass. Default setup models,
countermeasure parents/four bodies, reference build and 12 host/loading checks
pass. These probes do not prove successful hits, full combat or mission outcomes.
See `analysis/native_weapon_firing_milestone.md`; the shared checker accepts
`--mode 8 --weapon 1`, `2` or `3`. The complete-port goal remains active.

Normal mode-4 throttle/stick flight now runs region placement/orientation,
zone exit, draw $B4/C21EF8, NPC missile launches and the player's hit/restart
sequence, reaching 5,970 scene/HUD frames. Native composition fixes the owning
aircraft input to the paired-missile readiness gate, the carrier's retained
whole-list completion flag and the sight tail's inherited viewer. 56 actual
input/stage intervals and 47 sampled bodies match compared original RAM/display
with zero differences and no new exclusions. Derived geometry passes 320
complete parents. The new shared-runtime test is `fa18_native_region_flight`;
run `check_mode_two.py --mode 4 --flight`. Default setup-model checks, reference
runner builds, all 22 affected native tests and 12 host/loading checks pass.
These sampled bodies do not
prove an independent whole flight, all collisions or mission outcomes. Audio
fidelity and the 20 ms target remain unaccepted; the complete-port goal stays
active. See `analysis/native_region_flight_milestone.md`.


Controlled postflight conditions now connect C0A3EA readiness, mode-four
C1BEE8 result view, mode-five C0A12E record restoration, reached C083A6 control
request and C110A4/C24FA4/C11350/C08ED0 result messages and pilot-log updates.
Normal menu startup precedes validation-only terminal flag/position seeding;
all subsequent frames use the actual shared runtime. Nine input/stage intervals
and 25 sampled bodies match compared original RAM/display, with unchanged
RAM/display masks. 63 derived result-parent cases pass, including signed level and
unsigned attempt limits and completion-count wrap. The ready fixture verifies
its 78-byte saved log equals the result table. C1643A remains the explicit shared
native save boundary in these result comparisons; its disk/status/readiness
gates remain to be integrated and verified. C539F4 is DOS Write despite older
load/read helper names. Normal mode-four flight still passes 56 intervals and
47 bodies. All 26 affected native tests, seven native-only host/loading checks
and both reference builds pass. Public build/native/fa18_native.exe is updated.
Run python tools/native/check_postflight_schedule.py; see
analysis/native_postflight_schedule_milestone.md. Normal-input successful
missions, whole independent flights, audio and 20 ms performance acceptance
remain unfinished; the complete-port goal stays active.


Native configuration calls now compose complete source C1643A status/readiness
policy. Enlistment runs C162E4/C0EF08/C16386/C1631C refresh/read/create through
existing Amiga host file services. The read-only OFS mount is retained for the
frontend lifetime; writes use the overlay, including MODE_OLDFILE=1005 behavior.
Source readiness/load-result fields are now initialized. Nine postflight
input/stage intervals and 25 bodies match original RAM/display with complete
game file owners executing and unchanged masks; the C1643A comparison bypass
is removed. 77 result/config parents match RAM and persisted bytes; 2,560
CPU/child-entry contract cases pass. Normal mode-four flight still matches 56
intervals and 47 bodies. Retained recorded carrier input completes native
qualification, persists success, restarts and reloads. All 28 selected native
tests, 12 host/loading checks, default models and reference builds pass. An
existing model probe incorrectly forced aircraft lifetime fixtures onto the
carrier; both previous native and original rendering fail that artificial
case. Normal carrier comparisons remain, and five expiry cases/11 descriptors
pass on a real demo aircraft record. Public native executable is updated.
See analysis/native_config_owner_milestone.md; full mission successes,
independent sequences, audio fidelity and 20 ms acceptance remain unfinished.
The complete-port goal remains active.

Delete callback follow-up: host Delete maps to raw $46; C06BF0 removes and
reinstalls the input callback through existing C1748C/C17456 owners, preserving
mouse state. 976 full input-parent comparisons include 64 new reset cases; 16
additional menu command parents pass. Actual Free Flight Delete/Escape/restart
matches 57 input/stage intervals and 37 sampled bodies. The eligible-pilot
validation fixture now establishes source file readiness by entering
enlistment before saving, then verifies reload. A fresh mode-eight probe exposed
that cached eligible pilots concealed this fixture error in the previous
28-test run. Six fresh pilot runs (seven/eight, ejection and three weapons)
match 264 intervals and 211 bodies. Full native build and six selected native
gates pass. See analysis/native_callback_reset_milestone.md; full gameplay,
audio and performance acceptance remain open. The complete-port goal stays active.

Sustained native mode-six/eight flight now consumes C2C348 -> C06C02 and resumes
the existing readable guidance owner. Original-body checks also correct target
marker returned coordinates, signed attitude publication, retained scaled
matrix divisor and settling from previous roll. Normal throttle/stick/target/
fire probes match 114 input/stage intervals and 465 sampled bodies with unchanged
RAM/display exclusions, including three original guidance-child returns in
mode eight. Mode-four region flight still matches 56 intervals and now 49 bodies.
Component checks pass 512 point-return cases, 128 signed-attitude parents, 512
matrix transforms, 256 settling tails and 3,584 affected projection CPU contracts.
All 36 selected native gates, seven record checkpoints, default models and
native/reference builds pass. Final frontend/link checks pass and the public
native executable is refreshed.
The shared-runtime gates are fa18_native_combat_6 and fa18_native_combat_8; use
check_mode_two.py --mode 6/8 --combat. See
analysis/native_guidance_limits_milestone.md. Full mission successes,
independent complete sequences, readable typed state, audio fidelity and
performance acceptance remain unfinished. The complete-port goal stays active.
