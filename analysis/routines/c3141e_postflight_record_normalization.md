# `$C3141E` postflight record normalization

Classification: **static-only dataflow**. This middle slice belongs to the
larger postflight loop and is not covered by current P-code exports.

`source_amiga/observed/normalize_postflight_record_delta.asm` reproduces
`$C3141E-$C3149B` (126 bytes). It exchanges and right-shifts two fixed-point
values with rounding, derives a `$C46184`-relative record address from the
high byte of a record word, filters record bits and signed deltas, and either
clears a record bit or transfers to `$C31714`.

The record type and gameplay meaning of the rejection state are unproven;
this documents arithmetic, flag tests, and branch destinations only.
