# Controlled terrain-directory axis sweep

Classification: **controlled selector-input sweep**. This establishes how the two
live origin bins feed the static template-directory call inputs for one bounded
update packet. It does not assign global world coordinates, cardinal directions,
terrain geometry, or LOD semantics to the bins.

Each single-bin sample restores the same run033 pre-refresh checkpoint, replays
to `$C1C860`, sets `$C45785=$01`, changes exactly one long origin component to
`bin << 24`, and records all 41 `$C1D3F4` entries until `$C0F048` returns.

| Bin | group-axis changed selector indices | group-axis row-key changes | row-axis changed row keys | row-axis selector-index changes |
| ---: | ---: | ---: | ---: | ---: |
| 0 | 0 | 0 | 0 | 0 |
| 1 | 28 | 0 | 28 | 0 |
| 2 | 28 | 0 | 28 | 0 |
| 3 | 28 | 0 | 28 | 0 |
| 4 | 28 | 0 | 28 | 0 |
| 5 | 28 | 0 | 28 | 0 |
| 6 | 28 | 0 | 28 | 0 |
| 7 | 28 | 0 | 28 | 0 |
| 8 | 28 | 0 | 28 | 0 |
| 9 | 28 | 0 | 28 | 0 |
| 10 | 28 | 0 | 28 | 0 |
| 11 | 28 | 0 | 28 | 0 |
| 12 | 28 | 0 | 28 | 0 |
| 13 | 28 | 0 | 28 | 0 |
| 14 | 28 | 0 | 28 | 0 |
| 15 | 28 | 0 | 28 | 0 |
| 16 | 28 | 0 | 28 | 0 |
| 17 | 28 | 0 | 28 | 0 |
| 18 | 28 | 0 | 28 | 0 |
| 19 | 28 | 0 | 28 | 0 |
| 20 | 28 | 0 | 28 | 0 |
| 21 | 28 | 0 | 28 | 0 |
| 22 | 28 | 0 | 28 | 0 |
| 23 | 28 | 0 | 28 | 0 |
| 24 | 28 | 0 | 28 | 0 |
| 25 | 28 | 0 | 28 | 0 |
| 26 | 28 | 0 | 28 | 0 |
| 27 | 28 | 0 | 28 | 0 |
| 28 | 28 | 0 | 28 | 0 |
| 29 | 28 | 0 | 28 | 0 |
| 30 | 28 | 0 | 28 | 0 |
| 31 | 14 | 0 | 12 | 0 |

For bins 1--30, exactly the first 28 call positions change on their assigned
axis and none changes on the other axis. Bin 31 retains the same separation
but has fewer changed call positions because portions of the call sequence reach
the observed zero-valued boundary behavior. The unmodified final 13 calls retain
their selector inputs throughout this 0--31 sweep.

The exact per-call value sequences, including the boundary values, are in the
machine-readable companion JSON. The static `$C42390` directory remains the
authoritative group lookup; this result proves its two input axes in this packet,
not a complete world-cell-coordinate decode.
