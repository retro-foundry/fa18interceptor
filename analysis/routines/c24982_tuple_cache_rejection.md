# `$C24982`: tuple-cache rejection return

Classification: **runtime-backed common projection error path**.

`source_amiga/observed/return_tuple_cache_rejection.asm` is byte exact for
`$C24982-$C24995`.

The shared target reports error code `4` through `$C06C02`, restores the
`D0-D2` frame established by `$C248B2`, and returns to its caller. It joins
the reconstructed tuple-cache stage to the adjacent negated tuple stage
without assigning any scene-specific meaning.
