# `$C322EE-$C32317`: signed-state and control-timer gates

Classification: **static dataflow**.

The byte-exact source is in
`source_amiga/observed/gate_c322ee_signed_state.asm` and
`source_amiga/observed/gate_c322f8_control_timer.asm`.

`$C322EE` loads word `$C459C0` into `D2`; a negative value branches to
`$C32510`.  The fall-through at `$C322F8` masks `$C458CC` with `$0081` and
takes that same branch if either retained bit is set.  Otherwise it decrements
`$C45887`; a nonnegative result also branches to `$C32510`, while a negative
result is reset to byte `$FF` and falls through to `$C32318`.

This proves only the local gate and byte-timer/reset dataflow.  It does not
prove the ownership of the state word, mask bits, or timer; in particular it
does not establish a qualification, landing, or failure predicate.
