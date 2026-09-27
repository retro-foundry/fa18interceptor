# C port continuation handoff

## Starting point

- Branch: `coverage-accounting`
- Head: `c262d136 Port indexed update zero routes`
- Working tree: only untracked `.vscode/` (user-owned; leave it alone).
- Goal: complete the faithful C port, committing each coherent, validated stage.

The current native reference check reaches **192 exact frames**: global frames
200 through 391.  It first mismatches at global frame 392 (361 of 64,000
pixels; bbox x=7..318, y=101..199).  `ctest` currently passes **86/86** tests.

## Non-negotiable porting rules

- The Amiga source/disassembly and extracted assets are the authority.  Do not
  add invented gameplay, constants, timing schedules, frame-specific output,
  or fallbacks.
- Do not use Ghidra.  The repository handover explicitly disallows it.
- Do not put emulator-captured screen/frame data in `fa18_port`.  Captured data
  may be a test oracle/fixture only.  The native allowlist check enforces this.
- Preserve user-owned `scripts/check_native_build.py`,
  `scripts/native_frame_count.py`, and `port/native_data_allowlist.txt`.
- Run build/test and the native-frame verifier serially: the MSVC linker can
  lock when they overlap.
- Keep `.vscode/` untracked.  Commit only the coherent port work you make.

## What the port has now

Recent commits, newest first:

```
1314e869 Port matrix tuple validation prefix
103d0833 Port fixed matrix product tuple
c8873377 Port signed matrix product stage
67c78b1f Wire native record matrix update path
a1336578 Port rotation matrix builder
a9522cbc Port three-angle matrix composer
cef55fd1 Port single-angle trig matrix builder
635fcd34 Port matrix row scaling stage
```

The newly-portable transformation pieces include:

- `port/two_angle_matrix.{c,h}`: source routines `$C2E38E`, `$C2E346`,
  `$C2E370`, `$C2E3DE`, and `$C2E47A`; Hunk 63 trig lookup setup.
- `port/matrix_row_scale.{c,h}`: `$C2E5AC`.
- `port/signed_matrix_product.{c,h}`: core `$C2DEFC-$C2E017`.  The preceding
  `$C2DEE0` guards are intentionally not claimed as ported.
- `port/fixed_matrix_tuple.{c,h}`: `$C0DAEE-$C0DB3A` product/handoff prefix.
- `port/matrix_tuple_validation.{c,h}`: `$C2EC9C-$C2ECC5` plus rejection
  helper `$C2EC82-$C2EC8F`.
- `port/record_matrix_update.{c,h}`: concrete native `$C2D94E` path alongside
  its original generic callback API.

`port/game.c` still only handles menu flow.  It does not yet own the live
scene/root transform, projection-grid submission, five-plane page, or
viewport/Copper presentation needed for the flight scene.

## Completed matrix-product projection stage

`port/matrix_product_projection.{c,h}` now ports `$C2ECC6-$C2ED6B` for the
validated-tuple path.  The focused contract covers run041's accepted tuple,
project-limit rejection, both source clamps, the negative-`D7` return, and
68000 `DIVS.W` zero-divisor/quotient-overflow faults.  It deliberately returns
an explicit unported-continuation result for every nonnegative-`D7` child
route rather than inventing a child call.

`ctest` passes **87/87** tests.  The native reference result remains **192
exact frames** (200 through 391) with the same first mismatch at frame 392.

The active-record input to the earlier `$C1C54E-$C1C63D` projection publisher
is now decoded by `fa18_decode_scene_projection_seed_record`.  It extracts
only the observed root longwords (`+$14/+18/+1C`), type byte (`+$62`), and
nine signed matrix words (`+$92..+$A2`) from caller-owned record bytes.  The
containing record's scheduling and scene meaning remain unclaimed.
`analysis/routines/run075_c1c54e_active_projection_record.md` supplies the
bounded run075 trace join from those decoded fields to the prepared-page
projection packet.

