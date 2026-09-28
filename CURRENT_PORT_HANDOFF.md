# C port continuation handoff

## Starting point

- Branch: `coverage-accounting`
- Implementation head before this handoff update: `d519a92b Load positive root pose tables`.
- The current working tree adds a source-addressed five-plane/Chip-RAM binding
  for the reusable `$C2FF58-$C30037` backend. User-owned untracked `.vscode/`
  remains untouched; do not discard it.
- Goal: complete the faithful C port, committing each coherent, validated stage.

The current native reference check reaches **192 exact frames**: global frames
200 through 391.  It first mismatches at global frame 392 (361 of 64,000
pixels; bbox x=7..318, y=101..199).  `ctest` currently passes **121/121** tests.

## Latest root-owner work

The cold-boot trace proves startup selects the **nonnegative** root table path
`$C093BE-$C095BE`, rather than only the earlier ported `$C09498` negative
path. `port/scene_positive_pose.{c,h}` now ports that direct root-pose packet:
it consumes the five entry words, its three-word tail, and the two original
adjustment pairs; publishes root `+$06/+08/+0B/+0C/+0E`, `+$14/+18/+1C`, and
the corresponding delta state; then calls the existing `$C2D954` matrix
boundary. The cold-boot contract fixes the source result at
`+$14=$10545920`, `+$18=$00000708`, `+$1C=$10A404F0`, `+$06=$0041`,
`+$08=$0042`, `+$0C=$1459`, and `+$0E=$2404`.

`scene_root_placement` dispatches both signs of the Hunk-67 root entry. Its
positive callback is `fa18_resolve_scene_positive_pose`, supplied by
`scene_positive_pose_tables.{c,h}`. That resolver reads the actual Hunk-67
eight-word entry and Hunk-8 data, rejecting invalid selectors. Hunk-8 bounds
are measured, not guessed: `$C1D7E2-$C1D8D5` is 61 signed-word pairs and
`$C1D8D6-$C1D9D7` is 129 signed-byte pairs; `$C1D9D8` begins the separate
magnitude table. Related evidence: `analysis/routines/c093be_positive_scene_pose.md`.

Commits after the earlier handoff head:

```
d519a92b Load positive root pose tables
6c023c17 Bound positive root adjustment tables
321f7dbf Dispatch positive root placement
26ceed69 Port positive root scene pose
be5c3fdb Record post-menu transition trace
4fbce23b Bind projection blits to five-plane pages
```

Current gates after the root work: native build check passes 233 files;
`ctest` passes 123/123; native parity is unchanged at frames 200--391 exact,
with the first mismatch at frame 392.

`scene_entry_runtime.{c,h}` now composes the exact four `$C0FAA4` helper
boundaries into one state-driven owner: `$C28722`, `$C0924A`, `$C11312`, then
`$C082B0`. The run075 expiry trace proves that `$C28722` consumes mode `$7F`,
then `$C0924A` consumes the initializer-published stage byte three (Hunk-67
table-A entry three) after the dispatch returns with `D7=-1`. The native
composition uses the original Hunk-27 dispatch data, Hunk-67 record table,
Hunk-16 relocation-backed templates, Hunk-52 descriptor, and Hunk-63 trig
table; it does not retain captured state. Its new contract uses synthetic
Hunks to verify ordered direct state, record-14 creation, the negative root
route, message initialization, and finalization.

`scene_root_record.{c,h}` now publishes the direct `$C0924A-$C095BE` root
writes into mutable slot zero: root `+$06/+08/+0A/+0B/+0C/+0E/+10`,
`+$14/+18/+1C`, flags at `+$04`, and the `$C2D954` attitude output at
`+$92..+$A2`. `scene_entry_runtime` invokes it immediately after the bounded
root placement. The source-owned type byte `+$62` is published at `$C092EC`
from its caller-owned producer before the remaining root fields are written.

That producer is now bounded too: the run075 `$C0FEEA` continuation executes
`$C0FFB2-$C0FFBE` before the mode-$7F arm, writing root selector `3` and root
type `$11` to the bytes later consumed by `$C0924A`. `FA18MenuFlow` carries
these two direct values when its existing source-backed delayed transition
expires, and `scene_entry_runtime` writes the caller-provided root type to
slot-zero `+$62` at the observed `$C092EC` boundary before root placement.
`FA18Game` now owns the scene-entry runtime, initialization state,
`$C0FA04` followup state, and viewport-mode state. When the existing
source-backed mode-$7F arm consumes a scheduler tick, it retains that arm's
delay four. Later scheduler ticks use the shared `$C0F7D8-$C0F7FE` counter
and invoke the real `$C0FA04` composition; the negative tick runs
`scene_entry_runtime` before the observed followup stores set viewport target
15/current 0. No presentation-frame trigger is used. The run075 timing stream
now contains the five measured `$C0F5F8` entries at frames 290, 317, 335,
351, and 369. The live path reaches the observed scene-entry expiry without
changing the 192 exact-frame gate, because page/viewport presentation is still
not attached.

