# `$C1BB7A` raw `$44` weapon-cycle command

Classification: **behavioural with byte-exact source**. The first sealed
run003 Return event maps to raw `$44`, branches here from `$C1AD74`, and the
complete dispatcher packet returns to `$C0F45C` in 2,042 instructions.

In the observed default route, the handler calls `$C33186`, requires the low
nibble of `$C46200` to be zero, sets `$C4599A` bit 4, decrements the high
nibble of `$C461E7` by `$10` with `$30` wrap, writes 3 to `$C45843` and
`$C45844`, clears `$C458B4`, then reaches the common queue tail. This matches
the documented Return weapon-select control, while the field names remain
limited to these observed updates.

Canonical P-code is `pcode/raw/run003_return_first_dispatch/` (265 observed
RAM starts, 1,514 operations). The static `$C1BB7A-$C1BC11` slice is
`source_amiga/observed/cycle_weapon_selection.asm` (152 bytes).
