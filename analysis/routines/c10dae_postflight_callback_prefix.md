# `$C10DAE`: postflight callback prefix

Classification: **scenario-anchored static dataflow**.

The byte-exact source is
`source_amiga/observed/handle_postflight_callback_prefix.asm`, covering
`$C10DAE-$C10E95`.  It is bounded by the alternate-state branch at `$C10E96`
and by two word branches to the shared epilogue `$C11048`.

Adjacent native run060 checkpoints show that the frame-9,284-to-9,285
success-selector activation changes callback slot `$C1820C` from `$C110A4` to
this entry `$C10DAE`.  The prefix tests bit 9 of `$C46184`, the low nibble of
`$C46200`, and `$C4578C`; its nonzero-gate route increments word `+$10` at
the pointer in `$C1AB74`, clears exact bits in `$C458CA`, `$C458CC`, and
`$C46184`, and installs `$C11788` in `$C1820C` before joining `$C11048`.

The gate-clear route clears the same `$C46184` bit, calls `$C11BB0` with
`$4021`, and also joins `$C11048`.  The call targets and field meanings are
not promoted beyond their literal dataflow effects.  In particular, this
callback-slot transition does not prove that `$C10DAE` writes the selector
queue or decides qualification success.
