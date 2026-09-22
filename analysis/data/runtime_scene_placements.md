# Runtime coordinate-bearing scene placements

Classification: **scenario/dataflow evidence**. These are mutable runtime placement records, not an extracted static map mesh or a proven LOD table.

`$C1CB74-$C1CCB6` selects records from the `$C4E9AA` anchor. In all three run033 checkpoints, the first aligned populated record starts at `$C4E9AC` and has this 24-byte shape:

| Bytes | Observed role |
| --- | --- |
| `+0..3` | relocated descriptor pointer in `$C22000-$C22FFF` |
| `+4..9` | three signed coordinate-bearing words; the middle word is zero in every exported record |
| `+10..11` | additional record field (role unknown) |
| `+12..23` | mutable per-frame words |

The selector-loop report proves that the descriptor then selects renderer/control data. The coordinate words therefore locate scene instances before the renderer; they must not be merged with the immutable per-model vertex streams.

## Captured placement blocks

| Checkpoint | qualifying records | contiguous index blocks | all middle words zero |
| --- | ---: | --- | --- |
| run033_frame05250 | 121 | 0-68 (69), 70-102 (33), 140-158 (19) | True |
| run033_frame05500 | 123 | 0-68 (69), 70-102 (33), 140-160 (21) | True |
| run033_frame06250 | 123 | 0-68 (69), 70-102 (33), 140-160 (21) | True |

[Top-down X/Z diagnostic](../plots/runtime_scene_placements_xz.svg) uses the same coordinate scale in all three panels. It is a placement scatter plot only: points do not imply terrain triangles, roads, or missing links.

## Limits

The populated records and their coordinates change between checkpoints, so the captures establish a runtime scene-placement layer but not the original static source table, a terrain mesh, or a distance-selected LOD rule. Proving any of those needs a trace of the writer/refill path into `$C4E9AC` and a source-to-renderer association for individual descriptors.
