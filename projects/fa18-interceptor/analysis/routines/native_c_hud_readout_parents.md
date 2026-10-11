# Complete HUD numeric, cue and status parents

2026-10-04. Thirteen older partial adapters now have complete readable domain
C, full CPU effects, actual original children and source timing. Registration
stays at 526 translated plus 66 source-only entries, 592 rows; source-timed
entries rise to 439 (373 translated plus 66 source-only). Original game-function
and callback coverage remains incomplete.

| Original entry | Complete behavior | Owner PCs | Original-child PCs |
| --- | --- | ---: | ---: |
| C31F4C | Speed cache, quotient, context suffix and two text passes | 120 | 71 |
| C3201A | Fixed or record altitude, invalid cache and context suffix | 135 | 88 |
| C3212A | Record +72 shifted value and inverse small text | 84 | 60 |
| C32178 | Signed record byte, magnitude, quotient and small digits | 88 | 64 |
| C321D2 | Grid Z division, redraw counter and both planes | 98 | 75 |
| C32260 | Grid X division, redraw counter and both planes | 98 | 75 |
| C31EB6 | Context heading, HDG prefix and saved fixed-width digits | 36 | 3 |
| C31C60 | Weapon class, packed count and original label | 48 | 48 |
| C31D16 | Signed field, original sign character and BCD text | 23 | 23 |
| C31E6C | Weapon-selected fire cue and original six-character text | 18 | 18 |
| C31D64 | Load field, trim/status arithmetic, point and text paths | 64 | 64 |
| C33F54 | Three packed digits with original two-character field | 14 | 14 |
| C328A8 | Weapon redraw counter and ARM/NO status lines | 143 | 120 |

The sealed source owns 606 unique / 70 shared boundaries, original incoming
calls and 31 distinct / 43 per-owner child sites. Shared small-text formatting,
glyph and fault tails belong to complete parent scopes. Calls to cache, BCD,
point, text and first-pass formatters execute once and return complete CPU state.
MOVEM.L masks preserve the saved registers and memory order; the weapon state
owner retains its saved D0 word. Original child flags, including N xor V, control
subsequent branches. No child is replayed for register effects after drawing.

The domain preserves signed word/byte/long truncation, fixed and record altitude,
unsigned and signed divisions including overflow, cache invalid-bit handling,
redraw counters, context checks, packed labels, leading blanks and hexadecimal
small-text digits. The signed field keeps the original sign-character choices.
The three-digit helper retains its original two-character display count. The
weapon status paths preserve their original ARM/NO signed-word test and redraws.
No new clipping guard, format rule, label or convenience behavior is added.

All 425,984 complete calls pass full CPU/PC/SR/all Chip and Slow RAM: 16,384
controlled and 16,384 original-child calls per owner. Controlled child entry
snapshots additionally compare complete CPU/SR/RAM and return changed registers,
flags, valid cursors and RAM. Glyph/fault loop words are retained; formatter
counts and remaining test outputs are explicitly bounded. Controlled owner
coverage is complete in every row above, for a 606/606 shared source union.

Original-child frozen-clock fixtures use original clipped bounds and set
CONTEXT_SELECT to zero because context text has a fixed zero column. Their
individual coverage is the final table column. They do not prove context,
active DMA, glyph or fault paths. Both proof kinds check ordered Custom packets
and terminal hardware/latch state with the unchanged stream observer; these
fixtures have zero Custom writes. Actual drawing is independently checked on
the native recordings, where every one of the thirteen owners has completed
normal-C comparisons.

Actual ON/shadow/sandbox dispatch passes 39,936 bounded completed fixtures,
1,024 per owner per mode. They compare full CPU/PC/SR/all RAM and hardware
state, enforce exact hardware-free classification and one reference comparison,
and exercise OFF, selection, non-call and changed-source guards. Their bounded
owner coverage remains separate from the complete controlled coverage.

Independent normal C, with only timing steps disabled, passes 32,575 shadow /
33,952 sandbox matches across all thirteen owners. Hardware and incomplete
rows remain explicit. The source-first DMACONR input contract is retained.
Shared runtime, CPU/bus/arithmetic, classification, instruction fixtures and
hardware observer are unchanged. All 27 older generator outputs are byte-identical.

Local DMA passes 606 boundaries / 19,392 cases. Fresh combined DMA and the
independently derived union pass 24,478 / 783,296: previous 23,942 plus 606,
with 70 overlapping boundaries. GNU headless and MSVC Release builds pass;
build/ is 1.45 GiB. The full 592-row gate passes 568,440 shadow / 443,868
sandbox matches, zero mismatches, exact RAM seals and identical poison frames.
All 36,236 isolated live frames and RAM seals match. Family exact through
frame 600; ALL retains 416/361. The initial registry field-order build failure
and wrong CMake source-directory rejection remain in the checkpoint.

Next complete eight callable HUD cache and text helpers:
C31C20/C3271A/C32726/C32736/C32794/C32AA4/C32AA6/C32AB4. Their 182 unique /
141 shared boundaries and actual incoming calls are sealed in
`analysis/data/hud_text_helper_scope_inventory.json`. Older projection parents,
C2C392's computed transfer, C1612C graphics-wait integration and the full
original call/callback graph remain open. Stop when all game functions are
complete and only Kickstart services and timing remain.

Implementation is `port/game/hud_readout_parents.c`, its typed CPU adapter and
generated timing bridge. Exact coverage, classifications, hashes, retained
failures and limits are in `analysis/figures/native_hud_readout_parents_checkpoint.json`.
