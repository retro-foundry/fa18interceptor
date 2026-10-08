# Search and Rescue sequence - 2026-10-08

Search and Rescue (F4 / internal mode six) completes through ordinary keys:
deploy the rescue pod, satisfy the original objective, return to the carrier,
catch the wire, stop, save, finish messages and press Escape to return to the
menu. Completions advance 3 -> 4 and mode-six grade 0 -> 1. All 78 saved bytes
survive cold reload. The playable runner delivers the same 870 host events,
returns to the same menu and saves identical bytes without a crash reset.

All 44 input/stage intervals and 193 sampled bodies match original compared
RAM/drawing. These include 20 consecutive pod-release bodies, 20 ground-contact
bodies, 80 consecutive landing/result bodies and one original config write.
The 237 before/after pairs remain inside the existing 240-pair / 480 MiB cap.
Passing RAM is temporary; the 4 GiB build-cache pruning hooks remain enabled.

## Connected original path

`native_frontend_tick` -> `native_flight_tick` -> command selection/publication
-> `flight_commands.c:COMMAND_FLARE` -> record actions ->
`initialise_flight_record_release` (C23716) -> record motion/ground contact ->
`postflight_scheduler.c:mode_six` (C0A15C) -> readiness and result/save/menu.
Existing readable owners implement this entire sampled path. No runtime code,
game rule, comparison mask or dependency is changed by this batch, and no new
decompilation is claimed. New input and observation code belongs only to the
validation executable, which is not linked into the playable runner.

The initial pilot is the unmodified original ADF log in a fresh save directory.
It already admits this mission. This does not prove availability earned through
a complete port-played tour, and no flight state is seeded.

Shift+F reaches the original modifier-aware flare command: raw F $23 sees
modifier one, clears it and publishes action nibble $F at tick 15,134.
C1C23C clears the one-shot modifier after command publication, including key
releases. The validation pilot pauses steering changes around the chord so an
unrelated release cannot consume Shift. These are normal host events.

C23716 releases kind $31 into primary record one ($0200 offset). Its first
observed timer is -19 at body 7,172 / tick 15,136. Original autonomous motion
slows the pod and drops it to the ground at body 7,290 / tick 15,534. The ground
flag changes $50DA -> $D0DA. Its settled timer reaches zero at body 7,295 /
tick 15,551, admitting FF with sequence phase three. Horizontal fixed-point
distances to site record eleven are 11,691 and 5,747 (45.668 and 22.449 world
units), both within the source's inclusive $10000 bound on each axis.

The input pilot flies a higher return over terrain and a longer approach.
Deck touchdown occurs at body 10,441 / tick 24,577 ($8002 -> $8082, region $40).
Nine bodies later, the original geometry catches the wire at body 10,450 /
tick 24,608 ($8082 -> $C082, region $C0). The checker requires this wire
transition within the consecutive landing window; the earlier missions retain
their immediate-wire touchdown guard. The stopped aircraft admits FC at
24,773, C110A4 saves at 24,848, messages finish at 25,833, and Escape/menu
restart reaches C0FCB4 at 25,872.

## Validation and remaining scope

The serial gate is `fa18_native_mission_6_sequence`:

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_mission_6_sequence$'
```

The checker requires the original modifier/action path, pod kind/record/timer,
ground contact, near-site success, both complete 20-body windows, carrier wire
capture, consecutive landing/save coverage, completion/grade progression,
one actual config write, result callbacks, Escape/menu, cold reload and matching
playable replay. Evidence is in
`figures/native_rescue_mission_sequence_checkpoint.json`.

Debug/Release mission fixture builds pass. Eight selected Release checks pass:
mode-three/four/five/six/eight sequences, frontend/link checks and artifact
policy/cleanup. Three Debug checks pass: rescue sequence and artifact
policy/cleanup. Both configurations agree on inputs, pod events, landing,
saved bytes and result/menu sequence. The playable Release binary is unchanged
by this validation batch, and the frontend gate confirms its Release link map.
Build-cache use is 2.00 GiB after pruning.

Original instructions receive native before-states in a separate process.
This demonstrates the compared boundaries, rather than an independent original
whole flight. Successful internal mode seven, port-earned tour availability,
independent full-flight parity, remaining caller contracts, typed state, audio
and wider performance remain open. The complete-port goal stays active.

The user's mission six refers to F6 / internal mode eight, the final Carrier
Sub mission. Its accepted patrol/completion/save/menu/wrap sequence is documented
in `native_final_mission_sequence_milestone.md`; the rescue work is separate.
