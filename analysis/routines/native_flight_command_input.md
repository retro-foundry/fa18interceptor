# Native aircraft command actions

`port/flight_command_input.c/.h` implements all 28 aircraft actions selected by
the original $C1AC28/$C1AD74 owners, before their shared queue publication.
It uses ordinary fields, pointers to aircraft records, and explicit semantic
child arguments/results. It has no CPU, guest address, machine or ROM dependency.
The complete emulation-free game and runtime integration remain unfinished.

Authority: original instruction bytes from the sealed demo, the audited
`analysis/data/command_dispatch_source_scope.json`, and the existing reference
`port/game/flight_commands.c`. The `FlightCommandChild` enum and
`FlightCommandResult` layout moved unchanged into `port/flight_command_types.h`.
The reference actions and their register/flag adaptation retain their behavior.

## State and composition

`FA18FlightCommandState.commands` points to the same `FA18CommandInput` used by
keyboard/pending selection and indexed actions. Modifier latches, mode,
function level and throttle are shared directly. Player, view, target and three
spawn-slot records are ordinary pointers supplied by the native owner. The
radar action uses the latest viewed pointer after its child returns.

Outgoing `emitted_requests` models PENDING_COMMAND_WORD_A ($C4599A). It is
distinct from the ingested `commands.pending_a`, RECORD_WORD_A ($C45996).
Source byte masks are retained in the corresponding high or low word byte.

The actions preserve source ordering for controls, gear/hook, weapons, radar,
HUD/info pages, targeting, countermeasures and ECM. Countermeasure subtraction
uses the original signed overflow condition: old count $80 takes the empty
path even though decrement wraps to $7F. Hook/weapon sound paths preserve word
swaps and child results; countermeasures preserve the carried event word.
Flare spawning passes the original ordered arguments $1C and $30 and restores
the original event's low word after the child.

`fa18_apply_flight_control_child` also implements the actual direction leaves
($C1B50C/$C1B510/$C1B514 and $C1B558/$C1B55C/$C1B560), shared throttle reset
($C1B602), and space release ($C08394): nine child identities at eight source
entries. Axis latches always change; packed aircraft stick bits change only
under the original pause/context gates. Right/left fields are $08/$04.

Other children require explicit native owners through `FA18FlightCommandOps`.
An absent/failed owner returns failure; preceding source writes can have
occurred, and the published-event output is left untouched. There is no
production replacement behavior. In particular, eject's $C1C214 child includes
queue publication as well as a toggle; it is not claimed as a complete native
child here.

The `fa18_command_input` library now contains selection, indexed controls and
aircraft actions. `fa18_port` links it, but its incomplete game loop still does
not call these components.

## Validation

Run `python tools/recomp/check_native_flight_command_input.py`. Its default
1,024 cases for each of 28 actions compare **28,672** executions. Original
parent instructions and the eight actual control entries execute in Musashi
only inside the validation tool. Remaining children use explicit test
contracts to check call ordering, inputs, changed view/state, and event results;
this does not prove those remaining child implementations.

Every Chip/Slow RAM byte, output event and ordered child input/state matches.
Expected RAM includes exact parent JSR/argument stack writes. There are no RAM
exclusions or original instruction patches. Coverage is **222/222 parent** and
**37/37 actual control-child** instruction boundaries. Original byte checks
include both sets. Incidental registers and instruction timing are outside
this ordinary-state component proof.

GNU `-Wall -Wextra -Werror` and MSVC Release contracts pass. The new contract
composes both native selectors with real axis/throttle children and explicit
test-only status/sound results, verifies retargeted radar, countermeasure
overflow and missing-child failure. The GNU contract has no `m68k`, `fa18_bus`
or `fa18_machine` symbols. All three current native input CTests pass. Native
MSVC runner and reference MSVC ROM-free builds pass; the unchanged native guard
passes 414 files and the 1,104-boundary command ownership audit passes.
Checkpoint: `analysis/figures/native_flight_command_input_checkpoint.json`.

## Remaining work

Implement the actual status/audio, space-press, spawn and eject-publication
children, context actions, and complete queue publication. The view action
family and its actual zoom/redraw children are now complete as a component;
see `native_view_command_input.md`. Load
real native records/assets and compose them into startup, menus, flight,
update/scene/render/audio scheduling, outcomes, persistence and exit. The
playable ROM-free runner still uses Musashi and the chipset model. This module
alone does not change that runtime dependency.
