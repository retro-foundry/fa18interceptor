# F/A-18 Interceptor: recreated C source

Recreating the source code of *F/A-18 Interceptor* (Intellisoft / Electronic
Arts, Amiga, 1988) as readable C, proven against the original.

The authority is the original disk (`FA-18 Interceptor (1988)(Electronic
Arts)[cr A-Ha].adf`) running on a pinned Engine9000 build (UAE core), with
Kickstart 1.3, A500 PAL OCS, 512 KiB Chip + 512 KiB Slow RAM.

## Where things stand

The game runs natively as C. A mechanical translation of the original 68000
code runs on a small Amiga machine model, in an SDL2 window at 50 Hz.
Hand-written C is replacing the translated routines in source-backed batches;
480 translated game entries and sixty-five original source-only callable entries are
registered. Three sealed native recordings cover the
demo, a successful carrier landing, and qualification failure.

The immediate work is the next complete readable source batch. The user has
deferred the minor one-frame Copper-fade delay; its evidence is retained in
`CURRENT_PORT_HANDOFF.md` for later parity work.
The gauge correction matches all recorded frames and sealed final RAM in
isolation; 359 registered callable entries now have source timing. The complete
registered demo still matches through frame 415. The targeted gauge checkpoint
is complete, as are the placement-ordering parent and two workspace selector
helpers. The complete C1D10C template-placement parent now passes independent
domain, normal CPU-adapter and source-timing checks and is registered.
The complete primary/alternate scene-placement pair also passes 8,192
structural cases and 17,047 completed recorded whole-call comparisons;
its consumers remain explicit child owners. See
[scene-placement proof](analysis/routines/native_c_scene_placements.md).
The complete follow-up placement parent and workspace-position helper are
also registered. Their four-entry timing group matches all 36,236 live frames
and sealed RAM; independent structural proof covers the cold workspace entry.
See [follow-up placement proof](analysis/routines/native_c_followup_placements.md).
The complete C0F5F8 post-input parent now passes 16,384 all-register/full-SR/
all-RAM cases without exclusions, 31,930 completed readable-C replay
comparisons and all 36,236 isolated live frames and sealed RAM. See
[post-input tick proof](analysis/routines/native_c_post_input_tick.md).
Four more complete context-refresh/bootstrap/callback entries pass 32,768
full-register/full-SR/all-RAM cases and every isolated live frame and seal.
Normal readable-C proof includes an independent bootstrap body check on all
three recordings. See [bootstrap proof](analysis/routines/native_c_scene_bootstrap.md).
The complete record-update and enclosing update-stage parents add 338 source
instructions, passing 16,384 full CPU/RAM cases without exclusions and separate
recorded body proofs. See [record-update proof](analysis/routines/native_c_record_update_stage.md).
The complete C29042 active-origin parent includes all 229 cold instructions
beyond its generated list. It passes 32,768 complete CPU/RAM cases, independent
readable-C comparisons, every isolated live frame and final seal. See
[selector-origin proof](analysis/routines/native_c_selector_origin.md).
C0EFD4's complete update sequence and its pending-input/display owners now
pass independent readable-C, cold-path and live timing proofs. At that checkpoint,
coverage was 438/624 and the gate matched 554,286 shadow / 394,909 sandbox calls with
all RAM seals and poison frames exact. See
[update-sequence proof](analysis/routines/native_c_update_sequence.md).
Four complete event-source/raw-key/button owners raised coverage to
442/624. Their independent CPU/RAM proofs cover every source boundary, with
C13D34 explicitly cold in recordings. The full registered gate passes
555,538 shadow / 412,898 sandbox comparisons, all seals and poison exact.
The isolated group matches every recorded live frame; build/ is 0.386 GiB.
See [input-event proof](analysis/routines/native_c_input_events.md).
Complete C1AC28/C1AD74 command dispatch owners now raise coverage to 444/624.
Their 1,104 source instructions include shared actions and reset/error exits.
Independent structural proofs cover every boundary; normal readable-C replay
matches 798 shadow / 891 sandbox calls. The full registered gate passes
555,784 shadow / 413,307 sandbox comparisons, all seals and poison exact.
The isolated pair matches all 36,236 live frames; build/ is 0.472 GiB.
See [command-dispatch proof](analysis/routines/native_c_command_dispatch.md).
The complete postflight scheduler family now raises coverage to 454/624.
Its eleven adapters pass 180,224 full-register/full-SR/all-RAM cases covering
every owned boundary. Nine entries are cold in recordings; their structural
proof stays distinct from the active dispatch/mode-nine replay comparisons.
All 36,236 isolated live frames and seals match. The full registered gate
passes 554,025 shadow / 413,303 sandbox comparisons with poison identical;
the fresh combined DMA oracle passes 14,599 instructions / 467,168 cases.
See [scheduler proof](analysis/routines/native_c_postflight_scheduler.md).
The complete C1B7A6/C1BEE8/C1C214 context publishers and C083A6/C09DD0
selected-record helpers raise coverage to 459/624. Separate original-byte
real-child, shared-body and controlled-child proofs cover all 153 source
boundaries in 106,496 full CPU/RAM cases. All five entries are cold in the
recordings; the generic zero-call rejection remains retained. The full gate
and all 36,236 isolated live frames and RAM seals pass. Local DMA timing
passes 153 instructions / 4,896 cases; independent group coverage is now
14,650 / 468,800, without a fresh combined run for this batch.
See [context-publication proof](analysis/routines/native_c_context_publication.md).
The complete menu-transition family raises coverage to 465/624. Its six
adapters pass 147,456 full CPU/RAM cases, independently covering all 324
boundaries with both real and controlled children, including 66 cold delayed
callback instructions absent from the generated listing. Active readable C
matches 5,476 shadow / 5,332 sandbox calls; four peers remain cold. All isolated
live frames and seals and the full gate pass. The fresh combined DMA oracle
passes 14,963 instructions / 478,816 cases.
See [menu-transition proof](analysis/routines/native_c_menu_transition.md).
The complete menu setup and input/message family raises coverage to 469/624,
including a complete replacement for the existing sound-selector adapter.
Its 122,880 full CPU/RAM cases cover all 141 boundaries separately with real
and controlled children. Independent readable C completes two sandbox calls;
three helpers remain cold and two incomplete shadow calls remain retained.
All live frames, seals and full gates pass. Local DMA timing passes 141 / 4,512;
the independent instruction union is 15,104 / 483,328, without a fresh combined
run for this batch. See [menu-setup proof](analysis/routines/native_c_menu_setup.md).
The ten original source-only menu entries now have C and native activation,
with 245,760 full CPU/RAM cases, 7,680 actual dispatch fixtures and every live
frame and seal matching. Complete child-contract proof covers all 132 boundaries;
the real-child layer retains a narrower OS-returning scope for one load path.
The fresh combined DMA oracle passes 15,219 / 487,008. See
[cold-menu proof](analysis/routines/native_c_menu_cold.md).
Complete menu follow-ups and the table-file owner now raise coverage to 470/624
plus thirteen source-only entries. Their 122,880 full CPU/RAM calls cover every
boundary with controlled children; real-child file proof retains its six-boundary
status gate and explicitly defers original OS/file-load parity. Actual dispatch
passes 3,840 fixtures; normal C matches 19 calls in each reference mode. All
live frames, seals and full gates pass. The fresh combined DMA oracle passes
15,370 instructions / 491,840 cases. See
[menu follow-up proof](analysis/routines/native_c_menu_followup.md).
The nine complete delayed-menu/outcome owners now raise coverage to 471/624
plus nineteen source-only entries. Separate real and controlled children cover
all 238 boundaries in 221,184 full CPU/RAM calls, including the four outcome
table arms. Actual dispatch passes 6,912 fixtures; normal C matches 724 calls
per reference mode. All live frames, seals and full gates pass. The independent
instruction union is 15,563 / 498,016; the last fresh combined run remains
15,370 / 491,840. See [outcome proof](analysis/routines/native_c_menu_outcome.md).
The fourteen complete menu/context return owners now retain 471/624 and
extend source-only entries to twenty-nine: 500 rows and 300 timed entries.
Separate real and controlled children cover all 207 boundaries in 344,064
full CPU/RAM calls. Actual dispatch passes 10,752 fixtures; normal C matches
2,667 calls per reference mode. All live frames, seals and full gates pass.
The independent instruction union is 15,770 / 504,640; the last fresh combined
run remains 15,370 / 491,840. See
[return proof](analysis/routines/native_c_menu_return.md).
The fourteen complete menu/context completion owners raise coverage to 474/624
plus thirty-six source-only entries: 510 rows and 314 timed entries. Separate
real and controlled children cover all 441 boundaries in 344,064 full CPU/RAM
calls. Actual dispatch passes 10,752 fixtures, retaining hardware classifications
separately from completed reference C matches. Normal C matches 4,998 shadow /
5,020 sandbox calls; the timer has no completed recorded C comparisons.
All live frames, seals and full gates pass. The independent instruction union
is 16,211 / 518,752; the last fresh combined run remains 15,370 / 491,840. See
[completion proof](analysis/routines/native_c_menu_context_finish.md).
The thirteen complete postflight/reset/restart owners retain 474/624 and
extend source-only entries to forty: 514 rows and 327 timed entries. Separate
real and controlled children cover all 205 boundaries in 319,488 full CPU/RAM
cases. Actual dispatch passes 9,984 fixtures; normal C matches 1,890 shadow /
1,891 sandbox calls. Four peers are cold and one incomplete shadow call remains
retained. The shared transform sum now preserves original wrapping explicitly;
the fresh combined DMA run passes 16,384 instructions / 524,288 cases.
All live frames, seals and full gates pass. See
[postflight completion proof](analysis/routines/native_c_postflight_completion.md).
Next reconstruct the sixteen sealed postflight-message/text owners and all
four original mode table arms, then continue the original call graph.
The seeded 624 entries do not cover the whole game.
The frame-416 comparison
confirmed a fade starting and finishing two frames late. Source timing for
the scene initializer has removed one delayed frame; one remains inherited
from preceding HUD updates. See the
[visible checkpoint](analysis/routines/native_frame_416_checkpoint.md) and
[planning review](CURRENT_PORT_HANDOFF.md#planning-review-2026-10-02).

See [STATUS.md](STATUS.md) for the numbers,
[CURRENT_PORT_HANDOFF.md](CURRENT_PORT_HANDOFF.md) for the next steps, and
[PORT.md](PORT.md) for how the port works.

## Quick start

Build (Windows; CMake with MSVC, or gcc):

```sh
cmake -S port/recomp -B build/recomp-cmake
cmake --build build/recomp-cmake --config Release
sh scripts/build_recomp.sh            # headless gcc build: build/recomp/fa18_recomp.exe
```

The headless build uses Ninja to cache each source file and its header
dependencies. Structural oracles share those objects, so routine bridge edits
normally require one compile and one link.
The whole-call proof tool checks recorded children independently when a
selected parent absorbs their batch comparison, retaining all raw reports.

Open the native demo start state (click the window to capture the mouse, F12
releases it):

```sh
build/recomp-cmake/Release/fa18_recomp.exe --state captures/native/demo01/state.bin \
    --rom local/system/kick13.rom --window --frames 0
```

Check the work:

```sh
sh scripts/recomp_ports_check.sh   # three sealed native recordings, shadow/sandbox/poison
sh scripts/recomp_live_check.sh    # fresh source OFF vs live ON frames and sealed final RAM
```

Set `PORTS_ONLY` to a comma-separated registered batch for its isolated live
check. The live gate checks final RAM during the existing ON frame replay
and removes temporary frame/RAM outputs afterward.

Large replay and trace artifacts are bounded automatically. Headless builds
prune old disposable files when `build/` exceeds 12 GiB, while preserving
compiler outputs and preferring the current `frames_shadow_*.bin` references.
Run `python scripts/prune_build_artifacts.py` directly to prune on demand, or
set `FA18_BUILD_MAX_GIB` to change the cache budget. Individual RGB444 outputs
are limited to 4 GiB and boundary traces to 1 GiB; set the corresponding
`FA18_RGB444_MAX_MIB` or `FA18_BOUNDARY_TRACE_MAX_MIB` value to zero only for
an intentional unlimited capture.

`local/` (ROM, extracted files, toolchain) and `captures/` (sealed
recordings) are not in git.

## Layout

| Path | Contents |
| --- | --- |
| `port/game/` | The recreated game source |
| `port/game/glue/` | Temporary adapters between translated callers and the new C |
| `port/machine/` | Amiga machine model: bus, blitter, Copper, display, CIAs, savestates, input |
| `port/recomp/` | Runtime for translated code, port dispatch and the shadow proof, `generated/` output |
| `tools/recomp/` | Translator, register liveness, porting tools |
| `tools/musashi/` | Vendored Musashi 68000 core (reference CPU and fallback) |
| `scripts/` | Emulator bridge, recording, parity, lockstep and proof scripts |
| `captures/` | Sealed recordings (read-only; not in git) |
| `analysis/` | Memory map, routine reports, inventories |
| `pcode/`, `source_amiga/` | Earlier analysis products: P-code exports, byte-exact assembly |
| `port/*.c` (top level) | The earlier bottom-up port (`fa18_port`), kept as source material |

## Documents

| File | Purpose |
| --- | --- |
| [STATUS.md](STATUS.md) | Current state and numbers |
| [CURRENT_PORT_HANDOFF.md](CURRENT_PORT_HANDOFF.md) | Current count, timing blockers, next batch and gates |
| [PORT.md](PORT.md) | Port architecture, stages, proof method, conventions |
| [RE_COMPLETION_PLAN.md](RE_COMPLETION_PLAN.md) | Analysis plan and how it feeds the C source |
| [GAME.md](GAME.md) | Game dossier: history, controls, landmarks, experiments |
| [AMIGA.md](AMIGA.md) | Amiga hardware and OS guide for reverse engineering |
| [REVERSE_ENGINEERING.md](REVERSE_ENGINEERING.md) | The reverse-engineering process, end to end, and its lessons |

## Recording new scenarios

Record with `fa18_recomp --window --record OUT.fa18in`, then seal with
`python scripts/seal_native_run.py NAME --state START.bin --input OUT.fa18in`.
The result is a read-only `captures/native/NAME/` recording. Archived
`captures/uae/` runs remain source evidence; they are not the current gate.

## Rules

- Never present an emulator frame as native output; emulator frames are
  comparison oracles only.
- Do not run Ghidra or its import/export scripts on this account. Use the
  existing `pcode/` exports, `scripts/disasm_game.py`, `source_amiga/` and
  emulator traces.
- `scripts/check_native_build.py`, `scripts/native_frame_count.py` and
  `port/native_data_allowlist.txt` are owned by the user. Do not edit them.
- A recreated routine is done only when `scripts/recomp_ports_check.sh`
  passes on all native recordings and live ON RGB frames match the native
  shadow reference for affected scenarios.
