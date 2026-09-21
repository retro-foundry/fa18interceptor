# Renderer helper at `$C2F60A`

Classification: **structural**. The direct run001 edge `$C302D6 -> $C2F60A ->
$C302DA` completes in 69 instructions at replay frame 11. It has no nested
call target.

P-code: `pcode/raw/run001_c2f60a_renderer_helper/`, 69 observed starts /
365 operations. It is retained as bounded arithmetic/data-flow evidence;
its inputs and output ownership remain unassigned.


`source_amiga/observed/submit_adjacent_renderer_values.asm` is the byte-exact
24-byte static wrapper `$C2F60A-$C2F621`. For low-nibble-zero `D0`, it calls
`$C2F5F4` twice with adjacent values; the nonzero branch enters `$C2F626`.

`$C2F626-$C2F639` is now byte-exact source in
`source_amiga/observed/enter_alternate_renderer_table_helper.asm`. It loads
the same pointer block and second table as the primary entry, but selects the
alternate word table at `$C2F7C6` before joining `$C2F688`.

The bounded entry `$C2F66E-$C2F687` is reconstructed in
`source_amiga/observed/enter_bounded_renderer_table_helper.asm`. It redirects
to `$C2F60A` when `D1` meets/exceeds `$C45984`; otherwise it selects the
`$C2F7C6/$C2F7E6` table pair and falls into the same shared body.

The remaining `$C2F63A-$C2F66D` wrapper is reconstructed in
`source_amiga/observed/submit_offset_renderer_values.asm`. It offsets `D0/D1`
by `$C45988/$C458D8`, rejects out-of-range X through `$C2F622`, preserves the
adjusted pair on the stack while calling the alternate-table shared body, then
restores them. This range is static-only in the current captures.
