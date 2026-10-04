# Complete HUD cache and callable text helpers

2026-10-04. Eight old partial CPU-replay adapters now have complete readable
C, full CPU effects, original children and source timing. Registration stays
at 526 translated plus 66 source-only entries, 592 rows. Source-timed entries
rise to 447 (381 translated plus 66 source-only). Original game-function and
callback coverage remains incomplete.

| Original entry | Complete behavior | Controlled PCs | Original-child PCs |
| --- | --- | ---: | ---: |
| C31C20 | Cached value, forced redraw, invalid bit and pending event | 21/21 | 21/21 |
| C3271A | Small hexadecimal field at fixed column/rows | 64/64 | 41/64 |
| C32726 | Fixed small digits from D2 with zeros retained | 56/63 | 33/63 |
| C32736 | Inverse small field at view column and row offset | 65/65 | 42/65 |
| C32794 | Small-text loop, saved D1 and odd-destination fault | 39/39 | 16/39 |
| C32AA4 | Original BCD field with leading blanks | 83/83 | 39/83 |
| C32AA6 | Original BCD field with caller's zero mode | 82/82 | 38/82 |
| C32AB4 | Text loop at view column and four original planes | 66/66 | 22/66 |

The source seal owns 182 unique / 141 shared boundaries, actual original
incoming calls and six distinct / twenty per-owner child sites. These eight
entries were already registered; only their partial adapters in glue_batch10.c,
glue_batch16.c and glue_batch56.c are removed. Required older helper exports
and unrelated adapters remain.

The domain preserves the cache's signed-byte redraw test, invalid-word handling,
changed-value and pending-event branches, exact word writes and final flags.
Small digits use the original hexadecimal conversion; 8-pixel fields retain
original packed-nibble characters. Leading-blank scans, fixed zero mode,
postincrement/predecrement cursors, DBRA counts, word truncation, full swap
high halves, signed N xor V bounds, original font tables and word oddness are
preserved. Small text saves and restores full D1 around the glyph child and
runs the original fault call for an odd destination. The four-plane text
loop consumes the complete changed CPU state from each actual glyph call.
No child is replayed after drawing, and no new formatting or clipping rule
is added.

All 262,144 completed calls pass full CPU/PC/SR/all Chip and Slow RAM: 16,384
controlled and 16,384 original-child calls per owner. Controlled child entry
snapshots additionally compare complete CPU/SR/RAM. Their returns retain the
glyph/fault loop word while changing full registers, flags, valid cursors and
RAM. Controlled shared coverage is 182/182. C32726 forces keep-zero mode
before its digit loop, so seven static leading-blank PCs are excluded for
that entry: C32776/C32778/C3277A/C3277E/C32780/C32786/C3278A. Those paths are
covered by its sibling formatters. Per-entry coverage is retained above;
there is no claim of 63/63 entry coverage for the fixed-mode helper.

Original-child frozen-clock text fixtures use original clipped columns/layouts
and even destinations. Their exact owner coverage is the table's last column;
active glyph, fault and DMA paths are not claimed by those bounded fixtures.
Both proof kinds compare ordered Custom packets and terminal hardware/latch
state with the unchanged stream observer; these fixtures have zero writes.
Active drawing is checked independently on the native recordings.

Actual ON/shadow/sandbox passes 24,576 completed bounded fixtures, 1,024 per
owner per mode. They compare full CPU/PC/SR/all RAM and hardware state, require
exact hardware-free classification and one reference comparison, and exercise
OFF, selection, non-call and changed-source guards. Dispatch coverage is
reported separately from the complete controlled source union.

Independent normal C, with only timing steps disabled, passes every owner:
30,815 shadow / 32,589 sandbox matches. Hardware and incomplete classifications
remain explicit. Drawing retains the source-first DMACONR input contract.
Shared runtime, CPU/bus/arithmetic, classification, instruction fixtures and
hardware observer are unchanged; all 28 older generator outputs are byte-identical.

Local DMA passes 182 boundaries / 5,824 cases. Fresh combined DMA and the
independently derived union pass 24,511 / 784,352: previous 24,478 plus 182,
with 149 overlapping boundaries. GNU headless and MSVC Release builds pass;
build/ is 1.487 GiB. Full 592-row gate passes 568,440 shadow / 443,868 sandbox
matches, zero mismatches, exact RAM seals and identical poison frames. All
36,236 isolated live frames and RAM seals match. Family exact through frame
600; ALL retains 416/361.

Next complete eight older projection and postflight HUD parents:
C0DAEE/C0CFFA/C0D04C/C33CD2/C33B38/C33370/C332BC/C32662. Their 898 unique /
zero shared boundaries and original calls are sealed in
`analysis/data/legacy_hud_projection_scope_inventory.json`. Three additional
older dispatch owners C1FE24/C1FE46/C0CF98 have no direct calls in the audited
source-call set; their original indirect/table callability needs a separate
seal. They remain game-function debt. Other older HUD/rendering parents,
C2C392's computed transfer, C1612C graphics-wait integration and the complete
original game-call/callback graph remain open. Stop when all game functions
are complete and only Kickstart services and timing remain.

Implementation is `port/game/hud_text_helpers.c`, its typed CPU adapter and
generated timing bridge. Coverage, classifications, exact hashes and limits
are in `analysis/figures/native_hud_text_helpers_checkpoint.json`.
