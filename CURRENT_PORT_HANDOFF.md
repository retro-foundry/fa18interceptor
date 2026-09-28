# C port continuation handoff

## Context handoff — 2026-09-28

- Branch: `coverage-accounting`; all completed slices are pushed to
  `origin/coverage-accounting`.
- Worktree: user-owned untracked `.vscode/` only; leave it untouched.
- Native normal-render truth: `game.c` still does **not** schedule the real
  placement/control-stream owner.  Do not claim, show, or link a native frame
  until `scripts/render_native_visual.py --frame 402` reports nonblack native
  pixels.  Do not route `fa18_game_present_active_scene_render_diagnostic`, a
  capture fixture, or an emulator/oracle image into normal presentation.
- Authority: translate the supplied P-code/observed assembly directly.  The
  emulator is validation only, never implementation input or a displayed
  substitute for the native frame.
- Last full validation: 204/204 CTest contracts passed; native build checker
  reported 392 C sources.

### Most recent chain

`terrain_selector_origin_adjustment.{c,h}` now ports the shared
`$C29548-$C295D0` candidate tail.  It quarter-reduces the source magnitude,
uses the required `$C2574A` scale callback, sign-extends/scales the candidate,
smooths it with `$C45C4A`, adds it to the live origin, and writes the masked
X/Z negated companion separately.  Static adjustment-table dispatch and the
parent owner remain outside this bounded tail.

`matrix_transform_components.{c,h}` now ports `$C091F0-$C09249`, the shared
signed three-word transform leaf: wrapped signed word products through the
active 8.8 matrix, arithmetic longword shift by four, and wrapped addition of
the selected record `+$14/+18/+1C` triple.  It is available to the
matrix-table branch of `$C29042`; table selection and parent scheduling remain
caller-owned.

`current_record_matrix.{c,h}` now ports `$C2DAF2-$C2DB17`, the matrix
preparation called at the `$C29042` entry.  It reads the selected record
`+$68`, substitutes `0x7080 - angle` only for a nonzero source word, and
builds the resulting single-angle matrix.  The callback adapter lets a
source-owned active-record/matrix state satisfy the direct origin publisher;
it does not create the missing parent update owner or schedule presentation.

`terrain_selector_origin.{c,h}` now ports the direct-record lane of
`$C29042-$C291D3`.  In the source gate combination `C45785 != 0`, `C457B5 !=
0`, `C457AE == 0`, `C457AD != 0`, and `C458AE == 0`, it calls the caller-owned
`$C2DAF2` matrix-preparation boundary, reads the active record `+$14/+1C`,
retains the previous middle component, applies the exact signed-word floor
`(+$4E + 7) << 8`, then takes the common `$C295B6` masked/negated-companion
write after publishing the selector triple. Its adapter can satisfy `$C1C6BC`'s
origin-update callback only for that proven lane. The matrix-table route and
`$C291D4` continuation fail explicitly; this slice is not scheduled by
`game.c`, so normal frame 402 remains black/unverified.

`record_walker_runtime.{c,h}` ports bounded `$C1F6F8-$C1F966`, including
`$C1F7A0` A1/A2 selection and `$C1F910` negative-selector dispatch.
`line_record_dispatch.{c,h}` is the strict `$0034 -> $C212B0 -> $C2EE4A ->
$C2FA7E` adapter.  Its frame-602 contract starts at real A2 `$C3985A`, reads
offsets 342/348, and gets native callback coordinates `(173,68)->(163,68)`.
All non-`$0034` selectors fail explicitly.

`extended_record_dispatch.{c,h}` ports `$C1F94E-$C1F999`; it reaches the
required `$C1F99A` transform boundary and then dispatches the next selector.
It deliberately takes `$C1F99A` as a callback—no transformed records are
invented.  The next direct implementation task is `$C1F99A-$C1FB22` using
the bounded trace at `build/run031_frame7500_c1f99a_external_trace/` (231
instructions, return PC `$C1F970`) and
`build/run031_frame1965_c1f99a_transform_entries.json` for its descriptor,
source, workspace, and matrix ranges.

`c1f99a_record_transform.{c,h}` now ports the observed descriptor-bit-0
`$C1F99A-$C1FA90` lane: it retains descriptor-relative source/destination
offsets, the source D1/D2 exchange, both signed 8.8 matrix passes, word/long
wraps, and writes the transformed triples into the caller-owned `$C48390`
workspace.  The descriptor-bit-0-clear route remains an explicit failure
boundary because it transfers through `$C1FA92` into the separately unported
`$C1FB24` continuation.  `extended_record_dispatch` now permits distinct
transform and selector contexts, so its `$C1F99A` callback can use this real
transform without inventing a shared owner.  `record_walker_runtime` now
binds that `$C1F94E` positive route through the same enclosing `$C1F910`
status/restart loop; an unbound positive route remains explicit.  The next
task is binding this resulting line-emitter owner at the actual
placement/scene runtime schedule.  Run the normal visual gate immediately
after that binding; if it remains black, report exactly that rather than
presenting a contract or oracle artifact.

`flagged_record_component_bound.{c,h}` now ports `$C1FC42-$C1FCCE`, the
flag-controlled record-component bound used by the `$C1F77C` walker branch.
It preserves selector-specific word/long widths, source shift counts, and the
source-visible Z/D7 result rather than replacing it with a Boolean. The
frame-601 selector-one packet (`112` against bound `132`) is a contract. It
is a necessary walker predicate, not the missing live placement/page owner.

## Latest record-walker composition

`record_walker_runtime.{c,h}` now directly composes the bounded P-code path
`$C1F6F8-$C1F966`: ordinary record traversal, `$C1F7A0` negative-control
A1/A2 selection, and `$C1F910-$C1F94D` negative selector dispatch.  Its table
handler receives A2 after the selector word, which is the source cursor needed
by `$0034 -> $C212B0`.  `$C1F844`, `$C1F8EC`, and `$C1F94E` remain explicit
external routes; no selector is skipped or substituted.  The contract reaches
the real `$0034` dispatch boundary and resumes the source A5 terminator after
a negative target return.

This is a P-code translation boundary only.  It is not scheduled by `game.c`,
and normal native frame 402 remains unverified/non-rendering until the live
placement/stream owner supplies its real inputs and `$0034` is bound to the
five-plane line emitter.

Validation after this change: 195/195 CTest contracts pass and the native
build checker sees 372 sources.

`line_record_dispatch.{c,h}` binds only the proved `$C1F942` table slot
`$0034` to `$C212B0 -> $C2EE4A -> $C2FA7E`.  Its source-addressed `$C212B0`
input retains the big-endian A2 stream and `$C48390` table; it neither imports
a capture page nor fabricates host geometry.  The frame-602 contract starts at
the real A2 `$C3985A`, reads pair offsets 342/348, and reaches the native line
callback with the original output `(173,68)->(163,68)`.  Other table selectors
fail explicitly until their P-code targets are ported.

`extended_record_dispatch.{c,h}` ports `$C1F94E-$C1F999`, the positive table
control immediately before the `$C1F99A` transform and its following selector
target.  `$C1F99A` remains a required caller-owned transform callback; the
implementation preserves the source shift/limit loop and returns its control
transfer explicitly rather than supplying transformed data itself.

## Latest visible primitive

