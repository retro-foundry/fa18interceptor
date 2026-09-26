# `$C1104C/$C11078`: postflight callback stages

Classification: **static byte-exact callback reconstruction**.

`source_amiga/observed/advance_postflight_callback_stages.asm` reconstructs
the adjacent entries `$C1104C-$C110A3`.  Both test the signed word at
`$C45AD6` and return unchanged while it is nonnegative.

On expiry, `$C1104C` clears `$C45795`, writes selector word `$000E` at
`$C4574A`, clears `$C457E0`, and installs `$C118FC` in `$C1820C`.

On expiry, `$C11078` writes one to `$C457AE`, reloads `$C45AD6` to two,
installs `$C110A4` in `$C1820C`, and writes one to `$C458AD`.  This makes it
the exact static producer of the callback slot used by run060's later
success-queue activation.  The run060 invocation of this particular entry is
not yet captured as a bounded instruction trace; the source establishes
dataflow, not the causal result predicate.
