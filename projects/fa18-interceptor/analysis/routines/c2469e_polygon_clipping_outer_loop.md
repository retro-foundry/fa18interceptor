# `$C2469E`: polygon clipping outer loop

Classification: **runtime-backed common projection pipeline**.

`source_amiga/observed/clip_projected_polygon_segments.asm` is byte exact for
`$C2469E-$C247BF`. Its segment-intersection continuation begins at `$C247C0`
and is intentionally outside this slice.

## Contract

The routine consumes the count, shift, and triples prepared at `$C4BF90` by
the scene-record handlers. It scales each triple by the workspace shift,
keeps previous/current clip points in `$C4E91A` and `$C4E910`, and tests
successive segments across a clipping boundary. A crossing uses signed
multiply/divide interpolation with explicit rounding before placing an
intersection triple in `$C4E910` and entering `$C247C0`.

The outer loop reports error code `1` through `$C06C02` for its degenerate
clip path and then continues processing. It is the shared path entered by the
run031 Golden Gate record handlers after they have constructed their three- or
four-point workspaces.

## Runtime anchor

The run031 frame-12,000 trace enters `$C2469E` after the direct and
conditional projection-record formats. This makes the slice a live common
boundary between control-stream geometry and the later projection/raster
pipeline, rather than a Golden Gate-specific routine.
