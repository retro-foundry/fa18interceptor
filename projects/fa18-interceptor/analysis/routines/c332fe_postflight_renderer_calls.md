# `$C332FE` postflight five-call renderer sequence

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes the complete `$C332FE-$C3336F` helper body.

`source_amiga/observed/run_postflight_five_renderer_calls.asm` reproduces
`$C332FE-$C3336F` (114 bytes). It bounds an offset horizontal value, sets
`$C45954` to 8, and issues five calls to `$C2F60A` while preserving and
adjusting a coordinate pair.

The display role is unproven; the name describes the observed call pattern.
