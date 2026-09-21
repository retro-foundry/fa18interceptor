# `$C25754`: record-vector normalization

## Evidence

`build/run003_6000_c25754/trace.jsonl` enters `$C25754` from `$C14A54` and
returns to `$C14A5A` after 84 instructions with no future playback input.
Byte-exact source is `source_amiga/observed/normalize_record_vector.asm`, now
including the `$C257D4-$C257DB` zero-result continuation.

## Observed contract

The routine receives a signed magnitude and three signed components through
its stack frame, calls `$C1D974` to obtain a scale bound, normalizes the
components through shifts, unsigned division, and signed multiplication, then
writes three words to `$C45A4C-$C45A50`. A zero magnitude or zero helper result
clears all three output words; otherwise the sign of the first argument is
restored to the normalized components before return in sign-extended `D5-D7`.

The vector's gameplay ownership and the precise numerical convention of
`$C1D974` remain unassigned.
