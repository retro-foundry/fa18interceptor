# C port handoff

Updated 2026-10-02. This is the current work state. Older notes remain in git
history (the preceding handoff is in commit 0a19d5be); ignored gate logs may
also remain under build/recomp/.
PORT.md describes the architecture and source conventions.

## Objective and order

Recreate readable C for the whole game, proven against the original source and
sealed native recordings. Work in related batches. The order is Stage D game C,
Stage F native backend, then only the Stage E Kickstart services still needed.
The user explicitly deferred OS work and asked for larger routine batches.
After the C279D0 batch, the latest instruction is to return to game timing
parity. Selector, placement, post-input, context-refresh and bootstrap milestones
are complete at 432/624.
The latest user instruction defers the minor Copper-fade difference: keep its frame-313
HUD/countdown evidence for later and continue complete readable game batches.
Do not make fade timing the next work item or weaken the normal parity gates.

## Verified baseline

- 432 of 624 translated game entries are registered in port/game/glue/ports.c.
  The latest full gate for that registered set matched 636,016 completed shadow
  calls and 930,150 sandbox calls across three native recordings, with zero
  mismatches and identical poison frames. Ported parents absorb some formerly
  counted child calls, so the aggregate call totals need not rise monotonically.
  build/recomp/ports_report_*.json describe this 432-entry baseline. GNU and
  MSVC builds pass. In addition to active planes and the earlier audio batch,
  24 registered glyph, input, page, notification, command/audio, buffer,
  polygon, face, postflight and followup entries now use source-timed steps.
  These isolated bridges match fresh source OFF output on all 36,236 frames. The
  earlier combined instruction oracle matched 9,627 instructions and 308,064 cases
  with DMA contention enabled. See
  analysis/routines/native_c_face_predicate_timing_batch.md for that earlier
  timing set; analysis/routines/native_c_registered_timing_batch.md
  retains the earlier 24-entry evidence.
  The new four-entry map/region batch is independently exact across all
  36,236 live frames. Its 735 instructions also match 23,520 fixtures with
  DMA bus contention enabled. See analysis/routines/native_c_map_region_activation.md.
  C279D0 is also newly registered: its independent live output matches all
  36,236 frames, and its 271 instructions pass 8,672 DMA-contention fixtures.
  Readable whole-call C separately matches 3,258 shadow and 1,967 sandbox calls,
  plus 4,096 structural cases including partial writes and edge clamps. See
  analysis/routines/native_c_grid_projection_activation.md.
  Eleven additional renderer, clipping, record-view and transform entries now
  use source timing and independently match all 36,236 live frames. Their
  1,094 instructions pass 35,008 DMA-contention fixtures. The broader DMA
  oracle also found and removed an extra stack read in C17B08's older timing
  bridge. See analysis/routines/native_c_renderer_timing_batch.md.
  A further 22 sound-start/random and screen-frame/corner entries now use
  source timing and match all 36,236 live frames together. Their 1,026 source
  instructions pass 32,832 DMA fixtures. The sound correction moves the
  combined first difference from frame 297 to frame 416. Isolated ON final
  RAM also matches each recording's sealed SHA-256. See
  analysis/routines/native_c_sound_frame_timing_batch.md.
  Fifteen more polygon, line, projected-segment and cell-template entries now
  use source timing and match every live frame and sealed final RAM together.
  Their 1,062 instructions pass 33,984 DMA fixtures. ALL still first
  differs at frame 416 by 361 pixels.
  See analysis/routines/native_c_drawing_cell_timing_batch.md.
  Twenty additional startup, number-field, orientation and direction-tracking
  entries now match every live frame and sealed final RAM in isolation.
  Their 1,293 instructions pass 41,376 DMA fixtures. Those ALL traces
  matched through terrain-refresh
  entry and identify display-list sorting and condition updates as the next
  timing targets. See analysis/routines/native_c_startup_timing_batch.md.
  A further 41 terrain-sort, condition, pixel-lane and interrupt-counter
  entries now match all 36,236 isolated live frames and sealed final RAM.
  Their 611 source instructions pass 19,552 DMA fixtures; 144 registered
  entries now have timing steps. Fresh traces match the entire startup CSV
  and first terrain return, resolving sorting/condition debt. The interrupt
  counter correction removes the frame-7 loop drift. That batch's outer-loop
  rows matched through frame 295 and located the C12098 update gap. See
  analysis/routines/native_c_terrain_pixel_timing_batch.md.
  Nineteen further view-control, record-rate and matrix-pipeline entries now
  match every isolated live frame and sealed final RAM. Their 1,107 source
  instructions pass 35,424 DMA fixtures; 163 registered entries now have
  timing steps. C12098's return matched source; that batch located C1C63E's
  first flight-parent gap after fixed-charge selection helper C230B0. See
  analysis/routines/native_c_view_matrix_timing_batch.md.
  Twelve further selection, fire-initializer, target/projection, attitude and
  cockpit/list helpers match all 36,236 isolated live frames and sealed RAM.
  Their 746 source instructions pass 23,872 DMA fixtures; 175 registered
  entries now have timing steps. The complete first flight parent now matches.
  Enclosing update rows match through the map-stage return in frame 310;
  C0F090 after C0DAEE is the next gap, +386 cycles. See
  analysis/routines/native_c_flight_update_timing_batch.md.
  Seven more matrix-marker, projection, circle and fault-return entries match
  all 36,236 isolated frames and sealed RAM. Their 344 instructions pass
  11,008 DMA fixtures; 182 registered entries now have timing steps. The
  marker return now matches; the enclosing first gap is after C1CB26 in frame
  311. Nested traces locate C1FB82 after the already stepped C2005C dispatch.
  See analysis/routines/native_c_marker_projection_timing_batch.md.
  Five further face/component predicates and scan line-style entries match
  all 36,236 isolated frames and sealed RAM. Their 125 instructions pass
  4,000 DMA fixtures; 187 registered entries now have timing steps. The
  scene-stream/grid/first scan returns now match. The enclosing first gap
  is C0F132 after C11BFC in frame 311, +2,344 cycles and SR 0004/0000.
  See analysis/routines/native_c_face_predicate_timing_batch.md.
  Seven scene initializer/root-setup entries now preserve source timing and
  match every isolated live frame and sealed final RAM. Their 311 instructions
  match 9,952 DMA fixtures; there are 197 timing-step entries. The initializer
  span now matches 16,280 source cycles and removes one of two delayed fade
  frames. Independent group coverage is 10,007 instructions / 320,224 cases;
  this is not a fresh full combined oracle run. ALL still first differs at
  416/361. See analysis/routines/native_c_scene_transition_timing.md.
  The complete C1E540 placement-ordering parent is now registered, with its
  full CPU adapter and six existing children newly source-timed. Coverage is
  422/624 and there are 204 timing-step entries. Its readable adapter matches
  8,192 structural calls and 4,075 shadow / 4,090 sandbox isolated whole-call
  comparisons with original liveness; 15 shadow calls remain incomplete.
  The nine-entry live group matches all 36,236 frames and sealed RAM. Local
  timing matches 649 instructions / 20,768 DMA cases; a fresh full combined
  oracle now passes 10,622 instructions / 339,904 cases. Full registered
  parent counts are 4,075 shadow / 5,796 sandbox, zero mismatches or hardware
  classifications. ALL remains 416/361. See
  analysis/routines/native_c_placement_order_domain.md.
  The complete C1D10C template-placement parent is now registered as entry
  423, with its normal CPU adapter and C1D722 source timing. At that checkpoint
  there were 206 timing-step entries. All 8,192 structural adapter cases and
  492 shadow / 1,046 sandbox isolated whole-call comparisons pass with original liveness.
  The seven-entry isolated group matches all 36,236 live frames and sealed
  RAM. Local timing passes 681 instructions / 21,792 DMA cases; the fresh
  combined oracle passes 11,303 / 361,696. ALL remains 416/361. See
  analysis/routines/native_c_template_placements_domain.md.
  The complete C1CB14/C1CB26 scene-placement pair is now registered, including
  the shared distance refresh, countdown/skip gate and descriptor-result tail.
  There are 208 timing-step entries. All 8,192 structural CPU/RAM cases and
  5,601 shadow / 11,446 sandbox normal isolated whole-call comparisons pass;
  2,565 shadow and 187 sandbox calls are incomplete, zero hardware/mismatches.
  Local source timing passes 99 instructions / 3,168 DMA cases; a fresh combined
  oracle passes 11,402 / 364,864. The 600-frame pair probe is exact; ALL remains
  416/361. The isolated pair also matches all 36,236 live frames and sealed
  final RAM. See analysis/routines/native_c_scene_placements.md and
  analysis/figures/native_scene_placements_checkpoint.json.
  The complete C1CCBC follow-up placement parent and C1D0A4 workspace-position
  entry are now registered, bringing the total to 427 and timed entries to 212.
  The parent matches 8,192 structural CPU/RAM cases and 288 shadow / 5,131
  sandbox normal whole-call comparisons; 3,790 shadow and 699 sandbox calls
  remain incomplete. Both position variants separately match 16,384 complete
  all-register/full-SR/all-RAM cases, without exclusions; C1D0A4 is cold in
  recordings. The parent, both position entries and C25876 match every isolated
  live frame and sealed RAM. Local timing passes 333 / 10,656 DMA cases and the
  fresh combined oracle passes 11,735 / 375,520, now restoring the sealed
  machine before every case so synthetic writes cannot contaminate later
  source bytes. ALL remains 416/361. See
  analysis/routines/native_c_followup_placements.md and
  analysis/figures/native_followup_placements_checkpoint.json.
  The complete C0F5F8 post-input parent is now registered: 428/624, with
  213 timing-step entries. All 16,384 complete original CPU/RAM cases match
  every register, full SR and all RAM without exclusions. Normal isolated
  readable C passes 15,929 shadow / 16,001 sandbox completed comparisons,
  retaining four hardware and 79 incomplete shadow calls and ten incomplete
  sandbox calls. Local timing passes 118 / 3,776 DMA cases; the fresh combined
  oracle passes 11,853 / 379,296. All 36,236 isolated live frames and sealed
  RAM match. ALL remains 416/361. GNU/MSVC pass; build/ is 0.305 GiB.
  See analysis/routines/native_c_post_input_tick.md and
  analysis/figures/native_post_input_tick_checkpoint.json.
  The complete C1C860 context-refresh, C08F26 bootstrap and C0F920/C0F992
  callback wrappers are now registered: 432/624, with 223 timing-step entries.
  All 32,768 complete CPU/RAM cases match every register, full SR and all RAM
  without exclusions. Normal readable C passes 3,743 shadow / 5,815 sandbox
  comparisons, including three independent bootstrap body comparisons.
  Incomplete calls stay separate. The proof tool now automatically isolates
  a called child when its parent absorbs the batch comparison; it still
  requires a completed original comparison for every entry. The ten-entry
  timing group passes 272 / 8,704 DMA cases and all 36,236 live frames/sealed
  RAM. Fresh combined timing passes 12,125 / 388,000. ALL remains 416/361.
  GNU/MSVC and the corrected typed context contract pass; build/ is 0.317 GiB.
  See analysis/routines/native_c_scene_bootstrap.md and
  analysis/figures/native_scene_bootstrap_checkpoint.json.
