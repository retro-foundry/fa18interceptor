# Native keyboard and pending-command selection

`port/command_input.c/.h` ports the complete selection prefixes of $C1AD74
(keyboard) and $C1AC28 (pending commands) to ordinary C state. This is progress
toward the full emulation-free game; full parent action execution/publication
and native game-loop integration are not complete.

Authority: original instruction bytes in the sealed demo, audited by
`analysis/data/command_dispatch_source_scope.json`; existing reference policy
in `port/game/command_selection.c`. The neutral `CommandAction` enum and
`CommandRequest` layout moved unchanged into `port/command_types.h`. The
reference selector and its register/flag adapters remain unchanged.

## State and composition

`FA18CommandInput` owns `FA18IndexedControls` rather than a second snapshot of
its mode/gate fields. The complete source key routes and signed comparisons
remain in source order. Pending words preserve original byte/bit priority:
high-byte bit 0 first, then the low byte. Invalid low-byte bits $F0 produce
the fault action before changing the pending word. Recorder values 1 and 2
block the second pending word and preserve the modifier latches on that exit.

| Native state | Original storage |
|---|---|
| event_counter | COMMAND_EVENT_COUNTER |
| origin_mode | ORIGIN_ENABLE |
| modifier / indexed.function_modifier / other_modifier | KEY_STATE bytes 0 / 1 / 2 |
| return_state / block_flags / message_state | COMMAND_RETURN_STATE / COMMAND_BLOCK_FLAGS / MESSAGE_STATE_C |
| indexed.mode / mode_gate / enable_gate | MODE_SELECT / COMMAND_MODE_GATE / COMMAND_ENABLE_GATE |
| indexed.recorder_mode / origin_detail | RECORDER_MODE / ORIGIN_DETAIL_MODE |
| indexed.pose_inhibit | CONTEXT_GATE, the same byte as the indexed pose inhibitor |
| pending_a / pending_b | RECORD_WORD_A / RECORD_WORD_B |

`fa18_apply_selected_indexed_command` composes LOW_INDEX, FUNCTION_LEVEL and
INDEXED requests with the earlier native indexed implementation. Requests keep
the original full event, copied modifier and indexed selection. Other action
families are explicitly rejected by that narrow entry; they are not silently
ignored or replaced with another command. No action/child/queue behavior is
invented here.

`fa18_command_input` builds as a static C library containing `command_input.c`,
`indexed_controls.c` and now `flight_command_input.c`. All three native input
contract executables use this library;
`fa18_port` links it too. The library has no CPU, guest-memory, SDL, ROM or
machine dependency. Its component API is ready for the full native command
dispatcher, but the existing bounded game loop does not call it yet.

## Validation

Run `python tools/recomp/check_native_command_input.py`. Its default 16,384
cases per entry execute the original CPU instruction prefixes, stopping at
the selected action. Verify the original bytes against the audited manifest
before execution. Compare the native action with the actual source target,
event, modifier/origin/detail metadata, indexed selection and every Chip/Slow
RAM byte. The expected RAM includes the keyboard prefix's temporary LINK
frame write. There are no RAM exclusions or original-code patches.

All **32,768** comparisons pass, with **74/74 pending** and **243/243 keyboard**
boundaries covered. Fixtures include every raw/release key byte, counter signs,
recorder states, context gates, pending-word bits and CCR combinations. This
proof checks state/action semantics, not incidental CPU outputs or timing.
The earlier complete reference adapter proof remains authoritative for those.

GNU `-Wall -Wextra -Werror` and MSVC Release contract tests also pass. Composition
contracts check modifier retention, function-key throttle, mission-mode
selection and status-child results, pending priority/validation, and the
counter-zero release-bit clear. GNU symbol inspection finds no `m68k`,
`fa18_bus` or `fa18_machine` references in the standalone executable. Native
MSVC game build and the unchanged native guard pass (411 files). The original
1,104-boundary command ownership audit still passes. Checkpoint source hashes
are recorded in `analysis/figures/native_command_input_checkpoint.json`.

## Remaining full-game work

The flight action component is now implemented and validated; see
`native_flight_command_input.md` for its child scope and remaining dependencies.
Port/combine the view and context action families and remaining actual native
child owners. Port queue publication with the original signed-index
alias/write ordering. Import the real mode/pose records and audio data from
the original assets, compose status tones and other children, then replace
the bounded key-1 path in the native game loop. The complete native loop also
needs startup, update/scene/render scheduling, flight outcomes, persistence,
audio and exit. The playable ROM-free runner still uses Musashi and the machine
model. Validation's original-state/ROM dependency does not enter the new native
library.
