# Final mission completion context - 2026-10-08

The user's mission-six concern refers to the final Carrier Sub mission.
The earlier handoff incorrectly associated it with the internal `mode_six`
scheduler. That scheduler belongs to Search and Rescue.

## Mission numbering from original data and code

`analysis/data/mission_text_inventory.json` contains the original F1-F6 labels.
`port/game/indexed_commands.c:execute_indexed_command_result` converts function
keys to mission modes by adding three to the zero-based function-key index.
The original qualification and availability guards still apply.

| Menu key / mission | Internal mode | Original mission label |
| --- | --- | --- |
| F1 / 1 | 3 | Visual Confirmation |
| F2 / 2 | 4 | Emergency Defense |
| F3 / 3 | 5 | Intercept Stolen Aircraft |
| F4 / 4 | 6 | Search and Rescue |
| F5 / 5 | 7 | Intercept Incoming Cruise Missile |
| F6 / 6 | 8 | Carrier Sub |

Existing milestone names such as "mission-four sequence" identify internal
mode four (the second menu mission); their accepted evidence is unchanged.

## Historical account supplied by the user

[Wikipedia's gameplay section](https://en.wikipedia.org/wiki/F/A-18_Interceptor#Gameplay)
reports disputed final-mission objectives, attributes possible submarine
destruction without a visible explosion to Bob Dinnerman, and describes patrol
aircraft destruction as sufficient for completion. It also reports menu return
and a wrap to the first mission instead of an ending sequence. These are
historical claims, not independent acceptance evidence for this port.

The article links an [interview with Dinnerman](https://steiny.typepad.com/premise/2004/01/interview_with_.html)
and a [recorded final-mission completion](https://www.youtube.com/watch?v=xdojCc3HUT0).
Neither linked page could be fetched during this check; their contents were
not independently verified. The original code/disk behavior remains authoritative.

## Original scheduler and counter ownership

`source_amiga/observed/dispatch_c09e3c_mode_scheduler.asm` and
`port/game/postflight_scheduler.c:schedule_postflight` send mode eight to
C0A364 / `POSTFLIGHT_MODE_OTHER`, then `generic_mode` / C0A39E.
Unlike `mode_six`, this branch does not inspect rescue target position/lifetime.
It compares `SCENE_DISPATCH_ADMITTED` against `SCENE_DISPATCH_AUX`, waits for
the original two-step countdown and sequence-phase condition, then publishes
successful objective phase FF. It contains no direct submarine-explosion test.

The quota comes from the original mode/variant scene table. In
`scene_dispatch.c:initialize_scene_from_mode`, C287A6 publishes table-row byte
three to C458AA / `SCENE_DISPATCH_ADMITTED`; initialization resets C458AB /
`SCENE_DISPATCH_AUX` to zero. `dispatch_scene_records` uses admitted versus
created to limit spawns. C242CA also increases admitted alongside created when
an action-kind-eight record loses its paired-record bit. The readable owners
are `control_records.c` and `main_loop_flight_controls.c`; this is not a
submarine explosion requirement.

`flight_dynamics.c:advance_indexed_record_dynamics` owns C25B66's expiry path.
When destruction bit 0400 is set and its timer expires, C25BBE clears that bit.
The original C25BC4-C25BEE predicates increase C458AB only for aircraft class
10, excluding raw kind 15, records with byte-one bit three, and records with
byte-three bit seven. The generated comparison listing preserves these original
instructions; it is reference evidence, not the playable implementation.
An object of class 20 does not contribute through this aircraft-expiry branch.
Addresses, classes, kinds and flag masks in this paragraph are hexadecimal.

The active runtime reaches these readable owners through
`native_frontend_tick` -> native flight/record updates. The mode-eight scheduler
callback in `native/records.c` calls `POSTFLIGHT_MODE_OTHER` / C0A364. Its signed
quota comparison must pass before the original two-step countdown publishes FF.
No original gameplay routines were changed or newly decompiled in this batch.

## Accepted three-aircraft diagnostic

The serial gate `fa18_native_final_patrol_diagnostic` runs the existing
availability-only saved-pilot fixture, now recorded explicitly in
`tools/native/fixtures/final-mission-eligible-pilot.json`. Its prerequisite byte
was set by the established mode-eight test fixture and reopened through the
original loader. It does **not** establish earned final-mission availability or
a completed tour. The game receives only normal keyboard controls during flight.

This fixture's quota is four; that value is not a universal mission constant.
At tick 21,000, three counted aircraft destructions have occurred:

| Record slot | Hit body | Expiry body | Counter change |
| --- | --- | --- | --- |
| 10 | 6,353 | 6,368 | 0 -> 1 |
| 12 | 6,758 | 6,773 | 1 -> 2 |
| 8 | 8,013 | 8,028 | 2 -> 3 |

All 24 input/stage intervals and 120 sampled bodies match original compared
RAM/drawing. Each hit includes 20 consecutive compared bodies spanning the
15-step expiry. Two radar-counter hits and one infrared-counter hit are
observed; source expiry predicates and aircraft class are checked separately.
Record 14 remains active with class 20. Player phase remains zero, completions
remain three, and the original comparison performs no config write. The strict
native success fixture correctly rejects this incomplete flight.

The actual playable runner replays all 2,004 host events with none pending or
queued. Its captured RAM must independently agree on quota four, expiries three,
phase zero, all weapon-hit counters, completions and record 14's class/flags.
Its saved config remains unchanged. This distinguishes runtime integration from
the original boundary comparisons. Passing RAM is temporary and removed.

Reproduce with:

```text
cmake --build build/native-cmake --config Release --target fa18_native_mission_success_test
ctest --test-dir build/native-cmake -C Release --output-on-failure -R ^fa18_native_final_patrol_diagnostic$
```

The test pilot's target selection, missile cycling and firing thresholds are
validation input choices. It is never linked into `fa18_native`. Gameplay,
comparison masks, user settings and the accepted mission-four sequence remain
unchanged. See `analysis/figures/native_final_patrol_checkpoint.json` for build,
input and report hashes and validation configuration.
Debug and Release fixture builds and the new diagnostic pass. The three existing
Release mission sequences and artifact policy/cleanup checks pass as well; Debug
artifact checks pass. Both configurations produce identical inputs, final
counters and unchanged saved config. Build-cache use remains 2.00 GiB within
the 4 GiB budget; the canonical Release playable is unchanged.

## Remaining acceptance

Complete the fourth counted aircraft destruction through ordinary controls,
then compare original FF objective, landing, stopped-aircraft admission, config
save and menu return. Verify next-mission wrap through the original pilot-log
selection path. A longer input-pilot attempt reaches three expiries but crashes
before completing the fourth; this is a pilot diagnostic, not evidence that the
mission is impossible. Earned availability and independent original complete-
flight parity also remain open. Do not invent a submarine explosion, weaken the
counters or declare final-mission completion from the partial diagnostic.