`offset_pair_segment_submission.{c,h}` ports `$C212B0-$C2131B`, the direct
producer immediately above `$C2EE4A`.  It resolves the source offset-pair
stream through `$C48390`, retains the source's negative-second-offset terminal
pair, applies the original depth-AND skip, and accumulates the exact
segment-preparation result.  The `$C2EE4A` callback remains explicitly
caller-owned: do not connect it to a generic clipper or a diagnostic renderer.

`projected_segment_clip.{c,h}` now ports all four signed interpolation entries
`$C2F0C6`, `$C2F0F4`, `$C2F128`, and `$C2F156` through their shared
`$C2F186-$C2F1B6` result gate.  It preserves 16-bit add/subtract/negate,
signed multiply/divide, the `$C45AC6` candidate triple, and the gate bit that
the `$C2EE4A` dispatcher branches on.  The next direct consumer is that
dispatcher, not a host clipping replacement.

`projected_segment_preparation.{c,h}` composes `$C2EE4A-$C2F0C5`: it follows
the P-code's signed D3/D4 branch graph, retains the caller-owned
`$C45AC6/$C45ACA` state through retry entries, projects both selected
components via the `$C2F03A` 320x180 tail, and invokes the page owner's
`$C2FA7E` callback.  Its contract reaches the callback through the source
branch structure.  It is ready to bind as the `$C212B0` callback; normal
`game.c` still does not schedule the real placement/control-stream producer.

`workspace_segment_projection.{c,h}` ports the complete direct leaf
`$C2ED6C-$C2EE42`.  It consumes two source-format transformed triples from
the `$C4C592` workspace, preserves each signed depth/extent rejection, applies
the source's `160/depth + 160` and `90/depth + 90` projection/clamp operations,
reverses the resulting 320x180 endpoint coordinates, and invokes the
caller-owned `$C2FA7E` line-emitter callback.  Its contract covers one accepted
pair and both rejection categories.  It has no synthetic geometry, no capture
page, and no `game.c` scheduling.

Validation: 194/194 CTest contracts pass and the native build checker sees
370 C sources.
This is an exact renderer leaf, but normal frame 402 remains
black until the P-code placement/record path supplies the real workspace pairs
and binds the real five-plane line-emitter owner.  Do not present the contract
callback as a normal rendered frame.

## Latest dispatch boundary

`scene_stream_runtime.{c,h}` now composes the direct P-code route
`$C1EE14-$C1EF9C -> $C1F464-$C1F578 -> $C1F6F8-$C1F79F`.  It keeps the
selected descriptor/control-stream and its `$C48390` transformed triples in
one big-endian mutable workspace, then hands the same original offsets to the
record walker.  Descriptor control-bit branches and negative walker controls
remain explicit unported routes.  This is a source-data owner boundary, not
yet a `game.c` schedule: normal frame 402 remains black (zero nonblack
pixels).

The stream-runtime input now preserves the source's two independent pointer
families at `$C1EE44-$C1EF10`: descriptor `+8` supplies A2's control stream,
while descriptor `+4` supplies A0's record/vertex base.  The old combined
buffer contract could not represent relocated Hunk descriptors such as
`$C2232C -> A2=$C445DC`; the corrected API and contract use deliberately
separate control and descriptor byte ranges.  This is a P-code dataflow
correction only; the normal frame-402 visual gate is still zero nonblack
pixels.

`scene_descriptor_static_data.{c,h}` now resolves a `$C1CB74` descriptor
runtime address inside Hunk 16 through its four original `HUNK_RELOC32`
fields.  It returns bounded `(target Hunk, target offset)` pairs and target
bytes, never a host pointer or reconstructed Amiga address.  In particular it
can express the observed `$C2232C` route to the `$C1EE14` code Hunk and its
`$C445DC` control data.  Its next consumer is the placement-loop callback;
normal frame 402 remains black until that source-owned mutable list is
produced and scheduled.

The `$C1CB74` trampoline guard now compares the first descriptor relocation as
Hunk 10 offset `$10` (the loader-backed representation of `$C1ED48`) rather
than comparing a manually reconstructed absolute value.  The static-data
lookup adapter exposes exactly that pair to the existing placement traversal.

`prepared_record_dispatch.{c,h}` ports `$C2005C-$C200F5` through the
explicit `$C2469E` handoff.  It retains the source's two direct transformed
tuple offsets, variable continuation terminated by a negative masked offset,
depth-AND rejection, descriptor handling, `$C1FB82` branch result, stream
advance, and both local counters.  The frame-601 emulator packet
`{0x002a,0x0024,0x8072,0x000c}` is a reusable contract: it reaches the traced
negative return at `$C200F2`.  This code is deliberately not scheduled by
`game.c`; a live record-walker/page owner must consume its display-ready
workspace before it can affect normal replay.

Validation at this boundary: 188/188 CTest contracts pass; native replay is
still exact over frames 200--391 and first mismatches at 392.  The normal
frame-402 renderer gate remains black (zero nonblack pixels).  Do not present
this dispatch contract as a rendered frame.

## Starting point

- Branch: `coverage-accounting`
- Implementation head: `128cd121 Update live producer handoff`.
- The current working tree adds a source-addressed five-plane/Chip-RAM binding
  for the reusable `$C2FF58-$C30037` backend. User-owned untracked `.vscode/`
  remains untouched; do not discard it.
- Goal: complete the faithful C port, committing each coherent, validated stage.

The current native reference check reaches **192 exact frames**: global frames
200 through 391.  It first mismatches at global frame 392 (361 of 64,000
pixels; bbox x=7..318, y=101..199).  The configured suite contains **142** contracts;
the last full run passed 142/142.

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

Subsequent source-backed matrix work brings the native build check to 241
files and the contract suite to 127/127.  The current pushed commits are
`e97f6ee5` (matrix tail), `ee8aa794` (signed divide), `be061c83` (magnitude
refinement), `bfc81a8b`/`626bc430` (negative-pair route and table oracle),
and `d10a2361` (correct first-ratio operand).  The `$C12570` operand is the
unscaled `$18(a6)` component shifted by six, not its earlier scaled value;
the bounded route therefore reaches Hunk-63 table indexes 25 and 43 and
publishes the traced output pair `(760,6760)`.

The current live-producer path has new source boundaries: `4984b0d7` composes
`$C0EFD4-$C0F123`; `be796877`/`4cc67274` port the signed-control static
template band walk; `1c7b5a74` ports its sorted-row lookup; and
`43bdc63b`/`79b604f4`/`3f471e2b` port the append-enabled live-record marker
bridge. The workspace stream selector and the downstream `$C1DC1C` placement
builder remain required before this can supply a native C279 scene page.

## New-context resume point

The current native build audit passes 255 source files.  The user-owned
untracked `.vscode/` directory is the only worktree change.  Do not schedule
`$C279D0` from a presentation frame or import captured scene/page state.

`static_template_stream_selector.{c,h}` now ports the `$C1D3F4-$C1D51D`
immutable stream selector and its workspace-item expansion. It composes the
existing sorted-row search and append bridge at their source boundaries,
preserves the 16-item/96-byte cell limit, and retains the special-pair
partial-register result: the second output word keeps the sign-extension high
byte of the first source byte. The downstream `$C1DC1C-$C1E0B0` placement
builder and its upstream workspace terms remain unported. Normal replay is
still exact through frame 391 and first differs at frame 392 by 361 pixels.

