# `$C2F03A`: segment projection, clamp, and line submission tail

Classification: **scenario-backed perspective-projection dataflow**.

`source_amiga/observed/project_clamp_and_submit_segment_pair.asm` is byte
exact for `$C2F03A-$C2F0C5`. The run029/attract display packet reaches this
tail from the bounded `$C2EE4A` segment-preparation calls and subsequently
reaches `$C2FA7E` blitter line submission.

It reads signed `(x, y, depth)` words from `$C45AC6` and applies the exact
integer perspective calculation:

```text
x = clamp(160 + (160 * x) / depth, 0, 319)
y = clamp( 90 + ( 90 * y) / depth, 0, 180)
stored_x = 319 - x
stored_y = 179 - y
```

Nonpositive depth takes the existing clipping retry route with error code
`$16`. Once both projected endpoints are stored, the tail loads the pair from
`$C4B390`, calls `$C2FA7E`, and returns `D0=1`.

This is the first exact, runtime-connected projection formula in the project.
It proves a 320×180 display-space contract and gives a concrete target for
identifying model-coordinate record producers; it does not yet identify the
models or assign world-axis names beyond the arithmetic inputs.
