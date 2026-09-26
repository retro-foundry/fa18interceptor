# Run060 adjacent-frame slow-RAM mutation inventory

Classification: **scenario-backed mutation inventory**, not state ownership.

Authority snapshots were captured from the sealed deterministic qualification
replay at GUI frames 8,240 and 8,241:

```text
build/run060_frame8240_checkpoint/frame_08240_state.bin
build/run060_frame8241_adjacent_checkpoint/frame_08241_state.bin
```

Deserializing each snapshot and comparing slow RAM `$C00000-$C7FFFF` finds 285
changed bytes in 102 contiguous ranges. This is a frame-boundary comparison;
it does not identify individual CPU writers and includes stack, renderer, and
other transient workspace.

## High-value non-stack candidates

| Range | Frame 8,240 | Frame 8,241 | Classification |
| --- | --- | --- | --- |
| `$C45775` | `$E5` | `$E6` | input-callback count/adjacent state; not a spatial candidate |
| `$C4582E/$C45831` | `$00/$00` | `$20/$01` | input/control latches; not spatial candidates |
| `$C456E7` | `$07` | `$01` | renderer active-plane mask; excluded |
| `$C45B5E` | `$0000` | `$FFE5` | matrix-side helper output; structural only |
| `$C45960-$C45982` | multiple | multiple | transient blitter lane-state block; excluded from flight-state candidates |
| `$C45A64-$C45A87` | multiple | multiple | projection/intermediate workspace; excluded as direct player-state evidence |
| `$C4B390-$C4B3D1`, `$C4B990-$C4B9CB` | multiple | multiple | renderer/workspace candidates; excluded as direct player-state evidence |

The known root transform triple `$C46198-$C461A3` is unchanged in this
comparison, agreeing with the independent late-run write watches.

## Resolved renderer exclusion

Static and prior live evidence now resolve the initially notable
`$C45961-$C4597F` subrange. It is part of the `$C45960-$C45982` blitter
lane-state block built by `$C30306-$C3040A`: it contains lane pointers,
offsets, pair count, and `BLTSIZE` setup which are consumed by the subsequent
Custom-chip blit submission. See
[`c30306_renderer_lane_state_builder.md`](../routines/c30306_renderer_lane_state_builder.md)
and [`finalize_renderer_pair_blit.asm`](../../source_amiga/observed/finalize_renderer_pair_blit.asm).
It is renderer workspace, not a player position/orientation/velocity
candidate. The compact changes are expected address/size updates for the
per-frame blits.

The other non-stack rows are likewise not player-state evidence. `$C456E7`
selects active planar renderer lanes, while `$C4582E` and `$C45831` are
documented control-latch fields. `$C45B5E` is a bounded `$C1342C` matrix-side
leaf output; its component meaning remains unknown, but the update does not
tie it to a camera, a world coordinate, or aircraft ownership.

## Exclusions

The comparison intentionally does not call any changed field player state.
The visible qualification simulation contains renderer work, callback/stack
activity, and control state in the same frame. A candidate becomes flight
state only after its writer is tied to control/integration and its consumer is
tied to camera, world placement, instruments, or landing logic.
