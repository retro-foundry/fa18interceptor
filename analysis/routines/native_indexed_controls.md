# Ordinary-state indexed controls

This is a complete action component, not a completed game or input pipeline.
The active goal remains the full C port without CPU/chipset emulation.

`port/indexed_controls.c/.h` implements the indexed-action tail of the original
$C1AC28/$C1AD74 owners: $C1BC50-$C1BEE4 and the shared $C1C214-$C1C222 toggle.
Authority is the sealed demo's original instruction bytes, audited in
`analysis/data/command_dispatch_source_scope.json`. The oracle checks those
bytes before executing them. `port/game/indexed_commands.c` remains the
machine-backed implementation used by the reference runner.

The native component uses ordinary typed state. It has no CPU register file,
68000 addresses, opcode handlers, cycle counters, bus access or ROM input.
The separate native contract executable links only two C files. The main
`fa18_port` build links the component library; its existing bounded menu/demo
loop does not call it yet. Merely linking this component does not make that
runtime a complete game.

## State mapping

| Ordinary field | Original storage |
|---|---|
| mode / mode_request / mode_gate | MODE_SELECT / $C45792 / COMMAND_MODE_GATE |
| enable_gate / enable_selection | COMMAND_ENABLE_GATE / $C45849 |
| function_modifier / recorder_mode | KEY_STATE+1 / RECORDER_MODE |
| cockpit_high_byte / pose_entry / pose_inhibit | first byte of COCKPIT_FLAGS / SCENE_POSE_ENTRY / $C458AD |
| origin_detail / origin_gate_a / origin_gate_b | ORIGIN_DETAIL_MODE / ORIGIN_GATE_A / ORIGIN_GATE_B |
| function_level / control_record_level / player_ready | FUNCTION_KEY_LEVEL / CONTROL_RECORDS+$2B / PLAYER_READY |
| playback_bytes | PLAYBACK_BYTES |
| throttle / throttle_companion | CONTROL_ACCUMULATOR_Y / CONTROL_ACCUMULATOR_COMPANION |
| modes.status / level / available | original MODE_TABLE record word +0 / byte +6 / bytes +$12..+$19 |
| poses.values | signed first word of each original 16-byte SCENE_POSE_TABLE record |

Preserved behavior includes the enable-selection values $10/$11, mode gates,
special modes $7F/$7D/$FF/$FE, source availability checks for modes 4..8,
wrapped progression bytes, signed pose indices, the distinct -1 pose
terminator, and the F1 special $FF throttle case. F10 produces $79 and clamps
the scaled word to $3C0; later values retain signed byte wrap and can produce
negative throttle words. The recorder $FD branch and cockpit gating only
alter dead carried CPU intermediates before publication, so they have no
ordinary-state side effect. No progression unlocks or mode defaults are added.

The existing reference callback name `mode_changed` obscures its actual
meaning: $C3318E is `play_status_tone`, tone variant 2 with pitch 2 while volume
fades, otherwise pitch 4. The native callback is named `status_tone` and is
called after storing the mode, or directly when the mode record status is
zero. Its result is the event to publish, preserving child-clobbered D0.
This component does not implement the sound program/voice backend. Controlled
child effects in the validation harness are test inputs, not game behavior.

## Validation

Run `python tools/recomp/check_native_indexed_controls.py`. The default 65,536
cases execute original instructions with independently seeded registers,
flags, state, mode records and pose records. Compare full Chip/Slow RAM,
published events, child-call count and state at each child boundary. Include
the original JSR stack write in expected RAM. The proof covers all **184/184**
instruction boundaries of this component, including shared toggle paths.
The scope excludes $C1C224 onward, which belongs to other actions.

Result: 9,052 status-child calls, 9,810 changed pose bytes, 21,195 throttle
changes; all 65,536 comparisons pass. The reproducible checkpoint records
source hashes in `analysis/figures/native_indexed_controls_checkpoint.json`.

Separate GNU and MSVC contract tests exercise availability, callback ordering,
pose termination, malformed asset bounds and throttle signedness without
linking the machine. The GNU build passes `-Wall -Wextra -Werror`; a symbol
check finds no `m68k`, `fa18_bus` or `fa18_machine` references. The MSVC native
game target builds, and the unchanged native guard passes 408 source files.
The playable reference sources were not changed, so this does not require
rerunning their prior full recording/DMA gates.

## Remaining composition

Complete keyboard/pending selection now lives in `command_input.c` and composes
with this component using shared ordinary state. See `native_command_input.md`.
Load the actual mode record and pose table from original data into these
ordinary fields; compose the other action families and queue publication;
port the status-tone/audio child; then replace the bounded native input flow
with that complete pipeline. Follow with typed update/scene/render/audio
composition and startup/postflight/save/exit integration. The current
`fa18_romfree` runner still uses the CPU and machine model. The oracle's
Kickstart/savestate inputs are validation dependencies only.