`scene_placement_builder_tail.{c,h}` now ports the bounded
`$C1DE3E-$C1E0B0` direct-record path of the placement builder: live work,
projection-packet, and descriptor words produce the three normalized
magnitudes; the tail then selects its adaptive shift, ORs it into the selector,
writes three signed coordinates, and publishes the exact 24-byte record
suffix. Its caller still owns the preceding workspace/descriptor route and
the live terms; it is not scheduled by game.c.
Its `$C1DD98-$C1DE38` work-term prefix is also ported: the selected workspace
pair, C1D7E2 correction pair, and caller-owned translated/origin terms form
the first and third source work longwords while the middle word remains clear.

`scene_placement_builder_prefix.{c,h}` now ports `$C1DC44-$C1DD34`: its two
signed-byte maps select the source-scaled coordinate pairs and one bounded
96-byte `$C48390` workspace cell. `terrain_placement_pipeline.{c,h}` now
ports the following `$C1DD22-$C1DD34` cell walk, including its source
terminator and bit-4/bit-6 payload stride; it composes the prefix and cell
walk into one `$C1DC44-$C1E11A` record-emission boundary.
`terrain_placement_direct.{c,h}` now supplies all bounded `$C1DD36-$C1E0B0`
record routes: flags-clear entries use their six-byte payload, bit-4 entries
bind mutable 512-byte `$C46184` records and preserve `$C1DE82`'s conditional
bit clear, while bit-6 entries follow the 32-byte `$C48184` lookup before
forming the `$C22188` identity. Flagged entries retain the caller-owned prior
`$C456F6` third work value and use the distinct `$C459B5` tail. The normal
tail now carries `$C459BC` exactly from the signed placement-map byte to the
next ordinal. Top-level live-term ownership remains unported. Do not schedule
the renderer from a presentation frame or import a captured page.

`outer_update_loop.{c,h}` now ports `$C15D80-$C15DB3` as the source-level
owner above the parent update: it retains the one-time acquire/delay prefix,
then one explicit iteration of OwnBlitter, DisownBlitter, `$C0EFD4`, display
wait, and `$C1612C`. The caller still owns loop repetition at the original
back-edge and every unresolved child body, so this does not infer a
presentation-frame scheduler or wire incomplete parent work into `game.c`.

`matrix_cache_update.{c,h}` ports the direct `$C2DC9A-$C2DCC0` `$C45BD8`
publisher: it composes the three live `$C45A8A/$8E/$92` angle words and applies
the three `$C45A3E` row scales. `control_record_matrix_route.{c,h}` composes
the observed default `$C2DB18-$C2DCC0` route: a non-special active control
record's `+$66/+68/+6A` triple supplies `$C45A88`, `$C45BEA`, and the scaled
`$C45BD8` cache. The run075 matrix is contract-verified as
`{167,0,-8; 0,252,0; 6,0,127}`. Alternate selector/type routes and the live
row-scale producer remain explicit caller boundaries; no captured values enter
the normal runtime.

`scene_renderer_defaults.{c,h}` now supplies the direct cold-boot
`$C09010-$C09068` renderer stores to normal `game.c` initialization: matrix
input `$1C20/0`, row scale `$A8/$FC/$80`, and display bounds
`$A7/$32/$320`. These are original bootstrap constants, not run075 capture
values; page production and presentation remain separately scheduled.

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

`coordinate_update_negative_pair.{c,h}` now ports the fully traced low-scale
negative-pair path through `$C123FA`: source Hunk-63 angle words, `$C25980`,
and `$C2564E` produce the two `$C45AC0/$C45AC2` outputs. It rejects all
unobserved sign, dominant-component, scale, and terminal branches instead of
generalizing from the trace.

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

The capture-free `--render-active-scene` diagnostic now reaches the source
`$C279D0` ready route at run075 frame 392.  The correction is that this helper
gates on the published shifted middle packet word (`$C45A78`, represented by
`FA18ProjectionPacket.y`), not the upstream pre-shift intermediate retained as
`depth_metric`.  The source-root diagnostic reaches eight renderer submissions
after this correction, while still presenting black on its fresh page: the
observed direct-pixel state writes a zero-valued lane there, and the visible
frame requires the unported polygon/fill descendants plus the earlier prepared
page.  This diagnostic remains opt-in and has no effect on normal replay.

A full ordinary-replay sample of the displayed five-plane allocation
`$04DB30-$05776F` changes the earlier producer assumption: it records 63
mutating frames over run075 201--392, including large bursts of 6,249 bytes at
frame 382, 3,563 at 385, and 6,868 at 387.  The same 381--389 PC profile is
dominated by `$C304F8` span/fill wait-submit work and includes `$C2FE..`
active-plane packets and `$C3066A` submissions.  By contrast, the bounded
frame-384 `$C279D0` pass changes only six bytes of one lower plane.  Therefore
the normal native page owner must compose the existing active-plane and
page-blitter paths with their live parent state; a fresh root-grid pass cannot
produce the palette-revealed image on its own.  The sampler output is
`build/run075_display_page_mutations_201_392/memory_region_mutations.json`.

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

The next prepared-page producer witness is now narrowed further. A
return-bounded `$C304B2` capture in the run075 preparation window carries
`BLTSIZE=$0E14` and shared A/B/D pointer `$000076EE`; its stack proves
`$C24D60 -> $C2FF48 -> $C2FF58 -> $C3002A -> $C304B2`. The contemporaneous
`$C279D0` calls take their direct-line continuation instead and do not reach
that fill. Recover the `$C24D` polygon input/transform publisher before
wiring a normal page-render schedule; do not treat the projection-grid
diagnostic as the complete prepared-page producer. See
`analysis/routines/c304b2_renderer_child.md`.

`scene_root_placement.{c,h}` and `scene_root_record.c` now also port the
negative-root handoff at `$C09514-$C0951A`: after the selected source record
has driven `$C2D954`, its three `+$66/+68/+6A` angle words are copied into
root slot zero.  This is the record subsequently selected by the observed
`$C1C54E` projection seed and default `$C2DB18` matrix route.  The C contract
checks both the signed angle matrix inputs and the exact big-endian root
stores; no run075 capture values are introduced into normal runtime state.
The full native gate passes 142/142 contracts and retains 192 exact frames
(200--391), first differing at frame 392 by 361 pixels.

The selected record's run075 second angle is now traced to its real dispatch
producer, rather than treated as a root-placement value.  At global frame 272,
`$C28AFE -> $C28B34 -> $C28800` calls `$C123FA`, which publishes
`$C45AC2=$6FB8`; `$C288BC` then invokes `$C2D954` with `(0,$6FB8,0)` for slot
14 (`$C47D84`).  The full `$C28800` trace proves run075 takes its **negative**
selector route: `$04(A2)=$8D0E`, mask `$7F00`, ASR#7 to `$C295E0` table index
26, then resolve `(70,112,6144,6144,0)`. `$C28F16` copies that tuple to
`+$2C..+$34`, `$C28824` writes `+$38=$FF`, and the source longword expansion
forms the recorded coordinate packet `(0,0,$00800000,0,$0B000000,-1)`.
`scene_dispatch_negative_coordinate_pose.{c,h}` now ports this bounded
`$C28808-$C288C4` route behind a required Hunk-27 geometry resolver, while
`scene_dispatch_coordinate_pose.{c,h}` remains the distinct nonnegative
linked-record route. `coordinate_update_positive_pair.{c,h}` ports the
recorded `$C123FA-$C1294E` callback route, including shift 14, rounded ratio,
Hunk-63 index 11, magnitude, and clamp path to `$C45AC2=$6FB8`. The dispatch
runtime now carries `$C28AFE`'s final created target through this negative
route with the source Hunk-27/Hunk-63 tables before root placement. The
nonnegative linked-record resolver remains unported; do not seed `$6FB8` or
page data.
See
`analysis/routines/run075_scene_dispatch_coordinate_pose.md`.  Do not seed
`$6FB8` in normal runtime state.

