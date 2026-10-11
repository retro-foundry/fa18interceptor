# Independent-camera depth-sort input - 2026-10-08

The playable independent-camera path now passes the final scaled view
coefficient explicitly into context depth sorting. This resolves the flight
caller's previously unknown incoming factor. The original origin update is
an earlier producer: the intervening aiming matrix replaces it before sorting.
No camera position, flight rule, matrix arithmetic or comparison mask changes.

## Source contract and connected ownership

The actual path is `native_frontend_tick` -> `native_flight_tick` ->
`native_scene_project` -> `aim_view_depth_factor` -> `scale_matrix_rows`, then
`refresh_native_context` -> `sort_display_list_retained` -> the existing host
renderer retained result. Scene placement consumes that result when a model
returns before initializing its accumulator.

C1C63E calls C29042 to update the origin. C2D99C subsequently dispatches the
independent view to C2D9BA. At C2DACC, C2E5AC scales the view matrix. C2E5E4 and
C2E5EA produce the full first coefficient of the last scaled row. The following
yaw matrix and player-angle publication leave its upper word intact. Context
projection's C1C2C8/C1C406 save/restore preserves it, so this is the live input
to C1E328. Cached depth words and fixed far keys preserve that upper word;
C1E4A6 saves it into the later model accumulator. Template rebuilding and
uncached distance calculations may replace it, as before.

`scale_matrix_rows` now returns the actual full arithmetic coefficient before
its word store. `aim_view_depth_factor` completes the existing aiming update
and returns that value; the existing void `aim_view` remains its wrapper.
`native_scene_project` supplies it to its real flight caller. Ordinary views
return the existing masked aircraft Z factor from C1C5F0/C1C5F4 instead.
The depth sorter publishes its retained result through `native/model_state.c`,
without writing source register or stack state into native gameplay.

## Evidence

The external `native_view_depth_oracle` executes original instructions and
compares all non-stack RAM plus the relevant returned arithmetic output:

- 128 complete C2E5AC cases include signed coefficients, zero, negative and
  extreme row scales, and full products that cannot be reconstructed from
  the stored word alone.
- 512 complete aiming, projection and cached/fixed-far sorting sequences cover
  panning, record following, immediate and limited tracking, shortcut events,
  ordinary/ship record types and all view quadrants. The final factors include
  317 nonnegative and 195 negative outputs.

`check_view_depth.py` starts the actual runner from the ADF with ordinary host
keys. Map view and two moving independent-camera routes each compare 16
consecutive full C0EFEA-C0F3C0 bodies against original instructions from the
native before-states. Both drawing pages and compared gameplay RAM match.
Source instrumentation verifies sorting and preserved-factor entries were
actually reached, with both $0000 and $FFFF incoming upper words. This proves
the connected camera/projection/sorting scope; these camera windows do not
establish a complete independent original flight or a new successful mission.

The windows execute 16 actual source sort batches and 16 cached-depth entries;
none uses a fixed far key in these runtime windows. Debug playable and affected
fixture builds pass, along with all three selected Debug CTests. The final
Release build passes all 17 affected checks: both mission gates, both carrier
sequences, gun/radar/infrared kills, mode five, host keys, record expiry, frame
bodies, frontend, this camera gate, models, raster and artifact policy/cleanup.
The checkpoint is `figures/native_context_depth_checkpoint.json`; local logs
are `build/native-flight/view-depth-{debug,release}-ctest.log`.

Consecutive captures now include small timing sidecars containing each body's
actual PAL interval, saved tick and owner-exit status. The checker can compare
each window after one native flight run instead of restarting the flight per
body. Passing RAM is temporary and deleted; a failing source comparison retains
its case. The new CTest is serial because original GNU builds share objects.
The existing 4 GiB build budget and 512 MiB diagnostic capture default remain.

```powershell
python tools/native/check_view_depth.py --runner build/native/fa18_native.exe
ctest --test-dir build/native-cmake -C Debug -R '^fa18_native_(view_depth|host_keys)$' --output-on-failure
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_(view_depth|frame_body|host_keys|mission_three_success|mission_five_combat_reset|qualification_sequence|qualification_new_pilot|gun_kill|radar_kill|infrared_kill|models|raster|frontend|artifact_policy|record_expiry|mode_five)$' --output-on-failure
```

## Remaining scope

Startup/menu context callers without a projection pass still need their own
incoming factor traced if no template or distance producer replaces it.
`has_factor` continues to distinguish this unknown contract from a produced
zero. The flight caller is resolved; this supersedes the broader open C29042
flight claim in the preceding combat/reset milestone.

Successful mission-list modes four through eight, mode-three mission restart,
independent complete original flights, other caller contracts, typed state,
audio fidelity and wider performance remain open. The complete-port goal
stays active.
