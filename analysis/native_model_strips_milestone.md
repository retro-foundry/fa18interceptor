# Reached native model strips and warning messages

The native pullback scenario reached the missing C1F584 path in
`native_scene_placement` -> `native_model_draw`. `model_strips.c` now owns
C1F584-C1F6F8 as typed C: displaced/scaled points, full matrix products,
interpolated interior vertices, forward/reverse alternate lanes and subsequent
strip groups. Source signed counts, word wrapping and DIVS overflow behavior
are preserved. It writes the original model workspace, including C4AD90's
interpolation endpoints, through host memory; no opcode helpers are called.

The next reached dependency, C25704 warning publication, is also connected
for DY_DESCENT_ALERT, DY_RECORD_ALERT and DY_COLLISION_MESSAGE in the native
record consumer. The source message owner publishes the warning and its
classification remains available to the parent. No substitute geometry,
physics or warning behavior was added.

Validation separates component correctness from actual integration:

- 96 bounded original-instruction C1F584-C1F6F8 cases compare all non-stack
  RAM, including paired directions, multiple groups, signed counts, clipping
  shifts, wrapped matrix arithmetic and division overflow.
- The real 7150-tick pullback checkpoint executes 14083 model calls and reaches
  a positive strip group in the original rendering comparison. All 29 reached
  descriptors, vertex caches, plane bytes and complete grid/control/followup
  parents match. C12098/C1C63E comparisons pass at 7150 and 7280.
- MSVC native and GNU oracle builds pass. Native link omits CPU/chipset/glue.
- `python tools/native/check_strips.py --runner build/native/fa18_native.exe`
  reproduces the actual reached path and both component checks. It is also
  registered as the native strips CTest.

This batch is complete. Rough Free Flight startup wiring remains about 98%;
this is not whole-flight completeness. The same pullback sequence next stops
at wide terrain pass tick 7294, height 1800. Native scene errors now include
those values. The remaining terrain contract must be resolved before claiming
takeoff. Other open work includes Stores input values, complete/end-of-frame
ordering, alternate display pages and recorded-flight acceptance. Copper fade
is excluded; no full sealed replay repeated.
