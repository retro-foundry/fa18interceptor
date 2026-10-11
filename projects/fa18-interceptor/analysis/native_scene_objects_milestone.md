# Native setup scenery milestone - 2026-10-06

The playable `fa18_native` now draws the source scene placement lists after
horizon/terrain submission. Setup shows runway and scenery geometry. Aircraft
records, cockpit/HUD and active flight remain unfinished; this is not recorded
whole-frame acceptance.

Actual caller: `port/native/main.c` -> `native_frontend_tick` ->
`native_flight_tick` -> `native_scene_draw` -> `visit_scene_placements` ->
`native_scene_placement` -> `native_model_draw` / ground descriptor drawing ->
existing `draw_stream.c` -> native polygon/line raster operations.

The dependency removed from that native path is the deferred C1EE14 model
instruction driver and hardware drawing submission. Native model streams are
original game data commands; their command-directory offsets select named C
geometry functions. No original instruction fetching, CPU/glue dispatch, MMIO,
chipset scheduling, ROM, savestate or recorded frames supplies runtime behavior.
Original loaded model/placement tables remain authoritative.

Source ownership:

- C1CB14/C1CB26 placement traversal, descriptor selection and distance caching
  reuse `scene_placements.c`, with native descriptor consumers.
- C1EE14/C1ED48/C1ED3C distance gates, LOD shifts, first-vertex rejection,
  static triple/flat geometry and command/list traversal live in
  `port/game/native/model.c`. C07846 extends flat paired edges.
- C096BC/C096CA use their distinct ground height gates, absolute bound streams,
  flat ground transforms and range-dependent drawing commands.
- Segment, tested-face, quad, grid, lattice and derived-block commands reuse
  `draw_stream.c`. C1FFB4 exposes the existing unconditional clipped pair route.
  Ground face commands C09952/C099AA/C099F6 retain source vertex consumption
  and line-mask selection.
- C0DAEE's fixed matrix mark is connected before the placement lists.
  C2F1C0 retains its symmetric circle spans, clipping and word masks, replacing
  hardware writes with direct writes through the native page's plane table.
  Model scratch is 0x4200; circle span scratch is 0x33000, separate from the
  map frame and polygon mask.

Validation performed once for this batch:

- `python tools/native/check_models.py`: native frames 4000/5300/6100 reach
  stages C1075A/C10AE6/C10C08 with 246/10,334/17,005 descriptor calls. Each
  checkpoint independently compares descriptor returns, 0x2000 bytes of
  transformed vertices, and all low plane/buffer bytes with original opcodes.
  Only the host model frame 0x4168..0x41FF is excluded from low-buffer comparison.
  Six/sixteen/ten reached descriptors match. The oracle continues each
  traversal with original results/state to avoid compounding a discrepancy.
- Two preserved pre-batch location/aircraft checkpoints additionally compare
  44/20 descriptors with zero failures, including flat grids/lattices.
- 48 circle cases at each of the three checkpoints match original planes,
  covering radius 0/1, large radii, horizontal/vertical clipping and colours.
  The original uses contiguous planes; the native backend resolves plane
  addresses through the host table. Circle tests use the original layout to
  compare masks/spans without changing source instructions, then repeats the
  native draw with the actual 10,240-byte plane spacing/order and compares
  logical plane contents with that original result. Both layouts match.
- Native MSVC build, GNU oracle build, frontend settled-pixel/save/SDL/native
  link checks and existing Free Flight selection/pause/resume checks pass.
  Reference MSVC builds and all 16 configured CTests pass (12 reference
  contracts and four native checks). The native checks were repeated by CTest;
  future reference-only runs should exclude `fa18_native_` to avoid duplication.

Copper fade is excluded. No full sealed replay was repeated, and no recording,
ADF, protected checker or unrelated `.vscode/` data was modified.

Rough functional estimate: **85% of Free Flight startup wiring**, previously
80%, because terrain plus scene placements now execute in the playable runner.
This is an engineering estimate for that startup slice, not whole-game progress
or a CPU-instruction percentage. Pending: aircraft record hull/descriptor paths
C1ED4C/C1F000, positive shadow strips C1F584, record finish/marks C1F844,
grid/followup ordering, cockpit/HUD, view controls and their newly reached
flight-update children. Missing reached owners fail explicitly. Original
input/view/timer cadence and full recorded gameplay comparisons remain open.
