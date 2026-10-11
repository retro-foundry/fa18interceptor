# Native lost-target cleanup input return - 2026-10-07

Lost-target cleanup now composes its actual view-command output into first
depleted recorder flare/chaff input. Previously a selected cleanup invalidated
the result unconditionally. Empty targets, live targets and context-only drops
preserve the preceding result, as the original does.

The playable caller is native entry -> frontend -> native_flight_tick ->
native_frame_selection_cleanup -> drop_lost_selection_result ->
queue_view_key_result -> publish_command_event_result. Existing legacy APIs
delegate to the same owners, preserving reference/glue callers and observer
ordering. Native composition remains in port/game/native/frame_tail.c.

## Source contract

The relocated disk executable independently disassembles as follows:

- C12242-C12284 performs target/live/context gates without assigning D4.
- C12286-C12294 clears the view and span origins before the view-key call.
- C1BA86 calls the existing redraw owner; C1BA8C then loads the view mode.
- C1C23C-C1C25A gates claimed/release/full input queues. These skips retain
  the selected view mode.
- Accepted publication finally loads the signed translated-write index at
  C1C298, extends it at C1C29E and stores the translated event at that index.
  It does not advance that index. C1C2A4-C1C2B6 clears input modifiers and returns.
- C31F4A, immediately after lost-target cleanup in the frame, is RTS.

CommandPublicationResult now exposes the event, whether a translated store ran,
and the actual signed index loaded before that store. ViewKeyResult exposes
the mode loaded before publication and that publication result. This avoids
reconstructing queue gates or reading an index after an aliasing write. The
native frame keeps only the defined low-byte input contract; gameplay gains no
CPU register file, captured return or emulation dependency. Existing queue,
target, redraw and input-modifier writes retain their order and values.

## Connected comparison

The countermeasure fixture retains 91 prior full bodies and 108 input parents,
then selects twelve lost-target frames after ordinary disk/key Free Flight.
Validation supplies the target and queue gates, including successful, already
claimed and full queues with translated indices 0, 7, 9 and -1. It suppresses
later optional grid/label/debug drawing so those owners do not supersede this
cleanup result. It does not seed game counters, reference outputs or native
completed results. Normal progression avoids periodic clear/redraw boundaries.

All 103 complete bodies and 120 recorder input parents match original compared
RAM and every drawing-page byte. Original execution independently derives and
checks the body's output; its own input oracle alone receives that original
output. Native input uses its actual cleanup result. Existing 2,512 input
parents, 256 control-effect cases, two $FD parents and four collision parents
remain passing. Raw passing RAM remains temporary; no comparison masks change.

The focused frame-tail oracle passes 512 selection-cleanup input returns and
all non-stack RAM. It includes all sixteen target selectors, live/dead/context
gates, claimed/full queues, signed counts, raw-index reset/negative alias cases
and signed translated indices. Incoming skipped results are independently set
to $51AB12E7 in the source and $E7 in the native low-byte contract. The existing
256 gated debug-overlay return/RAM cases still pass. These bounded component
checks are separate from complete-body integration. Native Release/Debug and
both reference MSVC runners build.

Nine affected native CTests pass: host keys, scene exit, frame body, frontend,
frame tail, input, game input, qualification and artifact cleanup.

## Remaining scope

This closes the lost-target cleanup contract. It does not close intervening
command outputs, earlier unresolved HUD outputs, complete gameplay sequence
acceptance, typed-state migration, audio fidelity or measured frame performance.
