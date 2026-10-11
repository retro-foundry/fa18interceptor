# Negated tuple-cache stage at `$C24996` (Hunk 19 +`$30E`)

Classification: **structural**. This complete run001 packet manipulates
three-word caller tuples. It is on the human-flight display path, but neither
tuple contents nor output ownership is assigned.

## Runtime packet

- Direct edge `$C24956 -> $C24996 -> $C2495A`; hit at replay frame 9.
- 514 instructions, complete at the observed return boundary.
- P-code: `pcode/raw/run001_c24996_tuple_status/`, 132 observed RAM starts /
  938 P-code operations; all starts map to resolved Hunk 19.
- The packet calls `$C247C0` and `$C248B2` repeatedly. It also re-enters
  `$C24996` through `$C24970`, so the invocation is a bounded tuple-processing
  loop rather than a simple status leaf.

`source_amiga/observed/negated_tuple_cache_stage.asm` is the byte-exact
254-byte observed-entry slice `$C24996-$C24A93`. It loads a three-word tuple
from `A3`, compares against the cached tuple at `A2+$30`, conditionally uses
signed multiply/divide with quotient rounding, may append three words through
`A1`, increments `D7`, and updates byte counters at `A4+3` and `A4+7`.
It returns `D0 = 1` on the observed path.

The static zero-denominator branch writes named error code 5 to `$C4599E` and
calls `$C06C02`; the helper and error-code meaning are outside this slice.
