# Flight update, projection and cockpit timing

Updated 2026-10-02. This continues timing parity after commit 325b0df0.
Twelve existing readable-C entries replace fixed charges with resumable
source timing. Readable coverage remains 419/624; 175 registered entries now
have timing steps. Domain implementations remain unchanged.

## Source contracts

| Entries | Behavior | Original instructions |
| --- | --- | ---: |
| C230B0, C231A2 | Selection release and paired-record guard | 12 + 38 |
| C244E2 | Selected-record range classification, shared C245AA tail | 98 |
| C2374C, C2574A | Selected-fire record initializer and vector normalization | 179 + 56 |
| C1C54E, C1C2C8 | Projection seeding and target-point adjustment | 70 + 103 |
| C254E8, C122A2 | View octant and attitude flags | 36 + 86 |
| C25704 | Message publication and word flag updates | 17 |
| C2559A, C25864 | Cockpit-slide sequence and list reset | 48 + 3 |

Authority is the sealed original bytes and `python tools/recomp/port_info.py
ENTRY` listings. glue_flight_update_step.c retains registers, flags, source-
width arithmetic, bus accesses and instruction/event boundaries. It preserves
actual C3316E/C1D974/C082B8 child calls and shared tails. C2374C includes the
early C23744 rejection; C2559A includes C25592/C25598. The unregistered
C2377E sibling retains its original execution. No interpreter opcode handlers
or measured average instruction charges are used in the new bridge.

Unsigned division reuses the existing operand-dependent helper, including
zero divisors and overflow. Unsigned multiply charges the source word's set
bits; signed multiply retains sign transitions. MOVEM.W sign extension,
register high halves, postincrement order, stack word/long stores and PC-relative
table addressing follow the original. The DMA oracle caught the original
two-cycle reduction for register BSET with a bit below 16; that correction is
retained. CLR does not introduce an extra read.

## Verification

```sh
python tools/recomp/check_active_planes_step.py --group flight_update --bus
python tools/recomp/check_active_planes_step.py --group all --bus
python scripts/probe_recomp_timing.py C230B0,C244E2,C1C54E,C254E8,C122A2,C1C2C8,C2374C,C2574A,C25704,C231A2,C2559A,C25864 ALL --frames 600
```

The twelve-entry oracle matches all 746 original instructions across 23,872
fixtures with varied five-plane DMA phases. It compares every register, full
SR, PC, cycles and Chip/Slow RAM. Fixtures include signed word boundaries,
varied register high halves, guard masks, stack restoration, and fitting,
overflowing and zero-divisor unsigned division. Structural checks hold custom
writes and chipset events equally; live replay separately proves scheduling
and hardware effects on the recorded paths.

The combined batch matches fresh source OFF RGB444 on all 36,236 isolated
frames: demo01 20,833, carrier success 12,353, crash failure 3,050. The same
isolated ON replays match the sealed final RAM SHA-256 for each recording.
GNU and MSVC Release builds pass. The complete timing oracle matches 9,158
instructions across 293,056 DMA fixtures. The full 419-entry gate matches
703,357 completed shadow and 1,110,694 sandbox calls, with zero mismatches,
exact sealed final RAM and identical poison frames. Source-timed parents can
absorb formerly counted child calls; these totals are not a coverage metric.
No proof classification is suppressed. Some paths remain unexercised by the
recordings; their source instructions are checked independently. C1C2C8
has no direct gate calls. Shadow retains three incomplete C230B0, five
C244E2 and three C231A2 calls; these are not counted as matches. Those
entries independently match 11,587, 5,794 and 17,379 sandbox calls. After
scratch cleanup, build/ is 0.179 GiB; compiler objects and small proof logs
remain cached.

Use this `PORTS_ONLY` selector with `scripts/recomp_live_check.sh`:

```text
C230B0,C244E2,C1C54E,C254E8,C122A2,C1C2C8,C2374C,C2574A,C25704,C231A2,C2559A,C25864
```

The short isolated batch matches through frame 600. ALL still first differs
at one-based RGB frame 416 by 361 pixels. These corrections remove proved
call timing debt; they do not establish whole-game parity.

## Boundary evidence and next work

Fresh 500-frame demo OFF/ALL ON traces over C0EFD4-C0F3C4 have 36,198 source
and 36,090 ON instruction rows. The first 31,260 selected instruction rows
match every recorded field. The first enclosing instruction difference moves
from C0F01C after C1C63E in machine frame 296 (+1,442 cycles) to C0F090
after C0DAEE in frame 310 (+386). Source/ON cycles there are
44,177,236/44,177,622; cycles, next event and beam line differ, while registers
and SR match.

| First update boundary | Source frame | ON minus OFF cycles |
| --- | ---: | ---: |
| C0F008 after view controls | 296 | 0 |
| C0F01C after C1C63E | 296 | 0 |
| C0F030 after matrix routing | 296 | 0 |
| C0F036 after projection seeding | 296 | 0 |
| C0F03C after octant selection | 296 | 0 |
| C0F042 after attitude flags | 296 | 0 |
| C0F048 after terrain refresh | 296 | 0 |
| C0F056 after cockpit slide | 296 | 0 |
| C0F05C after list reset | 296 | 0 |
| C0F062 after display-record preparation | 297 | 0 |
| C0F082 after map stage | 310 | 0 |
| C0F090 after fixed matrix mark | 310 | +386 |

The C22C80-C230B0 flight-parent trace has 2,345 instruction rows on each
side. Its first 146 rows, including the complete first call in frame 296,
match every field. C230B0, C244E2 and C09E06 now return exactly on that
call; C09E06 remains unregistered and calls C230B0. Its former +42 cycles
were therefore not evidence that its own original execution needed a fee.
The first later difference is C22C80 entry in frame 318, +82 cycles inherited
from intervening work (source/ON 45,283,308/45,283,390).

An intermediate nine-entry trace retained +122 cycles after C1C63E. The first
inner difference was C22F6C after C231A2, +20 cycles. Source-timing that guard
made the complete first flight parent exact. The next enclosing gap was
C0F056 after C2559A, +256 with a dead-SR difference; the cockpit-slide/list-
reset pair removes it. These observations motivated the related additions
before final full verification. Intermediate costs are diagnostic observations,
not replacement charges.

Next, inspect and source-time the registered C0DAEE matrix marker with its
C2EC9C/C2ECA4 projection family. Preserve source-selected pixel/line children
and the early rejection/fault tails. C0DAEE has 32 original instructions,
including nine signed multiplies and a C2EC9C call; its fixed charge is 1,100.
Both projection entries still charge 650. Independently prove their source
timing before attributing the +386 to either parent or child. C1CB14/C1CB26
remain unregistered downstream parents. This enclosing gap does not establish
which inner instruction causes the combined frame-416 RGB difference.

Reproduce with separate OFF/ON `FA18_BOUNDARY_TRACE` paths and ranges
C0EFD4-C0F3C4 and C22C80-C230B0, the demo state/input, 500 frames,
`--ports off`/`on`, and `FA18_BOUNDARY_TRACE_MAX_MIB=64`. Task CSVs and
scaffolding are removed after recording evidence; small proof logs/reports
remain cached. C1D10C remains the next unregistered readable-C count batch.
Complete game source, then native backend and necessary OS replacement remain
the full objective.
