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
| `$C45775` | `$E5` | `$E6` | callback/update counter candidate |
| `$C4582E/$C45831` | `$00/$00` | `$20/$01` | mode/flag candidates |
| `$C45961-$C45963` | `$007B1A` | `$04E4B6` | compact three-byte changing-state candidate |
| `$C45965-$C45967` | `$007B1A` | `$04E4B6` | paired compact changing-state candidate |
| `$C4596A-$C4596B` | `$1AD2` | `$0986` | paired word candidate |
| `$C4596E-$C4596F` | `$0282` | `$0F54` | paired word candidate |
| `$C4597D-$C4597F` | `$C000DE` | `$00013F` | compact three-byte changing-state candidate |
| `$C45A64-$C45A87` | multiple | multiple | projection/intermediate workspace; excluded as direct player-state evidence |
| `$C4B390-$C4B3D1`, `$C4B990-$C4B9CB` | multiple | multiple | renderer/workspace candidates; excluded as direct player-state evidence |

The known root transform triple `$C46198-$C461A3` is unchanged in this
comparison, agreeing with the independent late-run write watches. The diff
therefore supplies a finite next target: trace writers and consumers of the
`$C45961-$C4597F` cluster before assigning position, orientation, velocity,
or aircraft ownership.

## Exclusions

The comparison intentionally does not call any changed field player state.
The visible qualification simulation contains renderer work, callback/stack
activity, and control state in the same frame. A candidate becomes flight
state only after its writer is tied to control/integration and its consumer is
tied to camera, world placement, instruments, or landing logic.
