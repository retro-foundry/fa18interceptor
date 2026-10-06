# Native setup aircraft rendering - 2026-10-06

`fa18_native` now traverses the followup scene list, including aircraft record
descriptors and their cached hull geometry. The real caller is
`port/native/main.c` -> `native_frontend_tick` -> `native_flight_tick` ->
`native_scene_draw` -> `visit_followup_placements` ->
`native_scene_placement` -> aircraft/model geometry -> direct host drawing.
This removes the C1ED4C/C1F000 instruction-driver dependency from the reached
native setup path. Original model streams are game data, not fetched opcodes.

Source owners connected:

- C1CCBC followup placements, C279D0 grid projection and C1518C control records
  run after primary/alternate scenery, in the C0EFD4 control/followup order.
  Only reset-face-state is reached among the setup control children; other
  reached children continue to fail explicitly until implemented.
- C1ED4C selects ordinary/cockpit aircraft streams. C22AC0 supplies the active
  record gate. C1F000 rotates cached hull points through the record matrix and
  places the visible subset through the view transform. C1F844/C1F87A connect
  expiry shapes and history projection. The source's extra cached point and
  retained early-expiry accumulator are preserved.
- C21B38/C21C86 compact/extended tails reuse the source word-vector operations
  in `vertex_tail.c`. Other reached commands reuse existing geometry, face,
  script, stage and projection owners. C1FF46 handles the tested point command.
- Rejection of C0CF98's circle ends that command sequence, matching C1F944's
  use of the source rejection status, even though the source return word is
  zero. The native driver expresses this as a semantic negative command result.

Validation:

- Native frames 4000/5300/6100 execute 290/22,484/31,567 descriptor calls and
  reach C1075A/C10AE6/C10C08. The oracle compares 10/34/14 reached descriptors:
  return words, every low drawing/buffer byte, cached record points,
  transformed vertices and all non-stack source-data bytes match.
- At each checkpoint, independent complete C279D0, C1518C and C1CCBC calls
  match all non-stack RAM, including planes and placement caches. Host local
  frames (model/grid/control) and source stack storage are excluded. Incoming
  source model local scratch is aligned with the native model scratch, since
  early expiry reads the incoming accumulator before its normal clear. Actual
  clear/OR operations and externally visible results remain compared.
- Eight compact/extended tail cases per checkpoint compare both record banks,
  transformed workspaces and stream advancement against original instructions.
  Forty-eight circle cases per checkpoint still pass in both plane layouts.
- Native MSVC build and GNU oracle build pass. Frontend settled-pixel,
  save/reload, SDL and link-omission checks pass; menu and Free Flight
  acknowledgement/location/aircraft/pause/resume checks pass. Reference MSVC
  build and twelve reference CTests pass. The native-only build has testing
  disabled; its four checks were run directly, without repeating via CTest.

Copper fade is excluded. No full sealed replay was repeated. Protected
checkers, original media, sealed recordings and `.vscode/` were preserved.

Engineering estimate: **90% of Free Flight startup wiring**, previously 85%,
because aircraft/followup and grid/control setup drawing now execute and pass
focused source comparisons. This is not whole-game completion, timing parity
or active-flight acceptance. Cockpit/HUD, view/input/timer ordering and active
flight dynamics remain open. Positive shadow strips C1F584, expiry transition
C22ADE and unreached model/control children still fail explicitly. Other modes,
audio, typed state and recorded gameplay acceptance remain unfinished.
