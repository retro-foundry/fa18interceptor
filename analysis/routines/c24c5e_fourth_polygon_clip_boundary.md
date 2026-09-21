# `$C24C5E`: fourth polygon-boundary closure

Classification: **runtime-backed common projection pipeline**.

`source_amiga/observed/close_fourth_polygon_clip_boundary.asm` is byte exact
for `$C24C5E-$C24CFD`.

## Contract

The final closure is enabled by `$C4E874+7` and works from the cache at
`$C4E91A+48`. It uses the alternate negated sign convention, computes a
rounded crossing if needed, appends the resulting negated triple directly to
the `A1` output sequence, and increments `D7` as the emitted tuple count.
Degenerate geometry reports error `9` and transfers to `$C24D8C`.

This completes the four post-loop boundary closures used by the common scene
projection pipeline.
