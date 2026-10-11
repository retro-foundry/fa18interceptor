# `$C1B4F4` high-nibble control-field routes

Classification: **structural with byte-exact source**. Three queue entries
select `$20`, `$10`, or `$00`. The common helper latches the selection at
`$C4582E`; if either observed gate `$C457AD` or `$C457B4` is nonzero, it
clears bits 5–4 of `$C461E9` and merges the selection.

The route is a precise byte-level state update. Its control-name binding is
deferred until isolated raw-key traces are available.

## run060 initial-turn direct route

The queue entry `$C1B4F4` is not reached in the frame-900--960 window, but it
is not the only entry into this contiguous helper block. The sealed run060
input phase at frame 942 directly calls `$C1B50C`, which loads `D2=$20` and
jumps to the shared publisher at `$C1B516`. With `$C457AD` non-zero, the
publisher clears bits 5:4 of `$C461E9`, merges `$20`, and stores `$21` at
`$C1B538` (the prior value is `$01`).

This is the fresh direction command that the frame-949 `$C1B410` update uses
to decrement root `+$28`. The input phase reaches `$C1B50C` through its
`$C16F88` direct call; the raw event-to-derived-bit interpretation remains
separate from the proven control-mask write.

The complementary sealed run060 `J 0 4 1` input phase (breakpoint hit at frame
1008) reaches `$C1B510` instead. It selects `D2=$10` and the same shared
publisher writes `$C461E9: $01->$11`. `$C1B410` defines `$10` as the increment
code for signed lane `+$28`, making `$10` and `$20` the observed opposite
commands for that lane. The physical joystick-direction labels remain
unassigned.