`port/angle_octant.{c,h}` now ports `$C254E8-$C2554B`, the subsequent native
angle-sector classifier. It preserves the source's byte-controlled primary or
alternate input selection and signed threshold comparisons, while leaving the
octant's downstream scene meaning unresolved. `ctest` passes **88/88** tests.

`port/update_stage_prefix.{c,h}` now ports the `$C1C63E-$C1C6BB` setup prefix.
It synchronizes the octant-derived byte, preserves the signed long threshold
and scale operations, then invokes the required caller-owned `$C22C80`
record-update boundary. It adds no record traversal or scheduler guess.
`ctest` passes **89/89** tests.

`port/record_stride_gate.{c,h}` now ports the first `$C22C80-$C22CCD`
record-stride decrement gate. It applies the observed 16 word decrements only
when the caller-owned state byte is clear; the succeeding record-bank calls
remain unported boundaries. `ctest` passes **90/90** tests.

`port/indexed_update_gate.{c,h}` now ports `$C25B66-$C25B93`'s selected-record
zero/nonzero split, zero-state fast return, and zero-index context route. The
context-clear path enters `$C25C3E`; the context-set path enters `$C25BAC`.
Both downstream bodies remain explicit until bounded. `ctest` passes **91/91**
tests.

`port/indexed_update_record_flag.{c,h}` now ports `$C25C3E-$C25C45`: bit 0 of
the caller-owned selected-record byte at `+2` routes to `$C25C54` when clear
or leaves the `$C25C46` fallthrough as an explicit continuation when set.
`ctest` passes **92/92** tests; the native frame check remains 192 exact
frames through global frame 391.

`port/indexed_update_control_path.{c,h}` now ports `$C25C54-$C25C69`: control
bit `$0040` clear routes to `$C25D86`; when set, a zero selected index routes
to `$C25C70`, while a nonzero index remains an explicit `$C25C6A`
continuation. `ctest` passes **93/93** tests with the same native frame result.

`port/indexed_update_control_stage.{c,h}` now ports `$C25C70-$C25C85` around
the caller-owned `$C1B27E` control-record boundary. It routes nonzero selected
indices to `$C25D22`, otherwise routes signed selected-record `+$42` to
`$C25CCA` when nonnegative or leaves `$C25C86` explicit when negative. `ctest`
passes **94/94** tests with the same native frame result.

`port/indexed_record_matrix_dispatch.{c,h}` now ports `$C25D86-$C25DA5`:
class `+$62` high nibble `$30`, or a clear record-header bit 7, invokes the
caller-owned `$C2D408` matrix dispatch; other classes with bit 7 set leave the
`$C25DA6` continuation explicit. `ctest` passes **95/95** tests with the same
native frame result.

`port/indexed_update_selected_record_gate.{c,h}` now ports
`$C25D22-$C25D3F`: index mismatch, nonzero context, or clear selected-record
`+3` bit 0 routes to `$C25D5E`; the all-clear case leaves `$C25D40` explicit.
`ctest` passes **96/96** tests with the same native frame result.

`port/indexed_record_selector_gate.{c,h}` now ports `$C25D5E-$C25D85`:
selected-record `+$20` bit 1 clear, class `+$62` high nibble `$10`, and header
bit 4 set invoke caller-owned `$C13D84`; all outcomes continue to `$C25D86`.
`ctest` passes **97/97** tests with the same native frame result.

`port/flight_scene_pipeline.{c,h}` now composes the evidenced data path from
decoded `$C1C54E` selected-record fields through packet publication and the
bounded Hunk-25 `$C279D0` grid traversal into an initialized five-plane page
renderer. Record selection, live matrix ownership, page selection, Copper
presentation, and scheduler timing are required caller inputs rather than
guessed runtime behavior. `ctest` passes **98/98** tests with the same native
frame result.

