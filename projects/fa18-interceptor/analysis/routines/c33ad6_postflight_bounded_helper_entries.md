# `$C33AD6` postflight bounded helper entries

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

Two byte-identical 48-byte entries exist at `$C33AD6` and `$C33B06`. Each
adds `$C45988` to `d0`, returns if `d0 >= $13B`, then rejects values for which
`d0 + 4 <= 4`. On the remaining path it supplies `$5A + $C458D8` in both
`d1` and `d3` before calling `$C2FA7E`.

`$C33B36` is a separate return entry. The helpers' user-visible effect is not
proven, so the repeated entries remain structurally named.
