# Registered renderer and record-view source timing

Updated 2026-10-01. The user requested a return to game timing parity after
the C279D0 source batch. The registered coverage remains 419/624.

## Source-backed scope

Eleven existing entries now resume at original instruction boundaries instead
of charging an approximate total at return:

| Entries | Original behavior | Timing bridge |
| --- | --- | --- |
| C212B0 | Segment-pair submission and child calls | glue_renderer_timing_step.c |
| C332BC | Postflight HUD dispatch and shared exit | glue_renderer_timing_step.c |
| C23CA6 | Record-view walk, state writes and shared exits | glue_record_view_timing_step.c |
| C2469E, C246A0, C247C0, C248B2, C24996 | Polygon clipping parent, NOP entry alias and three clipping stages | glue_polygon_clip_step.c |
| C091E0, C091CE, C091A8 | Local-to-world matrix transform variants | glue_world_transform_step.c |

Readable operations remain in draw_stream.c, postflight_hud.c,
control_records.c, polygon_clip.c and matrix.c. CPU state, stack effects,
source-width arithmetic, condition flags, bus accesses and instruction cycles
remain in glue. No bridge calls an interpreter opcode handler. The transform
family supplies the C23CA6 child at C2417A; its signed products retain the
source's data-dependent multiply cycles.

The authority is the original instruction bytes in the sealed state, together
with source ranges and call edges exposed by
`python tools/recomp/port_info.py ENTRY`. Shared ranges include the original
alternate entries and error exits. C2469E adds the source NOP preceding
C246A0. The existing readable whole-call implementations are unchanged.

## Instruction and live proof

```sh
python tools/recomp/check_active_planes_step.py --group renderer --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

The renderer group matches 1,094 original instructions across 35,008 fixtures.
The complete timing set matches 3,332 instructions across 106,624 fixtures.
Each compares all registers, full SR, PC, cycles and Chip/Slow RAM under
five-plane DMA contention and varied horizontal phases. Custom writes and
chipset events are held equally on both sides of these structural checks;
live recordings independently test their real effects and scheduling.

The broader DMA check found an old extra read at C17B1C in
glue_command_step.c. The source MOVE.L reads its frame argument and writes the
stack; the bridge additionally read the new stack slot to recover flags.
Retaining the value for flags removes that extra bus access. Its failing
fixture is C17B1C case 14: source consumed 24 cycles and the old bridge 28.
The corrected instruction passes the full DMA fixture set.

The eleven-entry batch matches fresh source OFF RGB444 bytes on all 36,236
frames: demo01 20,833, qual_carrier_success 12,353, qual_fail_crashes 3,050.
The live selector is:

```text
C212B0,C332BC,C23CA6,C2469E,C246A0,C247C0,C248B2,C24996,C091E0,C091CE,C091A8
```

Set `PORTS_ONLY` to that selector and run `scripts/recomp_live_check.sh`.
Include C17B08 to check the command bridge correction with this batch.
The combined selector including C17B08 also matches all 36,236 live frames.
GNU and MSVC Release builds pass. The complete 419-entry registered gate
matches 703,364 shadow and 1,129,295 sandbox calls with zero mismatches,
sealed final RAM unchanged, and identical poison frames. Call totals can
change as source-timed parents absorb children or alter incomplete-call
boundaries; registration coverage remains unchanged.

## Combined timing remains incomplete

The all-registered ON demo now first differs at one-based frame 297 by 5,440
pixels. The preceding committed registry first differed at frame 416 by 361
pixels. This batch establishes isolated fidelity, not improved combined
fidelity. Do not restore approximate charges to conceal this difference.

A diagnostic runner built from the preceding committed registry reproduced
frame 416. Keeping that registry and replacing only C23CA6's charge with its
source timing exposed frame 297. Replacing only C212B0, C332BC, or the clipping
family retained frame 416. Disabling the polygon drawing children in the full
new registry did not change frame 297. These are diagnostic variants, not
alternative accepted implementations.

Bounded 300-frame source OFF/registered ON traces show equal registers and SR
at the first C2AA9C entry, but ON arrives 8,168 cycles late. The difference
already exists before the new clipping instructions. In the update pass
leading to that map call, selected instruction boundaries have these deltas
(ON cycle minus source cycle):

| Source PC | Boundary | Cycle delta |
| --- | --- | ---: |
| C0EFE4 | Before indirect record-update dispatcher C0F5F8 | 0 |
| C0EFEA | After that dispatcher | -42,350 |
| C0F05C | Before display-buffer gate C0D730 | -39,764 |
| C0F062 | After display-buffer gate | +8,180 |
| C2AA9C | First map depth-stage entry | +8,168 |

The source enters the first dispatcher in frame 295 and returns in frame 296;
the map entry occurs in frame 297. These observations identify an upstream
timing investigation, without proving one remaining routine is responsible.
Different entry phases can change DMA waits and frame crossings.

A narrower trace resolves the indirect dispatcher target to C0FECE, the
delayed menu transition. The relevant invocation enters with identical cycles
and CPU state. It first acquires a -29,066-cycle offset between C0FF3C and
C0FF42, across the registered C17E4A noise-start call (still charged 2,000
cycles). The C28722 scene-initialization call adds another -10,208 cycles,
and the unregistered C1C860 terrain update adds -9,816 across its children.
These deltas measure complete calls including their original waits; they
must not be substituted as new fixed fees. Range C0FECE-C1017E reproduces
this transition trace. The first remaining timing target is the C17E4A
sound-start family and its source children, followed by scene initialization.

Reproduce with the usual demo state/input/ROM, `--frames 300`, and
`--ports off` or `--ports on`. Set `FA18_BOUNDARY_TRACE=PATH` and one of these
exclusive ranges: C0EFD4-C0F082, C2AA9C-C2AFFA, C24688-C24DA8. Trace rows include
CPU state and absolute cycles. Filter to `kind=instruction` for the update
table. Outputs stay in ignored build/recomp and retain the existing size caps.

Continue with the source call boundaries inside C0F5F8's C0FECE transition and
the fixed-charge entries before C0D730. C0D74A/C0D752 still share an untimed
screen-frame body and remain a separate coherent timing family. Rank bounded
probes with `scripts/probe_recomp_timing.py`; an isolated 500-frame match
alone does not establish whole-game timing.
