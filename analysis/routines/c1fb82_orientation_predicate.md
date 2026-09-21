# Three-vector orientation predicate at `$C1FB82` (Hunk 10 +`$E4A`)

Classification: **structural arithmetic helper**. The observed path proves a
fixed-point three-vector orientation calculation, but does not identify the
coordinate space or caller's record type.

## Runtime packet

- No-input `start_demo` replay from `captures/baseline_menu/state.bin`.
- Called from `$C200B8`; breakpoint `$C1FB82`, return boundary `$C200BE`.
- 41 instructions, complete at the real return boundary.
- P-code: `pcode/raw/no_key_c1fb82_helper/`, 41 starts and 357 operations,
  every start mapped to verified Hunk 10.

## Observed branch

The call begins by testing flag bits in `D7`; the attract path reaches
`$C1FBD4`. It loads three signed word triples from `$C4BF94` and computes two
component differences from the first triple. It then evaluates the three
components of their cross product using signed multiplies and arithmetic
right-shifts by eight. Those components are multiplied by a following word
triple and accumulated in `D7`.

If the resulting signed longword is non-negative, `$C1FC32` returns `D7 = 1`.
If negative, `$C1FC34` returns the negative value. This is sufficient to call
the observed branch an orientation/sign predicate. The flags, alternate
branches, scaling convention, and what the vectors represent remain unknown.
