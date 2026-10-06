# Native intro to menu: first functional milestone

Requested 2026-10-06. Existing game C is composed by `port/game/native/frontend.c`;
entry and explicit build sources are in `port/native/`. Source authority and
launch commands are in `port/native/README.md`.

The real native caller is `main` -> `native_frontend_tick` ->
`advance_main_loop_message_sequence` -> `plot_glyph8`; menu entry also calls
`queue_top_level_menu_messages`. SDL presents the ordinary bitplane buffers.
ADF/Hunk loading supplies original data, without executing any game/OS code.

Source OFF ADF reference screens were sampled only for this changed UI path:
splash at frame 1000, credits at 1800, settled menu at 3000 with Space at 1800.
Native settled images match all 81,920 RGB pixels for each screen. Digests of
raw RGB bytes, excluding the PPM header:

| Screen | SHA-256 |
| --- | --- |
| Splash | b7fdc5a1957229042f733eb1030332145cb7ef7cfa5d7eed91af4a2e506d1e2e |
| Credits | b6395078ddc314dc782613d7845bf3bd366b7aea27c292814b0073097d9e7f11 |
| Menu | 6d32c81d93af3cbdaf91a36dac49b31bbaacc5e0d86ecc19784891b30c9497c7 |

`tools/native/check_frontend.py` exercises these actual connected screens,
first-tour callsign editing (including Backspace and Return), exact 78-byte
save/reload, SDL dummy window presentation and frame-limit shutdown, missing
ADF and malformed saved-config rejection, and an unchanged ADF digest. It
also inspects the native link map: required game functions are present;
Musashi, translation/runtime, bus, glue and chipset symbols are absent.

MSVC native/reference builds and GNU reference runners build. The existing
twelve CTests and GNU profiling invariance pass. No full sealed replay ran.
The native check is also registered as `fa18_native_frontend` in CTest.

**Progress: first functional intro -> menu milestone complete.** This is not
whole-game progress or a timing-parity claim. The native source busy pause
uses nominal bus-free timing; CPU-paced text cadence and Copper fade timing
remain unmodeled. Source audio suppression is active. Numbered menu choices
are displayed; launching their game modes is the next milestone. Data still
uses original-address identifiers in ordinary checked host storage, rather
than fully typed state. Pixel captures are disposable reference evidence in
`build/native-reference/`, never an input to the native game.
