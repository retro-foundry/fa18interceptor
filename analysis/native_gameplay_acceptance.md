# Native gameplay-frame acceptance

Startup sample handoffs (2026-10-10): all 468,007 original consumed bytes
follow published buffers, addresses, byte order and periods. All 48
requests match the current native ordered payload/period/volume sequence;
the complete native Demo also preserves PCM/RAM/trace/counters exactly.
Original right-channel phase stays 5,450 chip clocks behind left, while
native starts both together. Onset/waveform acceptance remains open.
A conflicting startup DMA grid is retained as failed; the independent
byte observer passes without claiming separate DMA coverage. No playable
change. See [startup sound evidence](native_startup_sample_handoffs_milestone.md).

Complete escort comparison (2026-10-10): the successful original recording
has exact JSR/LINK identities for all 47,814 observations and a verified
arrest snapshot. Native consumes every key and reaches the menu without
a reset, but earns no escort grade. All 7,170 flight observations are
compared; strict parity remains rejected and retained. Different clock
inputs already select different initial carrier positions. No playable
change or fitted clock is introduced. Sound timing remains open.
See [complete comparison evidence](native_escort_wire_full_flight_milestone.md).

Successful original escort (2026-10-10): targeting the live original
arrestor geometry completes escort, earns the second mission completion
and returns to the menu. All 47,814 observations, final RAM and consumed
keys reproduce exactly without the controller, including the earned
prefix. Two wrong-target guards reject. Native runtime is unchanged.
The successful reference is ready for update-identity/native whole-flight
comparison; sound timing remains open. See [escort success evidence](native_original_escort_wire_milestone.md).

Original escort deck contact (2026-10-10): the validation standoff-height
route reaches the original carrier deck with gear down and hook extended,
but misses the arrestor and earns no escort grade. All 65,000 observations,
final RAM and consumed keys reproduce exactly; the first 46,126 equal the
preceding route. Actual wire geometry is aft of the controller target.
The playable runner is unchanged. Wire targeting, broader full flights
and sound timing remain open. See [deck/wire evidence](native_original_escort_standoff_milestone.md).

Original escort approach (2026-10-10): the existing validation height gate
reduces landing overshoot, but the original still earns no escort grade.
All 65,000 observations, final RAM and consumed keys reproduce exactly
without the controller; all 45,254 observations before the approach change
equal the preceding recording. Three profile guards reject. The playable
runner is unchanged. Successful escort landing, broader independent flights
and sound timing remain open. See [approach evidence](native_original_escort_approach_milestone.md).

Original escort steering (2026-10-10): repeated ordinary steering inputs
reach combat success in an independently started original game. All 65,000
observations, final RAM and consumed keys reproduce exactly without the
controller, including the complete earned prefix. The return stops beyond
the carrier; no escort grade is earned. The input option is validation-only;
native runtime is unchanged. Successful landing, independent whole-flight
comparison and sound timing remain open. See [steering recording evidence](native_original_escort_steering_milestone.md).

Default-host combat (2026-10-10): mission five and the final mission pass
all 43,100 presentations with sound, all 14 camera modes in each mission,
gear raised after takeoff and zero restarts. Maximum work is 16.2690 ms,
with no frame over 20 ms, model fault, heap violation or pool failure.
The earlier mission-five clip with premature G remains rejected and fully
retained; both PAL controls survive, so its two host restarts are not
attributed solely to gear. The checker now verifies the G press follows
takeoff. Strict PAL comparisons stay unchanged. Independent complete flights,
original sound timing and broader default-host outcomes remain open.
See [default-host combat evidence](native_host_combat_views_milestone.md).

