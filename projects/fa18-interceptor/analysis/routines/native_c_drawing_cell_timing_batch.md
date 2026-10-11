# Polygon, segment and cell-template source timing

Updated 2026-10-01. This continues game timing parity after commit f2d0bc6c.
Readable-C coverage remains 419/624; 15 existing entries replace fixed charges
with resumable source timing. There are now 83 registered timing-step entries.

## Original behavior and scope

| Source entries | Domain behavior | Bridge |
| --- | --- | --- |
| C2FF48 | Filled-mask compositing, skipped colour bits and blitter-priority publication | glue_polygon_submit_step.c |
| C301F0, C301F6, C3040C | Fixed/default-row bounds, thin primitives, polygon outline/fill and mask-between-planes blit | glue_polygon_prepare_step.c |
| C2FA78, C2FA7E | Fixed/default-row line setup and four-plane submission | glue_line_step.c |
| C2EE4A, C2F0C6, C2F0F4, C2F128, C2F156 | Projected segments and four truncated plane crossings | glue_segment_clip_step.c |
| C1D3F4, C1D4E4, C1D520, C1D5D8 | Cell bitmap/template expansion, sorted lookup, occupancy collection and level filing | glue_cell_templates_step.c |

The readable domain source remains in render_polygon.c, render_line.c,
clip.c, control_records.c and stages.c. CPU registers, full SR, original stack
frames, source-width arithmetic and instruction/event boundaries stay in
glue. The bridges never invoke interpreter opcode handlers or replace source
work with measured average charges.

Authority is the original instruction bytes in the sealed native state and
the lists from `python tools/recomp/port_info.py ENTRY`. Shared prefixes and
exits are included: C2FF46's early return, C301F0's fixed-row prefix, the
C2FA70 horizontal-line prefix, C2EE44's rejected-segment exit, and the four
clip helpers' C2F186-C2F1B6 shared tail. Original zero-denominator self-loops
and cell-list fault loops remain intact. MOVEM.W sign extension, register
high halves, byte markers, signed multiply/divide timing, BBUSY polls, DMA
priority changes and custom-register write order are preserved.

## Structural and live proof

```sh
python tools/recomp/check_active_planes_step.py --group drawing --bus
python tools/recomp/check_active_planes_step.py --group cells --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

- Drawing: 809 original instructions, 25,888 DMA-contention fixtures.
- Cell templates: 253 instructions, 8,096 fixtures, including signed-byte
  boundaries and preservation of bits 8-31.
- Complete timing set: 5,420 instructions, 173,440 fixtures.

Fixtures independently evaluate original opcodes and compare every register,
full SR, PC, cycles and Chip/Slow RAM under varied five-plane DMA phases.
Chipset events and custom writes are held equally for structural checks;
full live replays separately prove scheduling and hardware effects.

All 15 entries together match fresh source OFF RGB444 on every one of the
36,236 frames across demo01, qual_carrier_success and qual_fail_crashes.
Separate isolated ON replays also match each sealed final RAM SHA-256.
The full registered gate matches 703,346 shadow and 1,110,694 sandbox calls,
with zero mismatches, sealed final RAM exact and identical poison frames.
Source-timed parents absorb child calls, so those totals can decrease.
GNU and MSVC Release builds pass. No proof classification was suppressed.

Use this `PORTS_ONLY` selector with `scripts/recomp_live_check.sh`:

```text
C2FF48,C301F0,C301F6,C3040C,C2FA78,C2FA7E,C2EE4A,C2F0C6,C2F0F4,C2F128,C2F156,C1D3F4,C1D4E4,C1D520,C1D5D8
```

The short individual C2FF48, C301F6, C2EE4A, C2FA7E and C1D3F4 probes now
match through frame 600. Their preceding fixed-charge differences at frames
415/416 are resolved in isolation. ALL still matches through frame 415 and
first differs at one-based frame 416 by 361 pixels. This batch establishes
its own exact timing; it does not establish whole-game parity or move that
combined first-difference frame.

## Fresh startup evidence and next work

A bounded 500-frame source OFF versus ALL ON trace over C0FECE-C1017E
finds matching startup instruction rows through C0FF60 in frame 296. The
first differing instruction boundary is C0FF64, after the C11312 message
sequence reset. This is later than the old sound-start divergence; sound
timing is now exact at that call boundary.

| Return boundary | Source frame | ON minus OFF cycles | Change since preceding listed boundary |
| --- | ---: | ---: | ---: |
| C0FF64, after C11312 | 296 | -36 | -36 |
| C0FF82, after C28722 | 296 | -10,238 | -10,202 |
| C0FFA8, after C11B0E | 296 | -11,678 | -1,440 |
| C1017A, after C1C860 | 296 | -21,918 | -10,240 |

The last table delta includes intervening instructions; the actual C1C860
call adds -10,246 cycles at its boundary. These are observed complete-call
timing differences, including contention and scheduling, not replacement
cycle fees. Register values match at those return rows, but dead SR flags
differ after C28722/C11B0E. The enclosing source rewrites those flags.
Filtered traces omit events and instructions inside children outside the
selected range, so they identify timing debt without proving which inner
instruction owns it or that it alone causes the frame-416 pixels.

Reproduce with the demo state/input, `--frames 500`, `--ports off` then
`--ports on`, and `FA18_BOUNDARY_TRACE` set to separate CSV paths,
`FA18_BOUNDARY_RANGE=C0FECE-C1017E`, `FA18_BOUNDARY_TRACE_MAX_MIB=64`.
The task's two 0.52-MiB scratch traces were removed after recording this
evidence; sealed recordings and source bytes remain untouched.

Next, source-time the startup family rather than repeating the repaired
drawing probes: C11312 and C11B0E are small complete routines;
C28722/C287DA/C28AFE/C28800/C28B34 share scene initialization, with C24E2C,
C28F16 and C2D954 as remaining fixed-charge children. The C1C860 call also
reaches fixed C09A78/C09A98/C1CA82/C1E328/C2F66E and unregistered C1D10C
and C1E540. Preserve explicit child contracts and group shared source bodies.
The C1D10C readable-C candidate remains the next function-count batch.

A fresh 35-entry fixed-charge probe against the preceding gate report leaves
these independent early differences: C3201A/C31F4C/C20A40 at frame 424 by
34,144 pixels; C26EBE at frame 441 by 12,238; and C0D04C/C20D68 at frame
484 by 17. Their ordering can change with source-parent absorption. Use
`python scripts/probe_recomp_timing.py --frames 500 --rank-fixed 35
--differences-only` to rerank; each invocation reuses one source stream and
cleans both scratch streams. Subset effects are not monotonic.

## Followup

The subsequent 20-entry startup/math batch removes the message, scene and
long-table debt above. New enclosing and terrain traces identify C1E328
sorting and C09A78 conditions as the next timing targets. See
[native_c_startup_timing_batch.md](native_c_startup_timing_batch.md) for the
current proof and boundaries; this report retains the earlier batch evidence.
