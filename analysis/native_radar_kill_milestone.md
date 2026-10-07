# Normal-input radar shoot-down through enemy expiry and inactivation

2026-10-07. The active playable build remains `fa18_native`, composed from
the shared `port/game/native/` runtime. This batch extends gameplay acceptance
evidence; it introduces no new damage, guidance, flight or timing rule.

## Actual source path

The normal-input mode-eight radar-hit scenario also destroys its enemy aircraft.
The connected caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_records_update` -> the readable record-dynamics/collision owners and
`native_scene_draw` -> the C22AC0 aircraft descriptor.

The connected C26EBE candidate-record owner calls `scan_candidate_record`,
which marks the contacted record with $0200 when its header has $1000.
`candidate_side_result` increments the pilot-log radar-hit word at +68 for a
player-owned radar missile contacting an enemy record. These native owners
are in `candidate_record_update.c` and `candidate_record_scan.c`; the complete
`flight_geometry.c` translation provides the same source reference contract.
C22ADE..C22AFC starts the reached aircraft's lifetime at 15,
clears $0200 and sets $0400 before descriptor drawing. No target state is
supplied by the fixture.

The connected record-update path advances the actual countdown. The C25B66
dynamics owner consumes its expiry and increments $C458AB
when the expired record is an enemy aircraft of class $10, excluding its
transient-record flag. Scene initialization clears this byte; the generic
postflight scheduler compares it with the mission's admitted-aircraft quota.
The `SCENE_DISPATCH_AUX` comment now records this source-backed meaning.
This counter is not a general player kill score: the test ties this particular
increment to the enemy struck by the player's missile.

## Observed flight

The fixture uses the same ordinary menu, throttle, bounded pull-up,
weapon-selection, target-selection and fire events as the accepted radar-hit
test. Only the saved-pilot mission-availability byte is supplied before the
normal loader reopens it; no flight, projectile, damage, timer, terminal flag,
hit counter or outcome is seeded.

| Event | Actual native state |
| --- | --- |
| Impact body, ticks 17137..17139 | Record ten at C47584, kind $12, changes header $70C1 to $54C1; lifetime becomes 15; radar-hit count increases by one |
| Expiry accounting, tick 17190 | Lifetime reaches zero, $0400 clears and C458AB increases from zero to one |
| Inactivation, tick 19275 | The same record's header becomes $0081, clearing $0040; its source inactive timer is 20 |

The initial header has no destruction/expiry flags, so this does not select an
already-destroyed aircraft. Its damage nibble remains zero: the missile uses
the geometry contact/destruction route, rather than the gun's three-hit damage
route. Enemy expiry accounting precedes eventual inactivation; the fixture
does not replace that source sequence with immediate removal.

## Continuous comparison and bounds

The native observer assigns a serial to every actual body boundary. The exact
impact body is serial 7453 and the inactivation body is serial 8086. The checker
requires every serial in this inclusive **634-body** interval, plus the source
expiry-accounting transition. Each complete body executes original instructions
from its native before-state and compares original after-state and drawing.

Across the full probe, all **57 input/stage intervals and 802 bodies** match
compared original RAM/drawing. Existing scratch, asynchronous voice and busy
exclusions are unchanged; no drawing bytes or additional gameplay state are
excluded. This is a native input-driven shoot-down with continuous composition
comparisons, not an independent complete original mission replay.

The validation capture window is capped at 768 bodies. This cap affects only
test observation and fails explicitly if exceeded. Passing input/body RAM is
deleted immediately after comparison, and all captures remain in a temporary
workspace. Only the failed case is retained. Compact logs and a report remain;
the fixture adds no playable recording or state-loading dependency.

## Reproduction and remaining work

`fa18_native_radar_kill` is registered in CTest and links the same native
runtime as the playable executable. Native Debug/Release builds pass. Eight
affected Release checks pass: combat eight, radar hit, radar kill, all three
weapon probes, frontend and artifact cleanup. The frontend also checks the
native link's CPU/chipset omissions.

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_radar_kill$'
python tools/native/check_mode_two.py --mode 8 --kill --out build/native-flight/radar-kill
```

The compact checkpoint is `analysis/figures/native_radar_kill_checkpoint.json`.
Gun and infrared shoot-downs, complete successful missions, independent full
flight comparisons, other callback contracts, typed game state, audio fidelity
and broader visible-window performance remain open. One enemy destruction
does not establish mission success. The complete-port goal stays active.
