# `$C1D974`: table-based scale bound

## Evidence

The frame-6000 `$C25754` packet calls `$C1D974` and returns to `$C25784` after
32 helper instructions. Its complete static body `$C1D974-$C1D9D7` is already
contained in `source_amiga/observed/fixed_point_stage_tail.asm`.

## Observed contract

The helper orders the nonnegative magnitude inputs in `D2-D4`, uses the table
at `$C1D9D8` for two ratio-derived lookup indexes, and produces a capped scale
bound in `D1`; it also writes the same word to `$C45B40`. In this packet the
returned `D1` supplies the divisor/scale bound consumed by `$C25754`.

The older capped packet is superseded for this caller context: this bounded
run003 invocation does return. It does not prove all input combinations or the
physical interpretation of the scale.
