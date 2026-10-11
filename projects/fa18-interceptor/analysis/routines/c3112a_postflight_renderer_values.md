# `$C3112A` postflight renderer-value configuration

Classification: **static-only dataflow**. The observed parent postflight stage
at `$C0F124` calls this entry, but the available P-code exports do not cover
its body.

`source_amiga/observed/configure_postflight_renderer_values.asm` reproduces
`$C3112A-$C31223` (250 bytes). It sets `$C456E6`, tests bits 1, 2, 4, and 5 of
`$C4586E`, writes one of observed selector values to `$C45954`, and calls the
existing `$C2F63A/$C2F64E` renderer wrappers in a fixed sequence. Its final
selector also depends on a record-relative bit and `$C458DB` bit 1.

The selector values and resulting visible effect are unproven. The name only
records the parent-stage call site and established register/memory dataflow.
