# Joystick callback-descriptor removal at `$C1748C`

Classification: **bounded behavioral helper**. The 20-byte byte-exact source
is `source_amiga/observed/remove_joystick_callback_registration.asm`.

The routine pushes descriptor `$C1ABF0` and kind `5`, calls `$C53B18`, drops
the two arguments, and returns. The bounded native-Delete trace enters it from
`$C06BF0`, then executes `$C17456` before returning to `$C0F45C` in 104
instructions. In that trace, `$C53B18` reaches the runtime's linked-list
removal path, so the routine is named for its observed remove-then-register
pairing; the service ABI and descriptor-kind semantics remain unassigned.
