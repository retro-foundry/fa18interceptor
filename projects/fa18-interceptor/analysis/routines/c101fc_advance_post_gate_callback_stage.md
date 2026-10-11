# `$C101FC`: post-gate callback stage

Classification: **static callback/timer dataflow**.  Byte-exact source:
`source_amiga/observed/advance_post_gate_callback_stage.asm`.

The callback tests word `$C45AD6`.  While it is nonnegative it returns without
writes.  Once negative, it clears `$C45795` and `$C458A0`, sets `$C458A1` to
`$0F`, and installs `$C10228` in callback slot `$C1820C`.

`$C0FECE` installs this address in the run024 selected-mode-9 transition at
frame 627.  The subsequent `$C10228` equality wait and later callbacks remain
unobserved, so this is only a timer/callback-stage contract, not a claim about
flight start or qualification persistence.
