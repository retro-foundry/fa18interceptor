# Complete face-list and edge parents

2026-10-04. Fourteen older adapters now own complete typed C. Registration
remains 526 translated plus 66 source-only entries, 592 rows. Source-timed
entries rise from 481 to 494 (428 translated plus 66 source-only); C2005C
already had a timing bridge. Game-function porting remains incomplete.

| Original entry | Complete behavior | PCs in both controlled and original-child calls |
| --- | --- | ---: |
| C1FF0A | Every tested triangle and original rejection skip | 16/16 |
| C2005C | Complete indexed polygon and shared tested-face tail | 54/54 |
| C20100 | Every list descriptor and returned predicate/clip state | 53/53 |
| C21060 | Every quad, original saved pointers and accumulated result | 39/39 |
| C20C38 | Styled grid prefix and every cell | 83/83 |
| C20C22 | Plain grid prefix and every cell | 80/80 |
| C20A52 | Styled lattice and both distinct passes | 134/134 |
| C20A40 | Plain lattice and both distinct passes | 131/131 |
| C20002 | Derived fourth point and complete tested-face tail | 51/51 |
| C2084A | Both normalizers and signed edge/eye alignment | 43/43 |
| C2082A | Original near-plane gate and shared alignment tail | 52/52 |
| C219AE | All block copies, byte flag clearing and original shifts | 38/38 |
| C21C4C | Every half/quarter derived point | 23/23 |
| C21C2E | Actual split child and complete workspace split | 30/30 |

The sealed source has 533 unique / 294 shared boundaries, twelve distinct
and nineteen per-owner child sites. Original direct callers, indirect table
slots and caller bytes are sealed without assuming a complete selector domain
or inventing a guard. Complete shared tails remain explicit.

Readable behavior lives in face_list_parents.c. Typed glue calls actual
children once and consumes their complete returned state, preserving original
stack transfers, source counts, word arithmetic, MOVEM sign extension,
overflow-sensitive product branches and ordered RAM writes. C21060 now owns
its full quad loop. The two lattice passes retain their different pointer
saves and result accumulation. C2084A retains both normalizers and original
near/far tables. Old batches 57/58/66 are removed; other exports in batches
25/47/48/49 are byte-identical. Domain and whole-call glue use no opcode
handlers; source timing remains separate.

All 458,752 whole calls match every register, PC, full SR and every Chip/Slow
RAM byte: 16,384 controlled and 16,384 original-child calls per owner. Both
kinds cover all owned boundaries for every owner. Controlled children compare
full entry CPU/SR/RAM and vary returned state. Original drawing children use
degenerate geometry or original behind-view rejection. Frozen-clock fixtures
make no active fill/outline/DMA claim. Ordered Custom packets and terminal
hardware match with zero fixture writes. Independent recordings exercise
actual drawing. The controlled probe initially missed C208BE; decoupling the
range fixture from its near-plane gate covered it without production changes.

Actual ON/shadow/sandbox dispatch passes 43,008 calls with all boundaries,
guards and strict source classification checked. Per mode there are six
hardware-bearing calls (three C1FF0A and one each C20002/C2005C/C20100),
14,330 hardware-free calls and zero observed Custom writes. Exact terminal
hardware and source classification are independently checked.

Normal C disables only timing steps and retains busy/range/ownership contracts.
It passes every owner: 70,334 shadow / 92,495 sandbox matches. The initial
fourteen-owner checker correctly rejected zero calls for C21C4C, absorbed by
its parent. That failed log and its raw reports remain sealed. An independent
original recording selection passes C21C4C with 165 shadow / 166 sandbox
comparisons; verify_face_list_parents_recordings.py requires these positive
comparisons and every other owner's existing comparisons. Generic zero-call
rejection is unchanged. All owners are active in demo01; qualification
recordings call C1FF0A, C2005C, C20A40 and C20002. Hardware/incomplete rows
remain explicit.

All 36,236 isolated live RGB444 frames and final RAM seals match. The full
592-row gate passes 568,446 shadow / 443,868 sandbox, zero mismatches, exact
seals and identical poison frames. Local DMA passes 533 instructions / 17,056
cases. Independent union and fresh combined DMA pass 27,543 / 881,376:
previous 27,064 plus 533, overlap 54 (the already-timed C2005C boundaries).
All 33 older generator outputs and shared runtime/CPU/bus/arithmetic/observer/
classification and earlier completed domains are unchanged. GNU headless and
MSVC Release pass. Family timing is exact through frame 600; ALL remains
frame 416 / 361 pixels. Build/ occupies 1.676 GiB.

Next fifteen ground/HUD mark/panel/blit parents have 1,352 unique / zero
shared boundaries and original callers sealed. Selected-segment indirect
calls, other older adapters, C2C392's computed transfer, C1612C graphics-wait
integration and the complete original cold/indirect/callback graph remain
game-function work. Kickstart and standalone timing work remain deferred.

Evidence: analysis/data/face_list_parents_source_scope.json,
analysis/figures/native_face_list_parents_checkpoint.json and
analysis/data/hud_render_parents_scope_inventory.json. Reproduce with
audit_face_list_parents_source.py, check_face_list_parents.py --kind
contract/real --cases 16384, check_face_list_parents_dispatch.py --cases 1024,
the fourteen-owner check_whole_call_glue.py selection (retained zero-call
rejection), its independent C21C4C selection, verify_face_list_parents_recordings.py
and the standard replay/live/DMA/MSVC gates.