Interactive host clock (2026-10-10): windowed gameplay now acquires actual
microseconds, preserving all scene-selection bits. Headless defaults retain
PAL time; sealed visible comparisons explicitly select PAL diagnostics.
Original timer checks pass 72 cases plus two record passes; the final focused
CTest passes 5/5. A fresh default-host Free Flight run presents all 6,502 frames
with live sound, maximum work 15.3594 ms and zero heap violations/pool failures.
This grounded check does not establish default-host combat or full-flight
parity. Independent whole flights and sound timing remain open; campaign
continuity stays waived and named-state cleanup stays outside this goal.
See [host clock evidence](native_host_clock_milestone.md).

Escort scene clock (2026-10-10): the actual native C0FECE setup matches
original instructions with its own inputs. A reference-only microsecond
probe reproduces every byte of all 16 original cores; three wrong-clock
probes reject. Native timer quantization zeroes the five bits used for
source scene variation, so timer-resolution fidelity remains open. The
whole native replay preserves counters/RAM/save; full escort parity and
sound timing remain open. See [scene clock evidence](native_escort_scene_clock_milestone.md).

Complete mission-three message timing (2026-10-09): all 4,964 real transitions
obey original elapsed/countdown, delay/redraw, full text and colour-cache rules,
including 1,362 paused HUD periods. Nine priority-message producer fields match
at all 4,967 observations. Both first HDG events follow two sampled-second
changes (original tick168/native185); this is assessed under the accepted
cadence policy. Release/Debug agree; nine wrong results are rejected. Optional
read-only fields preserve all earlier traces, pages, final RAM, counters and
saves. Strict pages remain 287/4,967; other drawing and completion work remain
open. State cleanup stays deferred. See [mission message evidence](native_mission_message_timing_milestone.md).

2026-10-09 target-info update: the reported tick273 ALT/HDG mismatch is assessed
under the cadence policy. Both independently started mode-three recordings
obey the original elapsed/countdown, context/redraw and complete-text rules
through all 374 transitions of their selected-target episode. Seven initial
page transitions agree at equivalent second-countdown events; their actual timestamps
remain reported. Release/Debug agree and four wrong results are rejected.
See `native_demo_hud_timing_milestone.md`. This supersedes the unassessed
target-info timing notes below, without accepting other drawing differences
or full mission sequences.

Current policy (2026-10-07): the user's later clarification accepts different
native rendering/presentation cadence. Preserve gameplay physics, rules, input
and source-defined timers; compare equivalent gameplay states/events. Exact
Amiga frame counts, elapsed timestamps and missed rendering frames are not
completion gates. The older strict sequence/cadence assessments below remain
diagnostics, not an instruction to reproduce Amiga rendering delays. Copper
fade remains the only drawing exclusion. Complete scenario acceptance under
this revised policy is still unestablished; functional outcomes are 3/3.

The later independent segment now covers ticks 222..584: all 363 named player/
camera boundaries match, but only 83 complete drawing boundaries match. The
first cockpit cache difference at tick 508 follows the source seconds-driven
view-hold expiry and redraw. Same-state/clock body and expiry-parent checks
pass; equivalent state/event assessment remains open. Cold scene startup now
executes C08EE4/C08EB8 before scene construction, correcting the region flag and
sixteen-update countdown lead: all 5,808/5,808 later cores match. See
`native_scene_startup_milestone.md` for that correction and
`native_later_demo_assessment.md` for the earlier diagnostic findings.

Latest takeoff assessment (2026-10-07): the user-reported demo outside-view
clipping is fixed by supplying C1F2EE the original model vertex. All 223
independent pre-input boundaries at source 2179..2401 / native 2142..2364
now match both complete drawing pages, camera state and named player motion/
pose/matrices. Before the fix only 153 drawing boundaries matched, while
camera state and pose already agreed. The original demo also rolls; its motion
is preserved. This accepts that takeoff segment, not the full demo. See
`native_outside_camera_milestone.md`; the later tick-273 HUD assessment below
remains separate and unaccepted. No drawing exclusion changed.

Latest user clarification: gameplay frames are what must match. Intro, loading
and preflight duration may differ, including loading substantially faster.
Copper fade remains excluded. This overrides earlier whole-launch timing gates.