The same scene-pipeline module now exposes
`fa18_run_parent_flight_scene_pipeline`, a typed adapter for the `$C279D0`
renderer-packet callback in the existing `$C0F090` parent update boundary.
It does not provide the parent loop's other children or a top-level schedule.

`scene_placement` now also ports `$C1CB14-$C1CB73`, the selector prefix for
its existing `$C1CB74` traversal: it selects the primary/alternate list offset
and derives the comparison word through a caller-owned signed depth-table
lookup, including the source `$7ffe` saturation. `ctest` remains **98/98**
with the same native frame result.

`fa18_run_scene_placement_stage` now composes `$C1CB14-$C1CCB9`: the selector
prefix feeds its chosen offset directly into the already-portable placement
record loop. Tables, descriptor lookup, and descriptor consumer behavior stay
caller-owned. `ctest` remains **98/98** with the same native frame result.

`fa18_run_parent_flight_placement_stage` exposes that composed placement
stage in the callback shape used by the `$C1CB14/$C1CB26` parent-update slots;
its caller still selects the source entry variant through the stage input.

## Matrix-product projection evidence

Port `$C2ECC6-$C2ED6B`, documented in
`analysis/routines/c2ecc6_matrix_product_projection.md` and source in
`source_amiga/observed/project_matrix_product_tuple.asm`.

It projects a validated matrix-product tuple:

1. signed multiply/divide and add screen offsets (`#$a0`, `#$5a`),
2. source clamp to x `0..$13f` and y `0..$b3`,
3. reflection to the stored pair at `$C45958`,
4. project-limit rejection through `$C2EC82`,
5. the negative-`D7` continuation/return path.

Keep the non-negative-`D7` child dispatch as an explicit unported boundary;
do not invent a child call.  For 68000 `DIVS.W`, model signed 32-bit numerator
and signed 16-bit divisor, truncation toward zero, and detect quotient
overflow rather than silently using C overflow behavior.

The primary observed test case is run041:

```
input low signed tuple: D0=-19, D1=1452, D2=7990
pre-reflection:          (160, 106)
stored projected pair:   (159, 74)
negative D7 route:       ends at D7=-1, returns projected x
```

Add focused contract tests (including rejection and clamping), add the source
to CMake and `fa18_port`, then run the standard checks and commit the stage.

## Next implementation boundary

Do not integrate this standalone matrix projection into `game.c` until the
scene/root transform publisher and renderer scheduling path are evidenced.
Use the frame-392 display-state/Copper evidence below to identify the native
owner that supplies the projection packet and matrix, then connect the proven
data flow without frame-number gates.

## Frame-392 evidence and integration boundary

Read these before wiring it into the runtime:

- `analysis/routines/run075_frame392_cockpit_entry.md`
- `analysis/routines/run075_c279d0_prepared_page_handoff.md`

The visible change at global frame 392 is not fresh page rendering.  A page
was prepared at frame 384 by `$C279D0` from Hunk 25 records, while dynamic
Copper lower-16 palette state from the Hunk 21 mode table made it visible at
392.  Relevant observed entry values are `$C45A72.w=E7C1`,
`$C45A76.w=E64E`, `$C45A78.l=FFFFFF83`, and matrix `$C45BD8`.

Those values are evidence, never runtime constants.  The eventual data flow
must be:

```
scene/root transform -> projection packet + matrix -> FA18ProjectionGrid
-> five-plane page -> viewport/Copper present
```

Existing page, packet, grid, viewport, and palette modules are building
blocks, but they are not yet scheduled by the game loop.  In particular, do
not naively advance/present a blank page just to make frame 392 change.

## Standard validation after each stage

Use the existing build directory/configuration and run serially:

```
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
python scripts/check_native_build.py
python scripts/native_frame_count.py --to 392 --timeout 180
```

At this handoff, expected frame-check result is `NATIVE_FRAME_COUNT=192`, with
the first mismatch at frame 392 as described above.  A new exact result beyond
that is welcome only if it arises from the faithful runtime pipeline.
