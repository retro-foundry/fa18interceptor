# Mode-five forced-return objective - 2026-10-08

Normal keyboard input now reaches the stolen-aircraft proximity countdown
and paired restoration in the actual native runtime. All 24 input/stage
intervals and 107 sampled full bodies match original compared RAM/drawing,
including 64 consecutive proximity/restoration window bodies. This accepts
the forced-return objective path; complete mode-five mission success remains
open. No playable source, game rule, comparison mask or executable changes.

## Connected original behavior

The actual runner enters `native_frontend_tick` -> `native_flight_tick` ->
the native postflight scheduler in `port/game/native/records.c`. It calls the
existing `port/game/postflight_scheduler.c` mode-five owner, original C0A002.
With difficulty selector two, all three position differences must remain
within `$24000` fixed units (576 world units). Ordinary flight holds this
condition for 201 consecutive native bodies, reducing the original counter
from 200 to -1. The first decrement completes at tick 17,321; the negative
transition completes at tick 17,996.

C0A002 then reaches C0A12E through `SCHEDULE_RESTORE_FIRST` and
`SCHEDULE_RESTORE_SECOND`, for records four and six. The original instructions
run externally and report 12 restoration calls for each aircraft in the
compared frames, beginning at body 8,055, tick 17,997. For each observed pair,
the check reads each native record's kind, the actual C295E0 table offset and
five signed table words. The four restored words and sign-extended final long
match those original values. Full-body RAM/drawing comparison independently
covers the resulting state.

## Validation pilot and repeatable gate

The pilot lives only in `tools/native/mission_pilot.c`, linked into
`fa18_native_mission_success_test`; it is absent from the playable runner.
It reads flight state and emits normal frontend keyboard events. The revised
formation controller ends the takeoff pull-up while upright, initializes its
feedback from current orientation, uses the actual inverse matrix for pitch,
leads the moving aircraft and matches speed during the close approach.
C1B35A/C13D84 can settle a later F10 request at normal phase 120; the pilot
reapplies ordinary plus input when the source releases that throttle input.
These are validation flight choices, with no direct flight/outcome RAM writes.
The default mode-three and numeric mode-five pilots remain unchanged.

The observer retains two 32-body windows: countdown entry and its negative
transition/restoration. Read-only telemetry records every native decrement
and all three fixed position differences. Raw passing RAM stays temporary;
the existing 480 MiB combined capture limit remains in force. The complete
checked run uses 262 MiB before comparison and removes each passing case.
The new original frame-oracle counters observe actual C0A12E instruction
entry with the corresponding record pointer and do not alter execution.

`tools/native/check_mission_forced_return.py` is registered as serial CTest
`fa18_native_mission_five_forced_return`. It launches a fresh save, bounds the
run at tick 18,130, verifies all 201 native proximity scans and both consecutive
windows, compares every exported boundary and checks both restored table
sets against actual original call evidence. The fixture correctly returns
status 1 because the whole mission remains incomplete. Phase remains zero,
completion count remains 3, no reset occurs and no mission config write is
accepted. This check deliberately distinguishes the objective path from a
successful result.

```powershell
cmake --build build/native-cmake --config Release --target fa18_native_mission_success_test --parallel 8
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mission_five_forced_return$' --output-on-failure
```

Metadata is in `analysis/figures/native_mission_five_forced_return_checkpoint.json`;
consumed keys are retained in `tools/native/fixtures/mission-five-forced-return.e9k`.
The original comparison and runtime reports are under
`build/native-flight/mission-five-forced-return-final/`. Debug and Release
regression logs use `build/native-flight/mission-five-forced-return-*-ctest.log`.

Debug fixture build and all three selected CTests pass: mode-three success,
mode-five forced return and artifact cleanup. Release fixture build and all
four selected CTests pass: mode-three success, mode-five combat/reset, mode-five
forced return and artifact cleanup. The playable executable remains SHA256
`73f9bc00fd5db4f1f3a3e90097b902e0a4eb2b4d8ad07a24789626a3549189cd`.

## Limits and next work

The reference executes original instructions from native before-states;
independent complete original/native flight parity remains unverified. All
201 countdown scans are observed in the native flight; original full-body
comparison covers the selected windows and other exported samples, rather
than every body of that flight.

The longer trajectory still crashes in the subsequent combat approach at
tick 18,793. All 26 input/stage intervals and 111 sampled bodies, including
the reset, match original compared RAM/drawing. Completion count stays 3.
Continue the validation pilot through both enemy expiry increments, the
remaining objective admission, safe return, landing, stopped-aircraft gate,
saved result and cold reload. The forced-return check does not establish any
of those later outcomes. Other mission successes, remaining runtime contracts,
typed state, audio fidelity and wider performance remain whole-port work.
