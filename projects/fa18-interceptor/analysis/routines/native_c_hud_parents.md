# Complete HUD display parents and shared digit tails

2026-10-04. Eight existing partial adapters now have complete readable domain
C, CPU effects, original children and source timing. The registered count stays
526 translated plus 66 source-only callable entries, 592 rows. Source-timed
entries rise to 426 (360 translated plus 66 source-only). Original game-function
and callback coverage remains incomplete.

| Original entry | Complete behavior | Owner PCs |
| --- | --- | ---: |
| C30764 | Counter-gated panel streams and the other four-plane stream | 108 |
| C309B6 | Original panel-image table and renderer arguments | 12 |
| C30B5C | Indicator line, three status paths and the shared blit tail | 110 |
| C30D34 | Mode bar, indexed record image and conditional marker line | 94 |
| C30F78 | Cached compass value, word masks, blit and marker line | 82 |
| C3112A | Bit-selected point selectors and the original ten-call sequence | 59 |
| C31A64 | Record-class digits, BCD conversion and small-text tail | 91 |
| C31ACC | Scale digits, saved word registers, point and small-text tail | 102 |

The byte-sealed source contains 595 unique / 63 shared boundaries, actual
original incoming calls and 43 distinct / 45 per-owner child sites. Backward
returns, C30B5C's fall-through C30CC4 tail and both shared digit tails belong
to the complete parents. The eight old replay wrappers in glue_batch59.c and
glue_batch60.c are removed; other older adapters and their required helpers
remain for later complete upgrades.

The new domain preserves each counter's signed-byte test and decrement,
original branch order, word truncation, signed N xor V bounds, cached invalid
bits, masks and blit field order. It invokes each actual original child at its
original return PC and uses the complete returned CPU state. It preserves
MOVEM.W sign extension, the saved D1 stack value across glyph calls, DBRA
counts, leading blanks, hexadecimal digits and the odd-destination fault call.
The source's distinct logical shift, byte arithmetic shift and word rotation
retain their own flag behavior. No drawing child is replayed for its register
effects after already executing its effects.

The selected final proof set contains 262,144 complete calls, 16,384 controlled
and 16,384 original-child calls per owner. Every call compares all CPU registers,
PC, full SR and all Chip/Slow RAM. Controlled child boundaries additionally
compare full CPU/SR/RAM at each original call and return changed registers,
flags, cursors and RAM. Only the glyph/fault loop word and bounded blit
dimensions are retained in those explicit test contracts.

Controlled owner coverage is complete: 108/108, 12/12, 110/110, 94/94, 82/82,
59/59, 91/91 and 102/102; the shared union is 595/595. The first controlled
run passed every complete CPU/RAM comparison but rejected shared coverage at
592/595. Correlated counter/flag inputs excluded the status-E bit-test path.
The status fixture now selects that flag independently; another 16,384 calls
cover all 110 status PCs. The initial 107/110 status log and strict union
rejection remain evidence; those preliminary status calls are separate from
the final per-owner proof count. Production behavior was not changed.

Frozen-clock original-child fixtures use the original no-draw bounds to avoid
holding active DMA waits. Their coverage is explicitly limited to 3/108,
12/12, 9/110, 11/94, 28/82, 12/59, 3/91 and 3/102. They do not establish
active glyph or DMA coverage. Ordered Custom packets and terminal hardware
state are checked with the unchanged test observer; these fixtures have zero
Custom writes. Active drawing is independently checked on the recordings.

Actual ON/shadow/sandbox dispatch passes 6,144 complete bounded fixtures with
CPU/PC/SR/all-RAM, terminal hardware state and OFF, selection, non-call and
changed-source guards. All are hardware-free and require exactly one matched
comparison in reference modes. Bounded owner coverage matches the original-
child coverage above; complete source coverage comes from the controlled
parent proofs and the independent instruction oracle.

Independent normal C, with only timing steps disabled, passes all eight owners
on the recordings: 27,001 shadow / 41,163 sandbox matches. Hardware and
incomplete rows remain explicit. The registrations retain the existing
source-first DMACONR input contract. Shared runtime, CPU/bus/arithmetic,
classification, instruction fixtures and the hardware observer are unchanged.
All 26 older generator families produce byte-identical output. The new local
MOVEM.W recipe preserves the source's predecrement mask and order.

Local DMA passes 595 boundaries / 19,040 cases. Fresh combined DMA and the
independently derived union pass 23,942 / 766,144: previous 23,347 plus all
595, no overlap. GNU headless and MSVC Release builds pass. The full 592-row
gate passes 568,440 shadow / 443,868 sandbox matches, zero mismatches, exact
RAM seals and identical poison frames. All 36,236 isolated live frames and
RAM seals match. The family is exact through frame 600; ALL retains 416/361.

Next complete thirteen numeric, cue and status parents:
C31F4C/C3201A/C3212A/C32178/C321D2/C32260/C31EB6/C31C60/C31D16/C31E6C/
C31D64/C33F54/C328A8. Their 606 unique / 70 shared boundaries and original
incoming calls are sealed in `analysis/data/hud_readout_parent_scope_inventory.json`.
Older projection parents, C2C392's computed transfer, C1612C graphics-wait
integration and the full original call/callback graph remain open. Stop when
all game functions are complete and only Kickstart services and timing remain.

Implementation is `port/game/hud_parents.c`, its typed CPU adapter and generated
timing bridge. Exact coverage, recording classifications, hashes, build size,
retained failures and limits are in `analysis/figures/native_hud_parents_checkpoint.json`.
