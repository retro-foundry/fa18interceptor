# Tuple comparison stage at `$C247C0` (Hunk 19 +`$138`)

Classification: **structural**. This complete display-path leaf processes
three-word tuples through caller-provided `A2`, `A3`, and `A4`; the tuples'
geometric or object meaning is unassigned.

## Runtime packet

- Direct edge `$C2479E -> $C247C0 -> $C247A4` in the human-flight display
  stage.
- 33 instructions, complete at the real return boundary.
- P-code: `pcode/raw/run001_c247c0_display_leaf/`, 33 starts / 212 operations,
  all mapped to verified Hunk 19.

## Observed path

The leaf saves `D0-D2`, loads a three-word tuple from `(A3)`, and tests byte
`1(A4)`. In this invocation it copies the tuple to `16(A2)`, increments that
byte, and branches to `$C2487A`; there it copies the tuple to `10(A2)`,
compares signed components, calls `$C248B2`, and returns status `1` in `D0`.

The alternate static paths from `$C247DC` were not executed in this packet.
