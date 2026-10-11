# Runtime coordinate-bearing scene placements

Classification: **scenario/dataflow evidence**. These are mutable runtime placement records, not an extracted static map mesh or a proven LOD table.  Records are selected solely by their leading relocated descriptor pointer; coordinate values are not a selection criterion.

`$C1CB74-$C1CCB6` selects records from `$C4E9AA`. In all three run033 checkpoints, the first populated record starts at that address and has this 24-byte shape:

| Bytes | Observed role |
| --- | --- |
| `+0..1` | selector word, copied to `$C4585B` and split for downstream control |
| `+2..5` | relocated descriptor pointer in `$C22000-$C22FFF` |
| `+6..11` | three signed coordinate-bearing words; the middle word is zero in every exported record |
| `+12..13` | additional record field (role unknown) |
| `+14..23` | mutable per-frame words |

The selector-loop report proves that the descriptor then selects renderer/control data. The coordinate words therefore locate scene instances before the renderer; they must not be merged with the immutable per-model vertex streams.

## Captured placement blocks

| Checkpoint | qualifying records | contiguous index blocks | all middle words zero |
| --- | ---: | --- | --- |
| run037_m_map_stable | 111 | 0-68 (69), 70-101 (32), 140-149 (10) | True |

[Top-down X/Z diagnostic](../plots/run037_m_map_runtime_placements_xz.svg) uses the same coordinate scale in all three panels. It is a placement scatter plot only: points do not imply terrain triangles, roads, or missing links.

## Limits

The populated records and their coordinates change between checkpoints, so the captures establish a runtime scene-placement layer but not the original static source table, a terrain mesh, or a distance-selected LOD rule. Proving any of those needs a trace of the writer/refill path into `$C4E9AA` and a source-to-renderer association for individual descriptors.
