# `$C1B540` mid-nibble control-field routes

Classification: **structural with byte-exact source**. Three queue entries
select `$08`, `$04`, or `$00`, latch the byte at `$C45830`, and conditionally
replace bits 3–2 of `$C461E9` when either observed control gate is nonzero.

This is adjacent to the `$20/$10/$00` selector family but is reconstructed as
a separate exact route because it uses different latch and bit positions.
