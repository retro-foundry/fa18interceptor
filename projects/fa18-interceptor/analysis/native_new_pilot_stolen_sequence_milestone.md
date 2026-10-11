# Newly enlisted pilot: stolen-aircraft sequence - 2026-10-08

A newly enlisted pilot now has an accepted ordinary-key route through carrier
qualification and the first three menu missions. Intercept Stolen Aircraft
(F3/internal mode five) starts from the actual mission-four save, completes
formation and two radar kills, returns to the carrier, catches the wire, stops,
saves, finishes messages and presses Escape to return to the menu. Completion
count advances two to three and mode-five grade zero to one; cold reload
preserves all 78 bytes. This extends the earned tour, which remains incomplete.

## Original rules and connected evidence

The active playable runner is `fa18_native`. Its existing
`native_frontend_tick` -> native flight/records -> `postflight_scheduler.c:mode_five`
path executes C0A002's proximity checks and paired-aircraft restoration.
`indexed_commands.c:execute_indexed_command_result` admits F3 from the saved
mode-four grade at log byte 22. C110A4/C11350 update the grade and completion
count, and the original config owner writes the resulting log.

`region_pilot_fixture.py` reproduces menu reset/callsign NEW, qualification and
mission-three save. The existing mission-four gate now reproduces exact retained
inputs and its 78-byte saved log in `stolen-mission-pilot.json`.
`stolen_pilot_fixture.load_stolen_pilot()` checks that chain, ADF identity and
original availability/result fields. No flight state or grade is synthesized.
The new mission-five validation receives that log through the existing loader.

The pilot remains at original level zero. Original scene admission selects a
different encounter from the historical level-two ADF pilot: radar hits kill
records eight and twelve rather than eight and ten. The new gate requires those
exact targets, both 15-step expiry windows, counters zero -> one -> two, all
201 consecutive proximity decrements and the source FF objective. The earlier
level-two gate keeps its original targets and controls.

The first new-pilot attempt crashed before formation; all 26 intervals and
50 sampled bodies matched original compared RAM/drawing. It justified changes
to validation input only. The `5-tour` test pilot retains flying speed, damps
height control and pursues the regional enemy. It releases/represses ordinary
controls after the original result camera clears them, as existing escort and
rescue pilots do. It is never linked into the playable runner. No gameplay,
physics, comparison mask or original outcome predicate changed; no additional
decompilation is claimed.

## Accepted outcome and bounded captures

Objective admission occurs at tick 18,284. Carrier wire touchdown at 31,454
changes contact $8002 -> $C082 in region $C0. The aircraft stops and reaches
phase FC at 31,620; save occurs at 31,643 and menu return at 32,650.

All 42 input/stage intervals and 174 sampled bodies match original compared
RAM/drawing, including eight formation bodies, 40 combat bodies and 64
consecutive landing/result bodies. The original comparison performs one config
write. The playable replay delivers all 2,777 host events and agrees on saved
bytes and menu, without pending input or crash reset. Independent original
whole-flight parity remains open: original instructions receive native
before-states at these sampled boundaries.

The serial gate is `fa18_native_mission_5_new_pilot_sequence`; it invokes the
existing checker with `--mode 5 --sequence --new-pilot`. The retained input is
`tools/native/fixtures/mission-five-new-pilot-sequence.e9k`. Captures use 216
pairs within the unchanged 240-pair / 480 MiB cap; passing RAM is temporary.
Build/CTest pruning remains bounded at 4 GiB.

Search and Rescue, cruise interception and the final mission still need earned
new-pilot progression from this save. Their existing successful routes start
from earlier ADF pilot progress. Independent whole flights, remaining caller
contracts, typed state, audio and broader performance remain open; the complete
port goal stays active. Sealed evidence and configuration checks are in
`figures/native_new_pilot_stolen_sequence_checkpoint.json`.

Seven selected Release checks pass: the new route, historical mission-four/five
sequences, reset/qualification/mission-three progression, artifact policy/cleanup
and playable frontend/link validation. Five Debug checks pass: both historical
sequences, the new route and artifact policy/cleanup. New input/save hashes,
objective, landing and menu evidence agree across configurations. Historical
mode-five input/save hashes are unchanged. The playable Release executable is
unchanged, passing RAM is removed and pruned build use remains 2.00 GiB.
