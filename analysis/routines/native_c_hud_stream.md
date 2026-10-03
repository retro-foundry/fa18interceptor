# Complete stream stores, numeric fields and bounded markers

2026-10-04. Six new original game owners and two upgraded store adapters now
have readable domain C, complete CPU adapters and original source timing.
The registered set is 526 translated plus 66 source-only callable entries:
592 rows, with 418 source-timed entries (352 translated plus 66 source-only).
Original game-function and callback coverage remains incomplete.

| Original entry | Complete behavior | Owner PCs |
| --- | --- | ---: |
| C308E2 | Store the CD blit fields using the caller's A0 base | 5 |
| C30904 | Store the AD blit fields using the caller's A0 base | 5 |
| C308D8 | Consume a destination stream, wait and submit CD fields | 8 |
| C308F4 | Consume destination and pointer streams, wait and submit AD fields | 11 |
| C30F46 | Consume both streams and submit the other ACD fields | 13 |
| C31B76 | Convert and submit one or three numeric fields | 42 |
| C33AD6 | Clip and submit the first marker's original line child | 13 |
| C33B06 | Clip and submit the second marker's original line child | 13 |

The byte-sealed scope contains 100 unique / 10 shared instruction boundaries,
actual original incoming JSR/BSR sites and 11 distinct child sites. C31B76
also owns the preceding return at C31B74. Each adapter retains its actual
original child return PC; shared store tails remain owned by both callers.

The store upgrades replace old fixed-fee wrappers that hardcoded the Custom
base and omitted complete CPU effects. A0 remains the caller's base, including
RAM bases. Stream cursors advance in the original order, offsets use original
long arithmetic, and child returns can change every working register and SR.
Numeric fields retain EXT.L, the signed-word negation of the two optional
fields (including -32768 overflow), original BCD storage, literal layout
addresses and the byte gate after the first field. Marker siblings preserve
the 315 upper bound, the low bound after adding four, and both vertical
offset additions. No additional clipping or convenience return is introduced.

All 262,144 complete calls pass every CPU register, PC, full SR and all Chip
and Slow RAM: 16,384 controlled and 16,384 original-child calls per owner.
Both proof kinds cover every owner's source PCs: 5/5, 5/5, 8/8, 11/11,
13/13, 42/42, 13/13 and 13/13. The shared controlled union is 100/100.
Controlled child contracts compare complete CPU/SR/RAM at the original entry
and return changed registers, flags, cursors and RAM. Original children run
independently with their actual bodies and bounded service observations.

Hardware comparisons additionally require exact ordered Custom register/value
packets, all 256 terminal Custom registers, effective DMA/interrupt/audio
flags, blit and line-blit counters, four persistent blitter data latches, and
pending/size state. Original private machine state is restored from the loaded
baseline before each side; RAM/CPU snapshots alone left BBUSY from a previous
comparison. The observer uses a test-only machine copy that renames only the
Custom-write definition. Internal original bus calls retain their normal name
and pass through the observer. The original machine, bus and blitter source
files are unchanged, and no wait or hardware result is manufactured.

The original-child store fixtures validate 49,152 ordered writes per owner
for C308E2/C30904/C308D8/C308F4, and 81,920 for C30F46. Each marker validates
42,832 writes. Numeric fixtures complete without Custom writes; this is a
parent proof and does not assert complete nested glyph coverage. Controlled
children return a RAM register base, so stream parents do not claim hardware
writes in that proof; the direct store upgrades validate 49,152 writes each.

Original-child marker fixtures use bounded vertical screen offsets and zero
or one enabled plane. The wider signed-word offsets remain in controlled
contracts. Initial extreme offsets entered original ROM exception code;
multiple enabled planes can wait for DMA under a frozen whole-call clock.
Neither condition is silently converted into a successful original-child
proof. Earlier private-state, observer, instruction-recipe and stale MSVC
source-list failures are retained with the final proof logs.

Actual ON/shadow/sandbox dispatch passes 393,216 complete fixtures, all owner
PCs, ordered hardware writes and OFF, selection, non-call and changed-source
guards. Classification remains unchanged: hardware-free calls require exactly
one match; hardware-bearing calls require exactly one hardware classification
and no matched, mismatched or incomplete comparison. Custom writes alone are
hardware-free in the shared classifier and are independently checked here.
All completed fixtures in this batch are hardware-free under that classifier.
Whole-call proofs validate 462,496 ordered writes; dispatch proofs validate
another 1,092,576. Neither count is substituted for a CPU/RAM comparison.

Independent normal C with only timing steps disabled passes 9,146 shadow /
9,278 sandbox comparisons for the five store owners. Numeric and marker
owners have zero calls in all three recordings, including with those store
owners omitted. Both strict generic zero-comparison rejections and the raw
hardware/incomplete counts remain explicit. The six new drawing/wait rows
retain the existing source-first DMACONR proof contract; the two direct-store
upgrades do not need that input contract.

Local DMA proof passes 100 instructions / 3,200 cases. The fresh combined DMA
run and independently derived union pass 23,347 instructions / 747,104 cases:
previous 23,247 plus all 100, with no overlap. All 25 older generator families
produce byte-identical output. The new family's absolute-word LEA recipe is
local; shared arithmetic, CPU/bus, instruction fixtures and classification
proofs remain unchanged.

GNU headless and MSVC Release builds pass. The full 592-row gate passes
568,440 shadow / 439,147 sandbox matches, zero mismatches, exact RAM seals and
identical poison frames. All 36,236 isolated live frames and RAM seals match.
The family is exact through frame 600; ALL retains frame 416 / 361 pixels.

Next reconstruct C30764/C309B6/C30B5C/C30D34/C30F78/C3112A/C31A64/C31ACC.
Their original incoming calls and 595 unique / 63 shared boundaries are sealed
in `analysis/data/hud_parent_scope_inventory.json`. Shared digit tails and
fault calls belong to the complete parents. Older projection parents, C2C392's
computed transfer, C1612C's graphics-wait integration and the full original
call/callback graph remain open. Stop when all game functions are complete
and only Kickstart services and timing remain.

Implementation is `port/game/hud_stream.c`, its typed CPU adapter and generated
timing bridge. Exact coverage, classification counts, hardware packets, hashes,
recording reports and proof limits are recorded in
`analysis/figures/native_hud_stream_checkpoint.json`.
