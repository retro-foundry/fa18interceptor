# `$C3149C` postflight record-attribute guards

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation executes a guard path in this middle portion; the
remaining branches are not covered.

`source_amiga/observed/guard_postflight_record_attributes.asm` reproduces
`$C3149C-$C31517` (124 bytes). It filters a mode byte, absolute magnitudes of
two long values, and the high nibble of `$62(a1)`; it sets bit 6 at `$20(a1)`
and conditionally manages the byte at `$C4586F`.

The attributes and timing role remain unproven; this describes only the
decoded predicates and writes.
