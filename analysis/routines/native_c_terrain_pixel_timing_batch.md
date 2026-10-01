# Terrain sorting, conditions, pixel lanes and interrupt timing

Updated 2026-10-01. This continues game timing parity after commit 6b4ef04e.
Forty-one existing readable-C entries replace fixed charges with resumable
source timing. Readable coverage remains 419/624; 144 registered entries now
have timing steps.

## Original behavior and scope

| Source entries | Domain behavior | Bridge |
| --- | --- | --- |
| C1E328, C1E4A6 | Display-list distance keys, depth permutation and record copy | glue_display_sort_step.c |
| C1D91A, C1D974 | Target distance and table-scaled three-component magnitude | glue_display_sort_step.c |
| C09A78, C09A98, C09AB8 | Two terrain condition flags and their shared key/value predicate | glue_condition_step.c |
| C1CA82 | Pending flag on both control-record banks | glue_terrain_flags_step.c |
| C2F5C0, C2F5D4, C2F5F4, C2F60A, C2F63A, C2F64E, C2F66E | Pixel, pair and square masks, viewport offsets and palette-selected plane dispatch | glue_pixel_step.c |
| 25 registered C2F826-C2FA22 table targets | One-row and two-row planar AND/OR writes | glue_pixel_step.c |
| C06132 | Game interrupt-server count and data-pointer result | glue_interrupt_count_step.c |

The established readable domain routines remain in stages.c, fixed_math.c,
control_records.c, plot.c, planar_lane_masks.c and interrupts.c. CPU state,
source arithmetic widths, bus accesses, stack restoration and instruction/
event boundaries stay in glue. Authority is the sealed original bytes and
`python tools/recomp/port_info.py ENTRY` listings. These steps never invoke
interpreter opcode handlers or substitute measured average cycle charges.

The display sorter retains its count cap, fault loop, source stack-overlap
test at -0x2C(A6), and exact record/key write order. Distance includes the
C1D90A saturation prefix and shared C1D974 magnitude tail. Unsigned division
uses the existing shared operand-dependent helper; unsigned multiply counts
the original source word's set bits. Condition lookup preserves signed list
sentinels, key offsets, value gates, byte lists and alignment of the next
record. Pixel entry prefixes retain rejection branches, D0/D1 preservation,
MOVEM.W sign extension, XOR-plane paths, and actual computed handler targets.
Only the existing 25 registered lane targets change; other table targets
retain their original translated/interpreter path. No new game entries are
claimed from those undiscovered leaves.

## Structural and live proof