- The current recordings are captures/native/demo01,
  captures/native/qual_carrier_success, and
  captures/native/qual_fail_crashes. Each has state.bin, input.fa18in, and
  run.json with a sealed final RAM hash. Archived captures/uae runs, including
  run075, are historical evidence and are not an acceptance gate.
  Current RGB timing checks generate fresh source OFF and recreated-C ON
  streams with the same executable, state, input and machine model. The
  starting snapshot originated in UAE, but is shared by both runs. Their
  difference tests the replacement paths; agreement cannot detect a machine
  error shared by both runs or establish independent Amiga timing parity.
- The headless GNU and MSVC Release builds pass. The typed port's eight
  affected map/detail contract tests passed with the map source changes.
- Only .vscode/ is untracked; it belongs to the user. Leave it alone.
- C2AA9C, C2AB34, C2AB5A and C2B05A are now registered with resumable source
  timing. The map parent reuses port/map_packet_depth_stage.c; its normal/wide
  children reuse the established map packet core. Their readable whole-call
  glue is checked separately from the instruction steps by
  python tools/recomp/check_map_region_glue.py. This isolated registry variant
  matched 281 shadow and 3,988 sandbox calls without changing the normal runner.
- The region probe additionally matches original registers and RAM on 4,096
  direct structural oracle cases, including signed coordinates, sloped edges,
  endpoint exclusions and randomized high register halves. The oracle found
  and corrected the old C's near-endpoint exit: $C2B1E4/$C2B1FE abandon the
  directory walk, not just one segment. See
  analysis/routines/c2b05a_record_region_probe.md.
