# Cruise-missile interception sequence - 2026-10-08

Intercept Incoming Cruise Missile (F5/internal mode seven) completes through
ordinary keys: a radar missile intercepts record four, the original objective
is admitted, and the player returns to the carrier, catches the wire, stops,
saves, finishes messages and presses Escape to reach the menu. Completions
advance 4 -> 5 and mode-seven grade 0 -> 1. All 78 saved bytes survive cold
reload. The initial cruise availability is earned by the accepted rescue save,
starting from the original ADF's pilot log.
The playable runner consumes all 1,533 recorded host events, reaches the same
menu and saves identical bytes without CPU/chipset emulation or queued inputs.

All 42 input/stage intervals and 171 sampled bodies match original compared
RAM/drawing, including 20 consecutive interception/expiry bodies, 80 consecutive
landing/result bodies and one actual config write. The 213 before/after pairs
remain inside the existing 240-pair / 480 MiB cap. Passing RAM is temporary;
the 4 GiB pruning hooks remain enabled.

## Connected original path

The active build remains `fa18_native`, composed by `port/native/CMakeLists.txt`.
The caller is `native_frontend_tick` -> `native_flight_tick` -> native record
actions/motion -> C26EBE candidate geometry -> C25B66 record dynamics ->
record-update finish -> C09E06 -> C0A1E0 / `postflight_scheduler.c:record_mode`
with mode-seven selection -> C0A3EA readiness -> C110A4 result/save/menu.

Existing readable owners implement this sampled path. Runtime gameplay,
arithmetic, comparison masks and dependencies are unchanged, and no new
decompilation is claimed. Pilot choices and observation additions are confined
to the validation executable; it is not linked into the playable runner.

The original C0A1E0 requires a nonzero record-four cell (+6 OR +12). It waits
while an active record still has byte-one bit seven, then runs the two-step
countdown. Record-four +$20 bit one admits FF; otherwise the original indexed
context command and FE failure path apply. The original instruction scope and
earlier complete-call evidence are in `routines/native_c_postflight_scheduler.md`.

The input pilot selects record four through normal T commands and fires a
radar missile at tick 14,438. At body 6,686 / tick 14,776, target kind $15 gains
the interception bit ($40 -> $42 in +$20), flags $70C1 -> $54C1 and a 15-body
destruction lifetime. Radar hits advance 1 -> 2. At body 6,701 / tick 14,827,
the active bit clears ($74C1 -> $1081). The next body, 6,702 / tick 14,828,
admits FF with sequence phase three. The complete 20-body window includes
interception, expiry, countdown and admission.

The validation pilot uses a higher, longer return approach, ordinary F10/plus
on the long leg to separate from the remaining enemy, and F5 for landing.
Deck contact occurs at body 8,975 / tick 21,739 ($8002 -> $8082, region $40).
Wire capture follows nine bodies later at body 8,984 / tick 21,769
($8082 -> $C082, region $C0). The stopped result admits FC at 21,934,
the real save occurs at 21,958, messages finish at 22,931, and Escape/menu
restart reaches C0FCB4 at 22,970. No crash reset occurs.

## Earned availability and acceptance

`tools/native/fixtures/rescue-sequence.e9k` preserves the 870 host events from
the accepted rescue flight. Its actual save is retained in
`tools/native/fixtures/cruise-mission-pilot.json`, with input/config/ADF hashes.
`cruise_pilot_fixture.load_cruise_pilot()` checks those seals. The existing
rescue gate now reproduces both exact inputs and saved bytes, including its
original comparisons and playable replay. No availability byte is synthesized.
The original ADF already contains earlier pilot progress; a complete tour from
a newly enlisted pilot remains open.

The serial CTest is `fa18_native_mission_7_sequence`:

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_mission_7_sequence$'
```

It checks the intercepted target kind/flag/lifetime, active-bit expiry, original
success predicate, complete combat and landing/result windows, wire capture,
completion/grade progression, one config write, messages, Escape/menu, cold
reload and a matching replay in the playable runner. Checkpoint evidence is in
`figures/native_cruise_mission_sequence_checkpoint.json`.

Debug/Release fixture builds pass. Nine selected Release checks pass: all six
mission sequences, frontend/link checks and artifact policy/cleanup. Three
Debug checks pass: cruise sequence and artifact policy/cleanup. Both
configurations agree on input and saved hashes, interception/expiry, landing,
messages and menu sequence. The playable Release executable is unchanged,
its Release link-map gate passes, and cache use is 2.00 GiB after pruning.

Original instructions receive native before-states in a separate process.
These comparisons demonstrate the sampled boundaries, not an independent
original whole flight. All six menu missions now have successful native routes,
but full port acceptance remains unproven. Earned complete-tour/final-mission
availability, independent complete-flight parity, remaining caller contracts,
typed state, audio and wider performance remain open. The complete-port goal
stays active. Decompile any remaining instruction translation encountered on
the connected path, including its required tails and child contracts.