## New-context resume point: live pose complete, renderer page blocked

The current pushed head is `f3146e1a` (`Schedule negative dispatch coordinate
pose`), following `20e93a97` (`Port negative dispatch coordinate pose`).
The only user-owned worktree change is untracked `.vscode/`; leave it alone.

The `$C28AFE -> $C28B34 -> $C28800` run075 route is now composed in normal
scene entry, not merely represented by a diagnostic adapter.  The dispatch
runtime retains the final created target, resolves its negative selector via
the original Hunk-27 `$C295E0` relative-word table, uses the original Hunk-63
angle table through `$C123FA`, and publishes slot 14's `(0,$6FB8,0)` attitude
before `$C09514-$C0951A` copies that triple to root slot zero.  The Hunk-27
geometry decoder was corrected to preserve its source indirection and five
sign-extended-word `MOVEM` shape.  Relevant source-backed modules are:

- `scene_dispatch_negative_coordinate_pose.{c,h}` (`$C28808-$C288C4`);
- `scene_dispatch_runtime.{c,h}` (`$C28AFE` final-target scheduling);
- `scene_dispatch_table.{c,h}` (`$C295E0` indirect geometry lookup);
- `scene_entry_runtime.{c,h}` (Hunk-63 table ownership).

The normal runtime still does **not** draw/present flight polygons.  Its
existing `flight_scene_pipeline` can consume root record 0 as the `$C1C54E`
projection seed and the original Hunk-25 grid, but `game.c` does not yet own
the source `$C0F090 -> $C279D0` parent call, a five-plane render-page choice,
or Copper/page-pointer publication.  The capture fixtures remain diagnostic
only and must not be wired into normal replay.

The last evidence audit established that `$C1718E` is a JOY0DAT callback, not
the normal page scheduler.  The next evidence target is the live
`$C1612C` outer-loop child/page-pointer route: recover how it publishes the
render/display page and Copper state after the `$C0EFD4/$C0F090` parent update.
Relevant evidence is `analysis/routines/c1612c_outer_loop_child.md`,
`analysis/routines/run075_frame392_cockpit_entry.md`,
`analysis/routines/run075_frame392_parent_update.md`, and
`analysis/routines/run075_c279d0_prepared_page_handoff.md`.

`outer_update_loop` now includes the previously omitted mandatory `$C2F558`
selector stage before its `OwnBlitter -> $C0EFD4 -> DisownBlitter` bracket.
That is the source-owned `$C456B6` publisher feeding `$C279D0`; the actual
page tables and loop cadence remain caller-owned and are still not wired into
`game.c`.

The live `$C1612C` and `$C0EFD4` breakpoint samplers now correct the earlier
outer-loop cadence interpretation. Over frames 201--392 the child totals 392
entries, with early replay frames carrying zero, one, four, five, or six
iterations. In the frame-360--392 preparation window both adjacent calls run
six times per replay frame, and child indices alternate `1,0,1,0,1,0` before
their tail toggle. This confirms the `$C4566E` prepared-page handoff while
also proving neither a once-per-frame nor a globally fixed six-pass
`game_frame` schedule is source-backed.

The run075 `$C2D99C` return trace (`build/run075_frame373_c2d99c_matrix/`)
now proves the source matrix producer for the prepared-page pass. At global
frame 380 `$C45785=0` dispatches to the default `$C2DB18` control-record
route, rather than `$C2D9BA`: active-record `+$66/+$68/+$6A=(0,28600,0)`
feeds `$C2E3DE`, then the live `(168,252,128)` row scales feed `$C2E5AC`.
It publishes `$C45BD8=(167,0,-8;0,252,0;6,0,127)`, the matrix consumed by
the existing frame-384 projection-grid contract. The missing normal owner
must compose that existing control-record route and the active-record
projection seed, not import trace values. `$C2D9BA` and its `$C45A94/$C45A96`
input-selection callbacks are a separate enabled/fallback route.

`default_scene_render_pass.{c,h}` now makes that bounded composition callable:
it resolves the caller-owned active record once, executes the default
`$C2DB18` cache route, then feeds its `$C45BD8` result and the same record to
the existing `$C1C54E -> $C279D0` page pipeline. Its contract reaches the
renderer ready route with the run075 matrix oracle and rejects alternate
control-record selections explicitly. It is not wired into `game.c`: source
page selection, palette publication, and the parent-update/outer-loop cadence
are still required before a normal presentation can be scheduled.

`game.c` now invokes the bounded `$C1718E` viewport-mode tail once per replay
frame, matching the counted run075 callback cadence. It owns the mutable
COLOR00--15 instruction stream and source Hunk-21 palette buffer, so the
normal scene-entry transition advances its current/target/countdown state
without any frame-number branch. The `$C182BA/$C182C2` pointer publication is
still deliberately opaque in this owner: the real two-page pointer payload
and outer-child display publication have not been attached, so this state-only
step leaves the frame output unchanged.

`outer_loop_child.{c,h}` now composes the complete observed
`$C1612C-$C16283` packet behind explicit caller-owned OS boundaries.  It
performs `WaitBOVP`, publishes the indexed `$C182BA/$C182C2` pair, and invokes
`LoadView` before the source's idle or signed activity path.  The activity
path retains the exact static/dynamic 32-word RGB4 ordering and all
`WaitBOVP`/`WaitBlit` boundaries; no page, palette, or callback schedule is
invented.  `outer_loop_child_full_contract_test` verifies the two-iteration
activity sequence and the idle route.  This supplies the display child for a
future real outer-loop owner, but is intentionally not wired into `game.c`
until that owner has source-backed parent callbacks, render-page choice, and
Copper publication.

The run075 frame-389 child now has an exact display-page witness: entering
with outer index zero, it copies `$C074D8/$C07F00` from the two `$C182BA/$C182C2`
tables into the View/ViewPort display fields before `LoadView`; `$C07F00`'s
linked Copper stream is the `$5200` five-plane `$04DB30,$04FA70,$0519B0,
$0538F0,$055830` interval. This is the prepared `$C4566E` page rendered at
frame 384, not the alternate renderer table selected after the child tail.
Those source addresses are evidence only; the pending native page owner must
use page identities and preserve the index-zero publication relationship.

`flight_page_handoff.{c,h}` now provides that native two-page owner. It binds
caller-owned page-view identities to two five-plane renderers, returns the
current `$C2F558`-selected renderer, and executes the complete child so its
`LoadView` publication selects the matching native page and its RGB4 loads
apply to that page before presentation. The contract verifies index-zero
render/display publication and the child-tail toggle to page one. `game.c`
still does not schedule this owner: the `$C0EFD4` gate and its source page-view
identity producer must be composed first.

`periodic_notification.{c,h}` now ports `$C11B44-$C11BAF`, the direct
periodic byte stage called from the parent prefix after `$C0F5F8`.  It
preserves decrement-before-signed-test behavior, the `8/$86` expiry reload,
the `$06/$04` intermediate codes, and the signed clamp path; the latter is
important because source byte `$80` decrements to positive `$7F` before it is
clamped, rather than taking the expiry branch.  A new normal run075 parent
trace at the frame-392-era invocation (`build/run075_frame392_c0efd4_parent_full/`)
returns through `$C15DA8` after 91,544 instructions and reaches `$C0F090` /
`$C279D0`; its `$C11B44` call decrements the countdown from two to one.
The later observed parent invocation (`build/run075_frame391_c0efd4_parent/`,
hit frame 199) takes `$C0F370` instead after `$C11B44` expires its countdown,
so this evidence rejects attaching `$C279D0` to every native presentation
frame.  The remaining owner is the upstream `$C0F5F8/$C11B44` gate state,
not a frame-number schedule.