```sh
python tools/recomp/check_active_planes_step.py --group terrain_sort --bus
python tools/recomp/check_active_planes_step.py --group terrain_condition --bus
python tools/recomp/check_active_planes_step.py --group terrain_flags --bus
python tools/recomp/check_active_planes_step.py --group pixels --bus
python tools/recomp/check_active_planes_step.py --group interrupt_count --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

- Sorting/distance: 208 original instructions, 6,656 DMA fixtures.
- Conditions: 70 instructions, 2,240 fixtures.
- Record flags: 36 instructions, 1,152 fixtures.
- Pixel entries and registered lane targets: 292 instructions, 9,344 fixtures.
- Interrupt counter: 5 instructions, 160 fixtures, including word carry.
- New batch total: 611 instructions, 19,552 fixtures.
- Complete timing set: 7,324 instructions, 234,368 fixtures.

The independent original-opcode oracle compares every register, full SR, PC,
cycles and Chip/Slow RAM at varied five-plane DMA phases. Unsigned division
fixtures cover zero divisors and fitting/overflowing quotients; source shifts
exercise word/long boundaries and high register halves. Structural checks
hold chipset events/custom writes equally; live runs separately prove actual
scheduling and hardware effects.

All 41 entries together match fresh source OFF RGB444 on all 36,236 frames
across demo01, qual_carrier_success and qual_fail_crashes. The same isolated
ON runs match each recording's sealed final RAM SHA-256. GNU and MSVC Release
builds pass. The full 419-entry gate matches 703,360 shadow and 1,110,694
sandbox calls, with zero mismatches, exact sealed final RAM and identical
poison frames. Source-timed parents can absorb formerly counted child calls;
call totals are not a function-coverage metric. No proof classification was
suppressed. After scratch cleanup, build/ is 0.178 GiB; compiler objects and
small proof logs/reports remain cached.

Use this `PORTS_ONLY` selector with `scripts/recomp_live_check.sh`:

```text
C1E328,C1E4A6,C1D91A,C1D974,C09A78,C09A98,C09AB8,C1CA82,C2F5C0,C2F5D4,C2F5F4,C2F60A,C2F63A,C2F64E,C2F66E,C2F826,C2F83A,C2F844,C2F84E,C2F858,C2F862,C2F86C,C2F876,C2F880,C2F88A,C2F894,C2F89E,C2F8A8,C2F8B2,C2F8EA,C2F904,C2F91E,C2F96C,C2F986,C2F9A0,C2F9BA,C2F9D4,C2F9EE,C2FA08,C2FA22,C06132
```

The short combined batch and C06132-only probes are exact through frame 600.
ALL still first differs at one-based RGB frame 416 by 361 pixels. This batch
removes proved complete-call timing debt without establishing whole-game
parity or moving that combined first-difference frame.

## Fresh traces and next work

A fresh 500-frame OFF versus ALL ON trace over C0FECE-C1017E now matches
the entire CSV, including all 1,953 instruction rows. The first terrain-
refresh call also matches through its C1CA2C return. The previous -9,950
cycles across C1E328 and -310 across C09A78 are resolved. Later terrain
entries can inherit drift from work between calls: the second C1C860 entry
in machine frame 296 starts 11,684 cycles late. That is not a new fee or proof
that terrain entry itself introduces it.

Before the counter correction, the loop trace's first difference was
C15DB2 in frame 7, 16 cycles late after C1612C. An inner trace found 12
cycles after LoadView at C1617C. A wider ROM trace then identified the first
six cycles at FC1498 after Kickstart's call to the game counter C06132.
Its fixed 60-cycle charge hid source instruction/bus timing. The five original steps
replace that charge; Kickstart behavior remains unchanged.

With the final 41-entry batch, the complete 100-frame C1612C-C16284 CSV and
eight-frame FC0000-FD0000 CSV match source exactly (18,000 and 61,753
instruction rows). The loop trace now matches through frame 295 before its
flight-update call. The first remaining enclosing update difference is:

| Instruction boundary | Machine frame | ON minus OFF cycles |
| --- | ---: | ---: |
| C0F002, before C12098 view controls | 296 | 0 |
| C0F008, after C12098 | 296 | +1,764 |
| C0F016, before C1C63E | 296 | +1,762 |
| C0F01C, after C1C63E | 296 | +1,092 |

C0F008 is instruction row 31,239 (zero-based) in the update trace; registers
match but dead SR, cycles and event state differ. Source/ON cycles are
42,136,152/42,137,916. The source flags are 0008 versus ON 0000. The update
trace has 36,198 source and 36,000 ON instruction rows because later execution
diverges. The enclosing C15DA8 return is 50,804 cycles late in source machine
frame 312, spanning many children. These observations are diagnostic complete-
call differences, not replacement charges or proof of the frame-416 pixel cause.

Reproduce with separate OFF/ON `FA18_BOUNDARY_TRACE` paths and these
`FA18_BOUNDARY_RANGE` values: C0FECE-C1017E, C1C860-C1CA82,
C0EFD4-C0F3C4 and C15D96-C15DB4 for 500 frames; C1612C-C16284 for 100;
FC0000-FD0000 for eight. Use the demo state/input, `--ports off`/`on`, and
`FA18_BOUNDARY_TRACE_MAX_MIB=128`. Scratch traces and task scaffolding are
removed after their evidence is recorded; small proof logs/reports remain.

Next, source-time C12098 with its actual view-control children, including
C1B906, C08324, C1BA86 and C082B8. Inspect C1C63E's C22C80/C1C7F6/C29042
contracts before attributing its child debt. Preserve shared source bodies,
interrupts, bus contention and all calls; run DMA fixtures before full live
checks. C1D10C remains the next unregistered readable-C count batch. Native
backend work and necessary OS replacement remain after complete game source.
