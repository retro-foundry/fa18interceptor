# Recorded target-info timing assessment

2026-10-09. The reported ALT/HDG difference at source2452/native2415,
game tick273, is now assessed under the existing gameplay timing policy.
The recording named `demo01` actually runs mode three; this is distinct from
the menu's Demonstration mode127. No gameplay timer, display delay, clock
multiplier or HUD exclusion was introduced.

## Correct diagnostic ownership

The playable chain remains `fa18_native` -> `native_frontend_tick` ->
`native_flight_tick` -> HUD -> `begin_main_loop_timers` -> later clock polls.
The original runner observes the matching C0EFD4 pre-input boundary with ports
off. Trace acquisition reads direct host spans and does not execute a game
action or bus operation.

V1's field named `selected_record` actually addressed INPUT_X/INPUT_Y,
C45776..C45779. V2 correctly names those bytes `mouse_coordinates` and records
the actual SELECTED_RECORD word at C459C0, plus mode, target index, context,
HUD/delay/redraw gates and origin-detail mode. V1 remains readable: its old
field is explicitly renamed on ingestion, without fabricating missing target
selection. The comparator lists the game fields actually available in its
evidence. Existing V1 core, camera and drawing results remain valid; its old
field did not establish target-selection equality.

Fresh original/native complete recordings retain every previous row, frame,
core, plane hash and common field. Original final RAM and statistics are
identical to the preceding recording; native final RAM and statistics also
agree. This checks all 4,892 original trace rows and 3,330 observed native rows.
The latter are flight-caller observations within 4,855 native replay iterations,
not all menu iterations. Passing raw RAM is discarded; traces are compressed.

The Debug native run reuses the hashed independent original trace and starts
normally from ADF/input. Reference state, clocks and pixels never supply native
behavior. Every complete trace hash and assessment field matches Release.
Modified original input hashes and a modified trace are rejected before replay.

## Source rules and actual events

The assessment covers the complete selected-target information episode,
including acquisition and loss: **374 consecutive transitions per runner**,
game ticks264..638. All elapsed accumulator intervals match C25312's actual
seconds/fraction arithmetic, including DIVS truncation and overflow behavior.
Every request/page result and all 26 message bytes are checked.

| Actual independent evidence | Original | Native |
| --- | ---: | ---: |
| Checked elapsed/countdown/message transitions | 374 | 374 |
| Newly formatted target lines | 56 | 52 |
| Cached or blank line results | 318 | 322 |
| Map-context suppression of the message owner | 46 | 46 |
| Changed sampled-second events | 66 | 25 |

C25312 decrements INFO_REQUEST through C25482 once when its sampled seconds
change. C322EE's information branch resets a negative request to one and
advances ALT -> HDG -> SPD. C0F138's map-context branch omits the message owner
in mode three when no redraw is pending. C12098/C1B906 and view commands request
the original three-pass redraw. Cached text is retained until its actual
formatting branch runs. The assessment derives fresh numbers and names from
the real selected record and original asset strings, including decimal zero
policy and original DIVU behavior; it does not accept an arbitrary line label.

On the first ALT frame, tick265, the original crosses a sampled-second boundary
and leaves INFO_REQUEST=0; native remains in the same sampled second and leaves
it at one. Original reaches a negative request before tick273. Native reaches
it before tick288. Both first HDG transitions follow **two changed-second
events before the HUD**, counting the original's decrement on the acquisition
frame. The seven native initial cycling events agree with the corresponding
source page/countdown events before the first map-context transition.

The first HDG appears 1,081 accumulated milliseconds after the first source ALT
and 1,560 milliseconds after the first native ALT. These are deliberately
reported, not fitted away: different fractional clock phases at acquisition
and different rendering cadence change when the next whole-second sample is
observed. Comparing equal update counts or imposing a fixed relative delay
would not verify the source rule. The actual timer, page and cache results
follow that rule in both independent runs.

Wrong premature pages, lost second events, wrong freshly formatted text and
wrong cached text are all rejected. Source instruction HUD/timer oracles also
pass on the current runtime. The interpretation follows
`analysis/native_gameplay_acceptance.md`: preserve source-defined timers and
gameplay at equivalent states/events; exact Amiga timestamps and missed frames
are not required. This resolves the specifically reported ALT/HDG timing
question and this target-info episode.

## Validation and limits

Release HUD, clock, flight-trace integrity and cleanup CTests pass (62.07s).
The trace check compares its fields directly with live source2452/native2415
RAM and verifies enabling diagnostics preserves RAM/counters. Debug frontend,
frame-time integration, artifact policy and cleanup CTests pass (26.57s).
Release and Debug complete independent native traces and assessments agree.
Eight selected CTests pass; the earlier original-input 15-second timeout is
unchanged and separately documented in the handoff.

Strict complete drawing is still **1,235/2,046** first-flight boundaries.
All **32,736 complete cores**, camera/control fields and the newly observed
mode/target-selection fields match. The target-info assessment does not accept
the remaining cockpit/presentation differences or independently qualify every
mission's complete drawing. Original recorded audio/filter fidelity, visible
20ms performance and typed-state cleanup remain open. The user waived the
uninterrupted campaign requirement and retained original Escape restart.

Evidence: `analysis/figures/native_demo_hud_timing_checkpoint.json` and compressed
traces/reports under `build/native-flight/recorded-demo-hud/Release/` and `Debug/`.
Reproduce the assessment without another game run:

```powershell
python tools/native/assess_demo_hud_timing.py --traces build/native-flight/recorded-demo-hud/Release --previous build/native-flight/recorded-demo-trace/Release --out build/native-flight/recorded-demo-hud/Release/hud-assessment.json
```

For a fresh native build using the retained original evidence:

```powershell
python tools/native/check_recorded_demo_trace.py --runner build/native-cmake/native/Debug/fa18_native.exe --source-evidence build/native-flight/recorded-demo-hud/Release --out build/native-flight/recorded-demo-hud/Debug
```