- The registered all-native path is not yet frame-faithful. Complete
  source-timed bridges now give exact isolated output for the
  C31226/C3129A/C31312 postflight group and C305AA polygon edge across all three
  sealed recordings. C0FA04 is also source-timed and exact across the three
  recordings. The complete registered ON demo now matches through frame 415
  and first differs at one-based frame 416 by 361 pixels. The preceding
  renderer batch was at frame 297 by 5,440 pixels. Source timing for the
  sound-start/random family removes that early frame difference; the new
  screen-frame/corner family is also exact in isolation through all recordings.
  No fixed mean fees were substituted. The latest 15-entry drawing/cell batch
  resolves the independent C1D3F4/C2FF48/C2EE4A differences at frames 415/416,
  matching all 36,236 frames and sealed final RAM in isolation. ALL retains
  the same frame-416 difference. The remaining bounded ranking finds
  C3201A/C31F4C/C20A40 at frame 424 by 34,144, C26EBE at frame 441 by
  12,238 and C0D04C/C20D68 at frame 484 by 17. The latest 41-entry batch
  removes the sorting/condition debt and the interrupt counter's frame-7
  drift. Fresh startup CSVs and the first terrain-refresh return match source.
  C1612C's 100-frame CSV and the eight-frame ROM CSV are also identical.
  Outer-loop rows match through frame 295. The first flight parent and view,
  projection, attitude, terrain, cockpit, map and matrix-marker returns now
  match. The enclosing update's first 31,275 instruction rows match every
  field, including the scene-stream, grid and first record-scan returns.
  Its first difference is C0F132 after C11BFC in frame 311, +2,344 cycles
  (source/ON 44,324,676/44,327,020), with SR 0004/0000. The face predicate
  bridges removed the earlier +2,024-cycle stream gap; source timing for
  C2F490 removed the subsequent -16-cycle record-scan gap. C11BFC is a
  registered message update with a fixed charge and no child calls.
  These observations locate complete-call debt, not substitute charges or
  proof of which instruction causes the frame-416 pixels. See
  analysis/routines/native_c_face_predicate_timing_batch.md.
  Shadow/sandbox matches do not establish live ON fidelity for the whole
  registered set.
