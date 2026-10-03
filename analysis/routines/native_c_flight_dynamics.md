# Complete flight dynamics and region dispatch

Updated 2026-10-03. Complete C25B66/C266AE/C28996/C28B16 and an upgraded
C28B34 shared stream owner are implemented in `port/game/flight_dynamics.c`.
Four new rows raise registration to 509/624 translated entries plus 66 original
source-only callable entries: 575 rows, 395 source-timed (329 translated, 66
source-only). C28B34 was already registered and adds no row. Original game
function and call/callback coverage remain open. Stop when only Kickstart
services and timing remain, as requested by the user.

`analysis/data/flight_dynamics_source_scope.json` seals 1,308 distinct original
instruction boundaries. C28B16 and C28B34 share 218 PCs; count that body once.
All five retain actual original incoming calls. The source auditor also checks
complete-call fixture ownership and both production/contract child-site tables
against the original instructions: 25 distinct sites, 27 per-owner sites.
Handwritten domain/CPU adapters invoke no opcode handler. Generated steps
retain original source, bus, event and cycle boundaries.

| Owner | Complete behavior | Owned PCs | Controlled PCs | Real-child PCs |
| --- | --- | ---: | ---: | ---: |
| C25B66 | Advance indexed record control gates, warnings, motion, cells, decay, collision responses, slot requests and history. | 554 | 554 | 415 |
| C266AE | Scan component/map descriptors, integrate scene motion, retry original collision consumers, damage records and test convex plane chains. | 416 | 416 | 416 |
| C28996 | Enter, retain and leave regions; release, replace or copy their records using the original ten-byte stream rows. | 107 | 107 | 107 |
| C28B16 | Apply source mode gates, load the region stream and dispatch its original record count. | 231 | 231 | 231 |
| C28B34 | Complete the shared record initializer, model/placement tables, dither, root positions, orientation child and count loop. | 218 | 218 | 218 |

163,840 completed calls pass all sixteen registers, PC, full SR and all Chip/
Slow RAM: 16,384 controlled and 16,384 real-child calls per owner. Controlled
children compare full CPU/SR/RAM at every reached original child boundary and
return changed values, cursors and flags. Real calls execute original children,
with explicit failure on the frozen-clock instruction budget. No synthetic
service return or internal segment replaces a complete call. C28996's raw
real trace includes nested region/body PCs; coverage above intersects the trace
with each owner's sealed set.

C25B66's real fixtures cover 415/554 PCs. Its collision-response paths have full
controlled-child proof, rather than a claim of full real-child coverage. Nine
PCs in the duplicate collision-class read arm use a test-only ordered input
publication: after C26EBE returns collision code 32 and nonzero flags, the first
class-byte read returns 0x30 and publishes zero for the next read. Both CPU and
normal C facades return the original first value. Production memory/source
bytes and flags are unchanged. This contract does not claim reconstruction of
the original interrupt publisher or universal unreachability of that arm.

The implementations preserve sign-extending word loads and address indices,
mixed-width register halves, byte/word DBRA counters, MOVEM save frames and
actual restored RAM values. Signed branches after ADD/SUB use N xor V. The
source's negative-X integration arm clears Z, and scene retry rollback uses
the ordinary velocity vector. The original eight attempts, gravity values,
duplicate MOVE, two writes to record byte 100 and stream write order remain.
Region creation preserves the orientation child's changed outputs except for
the original D0/A0/A2 save set. C28B34's prior register reconstruction is replaced
by its complete normal adapter and exact owned instruction bridge.

Actual ON/shadow/sandbox dispatch passes 3,840 complete hardware-free fixtures
and OFF/selection/non-call/source-write guards. Reference modes require exact
one-call matched classification. Fixture trace coverage is deliberately smaller
than the controlled full-source layer and is recorded separately.

Independent normal C disables only selected timing steps while retaining
ranges, ownership, busy-input and dispatch contracts. C25B66/C28996/C28B34 pass
5,711 shadow and 14,168 sandbox completed matches over three recordings.
Hardware and incomplete rows remain in the checkpoint. C266AE and C28B16 each
have zero calls/comparisons/hardware/incomplete in all six independent reports,
with their parents omitted. The unchanged generic checker rejects each for
zero completed comparisons. No recorded normal-C comparison is claimed for
those two cold owners.

Local DMA proof passes 1,308 instructions / 41,856 cases. A fresh combined run
and independent union pass 21,578 / 690,496: previous 20,488 plus 1,308, with
218 old C28B34 PCs overlapping. All 21 older generator outputs remain identical.
Shared production CPU/runtime/bus/memory/math and instruction/classification
fixtures remain unchanged. GNU headless and MSVC Release builds pass. The full
575-row gate passes 559,969 shadow / 417,343 sandbox matches, zero mismatches,
exact final RAM seals and identical poison frames. All 36,236 isolated live
RGB444 frames and seals match. The family is exact through frame 600; ALL
retains frame 416 / 361 pixels. Copper fade and service/timing work stay deferred.

Reproduce with `audit_flight_dynamics_source.py`, `check_flight_dynamics.py`,
`check_flight_dynamics_dispatch.py`, the local/all DMA instruction oracle,
strict whole-call checker, full recording gate and isolated live gate. Raw
log/report hashes and per-layer coverage are in
`analysis/figures/native_flight_dynamics_checkpoint.json`.

Next audit and upgrade complete C2651E/C28E28/C26EBE/C27456. Their original
incoming calls and 1,015 unique / zero shared PCs are sealed in
`analysis/data/flight_geometry_remaining_scope_inventory.json`, reproduced by
`audit_flight_geometry_remaining.py`. They are already registered, so these
upgrades must not increase the translated count. C2C392's signed action-byte
computed transfer at C2C46E still needs target-domain evidence. C1612C's existing
domain/CPU/step implementation remains unregistered pending graphics-wait
integration; Kickstart exception-handler return and the complete original
call/callback graph also remain open.
