# Normal-update preparation at `$C12098`

Classification: **behavioural route with complete static reconstruction**.
From the sealed run003 frame-6,000 state, `$C45795` is one and the parent
calls `$C12098` from `$C0F002`. With empty future playback, it returns to
`$C0F008` after 25 instructions. P-code is
`pcode/raw/run003_6000_c12098/` (25 instruction starts, 125 operations).

The live route bypasses `$C1B906` because `$C45891` is nonzero, selects
`$C46184 + ($C458DC << 9)`, masks that record's byte at offset `$62`, and
uses bit 1 of `$C458C6` to select the observed return route. The record-type
and field meanings remain unassigned. The entire `$C12098-$C12241` routine is
byte-exact source in `source_amiga/observed/prepare_normal_update_state.asm`;
unobserved branches retain neutral helper and field names.
