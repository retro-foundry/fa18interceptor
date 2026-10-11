# `$C32E2E-$C32E5B`: message-layout state guards

Classification: **static message-layout dataflow**.

`source_amiga/observed/advance_message_layout_state_guards.asm` is byte exact
for the bridge between the existing `$C32E1A` state guards and `$C32E5C`
event lookup.  It decrements `$C45746`; a positive result branches to the
external `$C32CEE` message-sequence entry.  Otherwise it tests bit 0 of
`$C457DC`: clear restores the saved `(A1,A2,A4)` triplet from `$C4570A` and
branches to `$C32EFC`; set continues at `$C32E4E`, where it decrements
`$C45748` and branches to `$C32EFC` when nonpositive.

The surrounding message sequencing and the meanings of the three state words
remain unassigned.  The addresses are named by dataflow only; this slice does
not establish which visible text or result state selects either branch.