`matrix_pipeline_tail.{c,h}` now ports the source-ordered `$C2DAB0-$C2DAF1`
tail of `$C2D9BA`: it builds `$C45BD8` from caller-owned angle words, scales
its rows, builds `$C45BFC`, and copies the three `$C461EA` auxiliary words.
The bounded frame-608 trace validates its projection matrix
`(167,0,-8 / 1,248,39 / 6,-21,126)`. `$C091E0/$C123FA` still own the inputs,
so this is intentionally not scheduled by `game.c`.

`rounded_signed_divide.{c,h}` now ports `$C25980-$C259C1`, the signed DIVS.W
remainder-threshold rounding helper used by `$C123FA`. It reports zero-divisor
and signed-quotient overflow instead of pretending the original exception
returns. The enclosing coordinate-update formula is still an explicit evidence
gap and is not scheduled.

`magnitude_refinement.{c,h}` now ports `$C2564E-$C25703`, the three-range
unsigned divide/refinement helper that publishes `$C45B68` from `$C45B64` for
the same coordinate path. Its DIVU.W fault conditions stay explicit; it has
not been used to infer the still-unrecovered `$C123FA` branches.

## Next context: live scene rendering

Do not add a frame-number trigger or captured page to `game.c`. The source
join is established: run075 reaches `$C0FA04`'s expired branch at global frame
370, which calls `$C0FAA4`; only after it returns does the caller set viewport
mode `current=0`, `target=15`. The source cadence reaches current mode 8 at
frame 392, where the dynamic Copper palette reveals an already prepared page.
The game-level callback composition and measured stream now reach the
scene-entry transition. Connect its genuine record/matrix state to
`FA18FlightScenePipeline` and `FA18ViewportMode`/five-plane presentation. It
must provide genuine record, matrix, page, and Copper state; the existing
capture fixtures are diagnostic only. See `analysis/routines/c0fa04_post_input_followup.md`,
`analysis/routines/c0faa4_run075_scene_initialization.md`, and
`analysis/routines/run075_frame392_cockpit_entry.md` before editing `game.c`.

`FA18FivePlaneChipBinding` is the native bridge from caller-owned dynamic Chip
addresses to a `FA18FivePlanePage`. It validates five non-overlapping complete
plane ranges, preserves the source `$C456B6` reverse lower-plane order, and
copies page state only at its explicit boundary. `fa18_flight_renderer_page_execute_lane_stage`
uses that binding only when the supplied `$C456B6` table matches, then executes
the complete typed `$C2FF58-$C30037` lane tail and synchronizes the page back.
No capture, hard-coded Amiga address, page selection, or frame schedule is
present in the runtime path. Run-named recovered register packets were moved to
`blit_job_oracle.{c,h}`, which is linked only by its contract test, so the
native executable retains the no-recorded-output closure.

`scene_root_placement.{c,h}` now composes the observed negative-table route of
`$C0924A-$C095BE`: it runs the `$C09620/$C095C0` root reset, reads one original
Hunk-67 table entry, derives the source record index, resolves the mutable
record and template descriptor through required callers, then invokes the
already-proved pose transform/matrix update. The positive-table `$C093BC`
family is an explicit unported route. This is the first source-ordered path
from `$C0FAA4` scene initialization into root pose/matrix state. The mutable
selected-slot portion of the `$C46184` bank is now owned by
`scene_dispatch_runtime`; root slot zero and the update/presentation schedule
still have no native owner.