`post_input_tick.{c,h}` now separately ports the complete static
`$C0F5F8-$C0F811` local state machine: early guards, longword offset
calculation, all four callback selections, and the unconditional tail's
byte/word wrapping writes. `$C06C02` and `JSR (A0)` remain required,
caller-owned hooks; absent hooks return an explicit error after the local
source writes rather than silently supplying a callback. This makes the
upstream `$C45795` gate state representable without conflating it with the
bounded menu fixture, but it is not yet scheduled by `game.c`.

An ordinary-replay PC profile over global frames 370--392 samples `$C279D0`
at 373, 384, and 392, and `$C1612C` at 380 and 389; it cannot establish
absence on the other frames. The new frame-373 return trace takes 11,352
instructions and confirms `$C456B6=$C4567E`, versus the known frame-384
prepared-page entry's `$C4566E`; all three entries share packet
`($E7C1,$E64E,$FFFFFF83)`. This is a source page-selection clue, not
permission to add a native frame-number render schedule or infer cadence. See
`analysis/routines/run075_c279d0_prepared_page_handoff.md`.

Current validated state uses `build\\port-native`: native build audit passes
295 files, `ctest` passes 157/157, and frame parity remains 192 exact frames
(200--391).  Frame 392 is still the first mismatch: 361/64,000 pixels,
bbox x=7..318 y=101..199.

The frame-392 difference is a source palette reveal, not a new page write:
the prepared five-plane cockpit page was populated before the transition, and
the mode-8 lower-16 COLOR update makes 361 existing pixels visible.  Therefore
the next native work is to port and compose the live prepared-page producer,
not to schedule a blank page or add a frame-specific render hook.  The latest
return-bounded evidence is `$C0D752 -> $C2E758 -> $C0D7E0` in ordinary run075;
`$C2E758` writes `$C4B990` pairs before the existing `$C24A/$C24C/$C24D`
clipping/projection/fill path.

`display_record_candidates.{c,h}` now ports the preceding `$C0D752-$C0D7D2`
matrix preparation prefix using four `$C0D720` pairs: the source's count-four
decrement/BGT loop corrects earlier five-record prose. Its live run075
contract produces the four `$C4B390` triplets from the observed
`$C45A66/$C45BD8` inputs. It is not a renderer hook; the `$C2E758`
adjustment/selection iterator and `$C0D7E0` consumer still determine whether
those candidates become display pairs and polygons.

`display_record_iterator.{c,h}` now ports the full `$C2E758-$C2EC67`
eight-record adjustment/selection loop, including its four source arithmetic
forms, bounded projection, exact clears/defer behavior, and scratch slots.
It matches the return-bounded run075 `$C4B390/$C4B990/$C4E854` witness, but is
still not scheduled: `$C0D7E0`'s consumer must convert these pairs into the
source polygon submission before a native five-plane page can be rendered.

`display_record_selector.{c,h}` now ports `$C0D7E0-$C0DA9F` and turns the
iterator's workspace/scratch outputs into the exact `$C4B390` list layout.
The live path selects four reflected pairs `(0,89), (319,89), (319,0), (0,0)`
and publishes the success flag. The remaining direct path is the source
`$C2FEDE -> $C301F6` submission and its page-blitter owner; it is not yet
scheduled in `game.c`.

`display_record_pipeline.{c,h}` now composes the complete prepared-list
producer `$C0D752 -> $C2E758 -> $C0D7E0` against caller-owned source-shaped
workspaces. Its chained live fixture matches the selected list consumed by
`$C2FEDE`. The candidate cursor starts are 16 words apart: each iteration
writes three words and then advances by `$1A`; this corrects a tempting but
wrong 13-word placement. The remaining boundary is now exactly
`$C2FEDE -> $C301F6 -> $C30306` and its inherited five-plane blitter state.

`selected_display_submission.{c,h}` now bridges the selected source list into
the existing direct `$C301F6` bounds/far-list path (without adding the distinct
`$C2FF48` DMA wrapper). The live list takes `$C302DE` and reaches `$C30306`.
`fa18_flight_renderer_page_bind_projection_page_blitter` now binds those
callbacks to a selected native page when, and only when, the caller has built
an initialized page blitter whose page identity matches all four lower planes.
It rejects any other page, retaining the source inherited registers and lane
state in the caller-owned `FA18ProjectionPageBlitter`. The `$C2FEDE` parent,
its live inherited blitter inputs, and the source render cadence still need a
native owner before this path can be scheduled in `game.c`.

`selected_table_display_stage.{c,h}` now composes the full bounded
`$C2FEDE-$C2FF45` local sequence around the prepared-list pipeline and direct
submission: it substitutes/restores the saved table field, skips `$C301F6` on
a nonzero prepared-list result while retaining the source's post-pass
fallthrough, conditionally invokes the selector lane callback, and performs
the mode-clear or 24-byte selected-list copy. The `$C30466(D0=8,D3=0,D4=0)`
and `$C0D74A` children remain explicit caller callbacks because their live
blitter/table state is not yet bounded. This permits a future source-owned
parent to pass the matching page's bound blitter submission without inventing
either hardware state or render cadence.

`fa18_apply_viewport_copper_palette_to_video` now closes the native
presentation boundary after `$C1718E`: when that callback has updated the
mutable COLOR00--15 Copper moves, normal replay copies those published words
to `FA18Video` while preserving COLOR16--31. A live run075 state trace reaches
mode eight with COLOR01=`$100` and COLOR02=`$111`, matching the frame-392
palette evidence. Frame parity remains 192 because the native pixel indices at
the 361 revealed locations are still zero: the missing work is the prepared
five-plane page producer, not another fade or palette substitution.

`active_record_pose_commit.{c,h}` now preserves two bounded writers which
mutate the active `$C46184`-family record: `$C14D0C-$C14D93` commits
`+$18 -= local_delta` with its status/clamp/continuation decisions, and
`$C25E6E-$C25E7D` publishes caller-produced `D2/D4` into `+$14/+$1C`.
The run060 frame-925 oracle confirms `$72301 - $2580 = $6FD81`; because that
result exceeds the source limit `$708`, it takes `$C1505E` before clearing the
caller status bit. These are deliberately unscheduled leaves: the unported
`$C13D84` parent still owns delta production, the downstream continuations,
and the cadence that must drive the live scene record.

`fa18_flight_adjust_signed_word_pair` now preserves `$C15138` word-width
arithmetic: `ADD.W`, `NEG.W`, shifts, and `SUB.W` all retain 68000 16-bit
wrap. In particular `$7FFF + 1` becomes `$8000`; `NEG.W` leaves that signed
minimum unchanged, so the source takes the `<= 4` route and the final
subtraction wraps back to `$7FFF`. This closes an edge-case divergence in the
already ported `$C14B16` scaled-delta producer, but does not schedule it.

The live prepared page cannot be supplied by a fresh `$C279D0` root-grid pass:
normal run075 page mutations are dominated by `$C304F8` span/fill work, while
the frame-384 `$C279D0` pass changes only six lower-plane bytes. The proven
two-page `$C1612C` handoff must therefore receive the complete source
`$C0D752 -> $C2FEDE/$C301F6 -> $C304F8` producer state before it can replace
the capture diagnostic in `game.c`.

