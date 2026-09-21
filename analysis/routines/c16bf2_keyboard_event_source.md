# `$C16BF2`: keyboard-event source wrapper

Static, byte-exact reconstruction: `source_amiga/observed/read_keyboard_event_source.asm`.

The wrapper asks `$C53C08` whether the handle at `$C0815C` has data. On success,
it consumes and clears word `$C1ABAC+6`, calls `$C53C8C` with `$C08134`, and
returns that word's low byte. `$C16C56` turns a zero return into its `$FF`
no-event value.

The external wrapper APIs and descriptor ownership remain unassigned.
