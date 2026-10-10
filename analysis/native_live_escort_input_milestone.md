# Live-carrier escort input — 2026-10-10

Follow-up: [the complete escort body sweep](native_escort_complete_bodies_milestone.md)
now passes all 6,954 actual native bodies after correcting the ground
accumulator producer. The original recording, final RAM, pilot and counters
remain exact. The three-body scope below describes this earlier milestone.

An independently started native game now replays qualification and the complete
preceding mission, completes escort, lands on its live carrier, catches the
wire, earns the second mission completion and returns to the menu. A second
ordinary start in **the playable `fa18_native` executable** reproduces every
input-boundary trace row, complete final RAM and saved-pilot byte exactly.
This qualifies the native control recording. Original/native whole-flight
state and drawing acceptance remain separate and open.

The original full-flight recording starts its carrier 16,384 X units west of
this native game's carrier, as the preceding constructor-clock evidence
explains. The validation pilot therefore reads the native carrier's actual
arrestor vertices, rather than steering to coordinates from original RAM.
Its wire target is `(1153000, 119.03125, 1070584)`. Its optional final approach
aims at the observed aircraft touchdown height while retaining the existing
wire-centerline steering. These are validation keyboard choices, never new
game mechanics. Only the test pilot's temporary goal is adjusted; no game
position, geometry, clock, eligibility, contact flag or grade is written.

| Successful native observation | Result |
| --- | --- |
| First observed carrier contact/arrest | Frame 74,413; update 46,422; game tick 5,093; contact `C082` |
| Aircraft position at that boundary | `(1153010.46875, 119.03125, 1070779.75)` |
| Arrested stop | Frame 74,579; update 46,471; contact `C482`; speed zero |
| Earned second completion | Frame 74,601; escort grade one |
| Final menu | Frame 75,601; update 47,349; mode zero; `C0FCB4` |
| Resets, pending input | Zero |

Gear is raised after airborne contact is observed and lowered before landing.
The exact full trace has **27,176 rows**, covering the earned prefix and the
entire escort attempt. Complete RAM SHA-256 is
`5a787dc2c12cae220561051b498bbc91d9e55f03e513844d4470643b834c5dec`;
decoded trace SHA-256 is
`31f68f5b466b02b7f31860e20d173853df54affaf37ac25a310fb8f5cb895e1c`.
The full 78-byte earned pilot also matches between driver and canonical replay.

The original prefix is verified against its complete unchanged trace, RAM,
controls and explicit JSR/LINK identities. Its three existing declared menu,
context and flight-entry anchors retain all **1,432 prefix key edges**. The
controller begins after source update 40,644 / actual native update 40,398.
The playable replay extends only the diagnostic end bound and supplies the
recorded host keys. Every native wait remains counted. No reference clock,
RAM initialization, state search or fitted timing offset enters gameplay.

The connected path remains `port/native/main.c -> native_replay_update ->
native_frontend_tick -> native_flight_tick`. The driver uses the same frontend
with `begin_update` to select and record ordinary keys; it is not linked into
the playable executable. The native runner remains SHA-256
`36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.
There is no playable code change or newly removed runtime dependency.

The original curved final produces a rejected native attempt: escort objective
is reached, but the aircraft meets the carrier below deck height and resets
without a grade. A wider runway-style final lands in water without arrest or
a grade. Both complete attempts reproduce exactly in the playable runner and
remain explicitly rejected. An intermediate height-profile bookkeeping error
is also retained as an invalid intended controller profile: it overwrote the
test goal's first initialization. The final implementation restores only a
goal that was temporarily adjusted; the corrected route above succeeds.

The failed curved attempt is compared strictly with **6,027 corresponding
original observations**, **6,014 updates** and all **13 duplicate observations**
through its reset entry. The successful original recording continues for
another 1,143 flight observations; that remainder is not silently counted as
compared. Full cores, named fields and pages retain their differences. The
first selected motion difference is at game tick 150, immediately after gear
retraction. Steering bytes change one update earlier in native. This locates
an input/cadence difference; it does not explain every later difference.

Actual native bodies at updates **40,542–40,544** execute original
`C0EFEA–C0F3C0` instructions with **zero gameplay differences and zero display
byte differences**. The external oracle receives those native before/after
exports and the existing API timer contract. Its existing exclusions for ABI
scratch, asynchronous voice state, blitter busy counters and oracle stack
remain explicit. Reference state never feeds native gameplay. Three passing
bodies support that boundary's physics/drawing; they are not an entire-flight
original-rule proof or a validation of the successful arrest boundary.

The default patrol regression passes in the final driver. Its physical keys,
prefix, saved pilot and every current canonical runtime counter exactly
preserve the regression before the optional height-profile change. Against the
older sealed native milestone, input and save hashes remain exact; model-call
and nonzero-PCM counters reflect earlier renderer/audio changes and are not
claimed unchanged. Release build, Python compilation and diff checks pass.

All RAM and traces are retained compressed. Verified identical driver/canonical
files share compressed storage; duplicate passing failure-copy exports are
removed after byte equality. Temporary raw captures are gone. The 512 MiB
diagnostic and 4 GiB build-cache budgets remain enabled. The
[checkpoint](figures/native_live_escort_input_checkpoint.json) binds source
identities, each accepted/rejected outcome, actual landing observations,
original-body checks, regression, retention and helper hashes.

```powershell
cmake -S port/recomp -B build/native-cmake -DFA18_NATIVE_ONLY=ON -DBUILD_TESTING=ON
cmake --build build/native-cmake --config Release --target fa18_native_recorded_patrol_input_test --parallel 8
python tools/native/check_recorded_patrol_input.py --test build/native-cmake/native/Release/fa18_native_recorded_patrol_input_test.exe --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-escort-wire-target --source-updates build/native-flight/original-escort-wire-updates --level-final --out build/native-flight/<new-live-escort-input>
```

`--assess-failed-flight` retains and replays a rejected escort attempt while
still returning failure on its qualification; it does not relax acceptance.
Full original/native flight acceptance and sound timing remain open. Campaign
continuity is waived; named-state cleanup remains outside this goal.
