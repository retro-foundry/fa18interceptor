# Mode-five combat, expiry accounting and reset - 2026-10-08

The normal-key mode-five diagnostic now matches all 26 input/stage intervals
and all 115 sampled bodies against original compared RAM/drawing. This includes
all 63 consecutive combat-window bodies, seven gun-hit increments, both enemy
expiry-accounting increments and the natural crash reset. The comparison is a
CTest gate. The aircraft still crashes; completion count stays 3 and mode-five
mission success remains unaccepted.

## Connected retained-output producers

The playable path is `native_frontend_tick` -> `native_flight_tick` -> native
record updates and context refresh -> scene placements -> model rendering.
C1F074 can return before C1F712 clears the model accumulator. That early expiry
therefore consumes the actual preceding output from other source calls.

The first expanded-window failure was body 8,561: source C4F6DE was $7586,
native $FFFF. C149BE -> C25754 -> C1D974 saves the live control status pointer;
its low word overlaps the later model accumulator. `native/records.c` now
publishes the actual record + 2 pointer when the scale invokes that child.
A zero scale preserves the preceding output.

The following body, 8,562, exposed source $0002 versus native $7586. C1E328
used cached depth words for both entries, retaining the projection caller's
upper word. C1C5F0/C1C5F4 supplied the viewed aircraft's Z position masked to
22 bits; C1E4A6 saved that value over the model accumulator. Native context
refresh now passes that source-owned value explicitly to depth sorting.
Template-origin and reverse-record observations replace it when those owners
produce a new value.

`DistanceResult` exposes the actual C1D91A/C1D974 planar lookup output alongside
the distance. Its upper word is the first division's remainder; the saturated
height path retains the absolute height instead. `sort_display_list_retained`
publishes the upper word at the C1E4A6 boundary and carries the copied entry's
fourth long into the next list. Existing distance and sorting APIs still use
the same calculation. No collision, motion, damage, timer or rounding rule is
changed. No comparison mask is changed.

## Native state ownership

`native/model_state.c` owns the retained result as an ordinary C value. Control
normalisation, depth sorting and the existing map clipping owner publish to
that state. Scene placement materialises it in the native model scratch frame
and publishes the resulting accumulator back afterward. Cold frontend startup
resets the value with its other newly allocated scratch state.

An intermediate implementation wrote this result straight into model scratch
during startup. Carrier restart and gun regressions caught differences at
004184/004185, where original instructions do not write native scratch. Moving
the value into host renderer state corrects that ownership; masks remain
unchanged. External component oracles explicitly supply their existing caller
scratch input to this host state. Those isolated checks do not prove its runtime
producer; the full-body combat and regression comparisons do.

## Evidence and repeatability

The new checker requires all consecutive bodies in the 20-body windows started
by every observed gun hit. Overlapping windows contain 63 distinct bodies.
Expiry accounting advances 0 -> 1 at body 8,485 and 1 -> 2 at body 8,573. The
reset is body 8,705, C11830, host tick 20,352 and saved tick 2,810. The native
fixture's failure exit is expected because this is a failed flight. The gate
checks that completion count remains unchanged and no result config write occurs.

Separate source component comparisons pass:

- 64 complete C26EBE result/reference/non-stack RAM cases, including probe,
  early-return and level-selection paths.
- 128 complete C1E328 retained-output/non-stack RAM cases, including cached
  depths, fixed far keys, relative coordinates, multiple lists, the 22-entry
  cap and disabled/empty-selection paths.
- 256 C1D91A distance/planar-output/non-stack RAM cases, including saturation.

Debug and Release playable and affected fixture builds pass. Debug mission-three
success, mode-five combat/reset and artifact cleanup pass. All 14 final Release
checks pass, including both carrier sequences, all three shoot-downs, mode five,
record expiry, models, raster and frontend. The result is recorded in
`figures/native_mission_five_combat_reset_checkpoint.json` and the local log
`build/native-flight/mission-five-model-state-release-ctest.log`.

```powershell
python tools/native/check_mission_combat_reset.py --out build/native-flight/mission-five-combat-reset
ctest --test-dir build/native-cmake -C Debug -R '^fa18_native_(mission_three_success|mission_five_combat_reset)$' --output-on-failure
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_(mission_three_success|mission_five_combat_reset|qualification_sequence|qualification_new_pilot|gun_kill|radar_kill|infrared_kill|models|raster|frontend|artifact_policy|record_expiry|mode_five)$' --output-on-failure
```

The checker builds original oracles serially, keeps passing RAM in temporary
storage and deletes each passing pair immediately. Both mission checks share
the same comparison helper; the new combat gate and mission-three gate are
serial CTests because GNU builds share objects. Build/CTest pruning retains
the 4 GiB budget. The resolved body 96/97 failures remain compressed locally
with hashes. Canonical captures, media and user settings are preserved.

## Remaining scope

These comparisons execute original instructions from native before-states;
independent complete original-flight parity remains open. This does not accept
a successful mode-five mission or complete enemy inactivation intervals in
this mode. Complete its objective, carrier approach, successful landing, save
and reload using ordinary keys next.

The incoming context-view planar output from C29042 remains a named caller
contract when neither template rebuilding nor distance calculation replaces
it. `has_factor` distinguishes that missing input from a produced zero; the
ordinary viewed-aircraft caller and the tested producers are connected.
Other mission successes, independent complete flights, remaining contracts,
typed state, audio fidelity and broader performance remain open. The full
complete-port goal stays active.