A new return-bounded run075 frame-382 trace at `$C24D60` resolves one key
prepared-page producer edge: the stack return at its first `$C2FF48` submit is
`$C2AFE8`. The area-fill path is therefore the already bounded
`$C2AA9C-$C2AFF9` static-map packet pass feeding `$C246A0 -> $C24CFE`, not an
unclassified generic polygon publisher or the small `$C279D0` grid pass. The
native map static-data, transform, clip/display, and page-blitter components
must next be composed with their live parent selectors, matrix, detail, and
two-page owner; do not substitute the trace's packet or page values.

`fa18_load_display_record_candidate_input_pairs` now loads `$C0D75E`'s four
signed candidate pairs from the first 16 executable bytes of original segment
33 (runtime `$C0D720`). They are intentionally code words consumed as data by
the source routine, so the native pipeline no longer embeds the run075 values
as a fixture. The loader is one concrete normal-runtime input for the
`$C0D752` producer; its live matrix, source component, iterator workspace,
and inherited blitter registers still belong to the enclosing parent state.

The same run075 prepared-page snapshot resolves two remaining pipeline names:
`$C45A66=$FFFF83F8` contributes the high-word source component `-1` to
`$C0D752`, and `$C45ACA=$0248` is the signed alternate-adjustment gate used
inside `$C2E758`. `display_record_pipeline` and its iterator now expose the
latter explicitly as `adjustment_gate_5aca`; it is not a caller stack value or
an arbitrary test case selector.

The return-bounded `$C2FEDE` trace corrects the native page adapter boundary:
`$C303EC-$C30404` returns at `$C3040A` directly to `$C2FEF8`; it does not
execute the distinct `$C2FF58` lane tail. `FA18ProjectionPageBlitter` now
performs only the direct line/fill callbacks, leaving the lane stage to its
separate caller. The selected-table display-stage contract binds that callback
to a real native five-plane page using the run075 live values (bound 144,
vertical 89, horizontal 0), produces two direct line submissions, and verifies
the observed first-lower-plane `$80` write at page byte `$28`. This proves the
selected-list path produces page geometry; it is still unscheduled until the
parent owns the live matrix/workspace and source gate cadence.

The ordinary run075 prepared-page window now has an exact `$C246A0` polygon
clip witness. At global frame 382 it reads 13 zero-header triples from
`$C4BF94`: the entry `D3=2` is overwritten at `$C246EA` by the `$C4BF90`
header, which `$C2AA9C` clears before the map pass. It then runs both clip-cache leaves and all four final closures, then leaves
14 source-ordered triples at `$C4B990` before `$C24CFE` projects/submits them.
`polygon_clip_pipeline.{c,h}` ports that four-plane (`y=z`, `-y=z`, `x=z`,
`-x=z`) composition with source word rounding; its contract matches every
captured output tuple, including the closing-edge order. It is the missing
native producer immediately before the established projection/submission
tail, but still needs a typed `$C246A0` display-stage adapter to publish the
result to the selected renderer page and the parent cadence to schedule it.

`polygon_display_pipeline.{c,h}` now closes the bounded producer through
`$C24CFE -> $C2FF48`: it clips caller triples, projects the positive-depth
result into source screen-pair order, and invokes the distinct DMA-enabled
tuple-list wrapper once. The run075 frame-382 contract confirms all 14
`$C4B390` pairs (including the closing `(0,179)` pair) and the one `$8400`
DMA submission. `map_packet_polygon_display.{c,h}` is now that typed
`$C2AFE2 -> $C246A0` callback: it copies the just-produced map records and
passes the caller-owned `$C4BF90` workspace-header shift to the existing
submission object. The detail gate's shift has already been applied by the map
transform and must not be passed again; the frame-382 map pass proves header
zero. No shift, page, capture, or scheduler is retained by the adapter. Normal
cadence, map-record ownership, and the selected renderer-page binding remain
unresolved.

The map-polygon adapter also has a five-plane page contract: the same raw
zero-header run075 witness binds the source renderer scalars (bound 144,
vertical 89, horizontal 0) to `FA18FlightRendererPage` and
`FA18ProjectionPageBlitter`. It produces a DMA submission, page-blitter work,
and nonzero lower-plane bytes. This establishes actual native page geometry at
the adapter boundary; it is not a scheduler or a captured page import.

`map_packet_static_data.{c,h}` now binds the original immutable inputs required
by the existing map-pass runner: Hunk 28 (`$C29F00-$C2B3B4`) supplies selector
pairs and control streams, while Hunk 68 (`$C42CA8-$C444F8`) supplies the
packet-directory and pair payload. Its address resolvers reject out-of-range
requests and retain the source runtime bases; no replay memory is imported.
The remaining normal-runtime owner is the live parent record/matrix state that
must populate the pass input and bind the selected page submission.

`map_packet_original_pass.{c,h}` now composes the original-Hunk resolver with
the complete `$C2AB34-$C2AFF9` pass runner. It rebases the relative-directory
pointer to the source-selected `$C42CA8`/`$C42E6C` base before invoking the
existing selector, then leaves matrix, detail, coordinate, and page submission
state caller-owned. Its integration contract reaches the display callback via a
wide stream, selector pair, directory entry, and packet entirely from bounded
source-shaped data.

`map_packet_depth_stage.{c,h}` now ports the local `$C2AA9C-$C2AB33` parent
prefix. It consumes the already ported `$C1C636` full projection depth,
initializes the four renderer words, applies the source optional unsigned scale,
and selects normal/wide map passes. The frame-382 trace verifies
`$C45A78=-125`, nonzero `$C457DD`, and resulting metric `125`: normal is
skipped and wide is called. This supplies the exact metric owner immediately
above the original map-pass composition, while the live record/matrix/page
owner remains unscheduled.

`map_packet_parent_pass.{c,h}` now composes that depth owner with the original
map pass in source order: normal only above `$3F8`, then wide always. Its
frame-382-shaped contract uses the depth packet `-125`, the caller-owned low
filter row, and original `$C2ACA8` control stream/payload path to reach the
display callback. This is the full local `$C2AA9C` map producer; scheduling,
live record/matrix construction, and selected five-plane page ownership remain
the next parent boundary.

`default_scene_map_pass.{c,h}` now composes the source-owned active-record
state directly above that local producer. It runs the existing default
`$C2DB18 -> $C1C54E/$C1C636` route, transfers record `+$14/+18/+1C` to the
ordinary `$C2AB8C` map-coordinate input, transfers `$C45BD8` to the map
transform, and uses the published full packet depth for `$C2AA9C`. Its
frame-382-shaped contract reaches the wide display callback with depth `-125`
and metric `125`. The still-unrecovered selector-gate alternate components,
detail/filter state, page submission, and scheduler remain explicit caller
state; this bridge is intentionally not attached to `game.c`.

`context_selector_pack.{c,h}` now ports `$C1C6BC-$C1C7F5`, the immediate
writer for the map pass's `$C45850` table selector and companion mutable pack.
Its clear-context route copies selector fields from the active record; its
alternate route invokes the still-caller-owned `$C29042` origin update and
derives coarse/fine selectors from `$C45C3E/$C45C46`. The source mode/depth
suppression and accumulated change byte are retained. This makes the map
table-selector state a source-backed producer, but active-origin production,
detail state, page ownership, and scheduling are still outstanding.

