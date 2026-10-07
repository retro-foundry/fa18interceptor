# Native smoothing cancel connection - 2026-10-07

The source smoothing callback's cancel branch now reaches the existing native
cancel/reset composition. Previously C10A24 -> MC_CANCEL fell through the
setup child switch and aborted with `native setup child unavailable: 0`.
The controlled runtime probe reproduces that failure before the fix and now
continues through context messages, active flight and the source menu return.

## Connected ownership and original contract

The active native build is port/recomp/CMakeLists.txt ->
port/native/CMakeLists.txt -> fa18_native. The runtime caller is main ->
native_frontend_tick -> native_flight_tick -> run_post_input_tick -> stage ->
native_setup_stage -> follow_menu_smoothing -> MC_CANCEL. This batch removes
that unavailable child from the connected game path; it adds no emulator,
CPU state, new gameplay rule or replacement source routine.

MC_CANCEL now calls native_flight_cancel_context. The existing menu-return
cancel children use the same helper, which composes cancel_menu_return with
the established aircraft refresh and message reset owners. It does not
duplicate the original C10BAE behavior in setup.c.

Original evidence is analysis/data/menu_context_finish_source_scope.json and
analysis/data/menu_return_source_scope.json:

- C10A24 compares KEY_TAKEN with 2 and calls C10BAE at C10A2E on equality.
- C10BAE enables POST_INPUT_AUX and calls C10B90 at C10BB6. That existing
  native composition refreshes the recorder/root and control records.
- C10BC6 calls C11312, the existing message-sequence reset. The remaining
  stores clear context/event/smoothing gates, set transition/started flags,
  set POST_INPUT_COUNTDOWN to 5 and publish C10C68 through STAGE_CALLBACK.

The separate postflight-entry component oracle now declares this added
runtime dependency as an explicit aborting boundary. Its three bounded cases
do not exercise cancellation; it must fail if they unexpectedly reach it.
The connected smoothing test exercises the real helper and all its children.

## Runtime and comparison evidence

The existing mode-four validation entry starts from the ADF, acknowledges
credits, selects the mission and follows the ordinary briefing/viewport/menu
sequence. At the first naturally reached C10A24 input boundary, its optional
`smoothing` probe selects KEY_TAKEN=2. This is a validation-only source branch
condition, not a physical-input reproduction or a runtime convenience. A normal
keypad Enter at that preflight point was gated and did not select this branch.
No native return value, captured game state or drawing output is supplied.

The probe verifies the complete cancel-state stores before the body, observes
C10C68 -> C10CFE -> C10D8A -> C10DAE, runs 118 scene updates, and returns to
the menu with mode zero and C0FCB4. The shortened flight and subsequent menu
return are source behavior for this selected condition; normal mode-four
acceptance retains its original sustained-flight assertions.

All 44 actual input/stage intervals and 29 sampled complete frame bodies match
the independent original game instructions against compared RAM and drawing
bytes. The C10A24 parent includes its original C10BAE/C10B90/C11312 children.
The existing comparison scopes retain drawing/HUD bytes; no new exclusion is
added. These are sampled parents along the connected run, not a comparison
of every update or proof of all mission outcomes. Inherited return contracts
outside the prior bounded suites remain unresolved.

```powershell
python tools/native/check_mode_two.py --mode 4 --smoothing --out build/native-flight/smoothing-cancel-check
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_(smoothing_cancel|callback_reset|mode_four|postflight_entry|qualification|host_keys|frontend|artifact_cleanup)$'
```

The new fa18_native_smoothing_cancel CTest runs both the connected probe and
its original parent comparisons. Passing RAM is temporary. Reports remain
in the bounded build cache; the failed parent is retained if a comparison
fails. Native Debug and Release builds succeed; all eight listed Release
checks pass. Aggregate counts and log/binary hashes are retained in
[figures/native_smoothing_cancel_checkpoint.json](figures/native_smoothing_cancel_checkpoint.json).
Full mission/combat sequences,
other stage/reset/HUD contracts, readable typed state, audio fidelity and
broader visible-window performance remain unfinished.
