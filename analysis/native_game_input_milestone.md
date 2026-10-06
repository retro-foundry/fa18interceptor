# Native game-input comparison milestone

Scope: keyboard input consumed by the original game, distinct from hardware
injection in sealed `FA18_LOOP_INPUT_V1` recordings. This bounded milestone is
100% complete; it is not whole-game or video-frame parity.

The active native path is `port/native/main.c` -> `native_replay_update` ->
`native_frontend_raw_event` -> `native_input_process` -> the shared C0F3C4,
C16C56 and C1AD74 owners. Native continues to dispatch host events without
CPU, CIA or OS emulation. No recorded game state enters the native runtime.

The sealed carrier recording first diverges at iteration 5015 when native
receives both injected edges immediately. The original reference dispatches
the Down release there, but does not dispatch that simultaneous Right press.
The input file records injection into the reference keyboard queue; this is
not a guarantee that the game consumes every edge in that same update.

`fa18_recomp --game-input-out PATH`, restricted to `--ports off`, exports the
raw keys at the original C1AD74 call, after OS delivery. Its separate
`FA18_GAME_INPUT_V1` header identifies consumption iterations. Both formats
use `<iteration> <frame metadata> K <raw key> <down>` and an `end` row. Native
accepts either through `--input`; consumed-key exports are the appropriate
input for isolated game-behavior comparisons. The reference export excludes
first-instruction resumptions, which otherwise duplicate toggle commands.
Sealed recordings are neither edited nor relabelled.

Validation:

- GNU reference and MSVC native/reference/ROM-free builds pass.
- `tools/native/check_game_input.py` compares reference runs with and without
  export through 6080 PAL frames. Final RAM, registers, statistics and the
  iteration-5509 RAM snapshot are identical. Unsupported reference modes fail
  before creating an evidence file.
- Native cold startup followed by the exported keys through iteration 5508
  matches original stick state and 161 of the first 164 aircraft-record bytes,
  including position, orientation, speed, motion and the arresting-hook toggle.
  The existing placed-region bit 2 and countdown offset of 16 updates remain
  explicitly open. No extra mismatches are suppressed.
- Frontend/save/reload/SDL/link omission and the injected-input replay checks
  pass. Same-update injected edges still preserve their supplied order.
- A complete carrier reference export reaches 8038 iterations in 12353 video
  frames and contains 554 consumed keys, versus 556 injected edges. With the
  corrected export, native reaches touchdown but fails loudly at the missing
  `FC_RESET_CONTROL` / C083E2 child. This is not carrier acceptance.

Next: connect the existing C083E2 mission reset at the live touchdown caller,
then compare approach/outcome state. Whole C0EFD4 frame ownership, startup flag
and countdown alignment, audio and demo gameplay remain open. Copper fade is
excluded from visual comparisons. The expensive complete reference suite was
not rerun; carrier diagnostics were limited to the unresolved input/landing gap.
