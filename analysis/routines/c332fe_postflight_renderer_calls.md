# `$C332FE` postflight five-call renderer sequence

Classification: **static-only dataflow**. This local postflight helper is not
covered by current P-code exports.

`source_amiga/observed/run_postflight_five_renderer_calls.asm` reproduces
`$C332FE-$C3336F` (114 bytes). It bounds an offset horizontal value, sets
`$C45954` to 8, and issues five calls to `$C2F60A` while preserving and
adjusting a coordinate pair.

The display role is unproven; the name describes the observed call pattern.
