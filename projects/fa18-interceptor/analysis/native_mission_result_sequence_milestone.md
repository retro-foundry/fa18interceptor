# Mission-list result messages and menu restart — 2026-10-08

Modes three and five now have connected normal-key acceptance through objective,
landing, saved success, result messages, Escape, scene bootstrap, menu return and
cold pilot-log reload. This extends the accepted success flights without changing
gameplay or the playable executable. Completion count stays 4 and mode grade
stays 2 after each restart; all 78 pilot-log bytes survive unchanged.

## Original behavior and runtime route

The active path is `fa18_native` -> `native_frontend_tick` ->
`native_flight_tick` -> `run_post_input_tick` and its selected game stage.
`prepare_postflight_result`/C110A4 queues successful-result messages and invokes
C11350/C1643A for the actual result and config save. The message continuation
uses `advance_main_loop_message_sequence`/C32CEE. C10DAE's
`update_menu_context` returns successful phase FC to phase four when both
message-state bytes permit it.

The aircraft remains parked. A no-input continuation of mode five through tick
31,265 matched all 36 input/stage intervals and 162 sampled bodies against
original instructions, with exactly one config write, but did not restart.
The original does not justify an automatic gameplay change here.

The normal Escape key selects `COMMAND_SIGN_INPUT`/C1C224, which writes sequence
phase one. C0F5F8 then selects C0F946, followed by C0F974 and C0F992. This route
does not use the qualification-specific C11958/C119D4 continuation. C0F992's
`begin_sequence_after_bootstrap` rebuilds scene storage, resets player phase to
zero, clears recorder mode and publishes C0FCB4; the native flight caller selects
the existing menu screen. The result log is preserved throughout.

| Evidence | Mode three | Mode five |
| --- | ---: | ---: |
| Saved result tick | 25,984 | 26,265 |
| C10DAE phase FC -> 4 tick | 26,803 | 27,084 |
| Finished-message observation / Escape request tick | 26,966 | 27,233 |
| C0F992 / menu return tick | 27,005 | 27,272 |
| Compared input/stage intervals | 44 | 42 |
| Compared sampled bodies | 162 | 169 |
| Consecutive landing bodies | 64 | 64 |
| Original config writes | 1 | 1 |

## Repeatable acceptance and retention

The validation-only mission fixture adds `3-sequence` and `5-sequence`.
After the normal successful flight it releases held pilot keys, waits for the
actual message state, presses/releases Escape and stops at the actual menu
callback. It never writes flight, message, phase or result RAM. It rejects an
unfinished restart, queued key, crash reset, incorrect terminal stage/screen,
nonzero player phase or recorder mode, or any changed pilot-log byte. A cold
reopen must consume the original saved 78 bytes exactly.

The shared checker compares the actual input/stage interval that changes FC to
four, both Escape events, viewport/bootstrap callbacks and sampled message-body
state transitions. Every retained entry and body runs original instructions
externally with the existing comparison masks. Each result must reach exactly
one original DOS Write, and its completion/grade must remain unchanged after
bootstrap. The serial CTest gates are `fa18_native_mission_3_sequence` and
`fa18_native_mission_5_sequence`.

Each sequence uses 64 consecutive landing bodies to reserve room for later
callbacks within the existing 240-pair / 480 MiB capture cap. Mode three uses
206 pairs / 412 MiB; mode five uses 211 pairs / 422 MiB. The earlier success
gates retain their existing landing windows. Passing RAM is temporary and
deleted; build/CTest artifact pruning stays enabled with its 4 GiB budget.

```powershell
cmake --build build/native-cmake --config Release --target fa18_native_mission_success_test
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mission_(3|5)_sequence$' --output-on-failure
```

Reports are in `build/native-cmake/native/mission-{3,5}-sequence-check/`.
Retained checkpoints and consumed keys are
`analysis/figures/native_mission_{three,five}_sequence_checkpoint.json` and
`tools/native/fixtures/mission-{three,five}-sequence.e9k`.
Checkpoints distinguish the captured Windows CRLF input hash from the retained
Git LF fixture hash; their replay events are identical.

Debug and Release fixture builds pass. All five selected CTests pass in each
configuration: both existing mission success gates, both new sequence gates
and artifact cleanup. Debug/Release consumed input, sequence evidence and
saved-config hashes agree. Logs are
`build/native-flight/mission-sequence-debug-ctest.log` and
`build/native-flight/mission-sequence-final-release-ctest.log`. Each sequence
compares four message-body state transitions as well as its actual FC -> four
input/stage interval. The final build cache is 1.99 GiB; no pruning is needed.

The playable executable remains SHA256
`73f9bc00fd5db4f1f3a3e90097b902e0a4eb2b4d8ad07a24789626a3549189cd`.

## Scope and next work

These comparisons execute original instructions from native before-states.
They establish the sampled connected result/message/restart paths, including
the actual menu callback and saved log. They do not establish independent full
original/native flight parity, every message-drawing frame, or a second flight
after this particular successful-result menu return.

Successful modes four, six, seven and eight, independent complete flights,
remaining runtime contracts, typed state, audio fidelity and broader performance
remain open. The complete-port goal stays active.