`scene_dispatch_runtime.{c,h}` now composes `$C28722-$C28E08` from original
Hunk-27 dispatch records and Hunk-16 relocation-backed five-pointer templates.
The earlier attract-demo trace proves `$C28BEE` creates selected slot 14 at
`$C47D84`; the new runtime reconstructs that selected slot and exposes the exact
record/descriptor resolver pair needed by `$C09498`. Its template field four
resolves the verified Hunk-52 descriptor without retaining a runtime Amiga
address. The creation adapter was also corrected for `$C28DD8-$C28DE4`:
coordinate terms are `SWAP; ASL.L #6` (`* $400000`), not `* 64`. This produces
the trace-backed slot-14 `+$14/$18/$1C` tuple
`$11180000/$00000000/$11180000` before root placement. A fresh
cold-boot-to-menu capture now identifies the root producer: at chipset frame
7769, `$C08F76-$C08F8E` clears the first 164 bytes of each of the sixteen
`$C46184 + index*$200` slots, then its startup path reaches `$C09266` /
`$C0924A`. The resulting slot zero is not a `$C28BEE` record.
`build/cold_boot_menu_init_root_creation_instruction_trace/` contains the
8,511-instruction bounded frame trace (root clear at row 2037, root transition
at row 4308). Existing root setup and placement modules match the nested path,
but they are not attached to `game.c`: the source scene-entry scheduler, full
startup owner, and update/presentation schedule remain unported. Frame 392
therefore remains unrendered in normal replay.

The complete direct cold-boot body `$C08F26-$C090AD` is now byte-exact source
in `source_amiga/observed/initialize_scene_bootstrap.asm`, verified against
the cold-boot trace's Slow-RAM authority (392 assembled bytes, zero
differences). It owns the two bounded bank clears and enters `$C09266`; its
remaining helper calls and the menu-to-scene scheduler are still separate.

A CPU changed-write watch over the root prefix `$C46100/$FFFF00`, replayed
from the sealed run075 restore at recorder frame zero through frame 2,000,
finds no root mutation. The restored record is therefore already initialized
before this recorded run; replaying from its frame zero cannot recover the
first root producer. This is negative evidence only: it does not authorize
seeding native state from the save, and it leaves the root producer open.

The user explicitly authorized a temporary opt-in display diagnostic while the
scene producer is reconstructed. `--bootstrap-render-fixture CHIP` imports the
five traced run075 frame-392 page planes from a caller-supplied external
Chip-RAM capture and presents them with the native Hunk-21 mode-8 palette. It
contains no captured page in the executable, is never selected by normal
replay, and does not advance the 192-frame parity result. It is a visibility
diagnostic only, not a native scene renderer or completion of the frame-392
producer.

The next rendering boundary is the real far-polygon path.  A bounded run036
`$C2FF48 -> $C301F6 -> $C302DE/$C302EC -> $C30306` call is now recorded as
`analysis/routines/run036_c2ff48_area_blit_oracle.md`: it reaches four
`$C30668` prepared jobs and the following lane stage, with source register
images plus a 110-byte settled Chip-page delta oracle.  It proves the native
area path needs true inherited blitter channels and word/shift/modulo
semantics; it does not authorize a generic filled-triangle substitute or
normal `game.c` scheduling.

`port/blit_job.c` now has bounded synchronous OCS execution primitives:
`fa18_execute_ocs_block_blit` covers block and exclusive/inclusive fill state,
while `fa18_execute_ocs_line_blit` ports the line-mode register progression
used by `$C306AE`.  Both operate only on caller-owned Chip bytes; focused
contracts cover shift, fill, and line progression.  They are not attached to
a flight page yet: the next stage is a run036 diagnostic that applies the
complete traced line/fill/lane sequence and compares its page delta.

