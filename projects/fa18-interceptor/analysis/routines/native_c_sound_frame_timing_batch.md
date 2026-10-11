# Sound-start and screen-frame source timing

Updated 2026-10-01. This follows the renderer/record-view batch in commit
e8ff57f7 and continues the user's requested game timing parity work. Registered
readable-C coverage remains 419/624; these are timing activations for existing
domain implementations.

## Original behavior and scope

| Source entries | Domain behavior | Bridge |
| --- | --- | --- |
| C17E4A, C17CF6, C17DAA, C17C62, C17D6E, C18096, C1803C, C180FC | Noise/engine starts, engine slides, selected-record alerts and channel stop | glue_sound_start_step.c |
| C50AB4, C50B02 | Seed feedback bit and counted random-bit accumulation | glue_sound_start_step.c |
| C0D74A, C0D752, C0DAA0, C0DAD0, C0DAD4, C0DADC, C0DAE6 | Normal/wide screen-frame preparation, mirrored endpoints and corner appends | glue_screen_frame_step.c |
| C2E758, C2EA5A, C2EAD0, C2EB4C, C2EBC2 | Corner projection and four view/side-plane clipping variants | glue_corner_edges_step.c |

The readable source remains in audio.c, fixed_math.c, screen_frame.c and
clip.c. CPU register/flag effects, stack frames, source-width arithmetic, bus
accesses and data-dependent cycles stay in glue. The bridges do not invoke
interpreter opcode handlers. Source calls still reach existing voice steps,
the original long-division routine and other explicit children through the
dispatcher. No measured average charge replaces original instructions.

The source authority is the sealed state's original bytes and the instruction
lists from `python tools/recomp/port_info.py ENTRY`. Normal/wide frame entries
share their original body and endpoint helpers. Corner projection includes
the four clipping bodies and their shared C2EC36-C2EC66 exits, preserving
MOVEM.W sign extension, signed multiply/divide timing, rounded intersections,
coordinate limits and every return boundary. Original self-loops and unusual
exits are preserved.

## Structural proof

```sh
python tools/recomp/check_active_planes_step.py --group sound_start --bus
python tools/recomp/check_active_planes_step.py --group screen_frame --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

- Sound-start/random family: 279 instructions, 8,928 fixtures.
- Screen-frame/corner family: 747 instructions, 23,904 fixtures.
- Complete timing set: 4,358 instructions, 139,456 fixtures.

Each fixture checks all registers, full SR, PC, cycles and Chip/Slow RAM with
five-plane DMA contention and varied horizontal phases. Structural fixtures
hold chipset events and custom writes equally on both executions. Fresh live
recordings separately check real scheduling and hardware effects.

## Live proof and builds

The sound family separately matches fresh source OFF RGB444 bytes on all
36,236 frames across demo01, qual_carrier_success and qual_fail_crashes. The
combined 22-entry selector also matches every frame in those three recordings:

```text
C17E4A,C17CF6,C17DAA,C17C62,C17D6E,C18096,C1803C,C180FC,C50AB4,C50B02,C0D74A,C0D752,C0DAA0,C0DAD0,C0DAD4,C0DADC,C0DAE6,C2E758,C2EA5A,C2EAD0,C2EB4C,C2EBC2
```

Set `PORTS_ONLY` to this selector and run `scripts/recomp_live_check.sh`.
Separate isolated ON replays with `--ram-out` also match each recording's
sealed final RAM SHA-256, covering voice slots, sound parameters and random
state as well as the frames. Those small RAM outputs are removed after
comparison.
Both GNU and MSVC Release builds pass. The full registered shadow/sandbox,
sealed final RAM and poison gate matches 703,360 shadow and 1,129,295 sandbox
calls across three recordings, with zero mismatches and identical poison
frames. Call counts can change as source-timed parents absorb children.
No comparison or hardware classification was suppressed.

## Combined progress and next evidence

The preceding all-registered ON demo first differed at one-based frame 297 by
5,440 pixels. With source-timed sound starts and random helpers it matches
through frame 415 and first differs at frame 416 by 361 pixels. Adding the
screen-frame/corner family leaves that same combined first difference.
The screen and corner entries tested individually are exact through frame
600; isolated matches do not establish combined whole-game timing.

The sound correction addresses the earlier C17E4A startup trace, which had
measured a -29,066-cycle complete-call offset with the old 2,000-cycle fee.
The source family supplies instruction/event timing directly. Other
scene-initialization timing debt remains; the combined frame result alone
does not prove identical cycles before the first drawing difference.

A fresh 35-entry fixed-charge ranking against one reused 500-frame source
stream found these remaining independent differences:

| Entry | One-based first differing frame | Pixels |
| --- | ---: | ---: |
| C1D3F4 terrain stream | 415 | 361 |
| C2FF48 polygon submission | 416 | 361 |
| C2EE4A projected segment preparation | 416 | 361 |
| C3201A, C31F4C, C20A40 | 424 | 34,144 |
| C26EBE candidate-record update | 441 | 12,238 |
| C0D04C, C20D68 | 484 | 17 |

Reproduce with `python scripts/probe_recomp_timing.py --frames 500
--rank-fixed 35 --differences-only`, using the current demo gate report.
The probe overwrites one candidate stream and removes both scratch streams.
Ranking order can change as timing parents absorb children and gate call
counts change; the explicit entries above remain reproducible probes.

Continue with the related drawing families: C2FF48/C301F6/C3040C and
C2EE4A with its four clip helpers and C2FA7E line emitter. C1D3F4 is another
observed failure and shares terrain groundwork with the unregistered C1D10C
source batch. Inspect their exact child contracts before implementation.
Avoid treating subset bisection as monotonic or substituting fixed mean fees
to hide a phase difference.

## Followup

The drawing/cell family is now source-timed and independently exact. ALL
retains the frame-416 difference; fresh startup boundaries identify the next
timing debt. Current evidence is in
[native_c_drawing_cell_timing_batch.md](native_c_drawing_cell_timing_batch.md).
