# Native alternate selection and enclosing-frame exit

The actual runner path is `main` -> `native_frontend_tick` ->
`native_flight_tick` -> `native_scene_draw` -> `UPDATE_BUFFERS` -> C0D730's
`submit_update_display_buffers`. With UPDATE_DISPLAY_FLAGS bit $2000 set,
the source calls C0DA38 rather than ordinary active-plane submission. Native
previously aborted at this branch. Its comment incorrectly described page
presentation; source C0DA38 does not call a display service.

Source C0DA38-C0DA9E writes a four-corner viewport selection to CORNER_RECORDS:
(0,0), (319,0), (319,179), (0,179). It reads DISPLAY_MODE_ZERO_THRESHOLD when
CONTEXT_SELECT is zero, otherwise VIEW_PAN, and tests that signed word against
14400. Above the threshold it sets the two selection words to 1, clears the
selection long, sets the selection flag and returns zero. Otherwise it clears
only the selection flag and returns one. Both paths unlink the current source
LINK frame. In C0D730's alternate path that frame belongs to C0EFD4, so the
remaining scene, HUD, timers, counter and C32CEE final message are all skipped.
The outer runner still proceeds to C1612C display publication/page swapping.

`display_records.c:prepare_full_display_selection` now owns that complete
selection, shared with the existing ordinary record-selection rectangle path.
The buffer owner returns its child's `UpdateSequenceResult` instead of dropping
it. The native scene reports `owner_finished`; flight returns the distinct
NATIVE_FLIGHT_OWNER_EXIT result; frontend skips the final message and continues
display presentation. Selection acceptance/rejection is distinct from the
early-exit decision: both outcomes exit. No invented timing or geometry is used.

Component evidence: 18 original cases exercise both source selection modes,
signed extremes, 14399/14400/14401, direct C0DA38 and enclosing C0D730 calls.
Returns, actual stack/frame unwind and all non-stack RAM agree. Reject cases
preserve existing style/colour/complement values. These comparisons use the
existing model oracle and original instructions, never reference output as
native game behavior.

Runtime evidence: `native_scene_exit_test.c` links the same CPU-free runtime
objects as the playable runner. Disk loading and sealed demo inputs create
the state. Its observer supplies only controlled alternate-path/message-delay
inputs before one actual frame body, at demo update 2365/game tick 223. It
requires unchanged HUD/control/plane-submission counters and glyph count,
stage marker $60, no game-tick increment, no timer suspension, unchanged final
message delay and a pending outer display publication. The original frame
prefix from C0EFEA through the C0DA38 owner return executes 5,253 instructions
and agrees in every compared gameplay and display byte. Only nine native local
scratch bytes differ, using the existing frame-oracle exclusions; there are no
voice or blitter-busy differences. PAL start/end are both 5189 in this fixture.
This is a controlled branch integration proof, not an independently recorded
gameplay sequence or proof that a particular player input activates the flag.

Diagnostic `.after.dat` exports can now end at the alternate owner return.
Capture JSON reports `frame_owner_exit`; ordinary captures remain at C0F3C0.
The normal frame-body checker rejects an early exit, while the original oracle
accepts an explicit `owner-exit` contract for the alternate fixture.

Reproduce:

```powershell
cmake -S port/recomp -B build/native-cmake -DFA18_NATIVE_ONLY=ON -DBUILD_TESTING=ON
cmake --build build/native-cmake --config Release --target fa18_native fa18_native_scene_exit_test --parallel 8
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_scene_exit$'
python tools/native/check_scene_exit.py
```

Final regressions pass: active update 2401, crash update 2000 and map update
3200 original frame bodies (zero compared gameplay/display differences);
native record-expiry integration; frontend/save/reload/SDL and CPU/chipset link
omission; twelve reference host/loader contracts. GNU and MSVC reference builds
compile the shared buffer-owner API. No original recording was replayed.

This branch is **1/1 complete (100% of the alternate selection/exit connection)**.
Full recorded gameplay acceptance is still **0/3**. The independent 128-update
window's HUD timer mismatch remains unresolved. Five retained boundaries from
that window (ticks 222, 272, 273, 322, 349) also match named motion, rates,
orientation and matrices for all sixteen flight records; that is sample evidence,
not a complete record/flag/state comparison. Loading duration remains outside
acceptance and Copper fade remains ignored.
