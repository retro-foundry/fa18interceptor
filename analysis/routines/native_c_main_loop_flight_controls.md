# Complete main-loop control and flight owners

Updated 2026-10-03. Complete C149BE/C23A7E are newly registered; existing
C083E2/C25754/C24568/C2436A now have complete normal CPU/SR adapters and source
timing. The registry has 490/624 translated entries plus 66 source-only
callable entries: 556 rows and 376 timed entries (310 translated plus 66
source-only). Game-function porting remains incomplete. Stop after all original
game functions, including cold and indirect owners, are recreated and only
Kickstart services and timing remain. This checkpoint replaces no OS service.

The sealed `captures/native/demo01/state.bin` is authoritative.
`analysis/data/main_loop_flight_controls_source_scope.json` seals all six
complete owners: 1,416 unique boundaries, zero shared within this batch and
22 actual child sites. Original JSR/BSR bytes establish each callable entry;
C149BE is called at C25E26, and C23A7E has fourteen original update-stage call
sites. Earlier return arms and the later C243F2 target tail belong to their
original owners; the interleaved C2436A helper remains a distinct child.

| Owner | Complete behavior | Owned PCs | Real-child coverage |
| --- | --- | ---: | ---: |
| C149BE | Indexed control-record setup, normalisation and attenuation; source motion scaling and command adjustments; ground-height selection, contact/touchdown/recontact flags and timers; takeoff accumulation and non-primary control requests. | 493 | 168 |
| C083E2 | Original reset-message child, mode-dependent attempt byte, mission globals and three 41-long record clears, with original saved registers and DBRA outputs. | 35 | 35 |
| C25754 | Stack arguments, signed word magnitude preparation, actual length child and CCR-zero decision, power-of-four scaling, DIVU overflow/exception semantics, products and normalized-word publication. | 56 | 55 |
| C23A7E | Complete class-dependent record update, heading limits/blending, waypoint advancement and fault paths, reference/zone selection, projection and sight/status handling, autonomous motion and actual refresh children, selected-control targets and sticky proximity flags. | 711 | 102 |
| C24568 | Counter/period guards, original coarse/fine coordinate arithmetic, signed range magnitude and length child, hysteresis and script/state classification. | 82 | 70 |
| C2436A | Original counter/range guards, position differences and normalisation child, facing/alignment dot products and sight-bit publication. | 39 | 39 |

Readable behavior is in `port/game/main_loop_flight_controls.c`; CPU outputs,
frames, saved registers and original child calls are in its normal adapter.
The superseded four adapters and their unused register helpers are removed
from glue batches 24/27/49/63. Existing public math functions are unchanged.
Word/byte operations retain their original register halves, signed branches
retain overflow behavior, and child outputs and pointers are reloaded. The
normalizer tests the scale's lower word but uses its saved upper word for the
final result sign. Memory BTST with bit number eight tests byte bit zero.
These source behaviors are retained.

196,608 complete calls pass all sixteen registers, PC, full SR and all Chip/
Slow RAM without exclusions: 16,384 controlled-child and 16,384 real-child
cases per owner. Controlled cases cover every one of the 1,416 boundaries and
compare complete CPU/SR/RAM at actual child entries. Fixtures include command/
index combinations, waypoint advancement/sentinels, signed range and position
differences, projection byte halves, zone scanning, autonomous reset saves,
selected-target proximity and normalization sign/zero/overflow paths.

Two test-only ordered read contracts exercise explicit source reloads:
control bit 9 clears after its first word read, and record bit 6 clears after
its first byte read. Both original CPU and normal C receive the same original
read value before the publication. The test memory facade includes unchanged
production memory bodies with renamed exports. Production files and original
instructions are unchanged. These contracts prove reload behavior, not an
original hardware publisher or interrupt schedule.

Real-child coverage is partial as shown above. C149BE uses a valid airborne
fixture retaining actual normalization and attenuation children; its raw
217-PC observation includes nested owners, while 168 PCs belong to the parent.
C23A7E uses original class-20, class-30 and unclassified paths. No all-path
real clock, sound, fault-handler or projection-service completion is claimed.
The original minimum-word scale loop is retained and is not a completed
whole-call case. DIVU-zero exception entry is covered by instruction fixtures;
normalizer ROM-handler return is not established. Its normal adapter retains
the actual exception backend, with no invented zero result or service return.

Actual ON/shadow/sandbox dispatch passes 4,608 complete fixtures and all
non-call, OFF, selection and source-write invalidation guards. ON requires a
native continuation. Reference modes require strict completed classification;
all these real fixtures are hardware-free.

Independent step-disabled normal C passes 15,468 shadow / 15,714 sandbox
recording comparisons. The parent checker passes 9,444 / 9,637, and the helper
checker passes 6,024 / 6,077. C149BE retains five shadow / eight sandbox
hardware classifications and 199 shadow incompletes; C25754 retains 53 shadow
incompletes. There are no mismatches or sandbox incompletes. The six-owner
batch correctly rejects C083E2 for zero completed comparisons because whole
parents absorb nested helpers; its three zero-count helper reports and the
generic rejection are retained. All four helpers independently have completed
recorded comparisons. Ownership/start/end/busy/tail contracts remain intact
when steps are disabled.

Local instruction/DMA proof passes 1,416 instructions / 45,312 cases with CPU,
SR, PC, cycles and all RAM compared. A fresh combined run passes 19,855 /
635,360. The previous 18,889-PC union shares 450 boundaries with this scope;
they are counted once. All eighteen older generator outputs remain byte
identical; arithmetic-direction, MOVEM.W, EXG, DBRA, NEG and LSR recipes are
family-local. Shared runtime CPU/bus/memory/math/instruction fixtures are
unchanged.

GNU and MSVC Release builds pass. The full 556-row gate passes 553,158 shadow /
417,328 sandbox matches, zero mismatches, exact sealed RAM and identical poison
frames. Whole parents absorb nested calls, so comparison totals need not grow
with registry coverage. All 36,236 isolated live frames/seals match; the group
is exact through frame 600. ALL remains frame 416 / 361 pixels, with fade
deferred. C1612C remains inactive with its frozen graphics-wait failure explicit.
Evidence hashes and raw owner/report coverage are recorded in
`analysis/figures/native_main_loop_flight_controls_checkpoint.json`.

Next complete C230E8/C23116/C23186/C23228/C233AA/C23578/C236AA/C23716/C2377E/
C257EC. `analysis/data/flight_record_actions_scope_inventory.json` seals 575
unique / 245 shared boundaries and actual original JSR/BSR evidence for every
entry. This inventory implements none of those owners and replaces no OS
service. The seeded 624-entry denominator alone cannot establish completion.
