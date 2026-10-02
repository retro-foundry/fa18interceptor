# View controls, record rates and matrix pipeline timing

Updated 2026-10-01. This continues game timing parity after commit 6c92ab2d.
Nineteen existing readable-C entries replace fixed charges with resumable
source timing. Readable coverage remains 419/624; 163 registered entries now
have timing steps.

## Original behavior and scope

| Source entries | Domain behavior | Bridge |
| --- | --- | --- |
| C12098, C1B906, C1BA86 | View controls, mode-zero entry and queued view-key tail | glue_view_control_step.c |
| C08324, C082B8, C082B0 | Maximum zoom, cockpit redraw and scene-setup prefix | glue_view_control_step.c |
| C1C7F6 | Signed record-rate thresholds and status byte | glue_record_rate_step.c |
| C2D99C, C2D9BA, C2DB18, C2DEE0 | Matrix route, view aiming, control-record route and matrix composition | glue_matrix_pipeline_step.c |
| C2DAF2, C2D970, C258C8 | View matrix, inverse orientation and keyboard pan | glue_matrix_pipeline_step.c |
| C2E370, C2E346, C2E38E, C2E3DE, C2E5AC | Rotation variants and row scaling | glue_matrix_pipeline_step.c |

Established readable domain routines remain unchanged. CPU registers, flags,
bus accesses, stack effects and instruction/event boundaries stay in glue.
Authority is the sealed original bytes and `python tools/recomp/port_info.py
ENTRY` listings. The steps never call interpreter opcode handlers or replace
source timing with measured average fees.

View-key entries preserve their common C1C23C-C1C2B6 tail and actual child
calls. Matrix helpers preserve signed operand-dependent MULS/DIVS timing,
word/long shifts, high register halves, MOVEM.W sign extension, ordered
effective-address updates and predecrement long stores. C2D9BA's source
range includes the early C2D9B0 pan-key prefix. Common arithmetic reuses
glue_renderer_step_math.h. Memory CLR has no extra read; an untaken word
branch skips its extension exactly as the original execution does.

## Structural and live proof