- Plane shadow now replays the live source's ordered DMACONR inputs on saved
  entry RAM. It independently checks native outputs and write sequences, and
  fails extra/reordered/missing reads. All five former counter failures now
  compare and pass. Report fields busy_input_calls/reads expose this proof
  input model. Two intentionally incorrect bridge copies are rejected by
  tools/recomp/check_shadow_busy_inputs.py. Full ON replay separately proves
  the actual timing. No mismatch or hardware classification was suppressed.

## Newly activated grid projection packet

C279D0 now implements the complete three-table packet in
port/game/grid_projection_packet.c, with CPU effects and resumable timing in
separate glue files. It preserves source-selected depth gates, sparse-matrix
projection, negative record kinds, partial triangle writes, coordinate clamps,
and both pixel helpers. All 36,236 isolated live frames are exact.

The structural oracle found a word-overflow distinction at C27BD6/C27C98:
BLE after ADD.W tests the signed unwrapped sum, whereas subsequent CMP.W
instructions compare wrapped depth. That case is retained in 4,096 original-
instruction fixtures, including 12 partial-write and 13 edge-clamp cases.
Custom writes are held in these structural fixtures; the separate live replay
proves actual drawing and event timing.

The generic tools/recomp/check_whole_call_glue.py ENTRY... now supplies
independent readable-C checks for future batches. It reuses cached objects and
an isolated registry, running the three recordings concurrently. The earlier
check_map_region_glue.py delegates to it with its four default entries.

## Previous map and region batch

The previous map/region live blockers are resolved. C2AB34/C2AB5A share the
source-timed packet bridge, C2AA9C supplies the depth-stage frame and pass
sequence, and C2B05A retains the source's interrupt boundaries and unusual
D2/D4-D6 restore. Domain observers remain mathematical values; CPU effects
stay in glue. No new gameplay or placeholder behavior was introduced.

The first map bridge passed CPU-only instruction fixtures yet accumulated
bus delays from extra reads on CLR memory instructions. A boundary trace found
the first difference at C2AD22 in frame 297. Removing those reads made the
600-frame map trace identical and the complete isolated live batch exact.
The instruction oracle's new --bus option checks memory-access timing under
DMA contention so this distinction is covered before expensive full replays.

## New timing evidence

- Optional FA18_BOUNDARY_TRACE=PATH and FA18_BOUNDARY_RANGE=LO-HI (hex,
  exclusive HI) produce instruction/flow-target and chipset-service CSV rows,
  with CPU registers, SR, cycles, next event, frame/line and blit/interrupt
  state. tools/recomp/summarize_boundary_trace.py reports costs and event sites.
- On the complete sealed demo, 17 original region calls cost 20,214-47,376
  live cycles including waits and interrupts. Fourteen enter $FC0D14 and
  span the next frame. The first starts in frame 19,443 and interrupts at
  $C2B19E after 2,560 cycles. The traced replay matched sealed final RAM.
- First-600-frame traces include 22 wide-map calls, 5 normal-map calls and
  22 active-plane calls. Wide-map calls starting at frames 299, 327, 349,
  366 and 384 each include 416 blits before returning.
- Commands, artifact hashes, source interrupt sites and interpretation limits
  are in analysis/routines/native_c_call_boundary_timing.md. Filtered rows
  omit chipset events inside children and interrupt handlers outside the
  selected address range; start/end frames still prove those crossings.

## Planning review, 2026-10-02

The user asked to step back and plan because progress was too slow. The seven
batches after f2d0bc6c added 119 timing bridges (68 -> 187), but readable
coverage stayed 419/624 and ALL retained its frame-416, 361-pixel difference.
Those bridges have useful independent proof, but their count is not progress
in readable game coverage or the combined first-difference milestone.

The integration cost comes from mixing two execution models. Mechanical
translation preserves original instruction/event boundaries through the CPU
model. Many readable whole-call replacements still use fixed charges, so
matching registers and writes does not imply matching interrupt, drawing or
frame timing. Shadow retains the source timeline; live ON exposes this gap.
The final deliverable is readable game C with a plain-C backend, not another
complete CPU bridge. These proof and delivery milestones must stay explicit.

Following the earliest CPU-cycle gap selected C11BFC in machine frame 311.
That gap is real; it has not been shown to cause the first visible difference.
Do not automatically source-time it, then chase the next unrelated return.

Fresh 500-frame demo probes with the unchanged 50f6d6a7 executable give:

| Enabled entries | First RGB difference |
| --- | --- |
| ALL 419 | Frame 416, 361 pixels |
| All 187 source-timed entries together | None through frame 500 |
| All 232 entries without timing steps together | Frame 416, 361 pixels |
| ALL except C11BFC | Frame 416, 361 pixels |
| C30918 alone | Frame 416, 361 pixels |
| All 187 timing entries plus C30918 | Frame 416, 361 pixels |
| C321D2 and C32260 together | Frame 415, 361 pixels; each alone starts at 424, 34,144 pixels |
| C30D34, C30EAA, C309B6 and C30F78 together | Frame 416, 361 pixels; neither tested two-entry half differs through 500 |
| ALL except the seven HUD entries in the preceding three rows | Frame 416, 361 pixels |

