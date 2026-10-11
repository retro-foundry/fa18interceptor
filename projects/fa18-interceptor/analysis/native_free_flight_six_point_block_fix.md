# Free Flight six-point block crash — 2026-10-09

The reported scenery abort was:

```text
native model stream C3BA04 parameters C3B9BE record 6000 code 8090
native model missing draw command at 000090
native model player position x=1077241.250 y=264.469 z=1146902.141; pose_index=2 mode=1 stage=C10DAE
```

The connected caller is `port/native/main.c` → `native_frontend_tick` →
`native_flight_tick` → `native_scene_draw` → `visit_followup_placements` →
`native_scene_placement` → `native_model_draw` → the command dispatcher.
Original command directory C1FCE8+$90 points to C20F78. The immutable model
at C3B9BE contains `8090 0000 0002` at C3BA04: derive vertices from workspace
offset zero with shift two. This valid command had no native dispatch case.

The existing `draw_stream.c` now owns C20F78-C21050 as
`extend_six_point_block_scaled` and its shared C20FC4 entry as
`extend_six_point_block`. The scaled entry derives points 12-13 from points
2-4. The common tail derives points 6-9 from points 2-5 using p0-p1, then
points 10-11 using p2-p1 and p0-p1. Low-word arithmetic, signed ASR,
six-bit shift counts, stream advancement and the zero result follow the
original instructions. Commands $90/$94 dispatch to these owners.

The original four-point extension and new six-point extension share their
edge-shift helper. This preserves the existing command's arithmetic.
All work uses fixed local vertices and existing workspace storage; there is
no heap allocation, lazy initialization, geometry substitution or command skip.
The existing unsupported-command diagnostic remains for other unported IDs.

Validation:

- 184 complete original-instruction comparisons exercise both native dispatch
  cases, negative/positive workspace offsets, signed extremes, low-word
  overflow and shifts across the 16-bit and six-bit boundaries. Every
  non-stack RAM/display byte, stream cursor and result matches.
- 32 camera poses render the actual immutable C3B9BE model through the full
  scene descriptor/C1ED48 owner, requiring 16 original six-point commands.
  Returns, transformed vertices, records, non-stack source data and plane
  bytes match within the existing oracle's caller-frame exclusions. These
  camera poses are isolated component inputs, never injected into gameplay.
- Both builds pass `fa18_native_models`, `fa18_native_flight_start`,
  `fa18_native_preallocation`, `fa18_native_preallocation_output` and
  `fa18_native_artifact_cleanup`. Intro, Free Flight and final combat preserve
  complete WAV, RAM, pixels, saves and counters with zero heap violations.
- Each current build independently enlists an actual pilot for 9,000 frames,
  then runs the ordinary qualification/mission-three/menu input for 42,706
  frames. Complete traces, final RAM, counters, saved pilot and allocation
  reports match the preceding filtered build exactly. Strict cross-runtime
  pages remain 287/4,967; broader drawing is still open.

The user's precise route is not sealed, so the component reproduces the
reported command/model rather than claiming an identical flown route.
Canonical `build/native/fa18_native.exe` is refreshed. Hashes and results are
in [the checkpoint](figures/native_free_flight_six_point_block_checkpoint.json).
Full-flight drawing, original audio onset and visible performance remain
separate acceptance work; named-state cleanup remains deferred outside the goal.
