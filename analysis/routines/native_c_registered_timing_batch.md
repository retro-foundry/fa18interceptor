# Registered source-timed bridge batch

Updated 2026-10-01. Authority: original instructions, the sealed demo,
carrier-success and qualification-failure recordings, direct one-instruction
Musashi comparisons, and the complete registered-port proof gate.

## Scope

Eighteen existing registered entries now advance at original instruction
boundaries instead of charging one fixed cost after the complete C call:

| Group | Entries | Source instructions |
| --- | --- | ---: |
| glyph compositors | C330FE, C32806 | 111 |
| mouse and joystick input | C1715C, C16F1C | 66 |
| draw-page selection | C2F558 | 9 |
| notification cadence | C11B44 | 25 |
| command/audio routes | C4FFB0, C17B08, C17B2C, C17EF2, C3316A, C3316E, C33180, C3318E, C33186 | 116 |
| render-buffer clear | C2FD22 | 27 |
| polygon lane blits | C30466, C304B2 | 37 |

The bridges are in `port/game/glue/glue_*_step.c`. Domain behavior remains in
the readable game C. `glue_step.h` supplies source-backed byte/word subtract
and compare, MOVEM save/restore, DBF, SWAP and register-shift operations. A
not-taken word-displacement branch skips its extension without issuing the
taken-path fetch. The dispatcher recognizes nested stepped calls from the
source return address on the stack, because an event or interrupt can leave
`REG_PPC` outside the caller at the child boundary.

## Structural proof

The launcher derives every covered PC from the original instruction listing:

```powershell
python tools/recomp/check_active_planes_step.py --group all
```

With 32 fixtures per instruction, all currently stepped plane, audio and this
batch's bridges, plus the subsequent postflight and polygon-edge bridges, match
1,163 original instructions over 37,216 cases. Every case
compares all registers, full SR, PC, instruction cycles and RAM. The 32 cases
cover every CCR value and the arithmetic/shift boundary fixture set. The new
batch contributes 391 instructions and 12,512 cases. Separate group runs also
pass. The region oracle still passes, and both deliberately incorrect
busy-input bridge copies are rejected.

## Complete registered gate

`scripts/recomp_ports_check.sh` passes all three sealed recordings:

- 414 registered routines;
- 721,752 completed shadow comparisons;
- 1,169,610 sandbox comparisons;
- zero mismatches and sealed final RAM for every recording;
- identical demo poison frames.

The stepped glyph, page, notification and polygon entries have completed
shadow comparisons. Hardware/event-bearing input, command/audio and buffer
calls retain explicit incomplete or sandbox-only classifications where the
proof model cannot compare a live source call. Their instruction oracle and
the isolated live replay supply the complementary evidence; no exclusion was
relaxed.

## Live timing proof

Fresh source OFF and isolated ON RGB444 streams are byte-identical for all
36,236 frames in the three sealed recordings. The exact batch selector is:

```text
C330FE,C32806,C1715C,C16F1C,C2F558,C11B44,C4FFB0,C17B08,C17B2C,C17EF2,C3316A,C3316E,C33180,C3318E,C33186,C2FD22,C30466,C304B2
```

Run it with:

```powershell
$env:PORTS_ONLY='C330FE,C32806,C1715C,C16F1C,C2F558,C11B44,C4FFB0,C17B08,C17B2C,C17EF2,C3316A,C3316E,C33180,C3318E,C33186,C2FD22,C30466,C304B2'
& 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_live_check.sh
Remove-Item Env:PORTS_ONLY
```

The checker runs recordings concurrently, creates fresh `--ports off` and
`--ports on` streams, compares exact bytes, and removes both streams. Fresh OFF
is the source oracle: a SHADOW stream can differ from plain source timing when
stepped shadow input replay is active even though all call comparisons and
poison checks pass.

## Remaining registered timing debt

The complete all-registered ON path is not yet frame-faithful. A current
500-frame demo replay first differs at one-based frame 419 by 29,453 pixels.
The source-timed postflight group and C305AA polygon edge are exact in isolation.
Individual fixed-charge probes now find C0FA04 at frame 401, C0D752 at frame 416
and C0D74A at frame 484, while C2DEE0, C2DB18 and C2D99C remain exact through
500. These independent failures show that timing debt is distributed and that
subset bisection is not monotonic. `scripts/probe_recomp_timing.py` ranks entries
against one reused source stream and cleans its bounded scratch streams.

## Workflow cost and disk use

The headless build now uses one Ninja dependency graph shared by the runner,
instruction oracles and mutation builds. On this workspace an unchanged build
takes about 0.14 seconds directly (0.42 seconds through the pruning wrapper),
and a one-bridge rebuild takes about 0.64 seconds, down from about 20 seconds.
The combined 616-instruction oracle takes about 14 seconds.

Proof recordings run concurrently. The registered gate writes RGB only for
the demo poison reference and removes both large streams on exit. The live
checker also removes all temporary streams. After the gates, `build/` is about
0.13 GiB rather than retaining 5.5 GiB of frame references; the separate
12 GiB build-cache budget and per-file output limits remain fallback guards.
