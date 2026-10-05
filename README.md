# F/A-18 Interceptor: recreated C source

Recreating the source code of *F/A-18 Interceptor* (Intellisoft / Electronic
Arts, Amiga, 1988) as readable C, proven against the original.

The authority is the original disk (`FA-18 Interceptor (1988)(Electronic
Arts)[cr A-Ha].adf`) running on a pinned Engine9000 build (UAE core), with
Kickstart 1.3, A500 PAL OCS, 512 KiB Chip + 512 KiB Slow RAM.

## Where things stand

The current playable runners combine recreated C with a mechanical translation
of the original 68000 code on an Amiga machine model, in an SDL2 window at 50 Hz.
Hand-written C is replacing the translated routines in source-backed batches;
539 translated game entries and seventy-five original source-only callable entries are
registered. Three sealed native recordings cover the
demo, a successful carrier landing, and qualification failure.

The remaining **85** translated entries now use static C with direct native
instruction-helper calls, as requested on 2026-10-05. Each carries a
`STATIC_RECOMP` marker and a decompilation TODO in
`port/recomp/generated/recomp_static_deferred.c`, with addresses and source
contracts in `recomp_deferred.json`. The earlier count of 75 describes
already-readable source-only entries. Shared CPU and chipset state remain
required. See [static recompilation and return work](analysis/routines/static_recomp_deferred.md).

The active goal is the complete game without CPU or chipset emulation. The
ROM-free runner needs no Kickstart image but still uses Musashi and the machine
model. The separate CPU-free `port/` runtime remains incomplete. Indexed input
actions have an ordinary-state implementation with 65,536 passing source
comparisons. Complete keyboard/pending selection now passes another 32,768
source comparisons and composes with those indexed actions in a CPU-free
library. All 28 aircraft actions now pass 28,672 further comparisons, with real
native direction, throttle-reset and space-release children. All 16 view/origin/
zoom actions and both actual children now pass 32,768 further comparisons.
All five context actions and their actual geometry/observer children now pass
20,480 further comparisons. Other children, queue publication, complete parent
dispatch, data loading and full runtime integration remain open. See
[native context actions](analysis/routines/native_context_command_input.md),
[native view actions](analysis/routines/native_view_command_input.md),
[native aircraft actions](analysis/routines/native_flight_command_input.md),
[native command input](analysis/routines/native_command_input.md)
and [native indexed controls](analysis/routines/native_indexed_controls.md).

The current display-selection handoff implements seven owners and passes
229,376 whole-call comparisons, local DMA and dispatch smoke. Full integration
validation remains pending; see
[the current handoff](CURRENT_PORT_HANDOFF.md) and
[display-selection proof status](analysis/routines/native_c_display_record_selection.md).

Six complete record-steering owners pass 98,304 whole CPU/PC/SR/RAM calls,
covering all 72 boundaries across the original roll and pitch entries.
Dispatch passes 18,432 calls; four existing owners pass normal C at 162
shadow /162 sandbox, and two cold peers have separate whole-call proofs.
The full 614-row gate, all 36,236 isolated live frames/seals and combined
DMA at 30,239/967,648 pass. Source-timed entries rise to 543. See
[record-steering proof](analysis/routines/native_c_record_steering.md).
Whole display-record parents, the action dispatcher, older partial adapters
and full original callback coverage remain game work. Kickstart services
and timing remain deferred.

Six complete control/readout owners pass 196,608 whole CPU/PC/SR/RAM calls,
covering 866/867 boundaries with both child providers; the remaining clamp
is exhaustively proved unreachable and independently instruction-tested.
Dispatch passes 18,432 calls; four owners pass normal C at 15,961 shadow /
16,016 sandbox. The full 612-row gate, all 36,236 isolated live frames/seals
and combined DMA at 30,167/965,344 pass. Source-timed entries rise to 537.
See [control/readout proof](analysis/routines/native_c_control_readouts.md).
Family exact through 600; ALL remains 424/34,144. The action dispatcher,
older partial adapters and full original callback coverage remain open.
Kickstart services and timing are deferred.

Twelve complete corner/view owners pass 393,216 whole CPU/PC/SR/RAM calls,
covering all 515 boundaries in controlled union. The original non-returning
branch passes 16,384 observations. Dispatch passes 34,816 calls; three owners
pass normal C at 7,083 shadow /7,962 sandbox, while nine remain uncalled with
independent whole proofs. C200F6 retains aggregate tail timing. All 36,236
isolated live frames/seals match; combined DMA passes 29,300/937,600.
Source-timed entries rise to 531. See
[corner/view proof](analysis/routines/native_c_corner_view.md).
Family exact through 600; ALL remains 424/34,144. Those control-record selectors are complete above;
older partial adapters and full original callback coverage remain open.
Kickstart services and timing are deferred.

