# `$C1104C/$C11078`: postflight callback stages

Classification: **static byte-exact callback reconstruction**.

`source_amiga/observed/advance_postflight_callback_stages.asm` reconstructs
the adjacent entries `$C1104C-$C110A3`.  Both test the signed word at
`$C45AD6` and return unchanged while it is nonnegative.

On expiry, `$C1104C` clears `$C45795`, writes selector word `$000E` at
`$C4574A`, clears `$C457E0`, and installs `$C118FC` in `$C1820C`.

On expiry, `$C11078` writes one to `$C457AE`, reloads `$C45AD6` to two,
installs `$C110A4` in `$C1820C`, and writes one to `$C458AD`.  This makes it
the exact producer of the callback slot used by run060's later success-queue
activation.

The sealed run060 checkpoint at global frame 9,200 was resumed with the
recording suffix aligned to that checkpoint.  It reaches `$C11078` 63 restored
frames later (global frame 9,263) with `$C45AD6=$FFFF`.  Its bounded ten-
instruction trace takes the expiry route, stores `$0002` to `$C45AD6`, stores
`$C110A4` to `$C1820C`, and returns to `$C0F808`.  The retained fixture is
`build/run060_frame09200_c11078_handoff_trace_v2/`.  This directly joins the
post-input scheduler to the later success queue, but does not identify the
physical or result predicate that selected this scheduler state.