```sh
python tools/recomp/check_active_planes_step.py --group view_controls --bus
python tools/recomp/check_active_planes_step.py --group record_rate --bus
python tools/recomp/check_active_planes_step.py --group matrix_pipeline --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

- View controls/redraw: 203 original instructions, 6,496 DMA fixtures.
- Record rates: 36 instructions, 1,152 fixtures.
- Matrix pipeline: 868 instructions, 27,776 fixtures.
- New batch: 1,107 distinct instructions, 35,424 fixtures.
- Complete timing set, deduplicating shared source bodies: 8,412 instructions,
  269,184 fixtures.

The independent original-opcode oracle compares every register, full SR, PC,
cycles and Chip/Slow RAM under varied DMA phases. Rate fixtures include both
sides of 0x60, 0xC0 and 0x1000, signed extrema and negative values. Matrix
division fixtures cover zero divisors, both signs, signed extrema and quotient
overflow. Structural fixtures hold chipset events/custom writes equally;
live replays separately establish actual scheduling and hardware effects.

All nineteen entries together match fresh source OFF RGB444 on all 36,236
frames across demo01, qual_carrier_success and qual_fail_crashes. The same
isolated ON replays match each recording's sealed final RAM SHA-256. GNU and
MSVC Release builds pass. The full 419-entry gate matches 703,365 shadow and
1,110,694 sandbox calls, with zero mismatches, exact sealed final RAM and
identical poison frames. Source-timed parents can absorb formerly counted
child calls; call totals are not a coverage metric. Some entry paths remain
unexercised by these recordings; the independent instruction oracle also
covers their source bodies. No proof classification is suppressed. After
scratch cleanup, build/ is 0.179 GiB; cached compiler objects, small proof
logs and reports remain.

For the new entries, shadow retains 28 incomplete C12098 calls, one C2D99C
call and six C2DEE0 calls; these are not counted as matches. C12098 and
C2D99C independently match 5,792 and 5,791 sandbox calls. C1B906, C2D9BA,
C2DB18, C2E370, C2E346, C2E38E and C258C8 have no direct gate calls in these
recordings; their individual source instructions are covered by the structural
oracle. Full replay proves the batch on the captured paths, not every branch.

Use this `PORTS_ONLY` selector with `scripts/recomp_live_check.sh`:

```text
C12098,C1B906,C1BA86,C08324,C082B8,C082B0,C1C7F6,C2D99C,C2D9BA,C2DB18,C2DEE0,C2DAF2,C2E370,C2E346,C2E38E,C2E3DE,C2E5AC,C2D970,C258C8
```

The short combined batch matches through frame 600. ALL still first differs
at one-based RGB frame 416 by 361 pixels. These changes resolve demonstrated
call timing debt; complete registered ON output is not yet frame-faithful.

## Fresh traces and next work

Fresh 500-frame demo OFF versus ALL ON traces show the C12098 return at
C0F008 now matches cycles, registers, SR and event state. The prior +1,764
cycles and dead-SR difference are resolved. The first enclosing update
difference is now instruction row 31,242 (zero-based), C0F01C after C1C63E
in machine frame 296: source/ON cycles 42,146,072/42,147,514, or +1,442.
Only cycles, next event and beam line differ at this row. The update trace
has 36,198 source and 36,090 ON instruction rows; later execution diverges.

| Instruction boundary, first flight update | Machine frame | ON minus OFF cycles |
| --- | ---: | ---: |
| C0F008, after C12098 | 296 | 0 |
| C0F016, before C1C63E | 296 | 0 |
| C0F01C, after C1C63E | 296 | +1,442 |
| C0F02A, before C2D99C | 296 | +1,442 |
| C0F030, after C2D99C | 296 | +1,448 |
| C0F036, after C1C54E | 296 | +2,290 |
| C0F03C, after C254E8 | 296 | +2,262 |
| C0F042, after C122A2 | 296 | +2,336 |
| C0F048, after C1C860 | 296 | +2,340 |

The matrix-route call's observed drift increment falls from +9,704 in the
intermediate view-only build to +6 with the complete matrix family. It now
enters on an already shifted bus phase; the residual increment is not an
instruction-fee correction. Isolated live and DMA proofs establish the new
family's source timing independently.

C1C63E and its C22C80 flight parent remain unregistered. Their original
execution provides a narrower location for remaining fixed-helper debt.
The C1C63E trace has 743 source/ON instruction rows; its first difference is
row 18, C1C6BC after C22C80, +1,442 cycles. Within C22C80, both streams
have 2,345 instruction rows. The first difference is row 27, C22D52 after
C230B0, +48 cycles (42,136,908/42,136,956); all other fields match there.

| Boundary in C22C80 | ON minus OFF cycles |
| --- | ---: |
| C22D4E, before C230B0 | 0 |
| C22D52, after C230B0 | +48 |
| C22D7C, before C23228 | +48 |
| C22D80, after C23228 | +52 |
| C22D84, before C244E2 | +52 |
| C22D88, after C244E2 | +1,266 |
| C22D8E, after C25B66 | +1,266 |
| C230A6, before C09E06 | +1,398 |
| C230AC, after C09E06 | +1,440 |
| C230AE, return | +1,442 |

C230B0 is the registered 12-instruction selection-release helper with a
fixed 90-cycle charge. Source-time its complete branches, rather than
substituting a fee inferred from this path. C244E2 is the registered selected-
record range classifier with a fixed 1,300-cycle charge; it introduces
another +1,214 cycles here and loses the source's dead N flag. Its source
calls C3316E and C1D974, which already have timing steps. Preserve its shared
C245AA tail and actual child effects. Inspect the unregistered C09E06's
children before attributing its +42 increment to one helper. C1C54E projection
seeding, C254E8 view-octant update and C122A2 attitude flags are subsequent
registered fixed-charge targets.

The C29042 origin trace has 30 source/ON instruction rows and starts with
the inherited +1,442-cycle difference. Its child C2DAF2 changes that to
+1,438 under the shifted bus phase. This does not identify a new origin-
routine timing defect. Neither boundary costs nor isolated successes prove
which instruction causes the combined frame-416 pixels.

Reproduce with separate OFF/ON `FA18_BOUNDARY_TRACE` paths and
`FA18_BOUNDARY_RANGE` values C0EFD4-C0F3C4, C1C63E-C1C7F6,
C22C80-C230B0 and C29040-C295D2. Use the demo state/input, 500 frames,
`--ports off`/`on` and `FA18_BOUNDARY_TRACE_MAX_MIB=64`. Scratch CSVs,
selectors and the task scaffold are removed after recording evidence; small
build/gate logs and JSON proof reports remain cached. C1D10C remains the next
unregistered readable-C count batch; backend and necessary OS work remain
after complete game source.

Followup: native_c_flight_update_timing_batch.md records the twelve-entry
flight/projection/cockpit family. The complete first flight parent now matches;
the enclosing update's next timing gap is after C0DAEE in machine frame 310.
The observations above remain the evidence for this earlier nineteen-entry
baseline.
