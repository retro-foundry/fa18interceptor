# `$C17EF2`: guarded command-packet publisher

Classification: **bounded dataflow**.

`$C17EF2-$C17F8B` tests longword `$C0A448`.  If zero, it immediately returns.
Otherwise it calls `$C17B08` with literal 3, copies stack arguments into the
fixed slots `$C50B7C-$C50BC4`, left-shifting the argument values at stack
offsets 8, 12, 20, and 24 by 16 bits before publishing them.  It then writes
zero and one at offsets `$34` and `$2C` of the guarded pointer and calls
`$C17B2C` with stack literals 4, 3, and 0.

Authority: the return-bounded controlled run029 F1 command-side-effect trace
at `build/run029_f1_c3318e_side_effect/`.  It enters from `$C331BE`, executes
the full `$C17EF2` packet, and returns through `$C331C4`; the enclosing trace
returns to `$C1BDF8` after 1,923 instructions.  The same shared wrapper is
reached by the controlled digit-6 menu entry.

The slot and record identities are not assigned.  In particular, this packet
does not itself read the message selector table or prove mission-menu state.
