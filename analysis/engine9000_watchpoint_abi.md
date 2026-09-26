# Engine9000 v0.62-alpha watchpoint ABI

## Authority

`tools/engine9000-src/` is a Git submodule pinned to the RetroFoundry fork
[`fa18-interceptor` commit `ace4c3a`](https://github.com/retro-foundry/engine9000-public/tree/ace4c3a9553e7005ed32c6fee8817126a53a8887).
It derives from upstream `alpine9000/engine9000-public` tag `v0.62-alpha`,
commit `f9ca09b449866cba22ee9891757e8e6982f68600`. The fork only changes the
debugger's function-key defaults, leaving F1--F12 available to the emulated
F/A-18 runtime. It is an analysis dependency; the runtime DLL hash remains
recorded in `local/toolchain.json`.

`e9k-lib/e9k-lib.h` defines the watchpoint source identifiers used by the
DLL ABI: CPU `1`, DMA `2`, blitter `3`, Copper `4`, audio `5`, video `6`,
peripheral `7`, and disk `8`.  `scripts/check_replay_watchpoint.py` now
exposes those source selectors and an address-mask argument directly.

## Validation

On ordinary run029 replay, armed at frame 993, an exact CPU-write watch at
`$C02D06` hits in frame 993.  Its watchbreak reports a 16-bit CPU write at the
same address, with source ID `1`.  This validates the helper's argument order,
address comparison, and CPU source filter against the pinned ABI.

The normal frame-993/994 cockpit comparison proves bytes `$04DB58`,
`$051FAB`, and `$053EEB` differ.  A broad write watch (`address=0`,
`mask=0`) filtering source ID `3` nevertheless misses across frames 1–1000.
The three exact active-plane watches also miss.  Therefore this released
runtime does not expose the relevant blitter writes through its watchbreak
stream, despite the upstream source's blitter source tag.  The cause may be a
build/runtime path difference; it is not assigned further here.

## Consequence

Use the helper for CPU write provenance.  Do not use its blitter-watch misses
to infer that a Chip-RAM byte was unchanged.  For the cockpit formatter path,
the next oracle remains a blitter-completion/destination log or a capture of
the pending blitter state at the frontend snapshot boundary.
