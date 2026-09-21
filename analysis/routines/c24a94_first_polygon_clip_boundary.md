# `$C24A94`: first polygon-boundary closure

Classification: **runtime-backed common projection pipeline**.

`source_amiga/observed/close_first_polygon_clip_boundary.asm` is byte exact
for `$C24A94-$C24B27`.

## Contract

After the outer clip loop completes, this pass closes the first cached clip
boundary when its `$C4E874+4` count is nonzero. It compares the saved endpoint
pair, computes a signed rounded intersection when necessary, and passes that
triple to `$C247C0`. Degenerate geometry reports error code `6` and transfers
to the common projection cleanup at `$C24D8C`.

This is common polygon-completion logic reached after workspaces produced by
the run031 Golden Gate record formats; it does not identify landmark-specific
geometry.
