# `$C1B340` record-delta control-field update

Classification: **bounded dynamic trace plus byte-exact source**. The routine
uses a low-nibble gate at `$7C(A1)`, a signed delta between `$C45870` and
`$2B(A1)`, and the low nibble at `$39(A1)` to choose helper entries that write
the low two bits of `$C461E9`. Its threshold comparisons are `$F8` and 8.

The sealed frame-5 trace documents the positive-delta path: delta 1 and mode
1 select `$C1B4D0`, which writes field value 1. It then calls `$C25A6A` before
the three-axis gate. Other branch outcomes are static facts, not dynamic
coverage claims.