The default-scene/map bridge now consumes that selector pack directly: it
uses `$C45785` to choose the active-root or `$C45C3E/$C45C42/$C45C46`
coordinate triple and transfers `$C45850` to the map pass's low-filter/column
selector. Thus the parent map pass no longer has an opaque selector-template
gap; detail fields, native page ownership, and outer scheduling remain open.

`context_refresh_packet.{c,h}` now ports the structural
`$C1C860-$C1CA2D` packet immediately before the map conditional in the parent
middle. It refreshes active/origin selector terms, consumes the source request
bits in order, invokes the explicit selector/stage callbacks, and preserves
the guarded `$C2F66E` submission setup. Those callbacks still own their wider
state, but the eventual parent scheduler now has the source packet that
prepares its context instead of a placeholder refresh slot.

`terrain_template_static_data.{c,h}` now binds the immutable source inputs
for the next `$C1D330-$C1D51D` workspace producer: Hunk 65 at `$C412EC` is
the band control stream and Hunk 66 at `$C42390` is the static group/template
directory. The mutable bit gate, special pairs, placement cache, and workspace
bands remain caller-owned, so this adds original data without importing a
replay snapshot.

The immutable binding now also exposes the exact `$C411F0` control translate,
`$C1D764` delta-pair, and `$C1D8B6` special-pair tables. The new
`terrain_template_workspace_pass.{c,h}` composes those values through the
existing `$C1D330-$C1D51D` band/stream implementation using caller-owned live
terms, gates, append records, and workspace; it remains unscheduled pending
the recovered parent cadence.

`analysis/routines/c1d10c_template_cursor_resolution.md` now records the
source-owned `$C1D10C-$C1D32E` cursor selection: three control roots,
directory/gate packs, live term packs, and the shared static-table offset
calculation. The next implementation must resolve this cursor rather than
using the traced `$C412EC` subcursor as a universal template stream.

`terrain_template_cursor_resolver.{c,h}` now implements that `$C1D10C-$C1D32E`
selection boundary: it resolves the source control cursor, selected directory,
translate block, gate, and live row/group terms from caller-owned runtime
flags. It is available to compose with `terrain_template_workspace_pass`, but
is not attached to the native frame loop before the flight parent cadence is
ported.

`terrain_template_selector_pass.{c,h}` now composes the cursor resolver with
the workspace producer in the same source order. This is the direct native
callback payload for the parent pipeline's `context_refresh` slot; live
placement resolution and the parent scheduler remain the next integration.

## Current handoff — outer cockpit path

The native comparison is exact for frames 200--391 and first differs at frame
392: 361 of 64,000 pixels, in `x=7..318`, `y=101..199`.  Visual inspection of
the oracle/native diff shows dim cockpit/HUD transition pixels (horizon arc,
side outlines, and central indicators), rather than terrain geometry or a
global fade.  The cockpit visible at this point is therefore a meaningful
integration target; do not treat the black geometry alone as a rendering
verdict.

The recovered source sequence is:

```
outer update -> $C2F558 page selection -> parent marker 60 -> $C0D730
             -> $C2FD8C four-plane blits -> selected-page presentation
```

Recent commits implement bounded pieces of that route. `display_buffer_gate`
ports `$C0D730-$C0D749`: bit 13 of `$C458D2` selects the active `$C2FD8C`
callback or the still-untraced `$C0DA38` alternate. `active_plane_packet`
ports `$C2FD8C-$C2FEDA`, building and executing its four self-blits from
caller-provided active plane bases, size, and plane-3 select state. Its
contract includes an actual synthetic Chip-RAM blit. `outer_update_loop` was
also corrected to source order in commit `4dcd0bb6`:

```
$C2F558 -> OwnBlitter -> parent update -> DisownBlitter -> wait display -> child
```

The preceding ordering was wrong: the parent update must run while the
blitter is owned. The individual display-gate, active-packet, and outer-loop
contracts pass. The full suite passed 173/173 immediately before these final
small additions; rerun the standard suite after integrating further work.

Do not bind captured pages as normal game assets. The frame-392 trace has
`$C456B6=$C4567E`, whose mutable attract-family lanes are `$012BC0`,
`$014B00`, `$016A40`, `$018980` (8000 bytes each), while later DMA fetches
the normal five-plane family at `$04DB30`, `$04FA70`, `$0519B0`, `$0538F0`,
`$055830`. This is a live staging/timeline boundary, not immutable disk data.
`five_plane_chip_binding` and `active_plane_packet` are available to operate
on a genuine caller-owned page once its initialization and ownership are
recovered.

`game.c` still has no live `FA18OuterUpdateLoop`, flight-page state, or parent
pipeline owner. It must not be wired with no-op stages or replay/capture state.
The next context should first recover the real active page pair/Chip-memory
initialization and cadence, then instantiate the outer loop with source-owned
stages: connect `$C2F558` to `flight_page_handoff`, route the selected page's
plane offsets through `$C0D730` to `active_plane_packet`, and only then attach
the terrain selector pass to `context_refresh` and continue with placement and
display composition. Require a normal native frame run to show the cockpit
transition at frame 392 before pursuing the later terrain geometry.

The latest implementation commits are `6000d323` (active packet execution)
and `4dcd0bb6` (outer ownership ordering). The worktree should otherwise
remain clean except for the user-owned untracked `.vscode/` directory.

`f281806a` ports `$C2F582-$C2F5BF` as `renderer_pointer_set_clear.{c,h}`.
The parent-tail modulo-32 route can now clear both source pointer quartets,
forty bytes at each target, through caller-owned Chip offsets. It validates all
eight ranges before storing, so a bad page binding cannot leave a partially
cleared native renderer state. This primitive is deliberately unscheduled:
the parent-tail state and live page-pair initialization are still required
before it can contribute to normal replay. Full validation after this commit
passes 176/176 contracts and 192 exact native frames (200--391); frame 392
remains the first mismatch, at 361 pixels.

The five static slices of the full parent update are now composed by
`parent_update_pipeline.{c,h}` through `$C0EFD4-$C0F3C3`: prefix, middle,
flight update, postflight setup, activity stages, and tail. Commits
`7946482b`, `9e927ba0`, `10dfb13a`, `cc9534ff`, `6a85b5be`, and `ca71a26b`
add those boundaries, their source gates, the complete composition, its
contract linkage, and fail-loud dependency validation. The parent carries its
single caller-owned local-frame word through the postflight/activity/tail
gates; it does not infer a presentation-frame cadence. Full validation after
the composition passes 179/179 contracts and retains 192 exact frames.

The next required source owner is still the initialization of the two native
view/page identities corresponding to `$C182BA/$C182C2` and their five-plane
buffers. Frame-392 traces prove the child consumes and publishes those tables,
but do not identify their initialization writer; do not substitute the
observed `$C074D8/$C07F00` addresses as native identities.

This writer is now bounded.  In `captures/cold_boot_menu_init/`, the table is
zero initially; `$C160BE/$C160C8` publish slot zero at replay frame 5,926 and
`$C16110/$C1611A` publish slot one at frame 7,699.  Each slot copies the live
View `$C1821C` and display-instruction `$C18232` fields only after the paired
`$C53F18`/`$C53F04` graphics-library calls.  The preceding calls respectively
write `$C18232` then `$C1821C` for slot zero.  See
`analysis/routines/c16084_view_pair_initialization.md`.  A native owner may
now model this as a constructor-owned native pair followed by an explicit
slot publication; it must still recover the constructor's page/Chip-buffer
relationship and original scheduling before attaching it to `game.c`.
`view_pair_initializer.{c,h}` now ports that bounded publication sequence and
shares its native pair type with `flight_page_handoff`; it deliberately does
not create identities in `game.c` before the original transition owner is
recovered.