The run036 producer now matches all four traced `$C306AE` packets exactly.
The corrected `$C30634-$C30638` bounded-limit helper preserves the source's
post-ASR carry rounding, producing the original second job's `$0C02` size.
The next stage remains the inherited line/fill/lane diagnostic described in
`analysis/routines/run036_c2ff48_area_blit_oracle.md`.
## Current uncommitted polygon diagnostic
The standalone run036 oracle accepts external pre- and post-call Chip images.
It reproduces the four C306AE lines, the C303EC fill, and C304F4/C304B2 lane jobs
without embedding captured data or affecting normal replay.
Current result: 184 differing bytes; native changes 118 bytes while the original changes 110.
This improved from 195 after correcting BLTSING latch order.
The remaining mismatch is line-mode per-row state/math.
The source C30668 submitter waits for DMACONR busy clear, so per-job execution is correct.
The focused blit contract passes; rerun full serial gates before committing.
`$C302E6-$C30404` explicitly reloads the later fill job's C pointer with
`$FFFFFFFF`; the diagnostic now preserves that register image rather than
using the C pointer advanced by the final line job.  This does not change the
184-byte result because that `$09F0` job has C disabled.  Full serial gates
pass: 116/116 contracts and 192 exact native frames (200--391); frame 392
still first differs by 361 pixels.
The isolated no-future-input trace now stops at `$C303D2`, immediately after
the four line jobs: its 32-byte Chip delta matches the native line executor
exactly.  The key correction is the pinned custom-register `$FFFE` mask on
the low C/D pointer words (the trace-time `$74C7` submission executes at
`$74C6`).  The remaining run036 final-page mismatch is downstream in the
descending `$09F0/$000A` fill and lane sequence; do not reopen the line-mode
math without a new line-stage difference.
`build/run036_7000_before_desc_fill_trigger/` stops at `$C30404`, immediately
before the source `move.w D7,BLTSIZE` trigger.  It is the valid isolated fill
input: the preceding `$C303D2-$C303E0` busy-wait has settled the line jobs,
and `$C303EC-$C30402` has written the exact `$09F0/$000A` A/D/C/data image.
`fa18_run036_polygon_oracle_test --fill-only PRE_TRIGGER POST_FILL` now
compares that input to `build/run036_7000_after_desc_fill_settled/`, whose
`$C3049E` stop is the following `$C30466` helper's pre-lane wait exit.
The former 147-byte fill difference was traced to pointer handling, not a
hybrid fill direction. `build/run036_7000_dma_fill_window.json` captures the
ordinary frame-7000 DMA stream, including `$7400-$7700`. At vpos 186/hpos 86
it records the exact `$C30404` submission (`BLTCON0=$09F0`,
`BLTCON1=$000A`, A/D=`$76D8`, C=`$FFFFFFFF`, size=`$0486`). The following DMA
reads and writes are all even-addressed while the signed `$001D` modulus leaves
the internal pointers odd between rows. Therefore retain the raw pointer for
arithmetic, but mask bit zero at every OCS DMA word access. The block and line
executors now do so, with a synthetic odd-modulus contract test. The crucial
row-end rule is that the final word remains at its address for the modulo, and
the next row begins at the resulting even DMA address; do not retain an odd
pointer into the next row. The isolated fill is byte exact: zero differing
bytes and 105 changed bytes on each side.

The complete bounded run036 `$C2FF48` producer is now byte exact as well.
`fa18_run036_polygon_oracle_test PRE_CALL POST_CALL` replays the four
`$C30668` line jobs, `$C30404` descending fill, both `$C30466` lane copies,
and `$C304B2`, with zero differing bytes and the expected 110-byte pre-call
delta against `build/run036_7000_c2ff48_submission_trace_no_future/`.
`--post-lines` independently starts at the external settled-line checkpoint
and reaches its final image with zero differences. The lane reconstruction
must retain the final fill's A/B/D `$001D` modulos: the source lane leaves do
not rewrite them. This is still a diagnostic-only Chip-RAM producer; normal
`game.c` neither owns an OCS-address-to-five-plane-page binding nor schedules
the source scene/root state that calls it. Normal replay remains exactly 192
frames (200--391), with frame 392 still the first mismatch. The early
`$C2FF56` return remains invalid because blitter work is active.

`FA18RendererLaneStage` now ports the normal `$C2FF58-$C30037` lane tail as
one typed operation: its caller supplies the source-order `$C456B6` pointer
block, lane-enable/scale words, mutable `$C45956`, and the final lane
workspace fields. It performs each enabled `$C30466` submission and the final
`$C304B2` submission while preserving inherited blitter registers. The
non-negative-enable/nonzero-stage-flag `$C3040C` prelude remains an explicit
failure, not a substitute. The run036 oracle routes its two enabled lanes
through this API and stays byte exact. It is a reusable renderer backend, but
it still requires an OCS Chip-page binding and source scheduler before it can
be connected to `FA18FlightRendererPage` or `game.c`.

The separate opt-in `--bootstrap-c279-render-fixture SLOW CHIP` diagnostic
starts from external frame-384 pre-call state, then runs the native
`$C279D0-$C27D0F` packet/grid/direct-pixel/line path over that page. It does
not embed either capture or schedule it in normal replay. Against
`build/run075_frame382_c279d0_render_page/{slow,chip,final_chip}.bin`, its
one-frame RGB444 result is byte-identical to the original post-return page
(128,000 bytes, zero differences; 12 nonblack pixels under Hunk-21 mode 8).
This proves only that narrow producer invocation, not the later 361-pixel
frame-392 page or its display scheduler.

