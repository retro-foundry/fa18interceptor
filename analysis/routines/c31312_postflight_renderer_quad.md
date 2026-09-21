# `$C31312` postflight renderer quad

Classification: **static-only dataflow**. This dispatcher target is not
covered by the current P-code exports.

`source_amiga/observed/submit_postflight_renderer_quad.asm` reproduces
`$C31312-$C31391` (128 bytes). It applies `$C45988/$C458D8` to four fixed
`d0/d1` pairs and calls the shared renderer entry `$C2F5F4` four times. The
first two horizontal values are rejected outside `[0,$140)`; the first call
also sets `$C45954` to `$C`.

The resulting display role is unproven; this documents only the repeated
arithmetic/call pattern and state write.
