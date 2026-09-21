# `$C1B4D0` two-bit control-field helper

Classification: **structural with byte-exact source**. Three entries select
`D2 = 1`, `2`, or `0`; the zero entry additionally clears `$C45870`. The
common tail preserves the upper six bits of `$C461E9` and replaces its low two
bits with `D2`.

The values and field update are established statically. The callers that map
the values to gameplay controls require bounded traces.