Twelve complete selected-segment/projection/crossing upgrades pass 393,216
whole CPU/PC/SR/RAM calls, covering all 663 boundaries with controlled children.
All four original parallel loops pass 65,536 non-returning observations.
Dispatch passes 36,864 calls; every owner passes normal C:
106,838 shadow /126,128 sandbox. All 36,236 isolated live frames/seals match;
combined DMA passes 29,042/929,344. Source-timed entries rise to 521.
See [segment projection proof](analysis/routines/native_c_segment_projection.md).
Family exact through 600; ALL remains 424/34,144. Those corner/view owners
are complete above; older adapters and original callback coverage remain open.
Kickstart services and timing are deferred.

Thirty-seven complete renderer entries and writer callbacks pass 1,212,416
whole CPU/PC/SR/RAM calls and 65,536 independent cold-segment calls.
Controlled whole calls cover 546/550 boundaries. Dispatch passes 113,664
calls; thirty seeded owners pass normal C: 537,534 shadow /
571,393 sandbox. Seven cold source-only writers have
separate fixture evidence and remain uncalled in recordings. All 36,236
isolated live frames/seals match; combined DMA passes 28,931/925,792.
Source-timed entries rise to 518. See
[renderer entry proof](analysis/routines/native_c_render_entry_helpers.md).
Family frames match through 600; ALL remains 424/34,144. Those selected-segment and projection/clipping owners are now complete above;
original function and callback coverage remains open. Kickstart services and timing are deferred.

Twelve complete renderer-helper upgrades pass 393,216 whole CPU/PC/SR/RAM
calls and 81,920 independent production-segment calls. Controlled whole
calls cover 964/971 boundaries; seven cold PCs have separate evidence.
Actual dispatch passes 36,864 calls; normal C passes every owner: 423,745
shadow / 530,198 sandbox. All 36,236 isolated live frames and seals match;
combined DMA passes 28,931 / 925,792. Source-timed entries rise to 510.
See [renderer-helper proof](analysis/routines/native_c_render_leaf_helpers.md).
Family timing matches through 600; ALL remains 424 / 34,144 pixels.
Those thirty-seven renderer entries and callbacks are now complete above.
Full original game-function and callback coverage remains open.

Fifteen complete ground/HUD rendering upgrades pass 491,520 whole CPU/PC/SR/
RAM calls and 32,768 independent production-segment calls. Controlled whole
entry coverage is 1,350/1,352; the two cold branches have separate evidence.
Actual dispatch passes 46,080 calls; normal C passes every owner: 30,293
shadow / 42,601 sandbox. All 36,236 isolated live frames and seals match;
combined DMA passes 28,849 / 923,168. Source-timed entries rise to 509. See
[ground/HUD proof](analysis/routines/native_c_hud_render_parents.md).
Family timing matches through 600; ALL first differs at 424 / 34,144 pixels,
retained as deferred timing evidence. Those twelve renderer helpers are
now complete as described above.
Original game-function and callback coverage remains open.

