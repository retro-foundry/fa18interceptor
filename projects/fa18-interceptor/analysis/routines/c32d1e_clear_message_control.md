# `$C32D1E`: clear message control before record selection

Classification: **static dataflow**.

The byte-exact source is
`source_amiga/observed/clear_message_control_before_record_selection.asm`.
The six-byte shared fall-through at `$C32D1E-$C32D23` clears the byte at
`$C457F6`, then immediately continues at the established `$C32D24`
message-record selector entry.  The raw bytes are:

```text
$C32D1E  42 39 00 C4 57 F6    CLR.B $C457F6.L
```

The adjacent selector independently establishes that `$C457F7` is one byte of
its command-control state.  This byte clear neither establishes ownership of
the neighboring byte nor assigns this fall-through a scenario role.  In
particular, it is not evidence of a qualification success or failure test.
