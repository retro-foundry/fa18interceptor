# `$C31FBC`: qualification-line formatter variant

Classification: **static dataflow**.

The byte-exact source is
`source_amiga/observed/format_qualification_line_variant.asm`.

Following the `$C31F86` quotient setup, this block selects one of two static
layout triplets, keyed by `$C45785`, and joins one of two external submission
paths.  The nonzero branch increments `D0`, writes the literal bytes `K`, `T`,
and `S` to offsets `+4..+6` of `$C457FF`, submits once, then reconfigures two
immediates and submits again.

This proves formatting/control flow and a literal suffix write.  It does not
prove the displayed field's unit, the ownership of `$C45785`, the selected
layout records, or a qualification success/failure condition.  The native
run060 late-result checkpoint does not execute this block.
