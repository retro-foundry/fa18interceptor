# Complete renderer helpers

Twelve existing owners are complete as readable C in `port/game/render_leaf_helpers.c`,
with CPU effects in `glue_render_leaf_helpers.c` and separate source timing steps.
Original bytes, callers, all writer table slots and child sites are sealed in
`analysis/data/render_leaf_helpers_source_scope.json`. The evidence checkpoint is
`analysis/figures/native_render_leaf_helpers_checkpoint.json`.

| Original owner | Complete behavior |
| --- | --- |
| C2F5C0 | View-bounded pixel entry and actual C2F688 child |
| C2F5D4 | Pixel with individual saved word registers and actual C2F688 child |
| C2F5F4 | Pixel prefix, XOR choices and all sixteen original single-row writers |
| C2F60A | Pair alignment, saved word registers and both actual pixel children |
| C2F66E | Last-row pair or two-row block, both sixteen-slot writer tables |
| C310E2 | Signed span bounds, overflow branches and long cursor |
| C301F0 | Polygon bounds, tiny shapes, actual edge children and mask fill |
| C304B2 | Complete mask-clear packets and actual blitter wait |
| C32806 | Three-pixel glyph draw, clear and inverse with original save/restore |
| C2FA78 | Fixed-last-row line entry and complete shared line body |
| C2FA7E | Variable-last-row line, clipping, octants and four plane submissions |
| C2FD8C | Four plane submissions, counted busy polls, actual display/polygon/composite children and final copy |

The batch owns 971 unique source boundaries, 392 shared within the batch,
and thirteen distinct / sixteen per-owner child sites. Registration remains
526 translated plus sixty-six source-only entries, 592 total. Source-timed
entries rise from 509 to 510 (444 translated plus sixty-six source-only).
Timing-entry presence alone does not certify completion of older adapters.

Every disabled pixel plane still receives the original AND/OR write. Plane
order is A3, A2, A1, A0; two-row writers finish each plane before proceeding.
MOVEM.W sign extension, high register halves, partial stores, stack order,
signed overflow conditions and each actual returned child state are retained.
Line clipping uses original signed DIVS quotient/remainder and rounding.
The three counted plane polls retain the original 54-cycle busy loop and
26-cycle ready poll; source-byte-backed prefixes are 52, 78/84 and 52 cycles.
No production clamp, selector guard, busy-read substitute or loop cap was added.

There are 393,216 complete-call comparisons: 196,608 with controlled child
contracts and 196,608 with independently executed original child bytes.
Each compares all sixteen registers, PC, full SR, all Chip/Slow RAM,
ordered Custom writes and terminal registers, effective flags, blit counters
and data latches. Controlled children additionally compare complete entry
CPU/SR/RAM at every original call and return site.

Controlled whole calls cover 964/971 boundaries. The remaining PCs are
C30266, C3027A, C30296 and C302E4 (negative extent arms), and
C2FBC2, C2FBC4 and C2FBC6 (the first plane's busy loop). Five independent
production-segment proofs add 81,920 calls and cover all 21 segment PCs.
The extent proofs invoke the same helpers as C301F0; the wait proof invokes
the same actual hardware observer as the line body. These are separate
segment claims, not evidence of whole calls entering those seven PCs.
Stable polygon min/max ordering makes the negative arms cold; the first
plane has no intervening BLTSIZE after the initial completed wait in the
held-service whole-call fixture. All general source branches remain in C.

Original-child coverage is 947/971. C2FD8C additionally leaves seventeen
condition-dependent parent instructions cold; controlled child contracts
cover them. Exact missing PCs for every owner and proof mode remain in the
checkpoint. Glyph fixtures use positive original font row counts, while
production retains the full zero-count DBRA behavior. Line fixtures use
bounded original screen geometry; an earlier unbounded synthetic input
reached the original DIVS-zero exception and is retained as a failed proof.

Physical whole-call fixtures use the original running hardware clock and
real initial blits on odd scenarios, with five initial Custom packets per
odd case for polygon, clear, both lines and plane submission. Packet totals
include those initial fixture packets. Original children execute actual
source instructions with service held outside the call. This test-only CPU
oracle is independent of generated-child resume bookkeeping; production
domain/glue code never invokes opcode handlers.

Actual ON/shadow/sandbox dispatch passes 36,864 calls, with strict completed
classification and all admission/counting/source-integrity guards. Each
mode classifies 12,288 hardware-free source calls and validates 90,368 actual
Custom writes, including initial fixture packets. The original classifier's
hardware-free label is distinct from observing Custom writes. Dispatch
uses production generated-child continuations, separately from the raw-child
whole-C proofs. Its held-service boundary is enforced before the runtime's
event check; device clocks and reads remain real. The plane sandbox baseline
uses the runtime's suppressed-write reference policy, then commits the
original logged packets at return, as the unchanged runtime does. Guard
checks prove admission/counting; a generated source guard stopping at cold
C2FE02 is explicitly not a completed source-call proof.

Normal C independently passes every owner across all recordings: 423,745
shadow and 530,198 sandbox comparisons. The generic checker and its positive
owner rejection are unchanged. All 36,236 isolated live frames match fresh
source OFF streams, and every final RAM seal is exact. The full 592-row gate
passes 568,446 shadow and 443,870 sandbox matches, zero mismatches, exact
RAM seals and identical poison frames. Original busy-input opt-ins are
preserved; newly added opt-ins caused a retained one-byte C45927 busy-counter
drift in earlier full gates and were removed before the passing gate.

Local DMA passes 971 instructions / 31,072 cases. Fresh combined DMA and an
independent union pass 28,931 / 925,792: previous 28,849 plus 971 with 889
overlapping PCs. All thirty-five older generators are byte-identical.
Shared CPU/runtime/machine/bus/math/classification/observer and earlier
completed domains are unchanged. Remaining export bodies in old glue files
are unchanged; unused `glue_active_planes.c` callbacks are removed. GNU and
MSVC Release pass. The checkpoint records build size and all exact seals.

Family timing is exact through 600 frames. ALL still first differs at frame
424, with 34,144 pixels; Copper fade and Kickstart remain deferred.

The next sealed scope has thirty-seven renderer entries, 550 unique / 235
shared boundaries: C2F688, square entries, C301F6, C330FE and all thirty-two
original writer callbacks. Twenty-nine entries are registered upgrades,
C2F688 is an unregistered seeded entry, and seven writer callbacks have
source-only entry evidence from their actual table slots and C2F764 jump.
See `analysis/data/render_entry_helpers_scope_inventory.json`.
Selected-segment dynamic children, other older partial adapters, C2C392,
C1612C graphics-wait integration and the complete original cold/indirect/
callback graph still prevent declaring game-function porting complete.
