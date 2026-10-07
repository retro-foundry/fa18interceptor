# Normal-input failure, menu return and control-stream restart

2026-10-07. The playable build is `fa18_native`, composed from the shared
runtime in `port/native/CMakeLists.txt`. Whole-game acceptance remains open.

## Connected fix

The affected caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_records_update` -> `advance_flight_record_stream` (C233AA).
At the end of stream seven, C234FA saves the selected record, C234FC clears
its header, and C23500 calls C28722 before restoring the record and advancing
through C23578. Native `FA_RESET_NEXT_RECORD` previously aborted as unavailable.

`port/game/native/records.c` now connects that call to the existing
`initialize_scene_from_mode(NULL)` owner in `port/game/scene_dispatch.c`.
The adjacent C2340E / `FA_RESET_STREAM_RECORD` uses the same original owner.
The parent owns its saved record and reloads the next stream. No CPU or
captured state supplies the playable operation; no new game rule is introduced.
Original call sites and bytes are retained in
`analysis/data/flight_record_actions_source_scope.json`.

The mode-two fixture now requires all seven streams and an observed seven-to-one
wrap, captures every body in stream seven, and exits using normal Escape input.
Its historical assertion of spontaneous failure by tick 10000 no longer matches
the corrected cold scene state. The original comparisons verify the current
loop and Escape route rather than reinstating that earlier failure.
Thirty input/stage intervals and 206 bodies match compared original RAM/drawing,
including the actual scene-reset body. This exercises the next-record call;
the adjacent stream-record call is connected but is not separately demonstrated
by this scenario.

## Natural failure and another flight

`native_mode_two_test.c` starts mode six from the ADF through intro/menu and
ordinary throttle, stick, target and fire events. After tick 20000 it presses
throttle and nose-down controls. After each aircraft reset it releases and
presses those controls again, because the source clears its input latches.
Only key events drive the scenario: the test seeds no actor, motion, terminal
flag, outcome, timer or input result.

The scenario observes three aircraft losses in the actual pilot-log counter,
reset states 3/2/1/0, C118A0's failure publication, C118E6's completion and the
C0F920 return. It reaches the native menu at tick 39095 with 8759 scene frames.
Using that same runtime and log, ordinary digit 4 and Return events then start
Free Flight and produce 421 more scene frames. The fixture requires the active
C10DAE callback and mode 125 after this second launch.

Eighty-six input/stage intervals and 237 sampled bodies across failure and
relaunch match original compared RAM/drawing. The reference executes original
instructions from each sampled native before-state, using the existing host
input/timer contracts. These are sampled composition comparisons, not an
independent original replay of the entire mission or proof of mission success.
The outcome itself is a complete native normal-input failure/return/relaunch.
Existing scratch, asynchronous voice and busy exclusions remain unchanged;
all drawing bytes stay compared.

## Reproduction and remaining work

Both scenarios are registered in CTest. Their Python wrappers keep passing RAM
in temporary directories and retain compact logs/reports plus failed cases.
Reference builds use shared GNU objects and remain serial.

Native Debug/Release builds pass. Seven affected Release CTests pass: mode two,
mode six, natural outcome, countermeasures, frame body, frontend and artifact
cleanup. The frontend check also verifies the native link's emulation omissions.

```powershell
python tools/native/check_mode_two.py --mode 2 --out build/native-flight/mode-two-stream-reset
python tools/native/check_mode_two.py --mode 6 --outcome --out build/native-flight/natural-outcome-six
```

The compact checkpoint is
`analysis/figures/native_natural_outcomes_checkpoint.json`.
Full successful missions, complete combat and independent sequence comparisons,
remaining callback/reset contracts, readable typed state, audio fidelity and
broader visible-window performance remain unfinished. The complete-port goal
stays active.
