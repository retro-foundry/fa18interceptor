# Native view, origin and zoom commands

`port/view_command_input.c/.h` implements all 16 original $C1B77C-$C1BB76
view/origin/zoom actions before queue publication. `view_command_controls.c`
implements both actual children: $C08324 maximum zoom and $C082B8 cockpit
redraw. These modules use ordinary C fields, pointers and asset tables,
without CPU registers, guest addresses, machine services or ROM dependencies.
The full emulation-free game and game-loop composition remain incomplete.

Authority: sealed demo instruction bytes, the original ownership manifest
`analysis/data/command_dispatch_source_scope.json`, and existing reference
`port/game/view_commands.c`, `view.c` and `cockpit.c`. Reference code and its
CPU adapters remain unchanged.

## Shared state and original behavior

`FA18ViewCommandState.flight` shares native aircraft state; its command pointer
shares selection/indexed state directly. Origin mode, detail gate and origin
gate B have one owner each. Origin gate mode is the same original byte as the
aircraft pause gate, so origin range reads `flight.pause`. Cockpit redraw uses
the aircraft's existing weapon, info, scale and bar counters rather than copies.
Outgoing view `emitted_requests` is PENDING_COMMAND_WORD_B, distinct from the
ingested RECORD_WORD_B in `commands.pending_b`.

Actions preserve original detail requests, signed origin limits and arithmetic
wrap, view-mode increment/decrement rules, refresh requests, zoom signs and
flags. Maximum zoom preserves the incoming full event. Cockpit redraw updates
the original fourteen counters and clears retained render state only when the
original retain-state byte is zero. Kind-$30 viewed records skip span/row
updates; matching row modes use the original $32/$320 span pair and skip the
redraw child.

`FA18ViewSpanOffsets` stores the complete signed-byte indexing window of the
original span table. Element 128 is mode zero; element zero is mode -128.
Negative/out-of-range modes access the same surrounding original bytes as the
source. The asset owner must import that window from the original image;
there are no invented offsets or implicit replacements for missing data.

All 16 actions call the actual two native children directly. No child backend
or controlled production behavior is required for this component. Invalid
arguments return failure before mutation or event assignment. Queue
publication remains a separate owner.

## Validation

Run `python tools/recomp/check_native_view_command_input.py`. Default validation
executes 2,048 cases per action, **32,768** comparisons total. Original parent
and child instruction bytes are checked before execution. All Chip/Slow RAM
bytes, complete events and ordered child input states match, including the
original JSR stack writes. There are no memory exclusions, instruction
patches or substituted child results. Both native child implementations are
compared with their actual original instructions.

Coverage is **219/219 parent boundaries** and **24/24 child boundaries**.
Fixtures cover signed modes, origin gates and wrapped/clamped ranges, zoom
signs/limits, kind-$30 records, matching rows, and both redraw retention paths.
The source table is imported from the original image in the validation tool.
The component proof covers game values and ordering; incidental CPU outputs
and instruction timing remain reference-runtime concerns.

GNU `-Wall -Wextra -Werror` and MSVC Release contract tests pass. Composition
tests use actual pending/keyboard selectors, shared aircraft/indexed gates and
real children. GNU symbol inspection finds no `m68k`, `fa18_bus` or
`fa18_machine` symbols. All four current input CTests pass; the native MSVC
runner builds, the unchanged native guard passes 417 files and the original
1,104-boundary command audit passes. Checkpoint and source hashes:
`analysis/figures/native_view_command_input_checkpoint.json`.

## Remaining full-game work

Implement the five context actions and actual aircraft/status/audio/spawn
children, complete queue publication with signed-index alias ordering, load
the original records and tables, and compose the full native command owners
into startup, menus, flight, scene/render/audio scheduling, outcomes,
persistence and exit. The current native game links these components but
does not call them yet. The playable ROM-free runner still uses Musashi and
the chipset model.
