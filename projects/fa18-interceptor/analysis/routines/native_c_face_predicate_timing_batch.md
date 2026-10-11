# Face predicates and record-scan line-style timing

Updated 2026-10-02. This continues game timing parity after commit 29bd43c9.
Five existing readable-C entries replace fixed charges with resumable original
instruction timing. Coverage remains 419/624; 187 registered entries now have
timing steps. Domain behavior remains in the existing game modules.

| Entries | Original behavior | Distinct instructions |
| --- | --- | ---: |
| C1FB82, C1FB8C, C1FB9C, C1FC42 | Face/point orientation and flagged component bounds, including shared tails | 123 |
| C2F490 | Reset the scan pass's line style | 2 |

Authority is the sealed original bytes and `python tools/recomp/port_info.py
ENTRY`. `glue_face_predicate_step.c` preserves source-width overflow, signed
MULS operand-dependent cycles, register ASR/ASL counts and flags, MOVEM.W sign
extension, indexed/displacement accesses and ordered stack/bus operations.
The C1FC3A bound prefix remains part of C1FB82's shared body; it is not counted
as another registered entry. C2F490 retains the original longword write and
RTS stack access. No measured fees or interpreter opcode handlers are used.

## Verification

```sh
python tools/recomp/check_active_planes_step.py --group face_predicates --bus
python tools/recomp/check_active_planes_step.py --group all --bus
python scripts/probe_recomp_timing.py C1FB82,C1FB8C,C1FB9C,C1FC42,C2F490 ALL --frames 600
```

All 125 instructions match registers, full SR, PC, cycles and Chip/Slow RAM
on 4,000 independent DMA-contention fixtures. The full oracle matches 9,627
instructions and 308,064 cases. Fixtures vary register high halves, all CCR
combinations, signed bounds and shifts, frame locals and component buffers.
Structural checks hold chipset events equally; live runs prove scheduling.
GNU and MSVC Release builds pass.

The five-entry isolated batch matches fresh source OFF RGB444 on all 36,236
frames across the three native recordings. Each isolated ON run also matches
its sealed final RAM SHA-256. The full 419-entry gate matches
703,337 completed shadow and 1,110,694 sandbox calls, with zero mismatches,
exact sealed final RAM and identical poison frames. Source-timed parents can
absorb formerly counted child calls; call totals are not coverage.
No incomplete or cold comparisons are counted as matches. The new entries retain the following classifications:

| Entry | Completed shadow matches | Incomplete shadow | Completed sandbox matches |
| --- | ---: | ---: | ---: |
| C1FB82 | 128 | 0 | 887 |
| C1FB8C | 3,100 | 12 | 3,260 |
| C1FB9C | 402 | 1 | 401 |
| C1FC42 | 3,048 | 4 | 3,348 |
| C2F490 | 4,082 | 1 | 5,791 |

All sandbox comparisons for these five entries complete; none is suppressed.

Use this `PORTS_ONLY` selector with `scripts/recomp_live_check.sh`:

```text
C1FB82,C1FB8C,C1FB9C,C1FC42,C2F490
```

The short isolated batch matches through frame 600. ALL still first differs
at one-based RGB frame 416 by 361 pixels. Whole-game timing parity remains
incomplete; timing-step work does not increase readable function coverage.

## Fresh boundary evidence

After the four predicate bridges, the first 320 instruction rows of the
C2005C-C200F6 tested-face trace match all fields. Both sides have 6,020 rows;
the next tested-face entry in source machine frame 358 inherits +138,844
cycles (source/ON 50,930,474/51,069,318). The first C200BE return from C1FB82
and the subsequent polygon dispatch/return now match. The earlier first
+488-cycle face gap and enclosing +2,024-cycle scene-stream gap are removed.

The four-predicate enclosing trace then first differed at C0F0DE after
C1518C in frame 311 by -16 cycles (source/ON 44,323,968/44,323,952).
Inspecting the original record scan identified its two-instruction C2F490
line-style reset as a remaining fixed-charge child. Source timing for that
reset removes the scan difference without adjusting any parent fee.

The final five-entry C0EFD4-C0F3C4 trace has 36,198 source and 36,090 ON
instruction rows. Its first 31,275 rows match all fields, including both
scene-stream returns, grid return and the first record-scan return. The next
difference is C0F132 immediately after C11BFC's message update, in machine
frame 311: +2,344 cycles, source/ON 44,324,676/44,327,020. SR is 0004/0000;
cycles, next event and beam line also differ. C0F12C before the call is exact.
C11BFC is registered with a fixed charge and has no child calls. That is the
next source-timing target, not an inferred replacement fee.

The final C1518C-C153FC scan trace has 4,795 instruction rows on each side.
Its first 574 rows match all fields, including the complete first call.
The next scan entry in frame 358 inherits +138,876 cycles (source/ON
50,980,470/51,119,346). Later inherited entry offsets do not identify local
charges. These selected instruction comparisons do not assert that chipset
event rows outside the selected source ranges are identical, or establish
which instruction causes the combined frame-416 pixel difference.

Reproduce with separate OFF/ON `FA18_BOUNDARY_TRACE` paths and ranges
C0EFD4-C0F3C4, C2005C-C200F6 and C1518C-C153FC; use demo state/input,
500 frames, `--ports off`/`on` and `FA18_BOUNDARY_TRACE_MAX_MIB=64`.
Scratch CSVs, selectors and scaffolding are removed after recording this
evidence. Build/ is 0.180 GiB; compiler objects and small proof logs remain
cached. C1CB14/C1CB26/C1ED3C/C1518C remain original parents;
C1EE14 is a shared span, not a standalone catalogued function. C1D10C remains
the next unregistered readable-C count batch after the timing priority.
Complete game source, native backend and necessary OS replacement remain
the full objective.

Planning followup, 2026-10-02: C11BFC's measured return gap remains real, but
bypassing that entry does not move ALL's first RGB difference. Fresh bounded
probes isolate C30918 and interacting HUD groups as visual reproducers. The
next implementation priority is revised in CURRENT_PORT_HANDOFF.md; this
report retains the completed batch's proof and historical boundary evidence.