The subset/complement controls expose multiple and interacting failures.
Equal pixel counts do not prove equal changed pixels or a single root cause.
Removing one failing subset does not eliminate the full failure. These are
bounded diagnostic results, not full-recording proof or independent Amiga
machine parity. Logs/selectors/results are cached as build/recomp/planning_*;
RGB scratch streams are removed automatically. The first four probes took
4.29 seconds, eight partition controls 7.55 seconds, 35 minimization queries
(32 distinct candidate replays) 28.67 seconds, and six further controls
5.92 seconds. Minimization reuses one source stream and cached repeated
selectors. C30918's 35 original instructions
and sole C2F60A child give a smaller visual reproducer than the unrelated
256-instruction C11BFC message update.

At the planning review the candidate tool reported four ready leaves:
C1D10C and three already deferred Kickstart trampolines. This did not mean
only one game function remained feasible. Whole-family selection and explicit indirect-child
contracts are needed to move beyond the leaf-only ranking.

## Next work

Readable milestone completed: C1EBB0 and C1EC84 are now complete, registered
workspace-selector helpers, including their shared tails. Coverage is 421/624,
with 190 timing-step entries. Each readable whole-call adapter matches 16,384
original-instruction structural calls (all registers, full SR, PC and RAM);
their 34 instructions pass 1,088 DMA cases. Both helpers are cold in sealed
recordings: the temporary recorded whole-call tool correctly rejects zero
completed comparisons, and its rejection is retained. The independent
complete-call oracle supplies their proof; full isolated live runs are
nonregression checks, exact on all 36,236 frames and sealed RAM. The full
421-entry shadow/sandbox/sealed-RAM/poison gate passes with unchanged completed
call totals. GNU/MSVC pass. Independent instruction groups now total 9,696 /
310,272 cases; the combined oracle was not rerun for these local groups.
See analysis/routines/native_c_workspace_record_helpers.md. This was the
421-entry checkpoint; both complete selector parents are now registered below.

Latest visible checkpoint: the user's Copper-fade observation led to a
verified correction. C0FAA4 and six root-setup entries now preserve source
timing. The initializer span matches the original 16,280 cycles and no longer
crosses an extra vertical blank. Source/ALL terminal state writes now occur
at machine frames 436/437, improved from 436/438. The fade increments remain
three frames apart; first dim output is now RGB frame 417 instead of 418.
ALL still first differs at 416/361. The remaining delay is inherited between
the first two C0F5F8 countdown ticks: source 313/342, ALL 313/343. The actual
C0EFD4 update entered in frame 313 has the same 152 instruction PCs but
returns 70,498 cycles later in ALL, accumulating debt through HUD calls.
Restoring original execution for the 17 HUD entries listed in
analysis/routines/native_c_scene_transition_timing.md matches the fade reset
and completion and moves the first RGB difference to 425/36,650. Neither
tested half nor any of three individual entries suffices. This is a diagnostic
complement, not removal of registered C or a complete parity fix. Before and
after frame/fade JSONs are preserved separately under analysis/figures/.
This is the second timing-only batch since the review; return to the complete
selector parents next, retaining the causal HUD checkpoint for later parity.

Selector milestone completed: C1E540 is now fully registered as entry 422,
including selection, proximity, special heights, planes, indexed polygons/
triangles and scratch partitioning. Its semantic observer supplies the live
CPU outputs without repeating children/writes; CPU effects stay in glue.
The original C1C9AE mask is retained by the new --glue recorded proof and by
check_whole_call_glue.py. Both give 4,075 shadow / 4,090 sandbox matches;
15 shadow calls remain incomplete. All 8,192 structural adapter cases and
the captured failing demo call pass. C1E7F6's inherited zero shift and partial
register high words are preserved. The earlier memory-only proof remains
available but is no longer the integration evidence.

The complete parent and six formerly fixed-charge children now use source
timing, sharing the existing C1EBB0/C1EC84 tails. Nine entries match all
36,236 live frames and sealed RAM. Their 649 instructions match 20,768 DMA
fixtures; the fresh combined oracle matches 10,622 / 339,904. The full
422-entry gate passes 688,103 shadow / 1,111,316 sandbox comparisons, zero
mismatches, sealed RAM exact and poison identical. Aggregate calls changed
because the new complete parent absorbs child calls. GNU/MSVC pass. There
are 204 timing-step entries. A fresh bounded ALL probe is still 416/361;
coverage advanced, combined parity did not. Build/ is 0.228 GiB after cleanup.
See analysis/routines/native_c_placement_order_domain.md and
analysis/figures/native_placement_order_checkpoint.json. C1D10C is next for
423/624, as completed below. The user has since deferred the HUD/fade work.

