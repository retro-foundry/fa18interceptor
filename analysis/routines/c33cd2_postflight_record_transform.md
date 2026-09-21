# `$C33CD2` postflight record transform

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It selects `$C46184 + $C458DE`, subtracts record longs `$14/$18/$1C` from
`$C45722:$C4572A`, shifts all three differences right by 8, and consumes nine
signed words from `$C45BD8` as three multiply-and-accumulate groups. The
three resulting words are supplied in `d0:d2` to `$C2EC90`.

This proves a fixed-point vector/coefficients dataflow but not coordinate or
gameplay semantics. `$C33D3A` begins the transform's state-publication tail.
