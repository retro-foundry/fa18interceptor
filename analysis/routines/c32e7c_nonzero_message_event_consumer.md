# `$C32E7C-$C32EAD`: nonzero message-event consumer

Classification: **static message-event dataflow**.

`source_amiga/observed/consume_nonzero_message_event.asm` is byte exact for
the continuation entered after the existing `$C32E5C` lookup reads a nonzero
event byte.  It treats byte value `$44` as a distinct branch to `$C32EAE`.
For other values, a positive `$C457DE` is decremented along with `$C457F9`;
the selected byte at `$C457E1 + sign_extend(D5)` is cleared, and `D5` advances
modulo ten into `$C457F8`.  It then enters the existing `$C32F54` static-text
setup path.

The byte queue, event code `$44`, counters, and rendered text are not given
gameplay names here.  This slice proves only the consumed-byte and index update
contract, not which player action or result state produced the event.