C1D10C integration milestone is now complete in port/game/template_placements.c/h:
all selector packs, fourteen-band expansion, 24-byte cache emission, reverse
linked descriptor copies and final control-list publication. It is registered
as entry 423; there are 206 timing-step entries. All 8,192 complete adapter
fixtures match original caller-live CPU outputs and RAM outside the source's
bounded 128-byte private stack, including 2,743 cache-limit and 1,941 linked
copy cases. Full isolated normal whole-call checks pass 492 shadow / 1,046
sandbox calls, zero mismatches/hardware; 539 shadow calls are incomplete.
Production caller masks are retained: all sixteen registers and all eight
data-register high words, plus N/V/C at C1C924. The historical memory-only
proof remains separately reproducible and is not the integration claim.
The parent and C1D722 timing match 681 instructions / 21,792 DMA fixtures;
a fresh combined oracle matches 11,303 / 361,696. The full registered gate
passes 669,031 shadow / 1,075,296 sandbox calls, sealed RAM exact and poison
identical. GNU/MSVC pass. The seven-entry isolated live group matches all
36,236 frames and sealed final RAM across all three native recordings.
ALL still first differs at 416/361; coverage advanced, combined parity did
not. Build/ is 0.271 GiB after replay cleanup. Do not redo this selector
milestone. See
analysis/routines/native_c_template_placements_domain.md and
analysis/figures/native_template_placements_checkpoint.json. The earlier
native_template_placements_domain_checkpoint.json is historical domain proof.

The complete primary/alternate scene-placement pair C1CB14/C1CB26 is now
registered at 425/624, with 208 timing-step entries. It owns the entire
99-instruction shared traversal, including distance refresh, negative-result
countdown and descriptor return storage; consumers remain explicit children.
The earlier typed traversal omitted these full-parent gates and cannot be
substituted for this complete domain. Original source distinguishes C1ED3C's
bypass and the -128 countdown overflow branch; both are retained.
All 8,192 structural cases match every register/high word, PC, SR control and
RAM outside a bounded 160-byte child stack. Normal isolated whole-call proof
passes 5,601 shadow / 11,446 sandbox calls, zero mismatches/hardware; 2,565
shadow and 187 sandbox calls remain incomplete. Caller masks are unchanged.
Local timing passes 99 / 3,168 DMA cases; fresh ALL instruction proof passes
11,402 / 364,864. GNU/MSVC pass. The full 425-entry gate passes 653,694 shadow /
1,069,233 sandbox calls, sealed RAM exact and poison identical. Changed call
totals reflect parents absorbing their children. The pair matches all 36,236
isolated live frames and sealed final RAM; ALL remains 416/361. Build/ is
0.283 GiB after automatic replay cleanup. The full live proof and retained call
classifications are in analysis/figures/native_scene_placements_checkpoint.json.
See analysis/routines/native_c_scene_placements.md. Do not redo this pair.

C1CCBC's complete selected-record, alternate-placement and final relative-point
parent is now registered, together with the complete signed workspace-position
entry C1D0A4: 427/624, with 212 timing-step entries. C1D0B6's shared position
body and C25876's list-point child also use source timing. The domain retains
bit-6/bit-4 precedence, signed workspace indexing, overflow-aware depth and
countdown branches, selected distance's partial publication, and all sixteen
relative-point slots. Position additions and list-matrix sums wrap explicitly.
Normal CPU outputs stay in glue and children are not repeated for outputs.
All 8,192 complete parent fixtures and 16,384 direct position-child fixtures
pass. Parent proof excludes only 160 private child-stack bytes; position proof
retains full SR and all RAM, without any exclusion. C1D0A4 is cold in native
recordings, so its live run is nonregression only. Normal isolated parent
proof passes 288 shadow / 5,131 sandbox calls, zero mismatches/hardware, with
3,790 shadow and 699 sandbox calls incomplete. Caller masks remain unchanged.
Local timing passes 333 / 10,656 DMA cases; fresh combined proof passes
11,735 / 375,520 with a sealed machine reset before every case/opcode read.
The full 427-entry gate passes 633,759 shadow / 932,215 sandbox calls, sealed
RAM exact and poison identical. All four entries together match all 36,236
isolated live frames and sealed final RAM. GNU/MSVC pass; build/ is 0.294 GiB.
ALL still first differs at 416/361. See
analysis/routines/native_c_followup_placements.md and
analysis/figures/native_followup_placements_checkpoint.json. Do not redo these
complete parent/helper milestones or count their internal labels separately.

C0F5F8 is now complete and registered at 428/624, with 213 timing-step entries.
Its LINK -4 frame, optional D1 offset terms, D0 high word, phase MOVEQ resets,
callback CCR input, actual C0F806/C0F808 call/return and child-owned state
changes match independent original execution. The older C0F804 report address
was a transcription error and is corrected. The domain retains the source
hexadecimal $4650 limit (18,000) and partial-write order. Its 16,384 structural
cases match all registers, full SR and all RAM, including stack, with no
exclusions. Normal readable-C replay matches 31,930 completed comparisons;
the full gate matches 644,155 shadow / 944,089 sandbox calls with no mismatches,
all seals and poison passing. All 36,236 isolated live frames and final RAM
match. Fresh combined timing passes 11,853 / 379,296 DMA cases. GNU/MSVC pass;
build/ is 0.305 GiB; ALL remains 416/361. Detailed classifications are in
analysis/figures/native_post_input_tick_checkpoint.json. Do not redo this parent.

