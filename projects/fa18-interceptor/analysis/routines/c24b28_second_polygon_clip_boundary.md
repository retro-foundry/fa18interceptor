# `$C24B28`: second polygon-boundary closure

Classification: **runtime-backed common projection pipeline**.

`source_amiga/observed/close_second_polygon_clip_boundary.asm` is byte exact
for `$C24B28-$C24BC7`.

## Contract

When the `$C4E874+5` boundary count is nonzero, this complementary closure
uses the cached triple pair at `$C4E91A+16`. It negates the relevant
components before the same signed rounded intersection calculation, negates
the completed third component, then hands the result to `$C248B2`.
Degenerate geometry reports error code `7` and enters common cleanup at
`$C24D8C`.

It is a common post-loop clip closure used after the scene record handlers;
no landmark-specific geometry is inferred.
