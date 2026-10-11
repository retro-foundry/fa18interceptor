# Repeated individual mission acceptance - 2026-10-09

The revised campaign criterion is met. The user explicitly waived an
uninterrupted qualification-plus-six-mission run and accepted repeated individual
mission tests instead. Cold starts sharing actual earned saves are allowed.
The unsuccessful final flight in the earlier single-process investigation is
therefore no longer a completion blocker. Other port acceptance work remains.

## Current playable verification

The active Release and Debug executables each complete the existing earned cold
tour: enlistment, carrier qualification, all six missions, saved sixth result,
menu return and Next Mission wrap. The save directory starts empty; only the
playable runner writes the pilot log. No flight state or eligibility is supplied
by the tour checker. All nine stages match exactly between builds, including
every canonical counter, replay input hash and all 78 saved bytes. No CPU/chipset
emulation, crash reset, pending replay event or queued input is reported.

This repeats every mission with the current executable after the title/music
and Free Flight renderer fixes, in addition to the preceding per-mission source
gates and earned-tour runs. The new individual-tour repetitions take 118.59
seconds in Release and 264.44 seconds in Debug; these are unpaced headless test
durations, not visible gameplay performance measurements.

Fresh Release qualification and final-mission gates also compare original
instructions from observed native before-states. Both final-mission saved-pilot
contexts earn four airborne aircraft expiries, complete the objective, land on
the carrier, save the sixth result, return to menu and wrap to the next mission.
Each final source comparison runs the complete flight twice in capture
partitions, then replays its ordinary keys in the actual playable runner.

| Fresh Release source gate | Input/stage intervals | Sampled bodies | Consecutive landing bodies | Original config writes |
| --- | ---: | ---: | ---: | ---: |
| Qualification/original pilot | 50 | 174 | 96 | 1 |
| Qualification/new pilot | 50 | 174 | 96 | 1 |
| Final mission/original earned pilot | 42 | 239 | 64 | 1 |
| Final mission/newly enlisted earned pilot | 64 | 220 | 64 | 1 |

Seven Release and three Debug CTests pass, including artifact policy and cleanup.
The canonical Release executable matches the tested Release hash. Reports and
hashes are sealed in
`figures/native_individual_mission_acceptance_checkpoint.json`; compact original
reports and CTest logs remain under
`build/native-flight/individual-mission-acceptance/`.

## Scope and continuation

The latest failed bank/height input experiments and early-failure change were
reverted before these builds. No playable runtime or accepted test-pilot controls
changed in this batch. Fixed-input repeats establish reproducibility across
builds and two saved-pilot contexts; they do not exhaust all spawn outcomes.
The source comparisons remain sampled and native-seeded, rather than independently
started complete original flights.

Sealed cold reference inputs remain intact. The accepted gear-managed five-mission
single-process prefix remains separate evidence. Future generated flying inputs
must still raise gear after takeoff and lower it before landing as directed.

Remaining acceptance work is independent original whole-flight comparison/HUD
timing, initial startup sorting, original recorded sound/filter fidelity, visible
gameplay performance/presentation, and remaining named C state ownership.
`measure_visible_performance.py` currently expects a continuous geared campaign
report; adapt its current acceptance path to validated individual geared routes
before further visible mission measurements. The waived uninterrupted campaign
must not remain a prerequisite for that work. Historical gear-down benchmark
inputs are not current performance acceptance routes.

Passing RAM is discarded; the existing 240-pair/480 MiB final capture limit and
4 GiB build pruning hooks remain enabled. The complete-port goal stays active.
