# Complete history, postflight display and indirect stream owners

2026-10-04. Five partial adapters now own complete readable C with actual
original child contracts. Registration remains 526 translated plus 66
source-only entries, 592 rows. Source-timed entries rise from 451 to 456
(390 translated plus 66 source-only). Game-function porting remains open.

| Original entry | Complete behavior | Controlled PCs | Original-child PCs |
| --- | --- | ---: | ---: |
| C0D04C | History traversal, view transform, scale and interpolated circles | 237/237 | 232/237 |
| C33370 | Numeric and analog altitude/speed tapes, centre cue and heading | 382/382 | 165/382 |
| C1FE24 | Stream point and original projection child | 8/8 | 8/8 |
| C1FE46 | Stream point and original fixed-mode projection child | 8/8 | 8/8 |
| C0CF98 | Stream circle and original scaled-circle child | 8/8 | 8/8 |

The source seal contains 643 unique / zero shared instruction boundaries and
48 distinct / per-owner child sites. Direct incoming calls are sealed for
the two parents. The three stream owners retain the exact original ANDI,
table LEA, indexed pointer load and JSR at C1F942, return C1F944, their
original table slots and observed translated edges. No whole-table extent
or selector guard is invented. Only the five wrappers are replaced; three
now-unused sole-owner glue files are removed.

History retains original relative-world products, signed overflow-sensitive
branches, circular index updates, byte loop count and frame locals. Its
previous-radius word is the original validity test. Word MOVEM saves and
restores sign-extended registers and source stack effects. Circle children
run at the original quarter, half, three-quarter and current-point sites.
Postflight display retains complete numeric and analog paths, rounding,
caps, tick loops, text placement, centre-child branch and three-byte BCD
heading increments/decrements including invalid byte patterns. Original
returned CPU state and RAM drive the next operation. Stream owners retain
original vertex addressing, colour/radius reads and saved registers.
Domain and whole-call glue invoke no opcode handlers. Generated timing
bridges remain separate; no convenience loop bound or service return is added.

All 163,840 whole calls match every register, PC, full SR and all Chip/Slow
RAM bytes: 16,384 controlled and 16,384 original-child calls per owner.
Controlled contracts compare complete child-entry CPU/SR/RAM snapshots,
change returned registers/flags/cursors/RAM, and cover all 643 boundaries.
Original-child history uses original behind-view zero-XY projection and
history-count bounds; display uses the numeric record class and original
text column bounds. Analog tapes are proven through controlled children.
The missing original-child PCs are retained explicitly in the checkpoint.
Frozen-clock fixtures do not claim active glyph, fault or DMA behavior.
Ordered Custom writes and terminal registers, effective flags, counters
and data latches match; bounded fixtures produce zero Custom writes.

Actual ON/shadow/sandbox dispatch passes 15,360 complete bounded calls,
exact hardware-free classification and all OFF/selection/non-call/source
byte/stack guards. The three stream owners use the original indirect
caller. Independent normal C disables only timing steps and preserves
busy/range/owns contracts: all five owners complete comparisons, totaling
5,901 shadow / 8,073 sandbox. The three stream owners are cold in both
qualification recordings and active in demo01. Per-recording hardware,
incomplete and comparison counts are retained; strict zero-call rejection
in the generic checker is unchanged.

Full 592-row replay passes 568,446 shadow / 443,868 sandbox matches, zero
mismatches, exact RAM seals and identical poison frames. All 36,236 isolated
live RGB444 frames and final RAM seals match. Local DMA passes 643
instructions / 20,576 cases. The independently checked union and fresh
combined DMA pass 25,346 instructions / 811,072 cases (previous 24,703
plus 643, overlap zero). All 30 older generator outputs are byte-identical.
New signed divide, SUBA.W, SBCD and stack MOVEM.W timing recipes are local
to this family. Shared runtime/bus/CPU/arithmetic/observer/classification,
earlier complete HUD domains and user-owned native files are unchanged.
GNU headless and MSVC Release pass. Family timing is exact through frame
600; ALL remains frame 416 / 361 pixels. Build/ occupies 1.562 GiB.
The initial fixture build rejected a hexadecimal literal ending in e
adjacent to +; adding the unsigned suffix fixed tokenization. Its failed
build log remains. No production behavior workaround was required.

Next: ten older render parents, 1,101 unique / zero shared boundaries,
with original direct and six indirect callers sealed. C2C392's computed
transfer, C1612C graphics-wait integration and the full original cold,
indirect and callback graph remain game-function work. Kickstart and timing
remain deferred under the user's stopping condition.

Evidence: analysis/data/hud_history_stream_source_scope.json,
analysis/figures/native_hud_history_stream_checkpoint.json and
analysis/data/legacy_render_parents_scope_inventory.json. Reproduce with
audit_hud_history_stream_source.py, check_hud_history_stream.py --kind
contract/real --cases 16384, check_hud_history_stream_dispatch.py --cases
1024, check_whole_call_glue.py with the five owners and standard full
replay/live/MSVC/DMA gates.