The complete context-refresh/bootstrap/callback batch now raises the registered
set to 432/624 and source-timed entries to 223. C1C860 preserves request-bit
ownership, actual record flagging, both selector shifts, source save-frame
aliasing, ordered sort/cache/condition children and guarded pixel submission.
C08F26 includes all 80 instructions and all ten children, including the
C1C40C/C1C63E/C1C860 calls after C090AE. Its old initialization-only slice
ending at C090AD was not a complete parent. Both callback wrappers retain
their complete source branches and child-owned effects.

All 8,192 complete cases per new entry (32,768 total) match every register,
PC, full SR and all RAM without exclusions. Normal recorded proof matches
3,743 shadow / 5,815 sandbox completed comparisons, including C08F26's
independent body check on every recording. Shadow retains 347 context, three
wrapper and three standalone-bootstrap incomplete calls; no hardware/mismatch
calls are accepted. The proof tool now isolates recorded children whose batch
comparisons are absorbed by a parent; cold/unproven entries still fail, and
raw reports retain all classifications. The fresh combined oracle passes
12,125 / 388,000 DMA cases. Full registered proof passes 636,016 shadow /
930,150 sandbox calls, all seals and poison. The ten-entry timing group
matches all 36,236 live frames and sealed RAM. GNU/MSVC pass; build/ is
0.317 GiB; ALL remains 416/361. The typed context groundwork was corrected
to record flagging, ASR.W #8 for alternate origins, later request rereads and
the captured condition route; its focused contract passes. See
analysis/routines/native_c_scene_bootstrap.md and
analysis/figures/native_scene_bootstrap_checkpoint.json. Do not redo this batch.

Next complete C22C80's record-update and C29042's active-origin parents,
then C1C63E's update-stage owner as a related batch (226 + 153 + 112 source
instructions). Reuse the stride/indexed-record and flight-update groundwork,
preserve explicit children, signed widths, D5 save/restore and all normal
caller outputs. C08F26 still calls original C1C63E; do not claim it as ported.
Later complete the whole C0EFD4 update sequence; C0F090/C0F132 are its internal
labels, not extra functions. No handwritten glue may invoke opcode handlers.
Keep the fade deferred.

Deferred Copper fade, by user instruction on 2026-10-02: the minor visible
difference may be ignored for current work and revisited later. Source/ALL
reset writes are machine frames 393/394 and terminal writes 436/437; each
fade step is three frames apart. The first visible RGB difference remains
416/361. The delay is inherited between countdown ticks (313/342 versus
313/343), with the demonstrated frame-313 HUD interaction preserved in
analysis/routines/native_c_scene_transition_timing.md and
analysis/figures/native_scene_countdown_checkpoint.json. This is an accepted
deferral, not a parity fix or a change to automated comparisons.

Gauge checkpoint completed after the planning review: $C30918 now has source
timing. Its 35 instructions pass 1,120 DMA fixtures; all 36,236 isolated live
frames and sealed RAM match. All 49 bounded parent trace rows match every
field, including skip and drawing returns. Before correction its isolated
frame-416 bytes exactly matched ALL's incorrect bytes; after correction the
isolated 600-frame probe is exact, while ALL still differs at frame 416 by
361 pixels. The full 419-entry gate remains 703,337 shadow/1,110,694 sandbox
matches, zero mismatches, sealed RAM exact and poison identical. There are
188 timing-step entries; independent group proofs total 9,662 instructions /
309,184 DMA cases (the combined oracle was not rerun for this local bridge).
See analysis/routines/native_c_gauge_timing_checkpoint.md for byte hashes and
call classifications. That was the first timing-only batch since the review. The
next readable implementation work is item 2, complete parent batches; retain
item 1's failing combined checkpoint rather than chasing another early gap.

1. Defer further Copper-fade timing work as the user requested. Preserve the
   corrected initializer and remaining frame-416 checkpoint. When revisited,
   parity work must follow the
   actual frame-313 update/countdown and the proven 17-entry HUD interaction,
   not return to an unrelated first cycle gap in frame 311. Compare
   C0FA0E/C0FA12/C0FA32 and terminal C1741A; original-code complements can
   reproduce the remaining cause but are not port fixes. ALL is still 416/361.
   Repeat bounded RGB and callback checkpoints after a source-backed candidate.
   Require a changed combined result before claiming improvement. Do not
   adjust average fees, counters or replay/frame conditions. C11BFC remains
   timing debt; the HUD list is not an automatic transcription queue.
2. The selector family C1D10C, C1E540, C1EBB0 and C1EC84, scene-placement
   pair C1CB14/C1CB26, follow-up parent C1CCBC/workspace helper C1D0A4,
   post-input parent C0F5F8 and context/bootstrap/callback batch are complete
   and registered at 432/624.
   Preserve normal caller masks and the independent
   CPU, memory and timing proofs; do not repeat the memory-only milestones
   or count internal labels as extra routines. Complete the related C22C80
   record-update/C29042 active-origin and C1C63E update-stage batch next;
   the leaf tool excludes indirect calls. Select using source-owned semantics
   and explicit child contracts, without reopening the deferred fade investigation.
   Reuse the existing terrain, template and placement groundwork.
   Preserve all shared spans, emission/cache paths and signed word behavior;
   do not count prefixes or internal labels as completed functions.
   Plan original-byte entry/exit checkpoints for each source-owned selector
   pack and cache path: input globals/frame locals -> template/placement and
   cache writes, preserving pointers, live outputs and partial-write order.
   Use recorded whole-call comparisons plus structural cases for paths not
   reached by recordings. Prove readable domain C independently of its timing
   bridge with check_whole_call_glue.py, then run the required gates below.
