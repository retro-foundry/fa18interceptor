# Mode-five combat objective - 2026-10-08

Normal keyboard input now completes formation and both radar shoot-downs,
then reaches the original mode-five objective at tick 19,304. All 29
input/stage intervals and 118 sampled bodies match original compared
RAM/drawing. Coverage includes 32 consecutive formation-window bodies and
40 consecutive radar-hit/expiry-window bodies. Safe return, landing and the
saved successful mission remain open. This batch changes validation only;
the playable game source, executable and comparison masks are unchanged.

## Reached original behavior

The playable native path is `native_frontend_tick` -> `native_flight_tick` ->
the record/weapon update and postflight scheduler in `port/game/native/`.
Its existing `port/game/postflight_scheduler.c` C0A002 owner requires the
stolen-aircraft proximity countdown and both enemy expiry increments before
admitting objective phase `$FF`. C0A12E restores the stolen aircraft's views;
the preceding forced-return acceptance checks the actual original table and
both original calls separately.

This flight holds all 201 native proximity scans, reaches gate -1 at tick
17,996, and launches radar missiles at enemy records eight and ten. The two
hits advance the existing radar-hit log 1 -> 2 -> 3 and start each enemy's
original 15-tick expiry. Expiry accounting advances 0 -> 1 -> 2. The original
full-body comparison matches the actual objective transition from phase zero
to `$FF`, with gate -1 and two enemy expiries, at tick 19,304. The bounded run
ends at tick 19,500 with phase `$FF`, no reset, completion count unchanged at
3 and no result config write. The fixture correctly returns status 1 because
the complete mission is unfinished.

## Validation and retention

The optional `5-mission` pilot exists only in `tools/native/mission_pilot.c`,
linked into `fa18_native_mission_success_test`. It selects radar weapons with
two normal Return presses, follows the accepted formation path, retains an
active enemy target and emits ordinary pitch, rudder, bank, throttle and fire
keys. It reads current orientation and target motion to choose those keys.
It never writes hit, movement, expiry or objective state. An unused gun-aim
branch from the investigation was removed. Existing mode-three, numeric
mode-five and `5-formation` validation paths retain their accepted behavior.

`tools/native/check_mission_five_objective.py` registers the serial CTest
`fa18_native_mission_five_objective`. It uses a fresh save, fixes the diagnostic
end tick, requires the complete proximity scan sequence, checks both radar-hit
records and every body in the two 20-body combat windows, and verifies both
expiry increments and the objective phase transition. All exported input and
body cases are compared using original instructions. Passing RAM is temporary
and removed; 147 combined capture pairs use 294 MiB within the unchanged
480 MiB fixture limit. Build caches retain the existing 4 GiB cleanup hooks.

```powershell
cmake --build build/native-cmake --config Release --target fa18_native_mission_success_test --parallel 8
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mission_five_objective$' --output-on-failure
```

Debug and Release fixture builds pass. All four selected Debug checks pass:
mission-three success, mode-five forced return, mode-five objective and artifact
cleanup. All five selected Release checks pass, adding the mode-five combat/reset
regression. Metadata is in
`analysis/figures/native_mission_five_objective_checkpoint.json`; consumed keys
are retained in `tools/native/fixtures/mission-five-objective.e9k`. Local reports
are in `build/native-cmake/native/mission-five-objective-check/`; CTest logs use
`build/native-flight/mission-five-objective-{debug,release}-ctest.log`.

The playable executable remains SHA256
`73f9bc00fd5db4f1f3a3e90097b902e0a4eb2b4d8ad07a24789626a3549189cd`.

## Limits and next work

These comparisons execute original instructions from native before-states;
independent complete original/native flights remain unverified. The formation
and combat windows are consecutive, while the remaining flight is sampled.
The separate forced-return gate continues to retain its longer 64-body window.

An exploratory continuation reaches the objective but the validation pilot
subsequently climbs to the ceiling and crashes at tick 35,054. That later
trajectory has not been compared against the original and is not accepted
mission-success evidence. Continue the validation pilot through safe return,
landing, the stopped-aircraft admission, result save and cold reload. Other
mission successes, restart, remaining runtime contracts, typed state, audio
fidelity and wider performance remain whole-port work. The complete-port goal
stays active.
