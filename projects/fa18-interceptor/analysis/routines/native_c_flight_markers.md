# Complete grid, scene-label and record-marker owners

2026-10-03. Five complete original game functions now have readable C and
source timing. Registration rises to 514 translated plus 66 source-only
entries, 580 rows; 404 entries have source timing (338 translated plus 66
source-only). The original game-call/callback graph remains incomplete.

| Original entry | Complete behavior | Owner PCs |
| --- | --- | ---: |
| C2AFFA | Unpack signed X/Z offsets and transform a view-space point | 34 |
| C2B3C2 | Project the scene-position stream and number visible rows | 124 |
| C2B564 | Two grid runs followed by all sixteen record markers | 167 |
| C2B928 | Project the class-20 marker and consume its segment stream | 40 |
| C2B952 | Record-marker visibility, cached nibble adjustment and glyph selection | 135 |

The manifest seals 468 unique / 32 shared instruction boundaries, original
bytes, actual original incoming JSR/BSR sites, and 17 distinct / 18 per-owner
child sites. The shared C2BAA8 tail consumes signed byte triples and draws
horizontal marker segments. Original tables and drawing/projection children
remain authoritative. No production child result or behavior is substituted.

The domain preserves partial byte/word register writes, signed matrix products,
MOVEM.W sign extension, swapped packed coordinates, saved stack values and
LINK/UNLK frames. Both grid axes retain their distinct label clamps and loop
increments. Records retain the original activation/class/bit gates, selected
record visibility, cached coordinate nibble wrap, angle buckets and repeated
marker segments. Signed ADD/SUB branches retain N xor V behavior.

All 163,840 complete fixture calls match all sixteen CPU registers, PC, full
SR and every byte of Chip and Slow RAM: 16,384 controlled and 16,384 original-
child calls per owner. Controlled coverage is 34/34, 124/124, 167/167, 40/40
and 135/135. Original-child coverage is 34/34, 120/124, 132/167, 40/40 and
129/135. Raw parent traces include nested point and marker PCs; reported
coverage intersects each owner's sealed source PCs. Unobserved original-
child branches are retained individually in the checkpoint.

Controlled children compare complete CPU/SR/RAM entry states, then return
changed registers, cursors, flags and RAM. Their projected-coordinate and
line-endpoint returns exercise rejection, label clamps and marker segments.
These are explicit test contracts. Original children are checked separately,
with a bounded frozen-clock service-loop observation and no invented return.
Fixtures vary valid streams, inactive/active records, signed word/long edges,
angle bucket thresholds and cached coordinate nibbles independently.

Actual ON/shadow/sandbox dispatch passes 30,720 complete fixtures and all OFF,
selection, non-call and changed-source guards. All 2,048 fixtures per owner
and mode are hardware-free, requiring exact matched classification. Bounded
dispatch owner coverage is 34/34, 120/124, 132/167, 40/40 and 121/135.

Independent normal C, with timing steps removed, passes 20,090 shadow /
20,093 sandbox recording comparisons for C2B3C2/C2B564. The other three
owners have zero calls in all sealed recordings, including with their parents
omitted. Both generic zero-comparison rejections and all raw hardware/incomplete
rows remain explicit. Cold function correctness is supported by complete-call
fixtures and sealed original calls, not by a claimed recorded comparison.

Drawing registrations use the existing source-first DMACONR proof contract
for their original children. Without it, the carrier shadow run failed its
final-RAM seal despite zero call mismatches; with it, the seal is exact.
Shared runtime, bus, arithmetic and classification code remain unchanged.

Local DMA proof matches registers, SR, PC, cycles and all RAM for 468
instructions / 14,976 cases. Fresh combined DMA and an independently derived
union cover 23,061 instructions / 737,952 cases: previous 22,593 plus all 468,
with no overlap. All 23 older generator families produce identical output.

GNU headless and MSVC Release builds pass. The full 580-row recording gate
passes 568,155 shadow / 439,147 sandbox matches, zero mismatches, exact RAM
seals and identical poison frames. All 36,236 isolated live frames and RAM
seals match. The family is exact through frame 600; ALL retains the deferred
frame-416 / 361-pixel Copper difference.

Implementation is `port/game/flight_markers.c`, its typed CPU adapter and
generated timing bridge. Source audit and complete-call/dispatch proofs are
`tools/recomp/audit_flight_markers_source.py`, `check_flight_markers.py` and
`check_flight_markers_dispatch.py`. Counts, source/report/log hashes and limits
are recorded in `analysis/figures/native_flight_markers_checkpoint.json`.

Next reconstruct six complete projection/readout/sweep owners C2ECA8/C32A44/
C32AC8/C33F70/C33F8A/C33FB4: 264 unique / 85 shared boundaries, sealed in
`analysis/data/projection_readout_scope_inventory.json`. C2C392's signed
computed transfer and C1612C's graphics-wait integration remain separate open
work. Stop after all original game functions are complete and only Kickstart
services and timing remain.
