# Native postflight-stage input preservation - 2026-10-07

Nine connected postflight callbacks now preserve their actual preceding input
result. Before the fix, the extended runtime probe reaches C11872 with unknown
stage ownership, even though the original countdown/expiry path assigns no
replacement. This loss can affect the next depleted recorder command.

The active build is port/recomp/CMakeLists.txt -> port/native/CMakeLists.txt ->
fa18_native. Its caller is native entry -> native_frontend_tick ->
native_flight_tick -> native_input_process -> run_post_input_tick -> stage.
The selected callbacks run their existing domain functions; the stage caller
then retains the real NativeInputReturn instead of invalidating it. No CPU
state, original return value, new game rule or replacement timer is introduced.
The removed dependency is the unresolved-output assumption at these callbacks.

## Source contracts and exercised branches

All nine complete callback instruction sets are in
analysis/data/postflight_completion_source_scope.json. Their D0/D1/A0 and
memory assignments preserve the inherited byte. They call no external or
game children, including the message-flag reset, which is inlined in the source.
The selected C0F5F8 prefix/tail routes likewise assign no replacement byte.

| Callback | Connected owner | Branches exercised |
| --- | --- | --- |
| C11872 | expire_postflight_completion | Countdown wait / flags and fire-state expiry |
| C118E6 | end_postflight_message | Message wait / end-sequence publication |
| C118FC | follow_postflight_message | Message wait / cockpit-flag clearing and countdown |
| C11934 | clear_postflight_phase | Countdown wait / player and sequence phase clearing |
| C11958 | follow_postflight_message_or_phase | Finished message, sequence $FF, sequence 1 and unchanged phase |
| C119D4 | restart_postflight_after_countdown | Countdown wait / restart, with and without context request |
| C1104C | queue_postflight_end | Countdown wait / message 14 publication |
| C0F946 | await_postflight_viewport | Countdown wait, viewport mismatch and matched transition |
| C0F974 | mark_postflight_viewport_ready | Countdown wait / ready transition |

Callbacks with unexamined children, including the scene restart, table load and
result-message owners, retain their unresolved contracts. Active drawing still
starts unresolved until an existing drawing owner supplies its actual result.

## Runtime integration and original comparison

The existing disk/input-backed Free Flight comparison adds twelve controlled
cases per callback. Only the validation entry selects callbacks/countdowns,
message/phase flags and viewport conditions. C11958's sequence cases select a
negative player phase so C0F5F8's earlier positive-player selector does not
replace the callback being tested. No input result or return owner is seeded.

As in the setup-stage comparison, the complete input/stage parent is bracketed
before a separately selected idle body. A callback that enables record work
finishes first; the validation entry then selects POST_INPUT_AUX=0 at the body
boundary. This isolates its result's next input consumption without claiming
that its ordinary gameplay route is idle. The actual final message owner can
replace the result; stage/body outputs are checked separately.

Each original stage receives the preceding independently verified original
recorder result. Original game instructions execute the selected callback,
followed by the independently captured body and recorder parent. Native runs
finish before comparisons begin. The new stage values include 0, 7, 162, 190,
204, 211 and 232, produced by actual queue/message owners. No drawing/HUD
bytes are newly excluded and no clock/output values are fitted.

The expanded comparison passes 355 complete bodies, 372 recorder parents and
84 intervening keyboard parents against original compared RAM/drawing and
defined returns. Another 168 separately bracketed input/stage parents pass,
including the 108 additions. Existing component checks retain 2,512 input
cases, 12,576 command parents and sixteen Delete parents.

The three existing controlled postflight outcome probes also pass: nine
input/stage intervals and 25 sampled bodies match compared original RAM/display.
The ready outcome reaches complete C1643A/C0EF08 source decisions and one
actual DOS Write through the shared overlay contract; the native saved log
persists. These probes seed source terminal conditions after ordinary startup;
they are not proof of complete player-driven mission success.

```powershell
python tools/native/check_countermeasures.py --out build/native-flight/postflight-input-check
python tools/native/check_postflight_schedule.py --out build/native-flight/postflight-input-outcomes
```

Passing RAM remains temporary and reports stay in the bounded build cache.
Native Debug/Release builds and eight affected Release CTests pass:
countermeasures, frame_body, frontend, postflight_entry, game_input,
sequence_return, qualification and artifact_cleanup. Aggregate case counts,
stage values and report/binary hashes are retained in
[figures/native_postflight_input_return_checkpoint.json](figures/native_postflight_input_return_checkpoint.json).
Whole mission/combat sequences, remaining
stage/reset/HUD contracts, readable typed state, audio fidelity and broader
visible-window performance remain unfinished. The full-port goal stays active.
