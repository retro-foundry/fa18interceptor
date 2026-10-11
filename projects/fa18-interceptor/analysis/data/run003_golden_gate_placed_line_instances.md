# Run003 Golden Gate placed-line instances

Classification: **verified landmark placement-to-line ownership**. In the
video-hash-matched M-map trace containing the user-identified Golden Gate, each
red bridge stroke has its own exact template, placement, control, local-source,
and line-emission path.

| line context | template -> placement `(X,Y,Z)` | descriptor field -> stream | local source -> walker | line submit |
| --- | --- | --- | --- | ---: |
| `$C3559A` | `$C4264D -> (588,0,300)` | `$C35568 -> $C35598` | `$C35BAA -> $C35BB8` | 85416 |
| `$C355D2` | `$C42653 -> (588,0,276)` | `$C355A0 -> $C355D0` | `$C35BC2 -> $C35BD0` | 86114 |

The first path stores `$C35568` at trace index 85002, publishes `$C35598` at
85097, transforms `$C35BAA` at 85163, enters the `$C35BB8` walker at 85262,
and reaches `$C2FA7E` under `$C3559A` at 85416. The second repeats that
contract independently with `$C355A0`, `$C355D0`, `$C35BC2`, `$C35BD0`, and
`$C355D2` at indices 85700, 85795, 85861, 85960, and 86114.

Both walkers select the static `$C358B2` edge record. The M-map scenario's
red-pixel/context evidence identifies these two contexts as Golden Gate. The
result proves the landmark's two observed line instances and placements, not a
complete bridge mesh or immutable global source vertices.
