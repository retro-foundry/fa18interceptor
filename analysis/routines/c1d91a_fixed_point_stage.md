# Fixed-point stage at `$C1D91A` (Hunk 8 +`$1662`)

Classification: **structural arithmetic helper**. Its inputs and output are
proven at the word/longword level; their game meaning is unknown.

`source_amiga/observed/fixed_point_stage_tail.asm` is a byte-exact 190-byte
observed-entry slice for `$C1D91A-$C1D9D7`. It retains alternate static paths
and the branch to the earlier `$C1D90A` code, which lies outside this slice.

## Runtime packet

- Direct edge `$C1CC2E -> $C1D91A -> $C1CC34` in the deterministic human-flight
  stage.
- 53 instructions, complete at the observed return boundary.
- P-code: `pcode/raw/run001_c1d91a_stage_child/`, 53 starts / 481 operations,
  all mapped to verified Hunk 8.

## Observed arithmetic

The packet reads `$C45A72` (word), `$C45A76` (word), `$C45A78` (longword), and
the variable shift at `$C45AB8`. It combines the shifted values with input
registers `D2-D4`, normalizes signs, applies fixed right shifts by four, then
uses the word table at `$C1D9D8` for unsigned multiply/divide scaling. The
completed path constrains the result to `$7FFF` and stores its low word at
`$C45B40`.

Alternate code at `$C1D90A` is statically reachable but was not executed in
this packet.
