# Normal-key mode-five mission success - 2026-10-08

Mode five now completes formation, both radar shoot-downs, its original combat
objective, a carrier arrestor landing, stopped-aircraft admission, result save
and cold reload using ordinary keys. All 34 input/stage intervals and 155
sampled bodies match original compared RAM/drawing, including all 59 consecutive
bodies from touchdown through the saved result. Completion count advances
3 -> 4 and mode-five grade 1 -> 2 without a crash reset. This extends validation
of existing gameplay; the playable source and executable are unchanged.

## Original path reached

The connected path is `native_frontend_tick` -> `native_flight_tick` -> the
native record/weapon owners and `port/game/postflight_scheduler.c`. Normal
flight holds all 201 proximity scans, reducing C0A002's gate from 200 to -1
at tick 17,996 and reaching C0A12E's paired stolen-aircraft restoration. Two
radar hits start the original 15-tick enemy expiries; accounting advances
0 -> 1 -> 2. Objective phase FF arrives at tick 19,304.

Read-only return telemetry identifies pose three and the original starting
position `[1153068, 119.03125, 1071520]`: this flight returns to the carrier.
The validation pilot retains world heading feedback, levels its bank, controls
height independently of the turn, and approaches the deck centerline. Original
A/raw $20 invokes `COMMAND_HOOK` in `port/game/flight_commands.c`; it deploys
the arrestor for aircraft kind $11. These are keyboard choices, with no flight,
collision, expiry, objective or contact RAM writes.

The carrier qualification fixture's previously accepted landing provides
the local reference for the wire region. The new mission touches down at tick
26,076, position `[1153079.59375, 119.03125, 1070793.546875]`. Contact advances
$8002 -> $C082 and region stays $C1: the hook is deployed, the wire captures
the aircraft, and grounded contact is present. The native flight stops while
still on the carrier. C0A3EA admits successful phase FC at tick 26,242.

At tick 26,265, the actual C110A4 result interval reaches
`record_postflight_result`/C11350. Completion count and grade advance, and the
C1643A config owner reaches exactly one original DOS Write. All 78 saved bytes
equal native pilot RAM; a cold reopen consumes those same bytes. The fixture
fails unless objective, carrier contact, zero speed, no reset, result fields,
saved contents and cold reload all agree.

## Repeatable gate and bounded evidence

`tools/native/check_mission_success.py --mode 5` is registered as serial CTest
`fa18_native_mission_five_success`. The shared checker retains mode-three
acceptance and reuses `objective_evidence` from the bounded mode-five checker.
The new `5-success` fixture argument uses the same normal-key pilot as
`5-mission`, with shorter capture windows for formation. It observes all 201
native scans, eight consecutive formation-window bodies, all 40 combat-window
bodies, and every body from carrier touchdown through result save. The bounded
objective gate still retains 32 formation-window bodies; the separate paired
restoration gate retains 64. Their source comparisons remain separate evidence.

The landing window ends naturally when the successful result is saved: all
59 bodies are consecutive. The capture limit remains 240 combined pairs /
480 MiB. This run uses 189 pairs / 378 MiB, with passing RAM discarded and only
reports and consumed keys retained. Existing build/CTest 4 GiB pruning remains
enabled. No comparison mask or game rule changes.

```powershell
cmake --build build/native-cmake --config Release --target fa18_native_mission_success_test --parallel 8
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mission_five_success$' --output-on-failure
```

Debug and Release fixture builds pass. Four selected Debug checks pass:
mission-three success, mode-five objective, mode-five success and artifact
cleanup. Six selected Release checks pass, adding mode-five combat/reset and
forced return. Metadata is in
`analysis/figures/native_mission_five_success_checkpoint.json`; consumed keys
are in `tools/native/fixtures/mission-five-success.e9k`. Local reports are in
`build/native-cmake/native/mission-five-success-check/`; CTest logs use
`build/native-flight/mission-five-success-{debug,release}-ctest.log`.
The final Debug success/cleanup passes are recorded in
`build/native-flight/mission-five-success-debug-recheck-ctest.log`; the
mission-three and bounded objective passes are in the first Debug log.

The playable executable remains SHA256
`73f9bc00fd5db4f1f3a3e90097b902e0a4eb2b4d8ad07a24789626a3549189cd`.

## Remaining whole-port work

Original instructions execute externally from native before-states. These
comparisons cover the sampled flight and consecutive windows, and do not
establish independent complete original/native flight parity. The failed early
return also matched the original; no gameplay fix was justified by it.

Mode-five success/save/reload supersedes the earlier open-landing checkpoint.
Result message completion and restart for the mission-list modes, successful
modes four, six, seven and eight, independent complete flights, remaining
runtime contracts, typed state, audio fidelity and broader performance remain
open. The complete-port goal stays active.
