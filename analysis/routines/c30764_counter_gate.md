# `$C30764` counter gate

Classification: **runtime-backed structural/dataflow prefix**. In
`pcode/raw/attract_1800/`, `$C0F17C` directly calls `$C30764`; the bounded
packet executes `$C30764-$C3076B` and returns through `$C30762`.

The observed prefix tests byte `$C45836` and branches to the return when the
value is less than or equal to zero. The unobserved positive path decrements
that byte and enters a substantially larger blitter-related sequence beginning
at `$C30772`.

This establishes the gate and its no-work return path only. It does not prove
the counter's game-level meaning or the behavior of the positive branch.
