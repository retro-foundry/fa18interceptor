# `$C3129A` postflight tuple-pair variant

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes this complete bounded-dispatch target.

`source_amiga/observed/submit_postflight_pair_variants.asm` reproduces
`$C3129A-$C31311` (120 bytes). It loads two consecutive four-word tuples from
`$C3128A`, offsets their first and third words by `$C45988`, rejects either
word outside `[0,$140)`, offsets the second and fourth words by `$C458D8`, and
calls `$C2FA78` for each surviving tuple. It then transfers to `$C31392`.

The tuple fields and visible result remain unproven; the report intentionally
describes their observed arithmetic and control flow only.
