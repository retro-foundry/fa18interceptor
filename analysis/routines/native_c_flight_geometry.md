# Complete history, zone-exit and candidate geometry owners

2026-10-03. The four previously registered owners now have complete readable
C and complete source timing. Registration remains 509 translated plus 66
source-only entries, 575 rows; source-timed entries rise from 395 to 399
(333 translated plus 66 source-only). This is function-porting progress;
the original callable/callback graph is still incomplete.

| Original entry | Complete behavior | Owner PCs |
| --- | --- | ---: |
| C2651E | Position-history publication and count maintenance | 50 |
| C28E28 | Selected-record zone exit, placement and fault child | 79 |
| C26EBE | Candidate scan, polygon detail passes and level-volume collisions | 819 |
| C27456 | Candidate-face triangle tests and signed stream termination | 67 |

The source manifest seals 1,015 unique / zero shared instruction boundaries,
original bytes, all observed original incoming JSR/BSR sites, and four child
sites. Children remain actual calls: C06C02/C28F16 from the zone owner and
the two C27456 calls from the candidate owner. Source ownership includes cold
branches and original return prefixes, rather than only recorded paths.

The domain preserves mixed byte/word register halves, address sign extension,
LINK/UNLK frames, saved word indexes, changed child cursors and flags, MOVEM
ordering, variable shifts and signed ADD/SUB overflow branches. The candidate
scan retains both polygon passes, edge acceptance, side statistics, linked
volume traversal, special points and the original repeated velocity correction.
The first zero-class correction uses the original /256 sequence; subsequent
attempts retain the distinct /2048 sequence. No substitute collision model or
invented child result is used in production.

All 131,072 complete fixture calls pass all sixteen CPU registers, PC, full
SR, and every byte of Chip and Slow RAM: 16,384 controlled and 16,384 actual
child calls per owner. Controlled owner coverage is 50/50, 79/79, 819/819 and
67/67. Actual-child coverage is 50/50, 79/79, 816/819 and 67/67. The candidate
actual-child trace contains 883 PCs, including nested face-helper instructions;
its coverage count is intersected with its own sealed source PCs.

Controlled calls compare the complete CPU/SR/RAM state at each original child
entry and return changed working registers, cursors, flags and RAM. One explicit
controlled detail-child return changes the saved pass index and polygon
terminator to exercise the parent's velocity-clear branch C27448. This is a
parent input/output contract; it does not claim the original face child makes
those writes. Actual-child proof leaves original children intact and bounds
execution without substituting returns. Its unobserved candidate PCs are
C27186/C27188/C27448, retained explicitly in the checkpoint.

Actual ON/shadow/sandbox dispatch passes 3,072 complete fixtures and all
OFF, selection, non-call and changed-source guards. Every reference fixture
is hardware-free and requires one matched classification. Dispatch fixture
coverage is 47/50, 77/79, 702/819 and 67/67; the candidate raw count of 766
includes its nested face helper. These bounded dispatch checks do not claim
complete branch coverage.

Independent normal C, with selected instruction steps removed, passes 16,826
shadow / 17,899 sandbox recording comparisons for the four-owner selection.
A separate face-helper selection, with its candidate parent omitted, passes
1,505 shadow / 1,564 sandbox comparisons. All three recordings and both modes
retain their calls, matches, hardware and incomplete classifications in the
checkpoint. No zero-comparison exception was introduced.

Local DMA instruction proof passes 1,015 boundaries / 32,480 cases, comparing
CPU, full SR, PC, cycles and all RAM. A fresh combined DMA proof and independently
derived instruction union cover 22,593 boundaries / 722,976 cases: the previous
21,578 plus all 1,015, with no overlap. All 22 older generator families produce
identical output. Shared production/proof CPU, bus, memory, arithmetic and
classification fixtures remain unchanged.

GNU headless and MSVC Release builds pass. The full 575-row recording gate
passes 559,969 shadow / 417,343 sandbox matches, zero mismatches, exact RAM
seals and identical poison frames. All 36,236 isolated live frames and seals
match. The family is exact through frame 600; ALL remains at the deferred
frame-416 / 361-pixel Copper difference.

Implementation is in `port/game/flight_geometry.c` and its typed CPU adapter
and generated timing bridge. The source audit, complete-call and actual
dispatch proofs are `tools/recomp/audit_flight_geometry_source.py`,
`check_flight_geometry.py` and `check_flight_geometry_dispatch.py`. Detailed
counts, source/report/log hashes and limitations are in
`analysis/figures/native_flight_geometry_checkpoint.json`.

Next reconstruct five complete flight-action setup owners C2AFFA/C2B3C2/
C2B564/C2B928/C2B952: 468 unique / 32 shared boundaries, sealed in
`analysis/data/flight_action_setup_scope_inventory.json`. These are unregistered
translated entries with actual original incoming calls. Related C2C392 still
needs its signed action-byte computed transfer at C2C46E reconciled. C1612C
graphics-wait integration remains open. Stop only after all original game
functions are complete and the remaining work is Kickstart services and timing.
