# `$C1B540` mid-nibble control-field routes

Classification: **structural with byte-exact source**. Three queue entries
select `$08`, `$04`, or `$00`, latch the byte at `$C45830`, and conditionally
replace bits 3–2 of `$C461E9` when either observed control gate is nonzero.

This is adjacent to the `$20/$10/$00` selector family but is reconstructed as
a separate exact route because it uses different latch and bit positions.

## run060 `J 0 6` instance

The sealed run060 input phase reached at frame 1754 after the frame-1751
`J 0 6 1` event directly calls `$C1B558`. That route selects `D2=$08`, passes
the active `$C457AD` gate, and writes root control byte `$C461E9: $00->$08`
at `$C1B586`. `$C1B410` interprets bits 3:2 as the third signed control lane
(`+$2A`); `$08` is its decrement code. This proves a second input-controlled
lane, while its eventual orientation or motion role is not yet assigned.