`port/planar_pixel.c` now preserves the entire `$C2F786` primary dispatch
table: selector 0 reaches `$C2F826` (all clear), while selectors 1--15 map
directly to their matching lane-set targets `$C2F83A-$C2F8C6`. The adjacent
`$C2F830` all-XOR helper is absent from that table. The `$C279D0` packet's
own `$C456E6-$C456ED`/`$C45954` setup now also binds the native page's pixel
and line callbacks before its Hunk-25 traversal. Focused contracts pass;
normal replay remains 192 exact frames through global frame 391.

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

The external `$C279D0` producer diagnostic is deliberately not an exception:
it exists to validate the now-connected renderer boundary with caller-supplied
oracle state. The normal game still has no evidenced scene/root publisher or
scheduler, so it must not be wired into `game.c` without those missing owners.

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
its typed entry selects the source's fixed primary or alternate variant.

`port/flight_followup_record.{c,h}` now ports `$C1CCBC-$C1CD0D`, the common
parent followup reset/select prefix. It exposes the positive selector's
word-wrapped 512-byte record offset and leaves the nonpositive `$C1CE38`
route explicit. `ctest` passes **99/99** tests with the same native frame
result.

`port/flight_followup_pipeline.{c,h}` now composes the bounded positive
`$C1CCBC-$C1CE37` path (selector record, magnitude, shift, and descriptor
dispatch) and its proven nonpositive `$C1CE38` handoff. On that handoff it
uses the caller-owned `$C459B0` alternate-list offset to prepare one
`$C4F6CA` descriptor record, reporting prepared, skipped, and terminator
outcomes separately. The later scaling, record-loop, and indirect-handler
stages remain explicit. Record/table/fixed-point/descriptor operations remain
caller bound. `ctest` passes **103/103** tests with the same native frame
result.

`port/record_delta_scan.{c,h}` now ports the alternate-terminator continuation
`$C1CFD6-$C1D0A3`: source latch/mode gates, the bounded 16-record
`$C46184` scan, wrapped component deltas, the source's adaptive arithmetic
shift, and its selector-word construction. The unresolved `$C25876` call is a
required caller callback, so no submission behavior is guessed. `ctest`
passes **104/104** tests with the same native frame result.

`port/alternate_flight_scale.{c,h}` now ports `$C1CEA4-$C1CEE3`: direct
tuple scaling, the bit-4 `$C1D0B6` component-accumulation path, and their
word/long packet publication. The bit-6 `$C1D0A4` entry is retained as a
required callback because its `$C48184` helper body is not yet bounded.
`ctest` passes **105/105** tests with the same native frame result.

`port/alternate_record_value_gate.{c,h}` now ports `$C1CEE4-$C1CF35`:
long-component publication, signed shifted-value thresholds, the header/control
subroute, and the caller-owned `$C1D91A` fixed-point boundary. `ctest` passes
**106/106** tests with the same native frame result.

`port/alternate_record_loop.{c,h}` now ports `$C1CF36-$C1CFC9`: its signed
countdown early return, record-byte decrement, selected-kind limit override,
four-longword descriptor dispatch, result store, and 24-byte next-record
advance. `ctest` passes **107/107** tests with the same native frame result.

`port/flagged_slot_scan.{c,h}` now ports `$C265E8-$C26605`: the exact
20-slot reverse scan over 64-byte records, testing bit 0 at record `+$27`.
The first flagged slot transfers to a required caller-owned `$C26606`
evaluator; no flagged slots return the source zero result. `ctest` passes
**108/108** tests with the same native frame result.

`port/flagged_slot_evaluator.{c,h}` now ports `$C26606-$C266AB`: its bit-5
or context-selection gate, signed word-derived linked-record offset, both
wrapped absolute-difference triples, `$C1D974` scalar calls, and signed
`first <= second` decision. The `$C46184` linked-record lookup remains a
required caller resolver. `ctest` passes **109/109** tests with the same
native frame result.

`port/record_scan_renderer_pass.{c,h}` now ports the bounded direct path of
`$C1518C-$C1522D`, `$C153BC-$C153FB`, and `$C2F490`: renderer-bound
initialization, counter decrement, the ten even-indexed 64-byte slot sweep,
mode-gated `+$28` decrements, and the finish-state rewrite. The unresolved
candidate body is returned as an explicit continuation when its source gates
are met; a set scan-mode byte correctly bypasses the flag gate and can still
take the direct auxiliary-clear advance. `ctest` passes **110/110** tests with
the same native frame result.

