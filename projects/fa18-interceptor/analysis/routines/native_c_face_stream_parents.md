# Complete face-stream parents

2026-10-04. Seventeen partial segment, face, grid and block-stream adapters
now own complete typed C. Registration remains 526 translated plus 66
source-only entries, 592 rows. Source-timed entries rise from 465 to 481
(415 translated plus 66 source-only); C212B0 already had a timing bridge.
Game-function porting remains incomplete.

| Original entry | Complete behavior | PCs in both controlled and original-child calls |
| --- | --- | ---: |
| C2129C | Near-plane prefix and every segment pair | 37/37 |
| C212B0 | Complete shared pair loop and last-pair flag | 31/31 |
| C211DC | Every counted segment and accumulated result | 21/21 |
| C2131C | Every translated pair and saved base word | 39/39 |
| C20E4E | Four-corner face with eight-plane style | 35/35 |
| C20E40 | Shared face tail with two-plane style | 36/36 |
| C21490 | Near-plane face and original positive-offset Z reads | 33/33 |
| C2139E | Offset face and ordered fourth-corner writes | 32/32 |
| C21412 | Mixed face and returned clip state | 41/41 |
| C20F10 | All derived parallelogram points | 38/38 |
| C20EC4 | Signed scale and shared derived-point tail | 66/66 |
| C20D68 | Every grid row and segment | 59/59 |
| C20904 | Both lattice passes and differing saved pointers | 91/91 |
| C21A20 | Three block copies and original stream skip | 92/92 |
| C217EA | All scaled block writes and reloads | 77/77 |
| C2159E | Both dot-product branches and original clip sites | 72/72 |
| C210E6 | Every constructed strip quad and result | 72/72 |

The source seal contains 773 unique / 99 shared boundaries and thirteen
distinct / fifteen per-owner child sites. Shared tails remain complete for
C2129C/C212B0, C20E4E/C20E40 and C20F10/C20EC4. Original direct callers,
exact indirect table slots and C1F942 caller bytes are sealed. This evidence
does not establish the entire selector domain and introduces no guard.

Readable behavior lives in face_stream_parents.c. Typed glue calls every
original child once, receives its full returned state and preserves the
original stack transfers. Source word/long writes, signed shift counts,
overflow-sensitive dot-product branches, MOVEM sign extension, source RAM
loop counts and read-after-write ordering are preserved. The two lattice
passes retain their different A3 saves; the second pass consumes the actual
returned pointer. C21490 retains the original positive-offset Z reads.
Old batches 53/54 and their private replay helpers are removed. Batch57
retains the existing C21060 body byte-for-byte; that parent remains work.
Domain and whole-call glue use no opcode handlers; source timing is separate.

All 557,056 whole calls match every register, PC, full SR and every Chip/Slow
RAM byte: 16,384 controlled and 16,384 original-child calls per owner.
Both kinds cover every owned boundary for every owner, including the shared
tails. Controlled children compare complete entry CPU/SR/RAM snapshots and
change all returned registers except the source-owned frame and stack.
Fixtures include all CCR values, word edges, signed and large masked shifts,
bounded source counts, view rejection and both side-face branches.

Original drawing children receive degenerate geometry or the original
behind-view rejection. These frozen-clock fixtures make no active fill,
outline or DMA claim. Ordered Custom packets and terminal hardware/latches
match with zero fixture writes. Independent sealed recordings exercise the
actual drawing paths. Initial probes all passed; the second real-child
fixture extended rejection coverage without changing production code.

Actual ON/shadow/sandbox dispatch passes 52,224 complete calls, every owner
boundary, exact hardware-free classification and all guards. Independent
normal C disables only timing steps, retains busy/range/ownership contracts
and passes every owner: 22,333 shadow / 26,010 sandbox matches. All owners
are active in demo01; the qualification recordings call C212B0, C20D68 and
C20904 while the other fourteen remain cold. Raw hardware/incomplete rows
remain explicit. Generic zero-call rejection is unchanged.

All 36,236 isolated live RGB444 frames and final RAM seals match. The full
592-row gate passes 568,446 shadow / 443,868 sandbox, zero mismatches,
exact seals and identical poison frames. Local DMA passes 773 instructions /
24,736 cases. Independent union and fresh combined DMA pass 27,064 /
866,048: previous 26,324 plus 773, overlap 33. The overlap consists of the
31 already-timed C212B0 boundaries and the two C214FC/C214FE rejection-tail
boundaries shared with the earlier C21500 owner. All 32 older generator
outputs are byte-identical. Shared runtime, CPU, bus, arithmetic, observer,
classification and earlier complete domains are unchanged. GNU headless and
MSVC Release pass. Family timing is exact through frame 600; ALL remains
frame 416 / 361 pixels. Build/ occupies 1.637 GiB.

Next fourteen tested/list/grid/lattice face and edge owners have 533 unique /
294 shared boundaries and original callers sealed. Selected-segment indirect
calls, other older adapters, C2C392's computed transfer, C1612C graphics-wait
integration and the complete original cold/indirect/callback graph remain
game-function work. Kickstart and standalone timing work remain deferred.

Evidence: analysis/data/face_stream_parents_source_scope.json,
analysis/figures/native_face_stream_parents_checkpoint.json and
analysis/data/face_list_parents_scope_inventory.json. Reproduce with
audit_face_stream_parents_source.py, check_face_stream_parents.py --kind
contract/real --cases 16384, check_face_stream_parents_dispatch.py --cases
1024, check_whole_call_glue.py with the seventeen owners and the standard
replay/live/DMA/MSVC gates.
