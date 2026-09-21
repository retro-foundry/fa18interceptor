# `$C3149C` postflight record-attribute guards

Classification: **static-only dataflow**. This middle portion of the
postflight loop is not covered by current P-code exports.

`source_amiga/observed/guard_postflight_record_attributes.asm` reproduces
`$C3149C-$C31517` (124 bytes). It filters a mode byte, absolute magnitudes of
two long values, and the high nibble of `$62(a1)`; it sets bit 6 at `$20(a1)`
and conditionally manages the byte at `$C4586F`.

The attributes and timing role remain unproven; this describes only the
decoded predicates and writes.
