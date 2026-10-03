# Complete point projection, packed readouts and HUD sweeps

2026-10-03. Six new complete original game functions and four projection
upgrades now have readable C and source timing. Registration rises to 520
translated plus 66 source-only entries, 586 rows; 410 entries have source
timing (344 translated plus 66 source-only). Complete original game-call and
callback coverage remains open.

| Original entry | Complete behavior | Owner PCs |
| --- | --- | ---: |
| C2EC90 | Project a point without a drawing child | 80 |
| C2EC94 | Project and draw using the original dynamic mode | 80 |
| C2EC9C | Project and draw the circle mode | 80 |
| C2ECA4 | Project and draw a pixel pair | 80 |
| C2ECA8 | Project and draw a pixel | 79 |
| C32A44 | Convert and lay out the scene numeric label | 110 |
| C32AC8 | Unpack digits, suppress leading zeros and draw four planes | 83 |
| C33F70 | Prepare the speed tick's original layout and packed readout | 14 |
| C33F8A | Prepare the altitude tick's original layout and packed readout | 13 |
| C33FB4 | Draw the bounded upper/lower readout sweep | 50 |

The source manifest seals 272 unique / 163 shared instruction boundaries,
actual original incoming JSR/BSR bytes, and 16 distinct / 41 per-owner child
sites. Shared projection and packed-digit tails remain owned by each entry.
The four upgraded adapters now invoke the original drawing child once and
retain the C06C02 fault call. Other older drawing parents still use their
legacy projection replay and require complete original-child upgrades before
game-function completion can be claimed.

The domain retains signed entry bounds, DIVS quotient/remainder packing,
overflow behavior, reflected screen coordinates, last-row rejection, original
plot-mode selection, variable word shifts and the fault error word. Packed
readouts retain their original BCD word, leading blanks, font/layout tables,
odd destination rejection, four color-plane operations and DBRA count.
Sweeps retain saved coordinates and counters in their LINK/UNLK frame,
the original 71/112 row limits and distinct pixel/pair positions. Partial
word/byte outputs, SWAP, full child returns, memory flags and N xor V signed
branches are preserved.

All 327,680 complete fixture calls pass every CPU register, PC, full SR and
every byte of Chip and Slow RAM: 16,384 controlled and 16,384 original-child
calls per entry. Controlled child boundaries compare complete CPU/SR/RAM
entry contracts and return changed registers, flags, cursors and RAM. They
are explicit test contracts; original children are proved independently with
a bounded frozen-clock service observation and no invented return.

Whole-call owner coverage, in the table's order, is 59/80, 76/80, 60/80,
57/80, 54/79, 104/110, 83/83, 14/14, 13/13 and 50/50 for both proof kinds.
Fixed projection modes exclude other modes' paths in the shared static CFG.
Scene labels set glyph mode 1, excluding the shared six-instruction leading-
blank path; the packed-readout owner separately covers that path.

The controlled whole-call union covers 268/272 boundaries. The original entry
bounds make upper-clamp PCs C2ED4C/C2ED50/C2ED52/C2ED56 unreachable from a
complete projection call. Production domain helpers are therefore factored
at the original internal boundaries C2ECD2/C2ECE4 and used by the complete
projector. Another 65,536 completed internal source-segment comparisons
(16,384 per boundary and proof kind) pass full CPU/PC/SR/RAM. Combined coverage
is 272/272. These segments are explicitly separate from function owners and
complete-entry coverage; the initial strict union rejection remains recorded.

Independent normal C, with timing steps removed, passes 10,312 shadow /
10,360 sandbox comparisons for the four upgraded projectors. All six new
owners have zero calls in the three sealed recordings, including with those
projectors omitted. Both strict generic zero-comparison rejections and all
raw hardware/incomplete counts remain explicit. Cold-entry behavior is
supported by complete-call fixtures and byte-sealed original call sites.

Actual ON/shadow/sandbox dispatch passes 491,520 completed fixtures and all
OFF, selection, non-call and changed-source guards. All 16,384 fixtures per
entry and mode are hardware-free and require exact matched classification.
Bounded owner coverage matches the whole-call coverage listed above.

Drawing registrations retain the existing source-first DMACONR proof contract.
Shared CPU, bus, arithmetic, classification and whole-call proof code remain
unchanged. All 24 older timing generator families produce identical output.

Local DMA proof passes 272 instructions / 8,704 cases. Fresh combined DMA and
the independently derived union pass 23,247 instructions / 743,904 cases:
previous 23,061 plus 272, with 86 overlapping projection boundaries.

GNU headless and MSVC Release builds pass. The full 586-row gate passes
568,155 shadow / 439,147 sandbox matches, zero mismatches, exact RAM seals and
identical poison frames. All 36,236 isolated live frames and RAM seals match.
The family is exact through frame 600; ALL retains the deferred frame-416 /
361-pixel Copper difference.

Next reconstruct C308D8/C308F4/C30F46/C31B76/C33AD6/C33B06: 100 unique / zero
shared boundaries and sealed actual original calls in
`analysis/data/hud_stream_scope_inventory.json`. C2C392's computed transfer,
C1612C's graphics-wait integration and complete original callable/callback
coverage remain open. Stop once all game functions are complete and only
Kickstart services and timing remain.

Implementation is `port/game/projection_readouts.c`, its typed CPU adapter
and generated timing bridge. Source, report and log hashes, exact coverage
and remaining limits are recorded in
`analysis/figures/native_projection_readouts_checkpoint.json`.
