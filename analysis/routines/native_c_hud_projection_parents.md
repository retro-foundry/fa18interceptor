# Complete projection, weapon-cue and conditional-text parents

2026-10-04. Six old partial adapters now own complete readable C and actual
original child contracts. Registration stays at 526 translated plus 66
source-only entries, 592 rows. Source-timed entries rise from 447 to 451
(385 translated plus 66 source-only); C0DAEE and C332BC already had timing adapters.
Original game-function and callback coverage remains incomplete.

| Original entry | Complete behavior | Controlled PCs | Original-child PCs |
| --- | --- | ---: | ---: |
| C0DAEE | Fixed tuple multiplied by the original view matrix | 32/32 | 32/32 |
| C0CFFA | Word-shifted circle, original magnitude and radius division | 29/29 | 26/29 |
| C33CD2 | Record projection and previous/current position publication | 67/67 | 67/67 |
| C33B38 | Gun cue, marker, range ring and closure-rate cache | 90/90 | 24/90 |
| C332BC | Outer postflight HUD call order and context gate | 16/16 | 5/16 |
| C32662 | Conditional small-text entry and complete glyph/fault tail | 45/45 | 22/45 |

The source seal owns 279 unique / zero shared boundaries, all original
incoming calls, and twenty distinct / twenty per-owner child sites. Only
these six wrappers are removed from glue_fixed_mark.c, glue_scaled_circle.c,
glue_postflight_hud.c and glue_batch16.c. Older helper exports remain.

The fixed tuple uses the original 2.8 matrix, ordered signed products and
arithmetic shifts. The circle retains word shift counts masked to six bits,
large-count results, MOVEM.W sign extension, saved stack values, magnitude's
complete returned CPU state, unsigned divide overflow and signed radius clamp.
The record transform publishes the returned projected marker and uses the
returned record pointer for its second matrix transform. Its MOVEM transfers
retain flags and update previous/current positions in the original order.

The weapon cue preserves signed subtraction branches, gun-range tests,
cue/event bits, point/ring/range call sites, unsigned scale, saved full record
pointer, source byte BCLR flags and the closure-rate cache. The outer HUD
consumes actual changed RAM after every original child; no source effect is
replayed after rendering. Conditional text delegates the already-proven exact
C32794 tail through typed hooks with its original child return addresses.
No formatter, clipping rule, range guard, service return or timing workaround
is invented. Domain and whole-call glue invoke no opcode handlers; generated
timing bridges remain separate.

All 196,608 whole calls match full CPU/PC/SR and every Chip/Slow RAM byte:
16,384 controlled and 16,384 original-child calls per entry. Controlled
contracts compare every child-entry CPU/SR/RAM snapshot and cover all 279
owned boundaries. Returned registers, flags, valid cursors and RAM change;
only glyph/fault loop words are retained to bound those original loops.

Original-child fixtures keep projection behind the original view plane,
text at the original column bound, and the outer HUD behind its original
context gate while the clock is held. Their limited per-entry coverage is
shown above. They do not prove active drawing, fault or DMA paths. Ordered
Custom packets and terminal registers, effective flags, counters and latches
are independently checked; these bounded fixtures have zero writes.

Actual ON/shadow/sandbox dispatch passes 18,432 complete bounded fixtures,
exact one-call hardware-free classification and OFF, selection, non-call,
source-byte and stack guards. Its bounded coverage is retained separately.
Independent normal C, with only timing steps disabled and busy/range/owns
contracts intact, passes all six owners: 8,780 shadow / 13,276 sandbox matches.
C0CFFA is cold in both qualification recordings and active in demo01; the
record transform and weapon cue have zero sandbox matches in this group,
with completed shadow comparisons retained. Hardware and incomplete counts
are explicit in the checkpoint; the strict generic zero-call rejection is
unchanged.

All 36,236 isolated live RGB444 frames and final RAM seals match. Local DMA
passes 279 instructions / 8,928 cases. The independent union and fresh
combined DMA pass 24,703 instructions / 790,496 cases: previous 24,511
plus 279, overlap 87. All 29 older generator families are byte-identical.
The new stack MOVEM.W and unsigned multiply recipes are family-local.
Shared CPU/bus/arithmetic, runtime, classifier, observer and user-owned
native scripts/allowlist are unchanged.

GNU headless and MSVC Release pass. Full 592-row replay passes 568,446 shadow
/ 443,868 sandbox matches, zero mismatches, exact seals and poison frames.
The family is exact through frame 600; ALL retains frame 416 / 361 pixels.
Build/ is 1.524 GiB. Initial generation rejected unsupported MULU; the linked
probe then rejected missing step/owns symbols. Recipes were placed before
validation and bridges generated successfully; the failed build log remains.

Next complete C0D04C history projection and C33370 postflight display:
619 unique / zero shared boundaries and original incoming calls are sealed
in analysis/data/hud_history_display_scope_inventory.json. Other older
rendering parents and C1FE24/C1FE46/C0CF98 indirect/table entries remain
function work, together with C2C392's computed transfer, C1612C graphics-wait
integration and the full original game call/callback graph. Kickstart and
timing remain deferred under the user's stopping condition.

Evidence: analysis/data/hud_projection_parents_source_scope.json and
analysis/figures/native_hud_projection_parents_checkpoint.json. Reproduce
with audit_hud_projection_parents_source.py, check_hud_projection_parents.py
--kind contract/real --cases 16384, check_hud_projection_parents_dispatch.py
--cases 1024, check_whole_call_glue.py with the six entries, and the standard
full replay/live/MSVC/DMA gates.