3. Keep parity and source coverage as separate measured outcomes. Do not
   spend another chain of timing-only batches without moving either ALL's
   first difference or readable coverage. Review after at most two such
   batches; if ALL still does not improve, return to complete readable parent
   batches while retaining the failing renderer checkpoint. This is a work
   selection limit, not permission to weaken proofs or declare parity done.
   Complete the record-update/active-origin/update-stage batch next; the leaf tool
   excludes its indirect calls. Preserve explicit child contracts.
4. Reduce repeated work: cache one source stream within each bounded probe
   round; run changed-group DMA fixtures and short live probes while editing.
   Run the full shadow/sandbox/sealed-RAM/poison gate and isolated full live
   replay once per coherent completed batch. Rerun the full combined
   instruction oracle when shared CPU/bus/math helpers or fixture setup change,
   or at an integration checkpoint; unchanged groups already have independent
   proof. Preserve all required comparisons and incomplete/cold classifications.
   Keep Ninja's shared objects, size caps, cleanup traps and disjoint recording
   parallelism; never relink executables still used by active checks.
5. Finish all game source before the plain-C native backend, then reassess
   only the remaining Kickstart services. Stage D -> F -> E remains the
   full objective. Fresh OFF/ON comparisons test our replacements on the
   same machine model; an independent UAE/Amiga timing check is a distinct
   proof and must not be inferred from them.

## Gate for a registered batch

- Read original instructions with python tools/recomp/port_info.py ADDRESS,
  then the corresponding analysis/routines report and typed port code. Preserve
  exact word arithmetic, high register halves, MOVEM.W sign extension, memory
  writes, and child effects. Reuse narrow register-effect helpers; never run a
  child with side effects twice.
- Build headless on this Windows workspace with
  & 'C:\Program Files\Git\bin\bash.exe' scripts/build_recomp.sh
  and build MSVC with
  cmake --build build/recomp-cmake --config Release -j 8
- Probe the new batch, then run
  & 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_ports_check.sh
  over all three native recordings. It checks shadow, sandbox, sealed final
  RAM, and poison frames. Inspect per-entry calls, incomplete calls, and every
  mismatch. QUICK=1 is only a first-recording probe.
- Compare live `--ports on` RGB444 output with fresh `--ports off` source
  output for every affected recording, not just final RAM, frame count or blit
  totals. Set `PORTS_ONLY` to the comma-separated registered batch and run
  `& 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_live_check.sh`.
  The same ON replay now also checks sealed final RAM, saving three extra
  replays per batch. Recordings run concurrently; temporary RGB/RAM outputs
  are removed. Build size is recorded with each integration checkpoint. An
  inactive entry needs temporary registration for a probe and must be removed
  if live output differs. A stepped SHADOW stream is not the live source
  oracle because source-first hardware-input replay can alter its timing.
- Increase the registered count and update this file only after every gate
  passes. Commit a coherent source batch with its evidence.
- Rank registered fixed-charge timing debt with
  `python scripts/probe_recomp_timing.py ENTRY... --frames 500`. Each positional
  argument is one entry, a comma-separated group, or `ALL`. The bounded probe
  reuses a single source stream and removes its scratch streams by default.
  `--rank-fixed N --differences-only` selects the N largest accumulated drifts
  from the latest demo gate report before probing them.
- Independently prove readable whole-call C with
  `python tools/recomp/check_whole_call_glue.py ENTRY...`; registered timing
  steps are disabled only in the temporary proof registry. A called child with
  no completed batch comparison is checked again in isolation automatically;
  each entry still needs a completed comparison. Raw batch/isolated reports
  retain hardware/incomplete classifications. The tool never relinks its
  temporary executable while a previous recording check is running.
- The headless Ninja graph tracks source and header dependencies and shares
  objects with structural oracles and mutation builds. An unchanged build is
  subsecond. Do not manually touch generated C after a header change.

## References and constraints

- PORT.md: architecture and source conventions.
- port/game/glue/ports.c: actual registered set and ON cycle charges.
- scripts/recomp_ports_check.sh: native proof driver. Archived UAE replay and
  scripts/recomp_parity.py are not current gates.
- captures/native/*/run.json: sealed frame endpoints and final RAM hashes.
- Existing reports under analysis/routines/: original behavior evidence.
- Do not infer mechanics, constants, or object meaning without original source
  or capture evidence. Do not edit the user-owned scripts/check_native_build.py,
  scripts/native_frame_count.py, port/native_data_allowlist.txt, or .vscode/.
