# Native postflight entry and reset wait

The actual native caller is post-input stage dispatch -> C10DAE setup context.
Its reached MC_STAGE_SETUP now calls C0F4A6 `free_all_voices`; MC_STAGE_SOUND
uses the exact C10E64 arguments for C17F8C `start_sound_6(0x3f,0x78)`.
MC_STAGE_COMMAND uses C11BB0's filtered message 0x4021 and MC_STAGE_FINISH
requests C082B8 cockpit redraw. These are existing source owners, not new
outcome rules or audio substitutes.

Native flight stage dispatch now binds the existing C11788-C119D4 completion
and message callbacks, including C1104C. Runtime evidence currently reaches
C11788; later callback bindings are not claimed as exercised reset gameplay.
The source default child implementations use direct scene/renderer owners.

The 8200-tick pullback run now completes through postflight entry: callback
C11788, 1033 record updates, 1031 scene/HUD frames, 19318 model calls. The entry
and C11788's wait/release cases match original non-stack RAM in a bounded
oracle; the real 8200 checkpoint's view/record update comparison also passes.
MSVC native and GNU oracle builds pass and native link omission passes.
`tools/native/check_postflight_entry.py` reproduces the reached runtime path
and three original entry/reset cases. No full replay repeated.

This entry batch is complete; startup wiring remains roughly 99%. Full reset
is not demonstrated: C11788 waits on C45899's activity count 0x28. The source
C1612C outer display owner alternates palettes, waits for presentation and
consumes that counter before changing the drawing page. Native outer display
ownership/pacing and two-page presentation remain open. Do not substitute a
per-game-tick decrement or collapse this source delay. Continue by integrating
the existing C1612C source owner with resumable host PAL presentation, then
exercise the next reached callback. Complete frame/Stores ownership and
recorded-flight acceptance remain separate work. Copper fade is excluded.
