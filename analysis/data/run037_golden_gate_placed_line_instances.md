# Run037 Golden Gate placed-line instances

Classification: **verified landmark placement-to-line ownership**. The two
red Golden Gate map strokes each have a separate exact placement/control/source
chain in one uninterrupted M-map trace.

| line context | template -> placement `(X,Y,Z)` | descriptor field -> stream | local source -> walker | line submit |
| --- | --- | --- | --- | ---: |
| `$C3559A` | `$C4264D -> $C4ED22 (460,0,172)` | `$C35568 -> $C35598` | `$C35BAA -> $C35BB8` | 142533 |
| `$C355D2` | `$C42653 -> $C4ED3A (460,0,148)` | `$C355A0 -> $C355D0` | `$C35BC2 -> $C35BD0` | 143231 |

For the first instance, `$C1CC70` reads `$C35568` at trace index 142119,
`$C1EF10` publishes `$C35598` at 142214, `$C1F4AC` reads `$C35BAA` at
142280, and the walker loads that published pointer at 142382. `$C212B0`
selects `$C358B2`, whose sole edge is local slots `0 -> 1`; `$C2FA7E` emits
the `$C3559A` stroke at 142533.

The second instance has the matching independent sequence: `$C355A0` at
142817, `$C355D0` at 142912, `$C35BC2` at 142978, walker load at 143080, and
`$C355D2` emission at 143231. It uses the same `$C358B2` edge selector.

The red-pixel/map-context evidence separately identifies `$C3559A/$C355D2`
as Golden Gate. This joins that verified landmark to its two observed map
placements. It does not make the source triples immutable global vertices or
claim a complete bridge model.
