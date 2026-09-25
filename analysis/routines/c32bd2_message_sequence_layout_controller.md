# `$C32BD2-$C32CEC`: message-sequence/layout controller

Classification: **behavioral text/layout dataflow**. This routine controls a
queued selector-word cursor and layout/glyph state. It is not identified as a
crash, qualification, mission, or persistence-state routine.

The byte-exact reconstruction is
`source_amiga/observed/advance_message_sequence_and_prepare_layout.asm`.
`python scripts/verify_reconstructions.py` verifies all 284 bytes against the
runtime Slow-RAM image.

## Static contract

- `$C32BD2` reads the byte cursor at `$C457C6` into the word sequence rooted
  at `$C4574A`. A nonzero next word advances the cursor by two and branches to
  `$C32CE6`, which clears the active flag at `$C457C3`.
- A zero next word sets `$C457E0` to one and `$C45744` to `$32`, clears the
  cursor and first sequence longword, then returns through `$C32CEC`.
- `$C32C04-$C32CB2` restores/publishes the `(A1,A2,A4)` layout triplet,
  maintains the byte/word control fields at `$C457D7/$C457DB/$C457DC/$C457DF`,
  and resumes `$C330F4` after publishing the static layout base `$C41066`.
- `$C32CC4-$C32CEC` restores the `$1B8` layout offset, decrements positive
  `$C45744`, conditionally sets bit 7 of `$C457E0`, clears `$C457C3`, and
  returns.

## Runtime boundary

The run062 frame-2250 native checkpoint trace executes this block while the
crash message is appearing, and the later frame-2300 trace reaches its
completed-message tail. The frame-2250 direct-core continuation does not
pixel-match native frame 2255, so those rows establish executed control flow
and register/memory dataflow only. They must not be used to claim the exact
character cadence, crash payload source, or on-screen pixel ownership.
