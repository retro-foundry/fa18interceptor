# Native context command actions

`port/context_command_input.c/.h` implements all five original context actions
selected by $C1AC28/$C1AD74: special context, refresh, calculation, map and
context request. `context_command_controls.c` implements the actual $C091E0
local-to-world and $C0915A observer-position children. The two $C0F4A6
voice-release calls remain explicit audio-owner dependencies. This is progress
toward the full emulation-free game, not complete game-loop integration.

Authority: the sealed original demo, instruction bytes and ownership in
`analysis/data/command_dispatch_source_scope.json`, and existing reference
`port/game/context_commands.c`, `matrix.c` and `view.c`. The child enum moved
unchanged into CPU-free `port/context_command_types.h`; reference behavior and
its CPU adapter remain unchanged.

## Ordinary state and original aliases

Context state points to the existing native view state, which shares aircraft
and command/indexed state. There are no CPU registers, guest addresses,
machine services or ROM dependencies in the native implementations.

| Context value | Existing shared native owner |
|---|---|
| ORIGIN_ENABLE / detail / gates A and B | command/indexed state |
| ORIGIN_GATE_MODE | aircraft pause byte |
| ORIGIN_THRESHOLD_FLAG | aircraft context_started byte |
| ORIGIN_STATUS_WORD | aircraft command_word |
| SELECTOR_ORIGIN_MIDDLE | view origin_middle |
| outgoing PENDING_COMMAND_WORD_B | view emitted_requests |
| KEY_TAKEN | pointer to the queue-owned byte |

Context-owned records retain the same ordinary aircraft identity pointer plus
their angle, world position and inverse matrix. Copying the viewed angle
resolves the latest viewed identity, including after a child retargets it.
Pose descriptors explicitly identify preset or record poses. The asset owner
must resolve the original wrapped signed-word record displacement, import
the original preset words and resolve their signed, wrapped grid-pair index.
The native action selects descriptors with the original signed scene-pose
byte. Unresolved entries, absent records and missing tables fail explicitly;
they do not turn into synthetic positions or default poses.

The local-to-world child uses three signed-word dot products, wraps their
long sums before arithmetic shift by four, and adds the original world
positions with long wrap. Observer publication stores the actual position and
negates the low-$3FFFFF X/Z parts and the complete Y value. Context presets
preserve sign extension, SWAP, shifts and wrapped sums.

Map/refresh requests preserve source write order, cached middle position,
record angle/status requests and shared gates. Voice-release results preserve
the incoming low event word. Context request honors the detail sign, toggles
the existing nonzero gate to zero, and writes exactly four $FF recorder bytes
only when mode, destination and recorder-on conditions permit. The recorder
owner supplies an ordinary bounded destination; NULL represents a source
cursor that is not positive. Child effects are read after the call returns.

## Validation

Run `python tools/recomp/check_native_context_command_input.py`. Default
validation checks **20,480** actions: 4,096 for each of the five entries. The
original state seal must match the audited source; parent and actual child
instruction bytes are checked before execution. Every Chip/Slow RAM byte,
full output event and ordered child input/state matches. Expected RAM includes
parent JSRs, saved event words and the geometry child's saved A1/A2 values.
There are no RAM exclusions or original instruction patches.

All **134/134 parent** and **46/46 actual geometry/observer** boundaries are
covered. The real geometry/observer instructions execute in the oracle. Only
voice release uses controlled test contracts; that audio child is not claimed
as implemented. Fixtures include signed pose indices -128/-1/127, wrapped
record displacements, both equipment types, random inverse matrices and
world positions, signed presets and grid indices, zero/positive/negative
recorder cursors, terminator writes, and state/view changes after audio calls.
This proves game data semantics and ordering, not incidental CPU state or
instruction timing.

GNU `-Wall -Wextra -Werror` and MSVC Release contracts pass. Composition tests
use the native pending selector and actual geometry/observer children, shared
view/aircraft fields, signed preset calculations, explicit audio test results,
recorder termination, gate toggles and missing-owner/data failures. The GNU
contract contains no `m68k`, `fa18_bus` or `fa18_machine` symbols. All five
current native input CTests pass. Native MSVC runner and reference MSVC
ROM-free builds pass, native guard passes 421 files, and the original
1,104-boundary command audit passes. Checkpoint:
`analysis/figures/native_context_command_input_checkpoint.json`.

## Remaining full-game work

Selection and all four action families now have native components. Implement
queue publication and complete parent dispatch/reset/fault handling, the
actual remaining aircraft/audio/spawn children, and original data loading.
Compose those owners into startup, menus, flight, update/scene/render/audio
scheduling, outcomes, persistence and exit. The current native game links the
components but does not call them yet. The playable ROM-free runner still
uses Musashi and the chipset model.
