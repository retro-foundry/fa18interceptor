# Native keyboard and pending command parents

`port/command_dispatch.c/.h` now implements both complete original parents,
keyboard `$C1AD74` and pending `$C1AC28`, using ordinary native command,
flight, view, context and queue owners. No CPU/machine state, instruction
handlers, bus accesses or guest addresses enter this native library.

The dispatcher selects one request, executes its actual action family and
publishes the resulting event. Empty/wait/modifier-only exits retain their
original absence of publication. Invalid pending words store error `$33`;
the release-build `$C06C02` fault hook is an actual RTS and changes no state.
Reset removes then reinstalls the input callback through the actual native
`$C1748C/$C17456` game-side bodies in `input_callback_registration.c`.
Installation writes type 2, priority 0, the imported original name and the
supplied actual native callback before invoking the required host service.
The registration/removal kind remains 5. No callback implementation or host
registration service is invented.

Keyboard selection now has a composition entry which retains the inherited
action word. The block test replaces its low byte with the masked block flags;
indexed keys replace the word with their selected index. Other routes retain
it. Pending selection retains the caller's word. This matters when an empty
countermeasure count restores the inherited word after its sound child.
The older request-only selector API remains available and passes its complete
32,768-call prefix regression proof.

`command_dispatch_controls.c` executes real direction, throttle-reset and
space-release children directly. It adds the complete `$C1C214` eject toggle:
the byte toggles between zero/nonzero, then publication runs inside that child.
The parent resumes, writes its remaining flags and publishes again. The
second publication respects the taken latch from the first. Required audio,
space-press and sound-sweep calls reach their explicit native owners; a missing or
failing owner returns failure, preserving preceding original writes and
leaving the outcome unassigned. This is not substitute child behavior.

Run `python tools/recomp/check_native_command_dispatch.py`. The oracle checks
the sealed original bytes, executes every parent instruction directly, and
executes real control/eject-publication, zoom/redraw, geometry/observer,
registration/removal and fault-RTS instructions. Audio, space-press, sound sweep
and host registration use explicit test contracts that alter real shared
state and event words. At each child boundary, all RAM and semantic inputs
are compared. Final checks compare every Chip/Slow RAM byte without exclusions,
published events, selection carries and ordered child calls. Original stack
writes exist only in validation; the native API has no source stack.

Validation passes 8,192 complete calls per parent (16,384 total), including
signed queue aliases, active/inactive direction gates and all owner exits.
The runs cover 985/1,104 parent boundaries and all 125/125 actual-child
boundaries. Parent fixtures do not cover every cold action branch; separate
validated action components retain their broader branch proofs. The checkpoint
records this limit in `analysis/figures/native_command_parent_checkpoint.json`.
The selector regression also passes all 74 pending and 243 keyboard boundaries.

GNU strict-warning and MSVC native contracts pass. All seven input CTests
pass; GNU symbol inspection finds no CPU/bus/machine references. Native MSVC
game builds and the unchanged guard passes 428 files. Contracts check complete
dispatch, inherited words, nested eject publication, view composition,
modifier/wait/fault/reset exits and explicit child/service failures.

The remaining message/status/voice/space/sweep game children now have real
native owners in `command_effects.c`; see [native command effects](native_command_effects.md)
for their separate original-instruction proof and composition requirements.
The parent regression above retains its controlled contracts to test owner
interfaces and shared-state effects. `$C25704` posts messages and `$C17F8C`
starts sound 6; the earlier "spawn" label described neither routine accurately.

The library is linked into the incomplete native runtime but is not called
by its game loop. Remaining dependencies include `$C1718E` input callback,
host registration/audio services, audio updates/playback, original data
loading and the complete native game loop. The playable reference remains
emulated; the full emulation-free objective is still unfinished.
