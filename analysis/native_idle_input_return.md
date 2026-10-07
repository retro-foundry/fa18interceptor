# Native viewport stage and idle input preservation - 2026-10-07

The native viewport-message stage and idle frame now preserve the actual
preceding input result. The two idle bodies left unresolved by the context
camera batch return their original queue outputs, including nonzero index 7.
Twelve further idle bodies pass that result directly to recorder input,
without an intervening command assigning a replacement.

The connected caller is native entry -> native_frontend_tick ->
native_flight_tick -> native_input_process -> run_post_input_tick -> stage ->
native_setup_stage -> queue_menu_viewport_message. NativeSetupStageResult
identifies the demonstrated preservation contract. Other stages still
invalidate unresolved output at their actual callback boundary. Active record,
scene and drawing composition likewise starts with unresolved output until an
existing source owner assigns it.

The idle branch continues through tick_notification_cadence,
update_control_actions and the existing scene-label/debug owners. The former
unconditional frame-entry invalidation discarded a real result even when none
of these owners assigned one. Moving that invalidation to active drawing lets
the idle gates preserve the preceding named domain result. No CPU register
file, opcode helper or original-return value is added to native gameplay.

## Original contracts

- C0F5F8's prefix and C0F7D2-C0F811 tail use D0/D1 for offset/phase/countdown
  work and clear KEY_TAKEN after the selected callback. The callback owns its
  output. Original scope is in source_amiga/observed/run_post_input_tick_*.
- C11A26-C11A4E clears POST_INPUT_AUX, compares VIEWPORT_MODE/TARGET in D0/D1,
  and either returns or queues message $26 and installs C11A50 through A0.
  All nine original instructions preserve the inherited byte; no child is
  called. See analysis/data/menu_context_finish_source_scope.json.
- C11B44-C11BAF's notification countdown/code calculation uses D0 only.
  See source_amiga/observed/update_periodic_notification_code.asm.
- C12950-C129AA's record and gate prefix uses D0/D1/A0 and saves D2.
  With POST_INPUT_AUX=0 it exits at C1316E before sound children or an
  inherited-byte assignment. Earlier context/pause/event exits also lie before
  those assignments. See analysis/data/control_readouts_source_scope.json.
- The idle C2B3C2 scene-label and C0F386 debug gates assign no replacement.
  Their existing named preservation results compose into the final message
  owner, which retains its own existing assignment/preservation contract.

## Independent original chain

The cleanup comparison now follows the captured runtime order: complete body,
intervening keyboard command when present, then recorder input. The preceding
original recorder result supplies the next original idle body's incoming
byte. The oracle's FA18_FRAME_INITIAL_INPUT_CARRY parameter is reference-only;
the native run finishes before these original comparisons begin and never
reads their output. Thus nonzero idle preservation is checked against a
previous independently verified original result, rather than the oracle's
former default zero or a native expected value.

Twelve additional parent captures bracket the actual native input/stage call
before the controlled body fixtures. FA18_FRAME_STAGE_ONLY executes original
C0EFD4 -> C0F3C4 -> C0F5F8 -> C11A26 through C0EFEA and compares their stores
and low-byte result. Empty host GetMsg/release and zero physical buttons are
explicit shared host contracts; all game instructions execute normally.
Only the original viewport callback/mismatch gates are selected by this
bounded fixture. Its incoming result comes from a real preceding recorder
parent; native output is never seeded.

## Validation and remaining scope

The extended comparison retains 199 complete bodies, 216 recorder input
parents and 84 intervening keyboard parents. Fourteen actual idle bodies
preserve their queue owner and values 0/7; the additional twelve recorder
parents directly consume those idle outputs. The separate twelve input/stage
parents establish the preservation contract before notification/body work.
Both drawing pages remain checked and no new RAM/display exclusions or fitted
clock/output seeds are introduced. Passing RAM uses temporary capture storage.

`python tools/native/check_countermeasures.py` passes the complete expanded
comparison, including all twelve input/stage parents. The existing focused
input suite retains 2,512 pending-input cases, 12,576 selected command RAM/
return cases and sixteen Delete parents. Native Release/Debug and both
reference MSVC runners build. Ten affected native CTests pass: callback reset,
host keys, scene exit, frame body, frontend, frame tail, input, game input,
qualification and artifact cleanup.

This resolves the bounded idle gap from analysis/native_context_input_return.md.
It does not establish the other callbacks' outputs, reset/invalid command
returns or all early HUD paths. Independent full mission/combat/outcome
acceptance, readable typed state, audio fidelity and measured 20 ms frame
performance remain unfinished. The complete-port goal stays active.
