# Complete renderer entries and writer callbacks

Thirty-seven original callable entries are complete. Readable behavior lives
in `port/game/render_leaf_helpers.c` and `port/game/planar_lane_masks.c`;
CPU effects and original child consumption live in their glue modules.
Source bytes, every incoming writer-table setup, all callback slots and
child contracts are sealed in `analysis/data/render_entry_helpers_source_scope.json`.
The evidence checkpoint is `analysis/figures/native_render_entry_helpers_checkpoint.json`.

| Original entry | Complete behavior |
| --- | --- |
| C2F688 | Shared pixel body with the actual incoming mask/writer tables |
| C2F63A | Signed view-bound square entry |
| C2F64E | Two-row square with actual child and individual saved words |
| C301F6 | Polygon using the original variable last row |
| C330FE | Eight-pixel glyph draw/clear and original individual long saves |
| Sixteen C2F786 table callbacks | Ordered single-row plane AND/OR masks |
| Sixteen C2F7E6 table callbacks | Ordered two-row masks, finishing each plane before the next |

Twenty-nine entries upgrade existing registered adapters; C2F688 adds one
translated entry. Seven cold writer callbacks add source-only entries:
C2F8BC, C2F8C6, C2F8D0, C2F938, C2F952, C2FA3C and C2FA56.
Registration is now 527 translated plus 73 source-only entries, 600 total.
Source-timed entries rise to 518 (445 translated plus all 73 source-only).
Timing-adapter presence alone does not certify older function completion.

The batch owns 550 unique / 235 shared instruction boundaries and seven
distinct / eight per-owner child sites. All 1,212,416 whole calls pass:
606,208 controlled-child and 606,208 original-child calls. They compare all
sixteen registers, PC, full SR, all Chip/Slow RAM, ordered Custom writes and
terminal registers, effective flags, blit counters and data latches.
Controlled children also compare complete entry CPU/SR/RAM at every call.
Writer fixtures exercise aliased planes and overlapping scanlines to check
original write order. Full returned child state is consumed once.

Controlled whole calls cover 546/550 boundaries; original-child coverage
is 546/550. Four cold NEG instructions, C30266,
C3027A, C30296 and C302E4, have 65,536 independent production-segment
calls. Those segment calls are separate from whole-entry coverage. All
per-owner missing PCs remain explicit in the checkpoint.

Original-child proofs execute actual source bytes with event service held.
Devices keep their original clock and busy-read behavior. C301F6 starts
a real blit on odd scenarios, adding five fixture packets per such case.
Actual ON/shadow/sandbox dispatch independently passes 113,664 calls using
production generated-child continuations. Each mode classifies 37,888
hardware-free / zero hardware-bearing source calls and validates 6,912
actual Custom writes; classification and packet observation are distinct.
The source harness resumes generated-child hardware-event yields as the
runtime does. Its original failed yield check is retained. Production core,
bus, observers and source bytes are unchanged. Admission/source-integrity
guards prove dispatch/counting; they do not certify cold generated exits.

The unchanged generic whole-C checker passes all thirty previously seeded
owners: 537,534 shadow / 571,393 sandbox
comparisons. The seven source-only writers have zero calls on every sealed
recording, so the generic check rejects them; that rejection is retained.
Their full-call and actual-dispatch fixture proofs remain separate.
All twelve preceding renderer helpers pass the shared-domain regression:
423,745 shadow / 530,198 sandbox.

All 36,236 isolated live frames and final RAM seals match. The full 600-row
gate passes 568,446 shadow / 443,870
sandbox comparisons, zero mismatches, exact seals and identical poison
frames. Original busy-input opt-ins remain unchanged.

Local DMA passes 550 instructions / 17,600 cases. Fresh combined DMA and
an independent union pass 28,931 / 925,792: all 550 local PCs overlap the
previous union. All thirty-six older generators are byte-identical.
Remaining exported bodies in old glue modules are unchanged; unused glyph
epilogue helpers are removed. GNU and MSVC Release builds pass. The initial
MSVC missing-source link failure and configure-path error remain recorded.

Family frames match through 600; ALL remains at frame 424 / 34,144 pixels.
Copper fade, timing and Kickstart services remain deferred. No production
selector guard, clamp, busy substitute or loop cap was added.

Next are complete selected-segment and projection/clipping parents and all
eight crossing helpers: C1FF9C/C1FFA4, C2ED70/C2EE4A, C2F0C6/C2F0F4/
C2F128/C2F156 and C2EA5A/C2EAD0/C2EB4C/C2EBC2. Their original instructions
are read; indirect JSR contracts still need sealing. Larger render parents,
older partial adapters, C2C392, C1612C graphics-wait integration and full
original cold/indirect/callback reconciliation still prevent declaring
game-function porting complete.
