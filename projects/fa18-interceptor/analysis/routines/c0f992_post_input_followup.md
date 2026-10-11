# `$C0F992`: post-input follow-up callback

The complete 23-instruction entry is now registered through
`port/game/scene_bootstrap.c`, with normal CPU and source-timing adapters.
All 8,192 complete full-register/full-SR/all-RAM cases and the demo's normal
completed sandbox call pass. Both branches and required source children are
preserved. See `native_c_scene_bootstrap.md`; the static evidence below is
historical, not the current proof scope.

## Evidence

Static disassembly of `$C0F992-$C0FA03`, reconstructed byte-for-byte in
`source_amiga/observed/begin_post_input_followup.asm`. `$C0F974` installs this
address in `$C1820C` after its timer gate.

The callback invokes `$C0F4A6` and `$C08F26`. It then branches on
`$C4584B == 3`: that route sets `$C457AE`, `$C45AD6`, `$C458A1`, `$C458A6`,
and advances the callback to `$C0FA04`. The other route clears `$C4584B`,
calls `$C11ACC` with `$C08490`, and advances it to `$C0FCB4`.

Both routes now have complete source-backed proof as described above. Their
higher-level game purpose remains unassigned beyond these observed effects.
