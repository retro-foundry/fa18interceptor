# `$C24BC8`: third polygon-boundary closure

Classification: **runtime-backed common projection pipeline**.

`source_amiga/observed/close_third_polygon_clip_boundary.asm` is byte exact
for `$C24BC8-$C24C5D`.

## Contract

When the `$C4E874+6` boundary count is set, this closure operates on the
cached tuple at `$C4E91A+32`. It uses signed rounded intersection arithmetic,
places the completed tuple at the current `A3`, then submits it to the
negated tuple-cache stage at `$C24996`. A degenerate path reports error `8`
and goes to common cleanup at `$C24D8C`.

This is common clip-completion code reached by the live run031 scene path;
the cache state does not itself establish individual bridge-element identity.
