# `$C16C56`: raw keyboard-event poll

## Evidence

`source_amiga/observed/poll_raw_keyboard_event.asm` reconstructs the complete
`$C16C56-$C16CD7` routine byte-for-byte. It is called by the bounded
`$C0F43A` poll-and-dispatch loop in the recorded `H` press/release packet.

## Contract

The routine first consumes a pending word at `$C08182/$C1ABC8`. Otherwise it
calls `$C16BF2`, returns `$FF` when that source yields zero, strips bit 7 into
a seven-bit base value, filters values whose masked `$70` bits equal `$70`,
and restores bit 7 when the input carried it. It returns the resulting byte
sign-extended in `D0`.

This exactly accounts for the observed `$25` press, `$A5` release, and `$FF`
empty-poll values. It does not establish a complete mapping between physical
keys and raw values; that requires recordings such as the existing isolated
key packets.