Fourteen complete face-list/edge upgrades pass 458,752 full CPU/PC/SR/RAM
calls, all 533 owned boundaries with both controlled and original children,
and 43,008 dispatch calls. Independent normal C passes every owner: 70,334
shadow / 92,495 sandbox (including an isolated C21C4C check; the initial
batch's zero-call rejection is retained). All 36,236 isolated live frames
and seals match; combined DMA passes 27,543 / 881,376. Source-timed entries
rise to 494. See [face-list/edge proof](analysis/routines/native_c_face_list_parents.md).
Those fifteen ground/HUD rendering parents are now complete as described above.
Full original function and callback coverage remains open; Kickstart services
and timing remain deferred.

Seventeen complete face-stream upgrades pass 557,056 full CPU/PC/SR/RAM
calls, all 773 owned boundaries with both controlled and original children,
and 52,224 dispatch calls. Independent normal C passes every owner: 22,333
shadow / 26,010 sandbox. All 36,236 isolated live frames and seals match;
combined DMA passes 27,064 / 866,048. Source-timed entries rise to 481. See
[face-stream proof](analysis/routines/native_c_face_stream_parents.md).
Those fourteen face-list/edge owners are now complete as described above. Complete original function and callback coverage
remains open; Kickstart services and timing remain deferred.

Ten complete older rendering-parent upgrades pass 327,680 full CPU/PC/SR/
RAM calls, all 1,101 controlled source boundaries and 30,720 bounded dispatch
calls. Independent normal C passes every owner: 57,634 shadow / 62,893
sandbox. All 36,236 isolated live frames and RAM seals match; combined DMA
passes 26,324 / 842,368. Source-timed entries rise to 465. See
[render-parent proof](analysis/routines/native_c_render_parents.md).
Those seventeen face-stream parents are now complete as described above.
Full game-function and callback coverage remains open.

Five complete history/display and indirect stream upgrades pass 163,840
full CPU/PC/SR/RAM calls, all 643 controlled source boundaries and 15,360
bounded dispatch calls. Independent normal C passes all five: 5,901 shadow /
8,073 sandbox. All 36,236 isolated live frames and seals match; combined
DMA passes 25,346 / 811,072. Source-timed entries rise to 456. See
[history and stream proof](analysis/routines/native_c_hud_history_stream.md).
Those ten older render parents are now complete as described above.
Full original game-function and callback coverage remains open.

Six complete projection, cue and conditional-text parent upgrades pass
196,608 full CPU/PC/SR/RAM calls, all 279 controlled boundaries and 18,432
bounded actual dispatch fixtures. Independent normal C passes all six:
8,780 shadow / 13,276 sandbox comparisons. All 36,236 isolated live frames
and seals match; fresh combined DMA passes 24,703 / 790,496. Source-timed
entries rise to 451. See
[projection-parent proof](analysis/routines/native_c_hud_projection_parents.md).
The history-projection and postflight-display parents are now complete above. Original game-call/callback coverage remains open.

Eight complete HUD cache/text helper upgrades pass 262,144 full CPU/PC/SR/RAM
calls, all 182 shared controlled boundaries and 24,576 bounded actual dispatch
fixtures. The fixed zero-mode entry retains its seven excluded leading-blank
PCs explicitly; sibling formatters cover them. Independent normal C passes
all eight: 30,815 shadow / 32,589 sandbox comparisons. All 36,236 isolated
live frames and RAM seals match. Combined DMA passes 24,511 boundaries /
784,352 cases; source-timed entries rise to 447. See
[HUD text helper proof](analysis/routines/native_c_hud_text_helpers.md).
Six of the sealed projection/HUD parents are now complete as described above.
Original indirect/table/callback and game-function coverage remains open.

The preceding thirteen complete HUD readout, cue and status upgrades pass 425,984 full
CPU/PC/SR/RAM calls, all 606 controlled source boundaries and 39,936 bounded
actual dispatch fixtures. Independent normal C passes every owner: 32,575
shadow / 33,952 sandbox comparisons. All 36,236 isolated live frames and RAM
seals match. Frozen-clock original-child fixtures retain explicit clipping
and context limits. Combined DMA passes 24,478 boundaries / 783,296 cases;
source-timed entries rise to 439. See
[readout parent proof](analysis/routines/native_c_hud_readout_parents.md).
Their eight cache and text helper successors are complete as described above. Original game-function and callback coverage remains open.

The preceding eight complete HUD display-parent upgrades pass 262,144 full CPU/PC/SR/RAM
calls, all 595 controlled source boundaries and 6,144 bounded actual dispatch
fixtures. Independent normal C passes all eight owners: 27,001 shadow /
41,163 sandbox comparisons. All 36,236 isolated live frames and RAM seals
match. Frozen-clock original-child fixtures retain explicit no-draw limits;
active drawing is checked independently on the recordings. Combined DMA
passes 23,942 boundaries / 766,144 cases; source-timed entries rise to 426.
See [HUD parent proof](analysis/routines/native_c_hud_parents.md).
Their thirteen readout, cue and status successors are complete as described above. Original game-function and callback coverage remains open.

The preceding six complete stream/numeric/marker owners and two store upgrades pass 262,144
whole CPU/PC/SR/RAM calls, ordered Custom writes and terminal hardware state.
Both proof kinds cover all 100 source boundaries. All 36,236 isolated live
frames and RAM seals match; five recorded owners pass 9,146 shadow / 9,278
sandbox independent normal-C comparisons. Three cold owners retain strict
recording rejections. Original marker fixtures keep their bounded vertical
offsets and zero/one-plane limits explicit. Combined DMA passes 23,347
boundaries / 747,104 cases. See [stream proof](analysis/routines/native_c_hud_stream.md).
Their eight display parents are now complete as described above.
Game-function porting remains incomplete; stop when only Kickstart services
and timing remain, after reconciling cold and indirect owners.

The preceding six projection/readout owners and four projection upgrades pass
327,680 whole calls and 65,536 separate internal clamp segments. They have
272 unique source boundaries and pass 491,520 actual dispatch fixtures.
All 36,236 isolated live frames and RAM seals match. See
[projection/readout proof](analysis/routines/native_c_projection_readouts.md).
Other older projection parents still require complete original-child upgrades.

The preceding five complete grid, scene-label and record-marker functions pass 163,840 full
CPU/PC/SR/RAM calls, every controlled source boundary, and 30,720 dispatch
fixtures; all 36,236 isolated live frames and RAM seals match. Independent
normal C passes 20,090 shadow / 20,093 sandbox comparisons
for the two recorded parents; three cold children retain strict recording
rejections and complete fixture proofs. The combined DMA proof passes 23,061
boundaries / 737,952 cases. See
[grid and marker proof](analysis/routines/native_c_flight_markers.md).
Their projection/readout successors and the stream owners are now complete
as described above.

The preceding four complete history, zone-exit and candidate-geometry upgrades pass 131,072
full CPU/PC/SR/RAM calls, all 1,015 controlled source boundaries, 3,072 dispatch
fixtures and all 36,236 isolated live frames/seals. Independent normal C passes
16,826 shadow / 17,899 sandbox comparisons; a separate face-helper selection
passes another 1,505 / 1,564. Actual children cover 816/819 candidate-scan PCs;
the controlled-only paths remain explicit. The combined DMA proof passes
22,593 boundaries / 722,976 cases. See
[flight-geometry proof](analysis/routines/native_c_flight_geometry.md).
Their five grid and marker successors are now complete as described above.

The preceding four complete flight-dynamics parents and the upgraded shared region stream
pass 163,840 full CPU/PC/SR/RAM calls, all 1,308 controlled source boundaries,
3,840 dispatch fixtures and all 36,236 isolated live frames/seals. Three hot
owners pass 19,879 independent normal-C comparisons; two cold owners retain
strict zero-comparison rejections. The combined DMA check passes 21,578
boundaries / 690,496 cases. See
[flight-dynamics proof](analysis/routines/native_c_flight_dynamics.md).
Their four geometry/record helper successors are now complete as described above.

The preceding five complete motion-projection, publication and collision helpers pass
163,840 full CPU/PC/SR/RAM calls, all 229 source boundaries with controlled
and real children, 3,840 dispatch fixtures and all 36,236 isolated live
frames/seals. These five owners are cold in all three recordings; the strict
normal-C checker retains its zero-comparison rejection. A fresh combined DMA
check passes 20,488 instruction boundaries / 655,616 cases. See
[motion-helper proof](analysis/routines/native_c_flight_motion_helpers.md).

The preceding complete flight-record action and control-stream owners pass 327,680 full
CPU/SR/RAM completed calls, 7,680 dispatch fixtures and all 36,236 isolated
live frames/seals. Internal segments and nonreturning fault observations are
proved separately. Three recorded owners pass 40,695 independent normal-C
comparisons; seven cold owners retain fixture and original-call proof.
Their combined DMA checkpoint passed 20,259 instruction boundaries / 648,288
cases. See [flight-record action proof](analysis/routines/native_c_flight_record_actions.md).

Complete control/flight parents and four helper upgrades pass 196,608 full
CPU/SR/RAM calls, 4,608 dispatch fixtures and all 36,236 isolated live frames/
seals. Independent normal C passes 31,182 recorded comparisons, with hardware
and incomplete calls retained. A fresh combined DMA check passes 19,855
instruction boundaries / 635,360 cases. See
[control/flight proof](analysis/routines/native_c_main_loop_flight_controls.md).
Game-function porting remains incomplete; stop when only Kickstart services
and timing remain, after reconciling cold and indirect original owners.

Complete control-record action/alert owners and the offset adapter upgrade
pass 147,456 CPU/SR/RAM calls, 4,608 dispatch fixtures and all 36,236 isolated
live frames/seals. Five owners are cold in recordings; the offset body passes
25,833 independent recorded comparisons, with shadow incompletes explicit. See
[control-action proof](analysis/routines/native_c_record_control_actions.md).

Complete control-record and message-sequence owners pass 49,152 full
CPU/SR/RAM calls and 41,212 independent readable-C recording comparisons;
all 36,236 isolated live frames and seals match. See
[control/message proof](analysis/routines/native_c_main_loop_control_messages.md).

The immediate work is the next complete readable source batch. The user has
deferred the minor one-frame Copper-fade delay; its evidence is retained in
`CURRENT_PORT_HANDOFF.md` for later parity work.
The gauge correction matches all recorded frames and sealed final RAM in
isolation; 418 registered callable entries now have source timing. The complete
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
