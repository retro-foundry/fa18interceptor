# Matrix marker, projection entries and filled-circle timing

Updated 2026-10-02. This continues timing parity after commit 9e8ff77b.
Seven existing readable-C entries replace fixed charges with resumable source
timing. Coverage remains 419/624; 182 registered entries now have timing steps.
Readable domain implementations remain unchanged.

| Entries | Source behavior | Distinct instructions |
| --- | --- | ---: |
| C0DAEE | Fixed tuple through the view-angle matrix, then projection | 32 |
| C2EC90, C2EC94, C2EC9C, C2ECA4 | Four projection prefixes and common rejection, division, clamp and drawing tails | 86 |
| C2F1C0 | Radius-limited span table, viewport clipping and four-plane circle blits | 225 |
| C06C02 | Source fault-hook return | 1 |

Authority is the sealed original bytes and `python tools/recomp/port_info.py
ENTRY`. glue_marker_projection_step.c preserves CPU effects, operand-dependent
arithmetic, ordered bus accesses, stack frames and event boundaries. Projection
entries include C2EC70/C2EC82 rejection/fault tails; the circle includes C2F1B8
for its small-radius path. Calls to the source-selected pixel, pair, square,
circle and fault helpers remain actual calls. No average fees or interpreter
opcode handlers are used in the new bridge.

Circle rendering retains individual BBUSY byte polls and NOPs, the source's
span-table writes, four ordered BLTCON/pointer/BLTSIZE sequences per row and
both viewport clipping branches. Address-register quick arithmetic changes
the full address without changing flags. Memory LSR has its own source
instruction cost; register ROR retains count-dependent cycles, count-zero
flags and the untouched X flag. PC-relative mask-table fetches use the
original extension address.

## Verification

```sh
python tools/recomp/check_active_planes_step.py --group marker_projection --bus
python tools/recomp/check_active_planes_step.py --group all --bus
python scripts/probe_recomp_timing.py C0DAEE,C2EC90,C2EC94,C2EC9C,C2ECA4,C2F1C0,C06C02 ALL --frames 600
```

All 344 source instructions match registers, full SR, PC, cycles and Chip/Slow
RAM on 11,008 DMA fixtures. The complete oracle matches 9,502 instructions
across 304,064 fixtures. New fixtures include signed DIVS extrema, zero
divisors and overflow, varied shifts/rotates and register high halves. Circle
MMIO instructions use the actual custom-register base after fixture RAM
seeding. Structural checks hold chipset events and custom writes equally;
live replays separately prove real scheduling and drawing.

All seven entries together match fresh source OFF RGB444 on all 36,236
isolated frames across the three native recordings. Each isolated ON replay
also matches its sealed final RAM SHA-256. GNU and MSVC Release builds pass.
The full 419-entry gate matches 703,356 shadow and 1,110,694 sandbox calls,
with zero mismatches, exact sealed final RAM and identical poison frames.
Source-timed parents can absorb counted child calls; totals are not coverage.
No proof classification is suppressed. After scratch cleanup, build/ is
0.180 GiB; compiler objects and small proof logs remain cached.

C0DAEE matches 4,064 completed shadow and 5,791 sandbox calls; 19 shadow
calls remain incomplete. C2EC90 matches 230 shadow and 222 sandbox calls,
with two incomplete shadow calls. C2EC9C matches 309 shadow and 73 sandbox
calls; 458 sandbox calls remain incomplete. The circle matches eight shadow
calls, while its 474 sandbox calls remain incomplete. C2EC94/C2ECA4 have
957/123 incomplete sandbox calls and no completed direct comparisons; C06C02
has no gate calls. These are not counted as matches. Their original source
instructions are independently covered, and the isolated live batch proves
the captured paths through the complete family.

Use this `PORTS_ONLY` selector with `scripts/recomp_live_check.sh`:

```text
C0DAEE,C2EC90,C2EC94,C2EC9C,C2ECA4,C2F1C0,C06C02
```

The short isolated batch matches through frame 600. ALL still first differs
at one-based RGB frame 416 by 361 pixels. Whole-game parity remains incomplete.

## Fresh boundary evidence

The first matrix-marker call now matches all 32 source instruction rows,
including C0DB40 after projection. The second call also enters with exact
timing in source machine frame 339. The 500-frame marker trace has 480 rows
on each side; its first 64 instruction rows match all fields. The next marker
entry in frame 357 inherits +138,924 cycles from other work (source/ON
50,835,998/50,974,922); that is not a new marker charge.

The enclosing C0EFD4-C0F3C4 trace retains 36,198 source and 36,090 ON
instruction rows. Its first 31,266 rows now match every field. The first
difference moves from C0F090 after C0DAEE in frame 310 (+386 cycles) to
C0F0BA after C1CB26 in frame 311 (+2,024 cycles), source/ON
44,320,064/44,322,088. Cycles, next event and beam line differ; registers and
SR match. C0F0AC after the first C1CB14 scene-stream pass is also exact.

A C1CB14-C1CCBC trace has 15,983 source/ON instruction rows. Its first
difference is row 427, C1CC88 after the indirect C1CC86 call to C1EE14 in
frame 311: +2,056 cycles, source/ON 44,281,918/44,283,974. C1EE14 is an
original shared span inside C1ED3C, not an independently catalogued function;
`port_info.py C1EE14` has no standalone instruction body. Do not register or
count this label as new readable coverage.

The C1EE14-C1F99A renderer trace has 78,770 instruction rows on each side.
Its first difference is row 359, C1F944 after C1F942's indirect C2005C call:
+466 cycles in frame 311, source/ON 44,243,352/44,243,818. A0 is C2005C at
the source call; the recorded flow target independently confirms it.
C2005C already has source timing, so its children require inspection.

The C2005C-C200F6 trace has 6,020 instruction rows per side. Its first
difference is row 29, C200BE after C1FB82: +488 cycles, source/ON
44,231,844/44,232,332. A0 also differs; SR is 0004 on both sides. These nested
observations locate the next fixed-helper debt without assigning average fees
or proving the source of the combined frame-416 pixels.

Next, source-time C1FB82 with its C1FB8C/C1FB9C orientation and C1FC42
component-bound siblings. Preserve shared tails, signed word overflow,
MOVEM.W sign extension and actual predicate outputs. These are existing
registered helpers. C1CB14/C1CB26 and C1ED3C remain original parents.

Reproduce with separate OFF/ON `FA18_BOUNDARY_TRACE` paths and ranges
C0EFD4-C0F3C4, C0DAEE-C0DB42, C1CB14-C1CCBC, C1EE14-C1F99A and
C2005C-C200F6; use the demo state/input, 500 frames, `--ports off`/`on`, and
`FA18_BOUNDARY_TRACE_MAX_MIB=64`. Scratch traces/selectors/scaffolding are
removed after evidence is recorded; small proof logs and cached objects remain.
C1D10C remains the next unregistered readable-C count batch. Complete game
source, native backend and necessary OS replacement remain the full objective.
