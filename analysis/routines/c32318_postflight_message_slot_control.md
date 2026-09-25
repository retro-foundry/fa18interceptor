# `$C32318-$C32389`: postflight message-slot control

Classification: **static dataflow in a runtime-observed postflight parent path**.

`source_amiga/observed/select_postflight_message_slot.asm` is byte exact for
`$C32318-$C32389`.  The reconstructed parent invokes `$C322EE` only in its
postflight tail route, but this block has no independent branch oracle.

The block uses `$C4583C` as a two-tick delay byte and `$C459C4` as a
sign-marked slot word.  If the delay is inactive, it tests `$C45886`: a
negative value is converted to `1`, increments the slot word modulo the
observed `1..3` range, and sets bit 15 before storing it.  On the following
pass, clearing bit 15 identifies a changed slot; otherwise the delay counts
down and either submits through `$C325A6` or returns through `$C32678`.

This establishes slot/delay control flow only.  It does not identify the
message text, result state, or any qualification condition.
