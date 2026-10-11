# Newly enlisted pilot: earned tour - 2026-10-08

A pilot enlisted as NEW at original level zero earns carrier qualification and
all six menu missions, with objective, landing, saved result and menu return.
The final mission advances completion count five to six and its grade zero to
one. A cold Next Mission selection wraps to mission one/internal mode three.
The carrier submarine remains active at aircraft-objective
admission. The original counter path does not require its visible explosion.

## Connected behavior and preserved rules

The playable runner remains `fa18_native`, through `native_frontend_tick`,
native record/flight owners, `postflight_scheduler.c` and the original config
write owner. `indexed_commands.c:execute_indexed_command_result` admits the
final mission from the preceding cruise grade at saved-log byte 25. No new
playable behavior is implemented in this batch; validation replaces the
inherited ADF pilot progress with the actual newly enlisted saved chain.

`region_pilot_fixture.py` establishes normal menu reset/callsign, qualification
and mission-three save. The existing escort and new stolen-aircraft gates
establish the following saves. `tour_pilot_fixture.py` checks the actual
78-byte log, preceding log/input seals and original availability fields for
rescue, cruise and final. Availability is never synthesized.

The final `generic_mode` scheduler still uses the original four admitted
aircraft versus four expiry counter, countdowns and FF04 completion word.
`flight_dynamics.c`'s C25B66 expiry path excludes raw type $15 cruise missiles
from the aircraft expiry count. The original regional admission path can
consume an admission slot for a cruise missile. In one observed level-zero
route this leaves three aircraft expiries and no fourth aircraft; sampled
original comparisons match that stalled/crashed route. The successful test
pilot stays outside C29720's cruise region until another patrol is admitted.
This is a choice of normal pilot input, not a change to the original quotas
or regional rules. The user's report concerns F6/final Carrier Sub/internal
eight, not F4/Search and Rescue/internal six.

`MissionPilot` is a validation-only keyboard driver, never linked into the
playable runner. Its `8-tour` route uses observed attitude/target feedback,
two radar and two infrared kills, and ordinary C/F countermeasure keys.
`flight_commands.c:countermeasure` decides inventory use and diversion.
Six chaff and five flare presses and their releases are compared at actual
input boundaries against the original instructions. The earlier undefended
route reached four kills but crashed; its sampled final kill/objective/camera/
crash bodies also match the original. No physics, AI, damage, completion
predicate, comparison mask or audio implementation changes.

The final deck contact occurs at tick 38,847 in region $C0 with the arrestor
deployed. The wire catches one body later, at 38,851. The gate requires that
actual compared wire transition inside all 64 consecutive landing/result
bodies. Stop/result occurs at 39,039, followed by save, messages, Escape and
menu at 40,049. Surface record 14 remains active with raw kind $20 when the
fourth aircraft expiry admits success. The playable replay consumes all
6,585 flight host events and agrees on every saved byte and the menu; the
cold wrap consumes four events without changing the saved result.

## Reproducible evidence and scope

The serial per-mission gates are
`fa18_native_mission_6_new_pilot_sequence`,
`fa18_native_mission_7_new_pilot_sequence` and
`fa18_native_mission_8_new_pilot_sequence`. They compare original instructions
from native before-states, drawing bytes, objective/landing/result transitions
and the actual config write, then replay the same keys in the playable runner.
They retain rescue/cruise/final input fixtures and actual prerequisite/result
logs. Historical ADF-pilot routes keep their existing controls and checks.

| New-pilot mission | Flight input intervals | Sampled flight bodies | Consecutive landing bodies | Original config writes |
| --- | ---: | ---: | ---: | ---: |
| Search and Rescue/internal 6 | 44 | 191 | 80 | 1 |
| Cruise interception/internal 7 | 42 | 171 | 80 | 1 |
| Final Carrier Sub/internal 8 | 64 | 220 | 64 | 1 |

The final comparison also covers one cold-wrap interval/body. Its capture
partitions use 156 and 130 pairs; rescue and cruise use 235 and 213 pairs.

`fa18_native_new_pilot_tour` starts with an empty save directory, enlists and
qualifies normally, then replays missions three through eight using the same
directory. Only the game writes config. Each stage verifies the complete
78-byte earned result; cold starts load that actual preceding save. All six
grades are one and the completion count is six. A final cold Next Mission
selection verifies mode-three wrap with the saved result preserved.

This proves a playable earned tour across cold save/load boundaries. It does
not prove an uninterrupted single-process tour or independent original
whole-flight parity. Those comparisons, remaining caller contracts, typed
state/address arena, audio fidelity and wider performance remain open. The
complete-port goal stays active. No new decompilation was encountered or
claimed: the examined owners were already readable. The inventory remains
84 deferred, 514 readable translated and 75 readable source-only entries.

Final captures partition two identical flights at tick 23,000, retaining no
passing RAM between partitions. The 240-pair / 480 MiB cap and 4 GiB build
pruning hooks are unchanged. The aggregate tour stores no RAM snapshots.
The first Debug final capture exceeded its 90-second process deadline. The
longer level-zero final gate now allows 120 seconds per Debug run; the aggregate
gate uses that per-stage deadline too. Release remains 60 seconds. Game ticks,
capture limits and result assertions are unchanged.
Configuration checks, compact source/canonical evidence and seals are in
`figures/native_new_pilot_tour_checkpoint.json`.

Nine selected Release checks and seven Debug checks pass. These include the
three later new-pilot gates and earned tour in both configurations, the
stolen-aircraft regression in both, and the historical final and playable
frontend/link checks in Release. Artifact policy/cleanup pass in both. Rescue
and cruise were validated earlier in this batch before final-only test-driver
changes. The Debug final capture passes after the deadline increase; all
original RAM/drawing comparisons, result sequences and canonical outputs agree
with Release. All nine aggregate tour/wrap stages agree exactly. The playable
Release/Debug executable hashes remain unchanged. Passing RAM is removed;
pruned build-cache use is 2.01 GiB.
