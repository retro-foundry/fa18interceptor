# `$C0F992`: post-input follow-up callback

## Evidence

Static disassembly of `$C0F992-$C0FA03`, reconstructed byte-for-byte in
`source_amiga/observed/begin_post_input_followup.asm`. `$C0F974` installs this
address in `$C1820C` after its timer gate.

The callback invokes `$C0F4A6` and `$C08F26`. It then branches on
`$C4584B == 3`: that route sets `$C457AE`, `$C45AD6`, `$C458A1`, `$C458A6`,
and advances the callback to `$C0FA04`. The other route clears `$C4584B`,
calls `$C11ACC` with `$C08490`, and advances it to `$C0FCB4`.

Neither route has a bounded runtime packet yet, so its game-level purpose is
unassigned.
