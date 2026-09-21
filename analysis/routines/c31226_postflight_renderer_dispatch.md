# `$C31226` postflight renderer-variant dispatcher

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation executes `$C31226-$C31262`, establishing the bounds
setup and the dispatch branch. Its selected variant body is not covered by
that trace.

`source_amiga/observed/dispatch_postflight_renderer_variants.asm` reproduces
`$C31226-$C31289` (100 bytes). It initializes inputs for `$C310E2`, then uses
`$C45837` and masked bits 1-3 of `$C458DA` to select `$C3129A`, `$C31312`, or
`$C31392`. The positive activity path writes `-1` to `$C4E71C/$C4E744` before
executing the first two entries.

The selector's user-visible meaning remains unproven; this records only the
demonstrated dispatch and state-write contract.
