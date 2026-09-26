# run060 root pose initialization writer

Classification: **runtime writer trace / dataflow**. This identifies the
instructions that replace the root pose-candidate triple during run060's early
lifecycle. It does not identify the game-state label of that lifecycle.

The sealed run060 sampler first observes the root triple change during replay
frame 189. An exact Engine9000 checkpoint immediately before that event
(`frame 187`) retained the old bytes. Stepping from that checkpoint with no
future input finds these three writes to `$C46184+$14`:

| Instruction | Effect | Value written |
| --- | --- | ---: |
| `$C094FC: MOVE.L D1,$18(A2)` | root `+$18` | `$00007708` |
| `$C09534: MOVE.L D0,$14(A1)` | root `+$14` | `$11982C00` |
| `$C09538: MOVE.L D2,$1C(A1)` | root `+$1C` | `$1059A000` |

At every write, the resolved address register is `$C46184`. The before and
after tuple is therefore:

```text
$14/$18/$1C: 10545920,00000708,10A404F0
          -> 11982C00,00007708,1059A000
```

The first writer lies in the `$C0924A` early setup packet. Its `$C09498`
route selects a table-indexed record, derives a value for root `+$18`, then
calls the root transform entry `$C091E0` with `(D3,D4,D5) = (11,0,$68)`.
The transform reads root matrix `+$92..+$A2` and root base `+$14/+18/+1C`;
its transformed `D0` and `D2` are committed by `$C09534/$C09538`.

This proves that the frame-189 change is a deliberate record initialization
or placement update, not unobserved renderer scratch and not an input-driven
per-frame integrator. It does **not** yet prove whether the selected record is
the aircraft, camera, or a shared aircraft/camera pose, nor what external
scenario condition invokes `$C0924A`.

Authority: `build/run060_early_root_pose_mutations/memory_region_mutations.json`,
`build/run060_frame00187_root_pose_writer_stepped/memory_writes.json`, and
the byte-exact source around `$C091E0-$C09249`.
