# Complete ground and HUD rendering parents

2026-10-04. Fifteen older adapters now own complete readable C in
port/game/hud_render_parents.c. Registration remains 526 translated plus
66 source-only entries, 592 rows; source-timed entries rise from 494 to
509 (443 translated plus 66 source-only). Game-function porting remains open.

| Original entry | Complete behavior | Controlled / original-child PCs |
| --- | --- | ---: |
| C098C6 | Ground workspace transform, complete point and outlined-face loops | 51/51 / 51/51 |
| C09952 | Outlined face with stream restore | 32/32 / 32/32 |
| C099F6 | Near-tested outlined face and rejected stream advance | 36/36 / 36/36 |
| C099AA | Filled face and stream restore | 28/28 / 28/28 |
| C332FE | All marker pairs and original MOVEM.W state | 29/29 / 29/29 |
| C34146 | HUD frame segments, clipping and colour branches | 93/94 / 93/94 |
| C34066 | Both tick fans and full frame loop | 59/59 / 59/59 |
| C342D0 | Target/seeker range, clipping, bounding box and four lines | 196/196 / 196/196 |
| C347F2 | All ring quarters, direction and distinct Y bias | 63/63 / 63/63 |
| C33DC8 | Complete seeker alerts, lock state and sound branches | 94/94 / 94/94 |
| C322EE | All message pages, decimal formatting, glyph and fault paths | 280/281 / 277/281 |
| C3003A | Complete panel-marker span and direct blit packets | 95/95 / 88/95 |
| C304FA | Complete planar-lane mask blit | 47/47 / 44/47 |
| C345A0 | Ring-point quarter changes and clipped/unclipped paths | 198/198 / 198/198 |
| C348B2 | Entire conditional HUD symbol and flash paths | 49/49 / 49/49 |

The sealed source has 1,352 unique boundaries, no shared boundaries within
this batch and sixty distinct child sites. Original callers and bytes are
sealed. Typed whole-call glue calls actual children once, compares their
complete entry contracts, consumes returned CPU and RAM state, and preserves
original stack transfers, signed word arithmetic, MOVEM.W sign extension,
overflow-sensitive branches and bus order. Domain and whole-call glue use
no opcode handlers; source timing remains separate. Remaining export bodies
in batches 27/38/48/49/52/61/62/67 and glue_C28E28 are byte-identical.
Unreachable older helpers and their public prototypes are removed.

All 491,520 complete calls match every register, PC, full SR and every
Chip/Slow RAM byte: 16,384 controlled and 16,384 original-child calls per
owner. Controlled whole-entry coverage is 1,350/1,352. The original monotone
frame-X table and every signed word origin exclude C3429E from a stable
whole-frame call. C32380 is excluded when the positive redraw byte remains
stable between the original gate and decrement; no child occurs between them.
Both branches remain implemented. Additional tests execute the actual
production C34296 frame-right and C32376 page-delay segments: 32,768 calls,
every owned segment boundary, full CPU/PC/SR/RAM and zero Custom writes.
These segment calls are separate evidence, not whole-entry coverage claims.

Original-child missing PCs remain explicit: C34146 misses C3429E; C322EE
misses C32380 and odd-glyph fault PCs C327F6/C327FE/C32804; C3003A misses
four normalizer boundary-flag paths and three busy-loop NOPs; C304FA misses
three busy-loop NOPs. All other owners have full original-child coverage.
Frozen-clock children use original degenerate/behind-view, line-ceiling,
text-column and panel-Y rejection paths; active child multiplane drawing is
not claimed by these fixtures. Independent recordings exercise actual drawing.

Ordered Custom packets and terminal registers, effective flags, blit counters
and data latches match. Direct parent blits produce real packets even with
source-rejected children. Controlled C3003A/C304FA waits use an actual initial
blit on odd scenarios, with the original clock active; no canned busy reads.
Each full controlled direct-blit owner includes 40,960 initial fixture writes
(five per odd scenario). Totals including these writes are 116,800 for C3003A
and 237,568 for C304FA. Original-child totals are 151,680 and 196,608.
The shared machine observer is unchanged. Actual source PPC is set at parent
busy reads, preserving shadow input ordering after normalizer children.

Actual ON/shadow/sandbox dispatch passes 46,080 bounded calls and all guards.
Each mode has zero hardware-bearing / 15,360 hardware-free classifications
and 22,848 observed Custom writes. This classifier result is separate from
packet observation: the source classifier does not mark these direct writes
as unsupported reads. Exact missing PCs and per-owner classifications are in
the checkpoint; dispatch fixtures do not claim full source coverage.

Normal C retains busy/range/ownership contracts and passes every owner:
30,293 shadow / 42,601 sandbox. C304FA has 97 shadow comparisons and zero
sandbox comparisons; the generic positive-owner requirement is unchanged.
All 36,236 isolated live RGB444 frames and final RAM seals match. Full
592-row replay passes 568,446 shadow / 443,870 sandbox comparisons, zero
mismatches, exact seals and identical poison frames. Local DMA passes
1,352 instructions / 43,264 cases. Independent union and fresh combined DMA
pass 28,849 / 923,168: previous 27,543 plus 1,352, overlap 46. All thirty-four
older generator outputs and shared runtime/CPU/bus/arithmetic/observer and
earlier completed domains are unchanged. GNU and MSVC Release pass.
Build/ occupies 1.74 GiB.

Family timing is exact through frame 600. ALL now first differs at frame
424 with 34,144 pixels; the preceding checkpoint was frame 416 / 361 pixels.
This change is retained as deferred timing evidence. The earlier Copper-fade
difference and Kickstart services remain deferred by the user's stop condition.

Initial normal-C, frozen-clock and incomplete-coverage failures remain sealed.
The local instruction DMA failure exposed missing rotate-count timing;
the source-backed ROL.B helper now charges two cycles per count. The final
whole-call, controlled contract, real-child, dispatch and DMA evidence passes.
verify_hud_render_parents_recordings.py aggregates completed per-owner raw
logs rather than relabeling a failed earlier whole-batch coverage run as passed.

The next twelve plot/span/polygon/lane/glyph helpers have 971 unique / 392
shared boundaries and original callers sealed. Both original sixteen-entry
pixel-writer tables are complete, backed by the original four-bit colour
mask, doubled-twice index and dynamic JMP. No production guard is invented.
Selected-segment indirect calls, older adapters, C2C392 computed transfer,
C1612C graphics-wait integration and the complete original cold/indirect/
callback graph remain game-function work. Goal is not complete.

Evidence: analysis/data/hud_render_parents_source_scope.json,
analysis/figures/native_hud_render_parents_checkpoint.json and
analysis/data/render_leaf_helpers_scope_inventory.json. Reproduce with
audit_hud_render_parents_source.py, check_hud_render_parents.py --kind
contract/real --cases 16384, verify_hud_render_cold_segments.py,
verify_hud_render_parents_recordings.py, check_hud_render_parents_dispatch.py
--cases 1024, the fifteen-owner check_whole_call_glue.py selection and the
standard replay/live/DMA/MSVC gates.
