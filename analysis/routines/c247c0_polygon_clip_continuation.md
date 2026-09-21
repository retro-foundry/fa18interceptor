# `$C247C0`: polygon clipping continuation

Classification: **runtime-backed common projection pipeline**.

`source_amiga/observed/clip_projected_segment_pair.asm` is byte exact for
`$C247C0-$C248B1`. The next tuple-cache stage starts at `$C248B2`.

## Contract

This is the paired-boundary counterpart to the `$C2469E` outer clipping loop.
It maintains the alternate endpoints in `$C4E91A`, performs signed
multiply/divide interpolation when a segment crosses that boundary, and
normalizes the resulting third component by negating it. Accepted clip points
are submitted to `$C248B2`; two byte counters in `$C4E874` track the boundary
states. Degenerate paths report error code `3` through `$C06C02`.

## Runtime anchor

`$C2469E` invokes this continuation for accepted crossings while processing
the three- and four-point workspaces made by the run031 Golden Gate control
packet. It is common clipping machinery, not scene-specific bridge data.