The generic pair initializer is callback-scheduled: its slot-zero return at
cold-boot frame 5,926 is `$C0E344`, whose prefix consumes `$C1AADC`. That
slot is the loaded `pix/splsh` object, so this is startup display-resource
setup—not proof of the run075 flight-page constructor. The callback table
holds the initializer at `$C55058`; this is not a direct `$C1612C` or
`$C15D80` call. See `analysis/routines/c0e344_view_pair_continuation.md`.
Do not attach this callback to the flight handoff; recover the separate
flight page/view initialization and cadence first.

The actual pending five-plane render page is now bounded at
`$C15DB4-$C1601E`: it allocates five `$1F40` planes into `$C18272` and
`$C456BE`, duplicates the lower four sources, then calls `$C2F4DE` to build
the renderer tables. `renderer_page_setup.{c,h}` ports that private native
page and exact table layout; its contract passes. It is not yet connected to
the source callback/timeline that selects and presents it. See
`analysis/routines/c15db4_renderer_page_setup.md`.
`FA18Game` now owns this setup at native initialization, matching that the
source allocation predates restored run075 frame 200. Full validation after
this integration passes 181/181 contracts and retains the 192 exact-frame
prefix; the first mismatch remains frame 392 with the same 361 pixels.

Topology correction: `$C15FE8-$C16018` configures the second display record
from the **lower four lanes of that same newly allocated five-plane page**.
This does not prove two independent render-page allocations. The existing
`flight_page_handoff` two-page object is generic, unscheduled contract support
only; do not use it as the normal source initializer. Compose runtime display
ownership from `renderer_page_setup` and the recovered ViewPort/Copper
configuration, then separately account for the older `$012BC0` family.

## Standard validation after each stage

Use the existing build directory/configuration and run serially:

```
cmake --build build\port-native --config Release
ctest --test-dir build\port-native -C Release --output-on-failure
python scripts/check_native_build.py
python scripts/native_frame_count.py --to 392 --timeout 180
```

At this handoff, expected frame-check result is `NATIVE_FRAME_COUNT=192`, with
the first mismatch at frame 392 as described above.  A new exact result beyond
that is welcome only if it arises from the faithful runtime pipeline.

`ilbm_page_loader.{c,h}` now ports the bounded ADF decode side of `$C0E078`
for `pix/splsh`: its strict 320x200 five-plane ByteRun1/CMAP boundary builds a
separate native startup page from the original disk at `game.c` initialization.
This is the source-backed `$C1AADC` family, not a capture and not a normal
flight presentation route. Its View/Copper selector remains unresolved, so
the page is deliberately unpresented; normal run075 output remains the
authoritative validation path. See `analysis/routines/c0e078_splsh_ilbm_loader.md`.

`scene_stream_entry.{c,h}` now composes the `$C1EE14-$C1EF15` stream-entry
prefix: it derives the source threshold, walks the existing `$C1EE58` selector,
retains all observed early return gates, and publishes the selected descriptor
and post-selection cursor for `$C1F6F8`. The caller still owns the mutable
placement/stream banks and the later walker, transforms, renderer submission,
and scheduler; this is not attached to `game.c`. See
`analysis/routines/c1ee14_stream_entry.md`.

`record_walker_prefix.{c,h}` now ports the ordinary `$C1F6F8-$C1F79F`
control branch that consumes the cursor from `$C1EF10`: it initializes the
source masks, expands original three-word records from the caller-owned
`$C48390` table, calls typed triple/hex/other handlers, and preserves the two
relative jump forms and `$FFFF` return. Nonterminal negative controls and the
source-cleared local-count path remain explicit `$C1F7A0`/`$C1F844` caller
boundaries.
This is still not attached to `game.c`; it enables a source-owned bridge from
the recovered stream entry to the existing geometry callbacks. See
`analysis/routines/c1f6f8_record_walker_prefix.md`.

`record_stream_selector.{c,h}` now ports the negative-control selector
`$C1F7A0-$C1F837`: it preserves the source's `$FFFF`, `$1000`, `$2000`,
`$4000`, and `$8000` branches, publishes original-address A1/A2 stream
positions, and leaves `$C1F906` dispatch plus `$C1F844` post-stream work as
explicit caller routes. This is not attached to `game.c`. See
`analysis/routines/c1f7a0_record_stream_selector.md`.

`record_table_dispatch.{c,h}` now ports `$C1F910-$C1F94D`, the A2 indirect
selector entry following the stream selector. It retains the source signed
word branches, `$4000` count, `$3FFF` target index, error report, returned
status accumulation, and each enclosing-walker exit as explicit routes.
Target resolution stays caller-owned and this is not attached to `game.c`.
See `analysis/routines/c1f910_record_table_dispatch.md`.

## Frame-392 dynamic call graph

`build/run075_frame392_parent_callgraph_trace/` is a fresh Engine9000 trace
from the global-frame-392 `$C0F090` parent entry through its verified
`$C0F124` return: 31,800 instructions. `scripts/generate_trace_call_graph.py`
uses only observed `JSR`/`BSR` transitions that push a same-stack return
address, producing `analysis/frame392_parent_dynamic_call_graph.{json,dot,md}`.
It records 174 verified game calls over 64 distinct edges and 49 dynamic
callees. 26 callees have an address-linked native `port/` reference; **23 do
not** and are the immediate frame-392 porting backlog. This is a conservative
address-coverage ledger, not a claim that the 26 referenced routines are fully
semantically complete. The unrepresented set includes the high-frequency
`$C2EE4A` projection preparation (16 calls), `$C2005C` face dispatch (15),
`$C1FB82` predicate (7), `$C1F99A` record transform (2), and the
`$C2F0C6/$C2F0F4/$C2F156` segment-clip continuations. Do not treat the graph
as a static whole-program graph or schedule it directly in `game.c`.

## Native visual-progress gate

Run `python scripts/render_native_visual.py --frame 402 --output
build/native_visual/frame402_normal.ppm` after every renderer-path change.
It invokes the normal native replay from the original ADF, recorded input, and
replay-owned timing stream only, emitting a labelled PPM and JSON summary with
nonblack-pixel count and bounds. `--render-active-scene` is separately labelled
as a capture-free diagnostic; it must never be described as normal replay.
The initial 2026-09-28 result is zero nonblack pixels for normal frame 402.
The original frame 402 has 36,650 nonblack pixels over `x=0..319, y=1..199`,
making it the short-term visual gate rather than the sparse 361-pixel reveal
at frame 392. A meaningful rendering stage must change the normal image or
prove a source-defined earlier/later presentation boundary; structural
contracts alone are no longer sufficient visual progress.

The visual tool also writes a viewable PNG. For a strictly labelled
capture-backed native-renderer checkpoint, run the frame-382 `$C279D0`
pre-call Slow/Chip inputs first as `--bootstrap-render-fixture CHIP`, then
run `--bootstrap-c279-render-fixture SLOW CHIP --delta-against BASELINE.png`.
The resulting black-background delta is only pixels changed by the native
`$C279D0-$C27D0F` code. It currently has 12 nonblack pixels at
`x=52..297, y=101..112`; it is not normal replay and cannot satisfy the
frame-402 gate by itself.
