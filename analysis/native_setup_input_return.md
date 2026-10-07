# Native setup-stage input preservation - 2026-10-07

Four more connected setup callbacks now preserve their actual preceding input
result: smoothing message publication, smoothing restart, context entry and
viewport-message completion. Previously the native stage boundary discarded
their result ownership. Before the fix, the extended runtime probe reaches
C10AE6 and fails with an unknown owner. A later message renderer can hide that
loss, so stage outputs are now checked before body/message composition too.

The active build is port/recomp/CMakeLists.txt -> port/native/CMakeLists.txt ->
fa18_native. The caller is native entry -> native_frontend_tick ->
native_flight_tick -> native_input_process -> run_post_input_tick -> stage ->
native_setup_stage. NATIVE_SETUP_INPUT_PRESERVED lets the existing stage
composition retain its actual NativeInputReturn. No original return value,
CPU register state or emulator dependency is supplied to native gameplay.
The removed dependency is the unresolved-output assumption at these callbacks.

## Original contracts

The authoritative callback instructions are in
analysis/data/menu_context_finish_source_scope.json. Child evidence is
source_amiga/observed/initialize_message_sequence_state.asm and
source_amiga/observed/initialize_c16d04_activity_inputs.asm.

| Callback | Source contract | Exercised branches |
| --- | --- | --- |
| C10AB2 / queue_menu_smoothing_message | D0/A0 assignments only; no children | Countdown wait and expired message publication |
| C10AE6 / restart_menu_smoothing | D0/A0 and C11312 message reset; no inherited-byte assignment | Enabled-command wait and full reset/restart |
| C10C08 / begin_menu_context | D0/A0 and memory stores only; no children | Unselected context entry, selected positive event, selected negative event |
| C11A50 / finish_menu_viewport_message | D0/D1, C11312 and C16D04; preserves the inherited byte under the existing timer host contract | Negative-event wait, context-state-six return, timed completion with/without optional accumulator |

C11312 clears its sequence words/state through D0/A0. C16D04 initializes the
request through A0 and stores its returned pair without assigning the inherited
byte. The native timer owner writes seconds/microseconds from the existing
clock. The original comparison supplies that same OS timer request boundary,
preserving its existing register contract; game instructions and time-accounting
arithmetic execute normally. Wrapped pending-time subtraction is exercised.

Other setup callbacks still invalidate unresolved output at their boundary.
Active drawing continues to start with unresolved output until its existing
owner assigns one. This batch does not establish unexamined callback contracts.

## Connected original chain

The disk/input-backed countermeasure entry extends its ordinary Free Flight
run with twelve cases for each callback. Only this validation entry selects
the source callback/gates. Each case inherits a real preceding recorder result;
no native input result or return owner is seeded.

Each actual input/stage interval is captured before and after its callback.
Branches that enable record work are captured first, then the validation entry
selects POST_INPUT_AUX=0 for a separate controlled idle body. That body retains
the stage output or replaces it through the actual final message owner. The
next recorder parent consumes the completed result without an intervening
keyboard assignment. This separates callback integration from active physics;
it does not claim that these selected branches naturally produce idle frames.

The original chain executes C0EFD4 -> C0F3C4 -> C0F5F8 -> selected callback
through C0EFEA, followed by the independently captured body and recorder parent.
Each original stage receives the preceding independently verified original
recorder result. Native runs finish before original comparisons begin.

Stage and body results are separate expectations. In the first added case,
C10AB2 retains 7 while its later message renderer produces 190. Later selected
callbacks retain 190. Comparing a stage against the final body result would
miss this distinction; the stage metadata now records its actual owner/byte.
Existing C11A26 idle cases also retain their separate stage checks.

## Validation and remaining scope

`python tools/native/check_countermeasures.py --out build/native-flight/setup-input-check`
passes 247 complete bodies, 264 recorder parents and 84 intervening keyboard
parents against original compared RAM/drawing and defined returns. Sixty
separate actual input/stage parents pass, including the 48 additions. Component
checks retain 2,512 pending-input cases, 12,576 selected command parents and
sixteen Delete parents. No drawing/HUD bytes are newly excluded, no clocks are
fitted and passing RAM remains temporary. Native Debug/Release builds pass.

Eleven affected Release CTests pass: mode_four, smoothing_cancel,
countermeasures, frame_body, frontend, frame_tail, postflight_entry, input,
game_input, qualification and artifact_cleanup. Aggregate case counts,
stage/body outputs and report/binary hashes are retained in
[figures/native_setup_input_return_checkpoint.json](figures/native_setup_input_return_checkpoint.json).

Independent full mission/combat/outcome acceptance, other stage/reset/HUD
contracts, readable typed state, audio fidelity and broader visible-window
performance remain unfinished. The complete-port goal stays active.
