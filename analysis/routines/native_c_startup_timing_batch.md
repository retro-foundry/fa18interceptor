# Startup, number fields, orientation and tracking source timing

Updated 2026-10-01. This continues game timing parity after commit 44063a95.
Twenty existing readable-C entries replace fixed charges with resumable source
timing. Readable coverage remains 419/624; 103 registered entries now have
timing steps.

## Original behavior and scope

| Source entries | Domain behavior | Bridge |
| --- | --- | --- |
| C11312, C11B0E | Message-sequence reset and long-table clearing | glue_startup_reset_step.c |
| C28722, C287DA, C28800, C28AFE, C28B34, C28F16 | Mode-selected scene initialization, record dispatch, aim and view setup | glue_scene_init_step.c |
| C24E2C, C24F76, C25A08, C0F56A | Date line, decimal fields, packed display values and hexadecimal text | glue_number_field_step.c |
| C2D954, C2E47A, C2E514, C2E5F6, C2E6DA | Record orientation, rotation matrices and paired sine/cosine lookup | glue_orientation_step.c |
| C123FA, C25980, C2564E | Direction tracking, rounded signed division and integer square root | glue_tracking_step.c |

The established readable domain implementations remain unchanged. CPU
registers, SR, source-width arithmetic, stack frames, ordered bus accesses and
instruction/event boundaries stay in glue. Authority is the original sealed
instruction bytes and `python tools/recomp/port_info.py ENTRY` listings.
These handwritten steps never call interpreter opcode handlers or replace
source work with measured average charges.

Shared scene bodies and exits are retained, including C28720's early return
and the record initialization/dispatch tail through C28E12. Rotation variants
share their source tail; the direction tracker keeps its original branches
and fault paths. CLR memory instructions write without an extra read;
MOVEM.W sign extension, register high halves, source flags and operand-dependent
multiply/divide/shift cycles are preserved. The existing proven unsigned
division helper moves from the map bridge to glue_unsigned_division_step.h
without a behavior change, so map, date and square-root steps share it.

## Structural and live proof

```sh
python tools/recomp/check_active_planes_step.py --group startup --bus
python tools/recomp/check_active_planes_step.py --group number_field --bus
python tools/recomp/check_active_planes_step.py --group orientation --bus
python tools/recomp/check_active_planes_step.py --group tracking --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

- Startup: 441 original instructions, 14,112 DMA-contention fixtures.
- Number fields: 100 instructions, 3,200 fixtures.
- Orientation: 291 instructions, 9,312 fixtures.
- Tracking: 461 instructions, 14,752 fixtures.
- New batch total: 1,293 instructions, 41,376 fixtures.
- Complete timing set: 6,713 instructions, 214,816 fixtures.

Fixtures independently execute original opcodes and compare every register,
full SR, PC, cycles and Chip/Slow RAM at varied DMA phases. Division fixtures
include fitting and overflowing quotients, zero divisors, signed boundaries
and INT_MIN. Chipset events/custom writes are held equally for structural
checks; live replays separately prove actual scheduling and hardware effects.

All 20 entries together match fresh source OFF RGB444 on all 36,236 frames
across demo01, qual_carrier_success and qual_fail_crashes. The same isolated
ON runs match each recording's sealed final RAM SHA-256. GNU and MSVC Release
builds pass. The full 419-entry gate matches 703,353 shadow and 1,110,694
sandbox calls, with zero mismatches, exact sealed final RAM and identical
poison frames. Source-timed parents can absorb formerly counted child calls;
call totals are not a function-coverage metric. No proof classification was
suppressed.

Use this `PORTS_ONLY` selector with `scripts/recomp_live_check.sh`:

```text
C11312,C11B0E,C28722,C287DA,C28800,C28AFE,C28B34,C28F16,C24E2C,C24F76,C25A08,C0F56A,C2D954,C2E47A,C2E514,C2E5F6,C2E6DA,C123FA,C25980,C2564E
```

The live gate now checks sealed final RAM during its existing ON RGB replay.
That removes three additional ON replays per batch while preserving the
independent three-recording RGB comparison. Missing seals and differing RAM
fail the gate; its cleanup removes temporary RGB and RAM outputs.
After both full gates and scratch cleanup, build/ is 0.177 GiB. Compiler
objects and small proof logs/reports remain cached.

## Fresh combined traces and next work

ALL still first differs at one-based RGB frame 416 by 361 pixels, matching
through frame 415. This batch proves its isolated timing and removes startup
debt at enclosing call boundaries; whole-game timing parity remains open.

A fresh 500-frame source OFF versus ALL ON instruction/event trace over
C0FECE-C1017E now matches instruction rows, CPU registers, SR, cycles and
event state through C10174 before C1C860 in machine frame 296. Earlier
message-reset, scene-initialization and long-table return boundaries all
have zero cycle drift. Including C123FA and its math children was necessary:
the scene caller still reached that tracker's fixed charge with the first
17 entries enabled.

The first differing enclosing instruction row is C1017A, after C1C860:
source cycle 42,135,486 versus ON 42,125,232, a -10,254-cycle difference.
Source/ON next-event cycles are 42,135,736/42,125,294 and beam lines 164/141.
These are diagnostics, not replacement charges.

A second fresh trace over C1C860-C1CA82 narrows that first call:

| Instruction boundary | ON minus OFF cycles | Source / ON SR |
| --- | ---: | --- |
| C1C860, terrain-refresh entry | 0 | 0000 / 0000 |
| C1C996, before C1E328 display-list sort | 0 | 0004 / 0004 |
| C1C99C, after C1E328 | -9,950 | 0010 / 0004 |
| C1C9A8, before C1E540 | -9,948 | 0014 / 0004 |
| C1C9AE, after C1E540 | -9,944 | 0019 / 0019 |
| C1C9C2, before C09A78 | -9,944 | 0014 / 0014 |
| C1C9C8, after C09A78 | -10,254 | 0004 / 0004 |
| C1CA2C, terrain-refresh return | -10,254 | 0004 / 0004 |

Both terrain traces contain 513 instruction rows. Their first difference is
row 7 (zero-based), C1C99C, with matching CPU register values but differing
cycles, event state and dead flags. Filtered traces omit child instructions
and events outside the range. They locate complete-call debt without proving
which inner instruction owns it or that it alone causes the frame-416 pixels.

Reproduce using the demo state/input, `--frames 500`, `--ports off` then
`--ports on`, and separate `FA18_BOUNDARY_TRACE` CSV paths. Set
`FA18_BOUNDARY_RANGE=C0FECE-C1017E` or `C1C860-C1CA82` and
`FA18_BOUNDARY_TRACE_MAX_MIB=64`. Task scratch traces and scaffolding are
removed after recording this evidence; sealed captures remain untouched.

Next, inspect and source-time C1E328 with its C1E4A6 sort helper and C1D91A
distance helper, plus C09A78/C09A98 with their shared C09AB8 predicate.
C1CA82 and C2F66E are other fixed children reached on different terrain
paths. C1D10C and C1E540 remain unregistered; C1D10C is the next actual
readable-function-count batch. Group shared bodies, preserve child contracts,
and run DMA fixtures before full live checks. Do not assign the observed
-9,950/-310 cycle differences as fixed fees.
