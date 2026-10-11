# Native control-action input return - 2026-10-07

The existing flight-command owner now exposes preservation, HUD-mode and
landing-gear-gate outputs to native command composition. These outputs can
reach a later depleted recorder flare/chaff input when publication skips.
Successful publication still supersedes them with its actual translated index.
Previously selected control actions invalidated the result even when the
original left a defined value.

The playable caller is native entry -> frontend -> native_input_process ->
native_menu_dispatch_raw/pending -> command dispatch ->
execute_flight_command_result -> existing player control/status-tone owners.
Game behavior remains in port/game/flight_commands.c and its existing children;
native output composition stays in port/game/native/menu.c. The legacy event
API delegates to the same action body, preserving reference/glue callers.

## Original contract

- C1B4F4-C1B58C stick actions and their children update their control fields
  using D1/D2, preserving the preceding input output.
- C1B58E-C1B5D8 rudder/trim and throttle actions also preserve that output;
  C1B602 clears throttle state without replacing it.
- C1B236-C1B260 information-page changes use the event value and preserve
  the preceding output. C1C224-C1C234 sign-input changes do likewise.
- C1B264-C1B274 HUD toggling publishes the actual incremented/clamped byte,
  including the signed-byte wrap behavior already implemented by that owner.
- C1BC2C-C1BC32 loads and masks the gear gate to $40. The bit-seven bypass
  does not perform that load and preserves the prior output. C33186/C3318E
  status-tone gates preserve it; their accepted audio path saves and restores
  D0-D7/A0-A4 at C331A2/C331C8.

FlightActionOutput names these domain contracts separately from the event being
queued. It does not add a CPU register shadow or infer a value from captured
RAM. Actual HUD/gate expressions supply assigned outputs; unchanged control
children retain their explicit preservation contract. Native composition uses
the real selector block-byte result when selected, otherwise the prior result.
Other flight actions keep unresolved contracts until their owners expose them.

## Connected comparison

The suite retains all previous 115 full bodies, 132 recorder parents and twelve
intervening keyboard parents. Twelve additional ordinary Free Flight cleanup
bodies produce independent domain outputs before captured control inputs and
first depleted recorder input. The new controls cover both stick axes, releases,
rudder, throttle, a signed HUD toggle boundary and an assigned gear gate.
Validation selects full input queues so the real publication skip exposes the
action output; source/native owners still perform the actual control writes.
No native completed result or game clock is seeded.

All 127 complete bodies, 144 recorder parents and 24 intervening keyboard
parents match original compared RAM/drawing and defined returns. Independent
original body execution supplies the original keyboard oracle's incoming
output. That keyboard parent independently derives/checks its output before
supplying it only to the original recorder-input oracle. Native input consumes
its own actual result. Passing raw captures remain temporary, and no comparison
mask changes.

832 focused command parents match original non-stack RAM. 826 defined low-byte
outputs match: 382 preserve prior output, 144 use actual selection, 28 use queue
indices and 272 use flight-action values. Six other action-owned returns stay
unresolved. The added 320 parents cover all 256 HUD bytes, 32 gear bypass/gate,
context and mute combinations, and 32 rudder/throttle/info/sign combinations.
Independent incoming source/native output contracts remain $51AB12E7/$E7.
These bounded components remain separate from complete-body integration.

Nine affected native CTests pass: host keys, scene exit, frame body, frontend,
frame tail, input, game input, qualification and artifact cleanup. Native
Release/Debug and both reference MSVC runners build. Existing 2,512 input
parents, sixteen Delete parents and the control/collision comparisons pass.

The full-port goal remains active. Remaining view/other action returns, earlier
HUD outputs, complete scenario acceptance, typed-state migration, audio fidelity
and measured 20 ms frame performance remain open.