Compare corresponding gameplay phases and ordered gameplay drawing outputs,
with equivalent consumed control input. Preserve the actual game physics,
scene/HUD composition and gameplay time dependencies. Do not require matching
intro frame counts, global uptime or an arbitrary menu-relative update number.
Do not hide changed gameplay by searching for a visually similar later frame.
Any alignment offset must come from an equivalent source gameplay transition.

Current evidence is functional scenario outcomes **3/3**, and sampled assembled
frame-body comparisons **8/8** with zero compared state/display differences.
Those bodies start from the same captured state and receive the same timer
interval. They prove composition, not that independent runs reach the same
state or produce the same complete gameplay sequence. Complete gameplay-frame
acceptance remains **0/3**; no defensible whole-game parity percentage exists.

Five selected independently aligned gameplay checkpoints now pass. Original
start-of-update 2401 and native C0EFEA in update 2364 have game tick 222,
C10DAE and identical view/page selectors. **Both 320x200 four-plane gameplay
pages are byte-identical**, as are player position/motion, rates, orientation
and matrices. Geometry is taken from the original ViewPort; every byte of its
two page buffers is compared, without a pixel similarity mask or fade colours.
Native export: `build/native-flight/input-callback-aligned-frame2364.before.dat`;
existing original export: `build/native-flight/reference-demo2401.dat`.
The native process starts from ADF/input, never the reference state. Two
bookkeeping bytes outside the named kinematic comparison differ (+04 flags
and +4D counter); whole-state/sequence acceptance is not established.

An initial comparison mistakenly used native end-of-2364, which stops during
that update's timer wait after its physics pass. This compared before-update
source state with after-update native state and falsely suggested motion drift.
The actual C0EFEA boundary resolves it. The 36-update lead at the old
menu-relative checkpoint is startup evidence, not a gameplay failure by itself.
This checkpoint therefore supports the user's gameplay-only scope; it does
not prove that a fixed offset accepts every later phase or recording.

The checker now captures native state **before input/stage**, matching the
original C0EFD4 dump phase exactly. C0EFEA remains the separate assembled-body
fixture boundary. This distinction matters at key edges: original update 6001
still has its throttle/trim latch before a release, while native C0EFEA has
already consumed that release. Comparing those two phases falsely rejected
otherwise matching flight. `--frame-capture` now adds an `.entry.dat` export for
flight updates whose input/stage have not already run; its existing `.before.dat`
and `.after.dat` semantics are unchanged.

Pages are compared by **draw/display roles**, using C2F558's selected DRAW_PAGE
and active PAGE_PLANE_TABLE, rather than identical physical buffer numbers.
C1612C publishes a completed buffer then flips DRAW_PAGE. Different preflight
swap counts can change physical numbering without changing either gameplay
frame. The active drawing-table publication must still agree with each runner's
own selection. Both complete pages remain checked; page-role normalization
does not search later frames, mask the HUD, or accept the wrong presented page.

Four independently executed carrier checkpoints now match all drawing bytes,
phase/controls and named player motion/pose/matrices: original/native updates
5101 (tick 412), 5501 (812), 6001 (1312), and 6289 (1600). Original draw buffer
1 corresponds to native buffer 0 in all four. Combined with the demo checkpoint,
this is **5/5 selected checkpoints (100% of that sample)**, across two scenarios,
not a whole-game percentage. All original exports were reused; no original
replay was run for this batch. Reproduce one carrier checkpoint and verifier
strictness:

```powershell
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-carrier-approach.dat --input build/native-flight/carrier-game-input.fa18in --iteration 6289
python tools/native/check_gameplay_comparison.py --source build/native-flight/gameplay-window-source.2401.dat --native build/native-flight/gameplay-window-comparison/native.2364.entry.dat
```

Reproduce without rerunning the original recording:

```powershell
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-demo2401.dat --iteration 2364
```

One source dependency to investigate is microsecond clock state. C16D04
publishes the host timer fraction at C45AF6. The C28782/C28CAE branches in the
geometry owner use its low word C45AF8/C45AF9 to derive signed small placement
offsets stored at C45B18/C45B1A. Native currently acquires timer samples only on
20 ms PAL boundaries. This is source-backed evidence of a possible gameplay
input difference, not evidence of a failure at this accepted checkpoint.
Original parent state, recorder cursors and clock samples should be compared
when a subsequent equivalent gameplay boundary differs before choosing a fix.

The first consecutive independent window is now retained: original updates
2401..2528 and native pre-input/updates 2364..2491, game ticks 222..349. **128/128
phase/control and named player motion/pose/matrix comparisons match (100% of
this window's state scope)**. There are 128 distinct original motion and drawing
states. **53/128 complete two-page drawing comparisons match (41.4% of this
window)**. The first 51 boundaries agree; the first failure is source 2452 /
native 2415, tick 273. Only 20 bytes in plane 3 of page 1 differ, in the target
information line at y=192. The source displays HDG:166 while native still
displays ALT:5110. Later failures stay in that same information-line region;
the last two boundaries agree again. This is a real gameplay HUD difference,
not Copper fade, startup duration, a physics mismatch or a similarity mask.

C25312 / `begin_main_loop_timers()` decrements INFO_REQUEST (C45886) through
C25482 when sampled seconds change; `message_line.c:info_line()` cycles
INFO_PAGE (C459C4) on a negative request. The retained source samples advance
18,000 ms across the window, versus 8,560 ms native. Both execute the existing
timer/message owners, but independent frame pacing changes their seconds-based
HUD phase. This identifies the connected timing dependency; no guessed speed
factor, constant offset or capture-fed clock has been introduced. Original
rendering/poll/display duration must be understood before correcting cadence.
The possible C28782/C28CAE clock-dependent geometry offsets do not cause this
window's drawing mismatch.

Further timing inspection found the source rate-limit table C2502E entry 15
is 67 ms. Both runners select that same entry and execute the same C25312
arithmetic; the original frame-body oracle already verifies the result given
the same samples. The native window approaches that limit, while original
rendering takes longer between corresponding game updates. No incorrect rate
index or timer arithmetic has been found. Simply multiplying native time or
setting a fitted frame delay would replace the dependency rather than port it.

Optional diagnostic ranges preserve actual runner boundaries:
`--frame-capture FIRST+COUNT PREFIX` writes native `PREFIX.ITERATION.entry.dat`,
`.before.dat` and `.after.dat`; `FA18_LOOP_DUMP=FIRST+COUNT:PREFIX` writes original
`PREFIX.ITERATION.dat`. Single-capture filenames remain unchanged. The one
bounded 6,000-PAL original prefix produced RAM/registers identical to the
previous retained prefix, and its first boundary equals the old accepted
checkpoint. Captures observe execution; they never supply behavior or pixels.
Reuse them for subsequent native changes:

```powershell
python tools/native/check_gameplay_window.py --source-prefix build/native-flight/gameplay-window-source --source-first 2401 --native-first 2364 --count 128 --out build/native-flight/gameplay-window-comparison
```

This currently exits 1 and retains `comparison.json` with every failure.
The check compares complete source drawing geometry, phase, controls and the
same named player fields as the single checkpoint. Palette/audio/full-state
acceptance is separate. Full gameplay sequence acceptance remains **0/3**.

Next: extend independent gameplay comparisons through equivalent source
phases, using existing checkpoints or bounded source windows. Distinguish
input/record/state differences from clock-driven offsets if one fails, then
correct the connected owner. Avoid a guessed
fixed update count, fabricated delay, changed physics or source RAM seeded into
the playable runner. Use expensive complete original runs only when needed to
establish or finally accept the changed gameplay sequence.
