# Native port handoff — 2026-10-06

## Objective and user constraints

The active goal is **get gameplay going and matching the recorded runs**.
It is still active and unfulfilled. The user requested this file to transfer
context; this is not a request to cancel or mark the gameplay goal complete.

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

Branch: `coverage-accounting`. The latest implementation batch connects keyboard
countermeasures; use `git log -1` for its commit. Prior scene implementation:
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
`python tools/native/check_countermeasures.py`. Its exports persist under
`build/native-flight/countermeasure-check/`. Depleted pending recorder carry
remains an explicit unsupported path. Full gameplay
goal remains active.

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

`native/menu.c:carried_selection()` still aborts for depleted pending recorder
countermeasures: their inherited selection needs the real source caller
contract. Keyboard flare/chaff, successful pending commands and $FD function
keys are connected. Do not invent a carry for the
remaining paths. Control-effect component/face children are now connected;
complete component-hit/weapon-kill sequences remain unverified.
Other command/dynamics/model dispatches retain explicit missing-child failures.
Some menu modes only reach their banner; native flight currently enables modes
1, 9, 127 and demonstration mode 3 with recorder mode 3.

Whole gameplay sequences under the revised timing scope, complete
record/flag/counter state and post-result behavior remain unverified. Exact
audio fidelity is also unverified. The native link's omission
of emulation objects is real evidence for this runner's current connected scope,
not proof that every original game path has been ported. The gameplay goal must
remain active until its full requested end state is implemented and verified.
