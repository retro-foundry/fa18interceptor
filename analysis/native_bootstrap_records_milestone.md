# Native bootstrap record composition

The native runner now calls the complete `bootstrap_scene` (C08F26), including
its C1C63E record-update and C1C860 context-refresh children. The runtime caller
is `port/native/main.c` -> `native_frontend_open` -> `native_flight_initialize`
-> `bootstrap_scene` -> `native_records_update`. These calls replace the CPU
child dispatcher for this startup scope.

The native composition uses the existing C22C80 slot loop and C25B66 dynamics
continuation, record matrix, collision-candidate selection, ground projection,
region probe, motion-slot publication and position history owners. C230E8 now
returns its existing action decision as a C result; C26322 returns its working
motion values. The CPU adapters retain their original observer effects.
Pure control-state functions moved unchanged from `player_input.c` to
`control_input.c`, leaving actual device reads in the former. Direct-call
ownership metadata was updated and the generated manifest regenerated.

Validation:

- Native boot executes the full bootstrap without CPU/chipset objects; the
  frontend omission/pixel/save/SDL checks pass.
- `python tools/native/check_records.py` exports the native boot checkpoint,
  then runs another C1C63E update from that same state through original opcodes
  and through the connected native composition. All RAM outside the oracle's
  top 4096-byte guest-stack arena matches. This is a bounded component check,
  not whole-bootstrap, frame-timing or active-flight parity.
- The focused Free Flight start check still passes pose/camera/gate checkpoints.
- Both reference targets and the native target build with MSVC; GNU builds the
  validation oracle. All 14 CTests excluding the expensive menu check pass.
- Motion helper and record-action proofs pass 64 cases per owner against both
  controlled and original children, including full registers, PC, SR and RAM.
  Action segments use 64 cases and fault observation uses 16 cases.

Runtime limitation: native record updates currently execute during bootstrap,
not the ongoing C0EFD4 update loop. A later original Free Flight checkpoint
requests an active slot-14 dispatch child that remains unconnected. Dynamics
controls, periodic region updates and origin matrix routes are also explicit
unconnected boundaries; no substitute outputs are supplied for them. The
runner still ends at `scene-setup` / C1072E. Later player state differs and there
is no active native flight or moving world rendering yet.

Estimated Free Flight startup wiring: about 55%, up from about 50%, reflecting
completion of the bootstrap record/refresh call chain only. Ongoing selection,
rendering and simulation are the larger remaining work. This estimate does not
measure whole-game completion. Copper fade is excluded; no sealed full replay
was rerun for this batch.

## Follow-up: repeated setup updates and active dispatch

The runner now repeats the record/context slice after its source post-input
stage tick when POST_INPUT_AUX permits it. The 4000-frame focused Free Flight
run performs 12 record updates, exposed by `record_updates` diagnostics. The
source input/view children and drawing sequence between these stages remain
unconnected; this is not a complete C0EFD4 implementation. UPDATE_TICK scheduling
and periodic region work remain open. C1072E remains the endpoint.

The C23A7E dispatch owner now returns its existing working values in C, allowing
the native loop to use the original keep/clear decision. Companion-record state
is retained from the source slot-loop events. The C2D970 inverse matrix child
is connected for the source's cell-only route. Reference adapters keep the same
observations and ignore the added C return value.

`check_records.py` now checks both native boot and native Free Flight setup
exports. With `--reference build/native-flight/source.ram`, it also checks an
existing original frame-3035 export, exercising active slot 14 and its cell
matrix. All three inputs match original C1C63E RAM outside the guest stack.
This validates the record composition for those states; it does not establish
that native startup has reached the same full game state as the original.
The focused flight-start test confirms ongoing updates execute in the runner.
C23A7E controlled/original-child proofs pass 64 cases each with full CPU/SR/RAM.
Both reference targets and native build; the 14 CTests excluding menu pass.

Correction, 2026-10-06: the banner is **CRACKED BY A-HA**, original disk
message $47, not a crash message. At this batch's checkpoint the
native root still has its bootstrap kind while the later original checkpoint
has aircraft kind $11. The remaining source command/context stages and buffer
presentation need tracing; this batch does not substitute a new aircraft state
or replace the banner. No full replay was run. Estimated flight-start
wiring is now about 60%, based on the added repeated record route and dispatch
children, with input, world rendering and flight simulation still open.
