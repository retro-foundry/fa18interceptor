# Run031 projected tuple-list mutation probe

Classification: **controlled runtime mutation with ordinary replay screenshots**.

The experiment replayed run031 from its initial state to frame 12,001. At the
live `$C2FF48` entry, a fresh emulator instance changed only the first
`$C4B390` screen pair through Engine9000's supported
`e9k_debug_write_memory` API:

| Item | Original | Mutated |
| --- | --- | --- |
| Pair-list count | `$0003` | `$0003` |
| First screen pair | `(196, 17)` | `(40, 150)` |
| Remaining pairs | unchanged | unchanged |

The unmodified ordinary full-replay screenshot at frame 12,001 shows the
Golden Gate bridge and horizon. After the pair mutation, the same frame has no
scene geometry. The images are:

- `build/run031_frame12001_baseline/frame_12001.png`
- `build/run031_frame12000_tuple_mutation_probe3/mutated_frame12001.png`

## Result

This is direct visual confirmation that `$C4B390`, produced by `$C24CFE` and
consumed by `$C2FF48 -> $C301F6`, is an authoritative live renderer-input
list. The mutation is reversible and touched no recording, disk image, or
saved game state.

The result does **not** assign one visible bridge line to the changed pair:
`$C301F6` reduces the entire list to bounds before selecting its line path, so
moving the first pair causes a global list/raster rejection rather than a
localized segment displacement. A future experiment must mutate a source
record while preserving list bounds to obtain per-element attribution.

## Reproduction data

`build/run031_frame12000_tuple_mutation_probe3/probe.json` records the hit
frame, original list bytes, and modified bytes. The unmodified replay image
was generated independently by `capture_keyframes.py` from the run031 initial
state and playback recording.
