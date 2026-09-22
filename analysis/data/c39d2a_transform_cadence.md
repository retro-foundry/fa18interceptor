# `$C39D2A` Golden Gate transform cadence

Classification: **runtime-observed transform-pass cadence; not an instancing proof**.

From the frame-12,000 Golden Gate checkpoint, a 128-host-frame replay
breakpoint at `$C1F100` observed exactly twenty entries.  They alternate
between only two immutable input sources:

| Source | Entries | Host frames |
| --- | ---: | --- |
| `$C3515E` | 10 | 11, 24, 36, 49, 63, 76, 90, 103, 116, 128 |
| `$C39D2A` | 10 | 11, 24, 36, 49, 63, 76, 90, 103, 116, 128 |

Every observed `$C39D2A` entry has `A0=$C47E28`, `A3=$C48390`, and
`A4=$C27D24`.  Thus this direct transform pass processes the long component
once at each observed simulation-update cadence, alongside the flight-object
source.  It does not process a second `$C39D2A` copy in that pass.

This is consistent with one bridge-pylon/deck component transform, but it
cannot rule out another pylon drawn by a different renderer route or another
static source.  It is therefore evidence against treating this single source
as a proven complete bridge or as a repeated road-segment instance.

Authority: a 128-frame replay from
`build/run031_frame12000_golden_gate_checkpoint/state.bin`, using the recorded
`captures/setup_gui_check/inputs.e9k` stream and an `$C1F100` breakpoint.