`port/record_scan_candidate_prelude.{c,h}` now ports `$C1522E-$C153DB`:
the selected-slot gates, table/counter and renderer-budget state updates,
`+$26/+2E` word writes, auxiliary transitions, and loop re-entry. The
`$C17F8C`, `$C181A0`, `$C15688`, and `$C153FC` children are required
caller-owned callbacks, preserving their source order without assigning their
unbounded behavior. `ctest` passes **111/111** tests with the same native
frame result.

`port/record_scan_indexed_stage.{c,h}` now ports `$C15688-$C158D6`:
flag/mode-derived coefficient selection, both signed matrix-product triples,
masked origin additions, `+$28/+30/+32` slot writes, and the turn-word
secondary adjustment. Its `$C159AE` tail is a required caller callback.
`ctest` passes **112/112** tests with the same native frame result.

`port/record_scan_tail.{c,h}` now ports `$C159AE-$C15BF4`: turn/flag selector
formation, source-wrapped delta shifts, the `$C257EC` scalar boundary, six
longword result publications, and the flagged motion additions. The scalar
primitive is caller-owned. `ctest` passes **113/113** tests with the same
native frame result.

`port/record_scan_scalar.{c,h}` now ports `$C257EC-$C25862`: signed selector
and component normalization, the `$C1D974` table magnitude, source scale-loop
and DIVU-overflow register behavior, signed result shifts, and word-width
negation. `ctest` passes **114/114** tests with the same native frame result.

`port/scene_active_record.{c,h}` now models the `$C1C54E-$C1C564` active
record binding: the signed `$C458DE.w` offset applied to the caller-owned
`$C46184` record-store base, with a bounded raw-record view for its immediate
consumer. The frame-392 trace observes offset zero, but no captured record is
in native code. `ctest` passes **115/115** tests with the same native frame
result. This is a data binding only; it does not schedule or present a flight
scene.

`fa18_render_active_flight_scene_pipeline` now composes that selected raw
record with the existing `$C1C54E -> $C279D0` packet/page contract. It resolves
the required 164-byte active record before decoding; matrix, grid, render page,
and scheduler are still caller-owned. The existing pipeline contract exercises
both its valid and out-of-bounds paths; `ctest` remains **115/115** with the
same native frame result.

`port/scene_bootstrap_template.{c,h}` now ports the `$C092D4-$C09302`
relocation-backed template binding from original Hunk 16: source selector
`$10` chooses `+$3C`, all other values choose `+$50`, and the copied second
longword is retained as a native `(segment, offset)` descriptor reference.
This is the source-owned input installed before the root-record pose path; it
does not use a captured Amiga address. `ctest` passes **116/116** with the
same native frame result.

Its `$C0930A-$C09334` descriptor-class decode is now included too: it follows
the relocated descriptor segment/offset, preserves the signed/bit-14 selector
routes, and returns only the source low nibble. This is the class input for
the mutable dispatch record; `ctest` remains **116/116** with the same native
frame result.

`port/flight_followup_magnitude.{c,h}` now ports `$C1CD0E-$C1CDB1`, deriving
the three source magnitudes from the selected record and prepared components,
then applying the `$EF` cap through its required `$29` error callback. `ctest`
passes **100/100** tests with the same native frame result.

`port/flight_followup_shift.{c,h}` now ports `$C1CDB2-$C1CDFB`, looking up the
source signed-byte shift count from caller-owned table data and publishing the
shifted depth/components for the following descriptor dispatch. `ctest` passes
**101/101** tests with the same native frame result.

`port/flight_followup_descriptor.{c,h}` now ports `$C1CDFC-$C1CE37` around
caller-owned fixed-point, descriptor-lookup, and descriptor-handler boundaries.
It publishes both descriptor control longwords and advances the source record
index by two. `ctest` passes **102/102** tests with the same native frame
result.

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
- `analysis/routines/run075_frame392_parent_update.md`
- `analysis/routines/run075_frame392_c1cb26_placement.md`

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

`projection_page_blitter.{c,h}` now bridges the typed `$C30668` line packets
and `$C303EC-$C30404` final fill/lane tail onto a caller-owned five-plane page.
It validates that the source-order lane pointers match the page's Chip binding,
preserves the caller-provided inherited blitter registers, and synchronizes at
each line/fill boundary. The adapter is available as the two
`FA18ProjectionPairSubmission` callbacks, but is deliberately not yet wired
to `game.c`: the parent still must provide its actual record, matrix, page,
lane state, and scheduler cadence. `ctest` passes 120/120 contracts.

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
