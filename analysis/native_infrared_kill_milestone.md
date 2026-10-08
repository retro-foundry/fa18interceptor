# Normal-input infrared shoot-down through expiry and inactivation

2026-10-08. The active playable build is `fa18_native`, using the shared
`port/game/native/` runtime. This batch adds required acceptance coverage for
existing infrared gameplay. It changes no flight, damage, guidance or timing
rule; the playable executable's SHA256 remains unchanged.

## Connected source ownership

The actual caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_records_update` -> `update_candidate_record` (C26EBE) ->
`scan_candidate_record` / `candidate_side_result`. A player-owned projectile
with kind one increments the infrared-hit word at pilot-log +64 when contacting
an enemy. Kind zero uses radar-hit +68. The observer now selects the appropriate
kind and counter from the ordinary Return weapon selection.

The candidate scan marks the enemy with $0200 and publishes HISTORY_RECORD.
The native scene's C22AC0 aircraft descriptor enters C22ADE..C22AFC, clears
$0200, sets $0400 and starts a lifetime of 15. Connected C25B66 dynamics later
consumes expiry and increments C458AB for a non-transient enemy aircraft.
These are the same original owners used by the radar proof. No new gameplay
dependency is removed in this batch; the missing infrared acceptance check is
now connected to the playable runtime's actual objects in CTest.

## Actual input-driven result

The fixture starts from the ADF and uses the normal menu, throttle, bounded
pull-up, target and Space firing controls. It presses Return once for infrared,
rather than twice for radar. Only saved-pilot mission availability is supplied
before the loader reopens the file. Actors, motion, projectiles, damage,
lifetimes, counters and outcomes are not seeded.

| Event | Observed state |
| --- | --- |
| Impact, ticks 17137..17139, body 7453 | Enemy record ten, kind $12, changes $70C1 to $54C1; lifetime becomes 15; the actual pilot log at $2000 advances its infrared-hit word from four to five |
| Expiry accounting, tick 17190, body 7468 | Lifetime zero, $0400 clears, C458AB advances from zero to one |
| Inactivation, tick 19275, body 8086 | The same record becomes $0081, clearing active $0040; inactive timer is 20 |

The target has no initial destruction/expiry flags. Its damage nibble remains
zero: this missile collision destroys directly, unlike the gun's three-hit
damage route. Inactivation follows expiry accounting; this does not establish
permanent record deletion or mission success.

## Validation and retention

The standalone infrared-hit check passes **55 input/stage intervals and 188
bodies**, including the precise impact. The required infrared-kill check passes
**55 intervals and 802 bodies**, including every serial in the inclusive
**634-body** impact-to-inactivation interval. Each comparison executes original
instructions from the native before-state and compares original after-state
and drawing. Existing scratch, asynchronous voice and busy exclusions are
unchanged. This is not an independent complete original mission replay.

The hit observer holds one 1 MiB before-state in memory while the selected
player missile is active. Destruction observation retains the existing
768-body validation cap and fails explicitly if exceeded. Passing RAM is
deleted immediately after comparison; the temporary workspace is cleaned on
exit. Only a failed comparison case is retained unless `--keep-captures` is
explicitly selected for debugging.

Native Debug/Release builds pass. Four affected Release CTests pass: radar
kill, infrared kill, frontend and artifact cleanup. The frontend still checks
the native link's CPU/chipset omissions. Logs are in
`build/native-flight/infrared-ctest.log`; the compact checkpoint is
`analysis/figures/native_infrared_kill_checkpoint.json`.

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_infrared_kill$'
python tools/native/check_mode_two.py --mode 8 --kill --missile infrared --out build/native-flight/infrared-kill
```

Default `--hit`/`--kill` still select radar. The JSON observer now reports
the selected weapon and generic `hits_before`/`hits_after` and `missile_kill`
fields; historical radar checkpoints retain their original schema.

Gun shoot-downs, complete successful missions, independent complete flights,
remaining callback contracts, typed game state, audio fidelity and broader
visible-window/combat performance remain open. The complete-port goal remains
active.
