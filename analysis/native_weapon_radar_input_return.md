# Native weapon/radar command outputs - 2026-10-07

Radar-range and weapon-mode commands now expose their existing domain values
to subsequent depleted recorder input when queue publication skips. The
weapon-mode subtraction also follows the original signed overflow condition:
the masked $80 input selects $30, rather than the previously wrapped $70.

The playable caller is native entry -> frontend -> native_input_process ->
native_menu_dispatch_raw/pending -> command dispatch ->
execute_flight_command_result -> existing tone/message owners. Flight behavior
remains in port/game/flight_commands.c; native output composition remains in
port/game/native/menu.c. Legacy event-only callers delegate to the same body.
This removes the unresolved output dependency for these action families and
corrects an actual signed branch in that connected owner.

## Original contract

- C1B1E2-C1B20C reads the selected record's radar nibble and publishes the
  actual selected value (9, 11 or 13).
- C1BBC0-C1BBCA reads/masks the command-block nibble and retains it when
  blocked. C1BBD6-C1BBEA otherwise reads/masks the weapon nibble and subtracts
  $10. BGE tests the signed subtraction, including overflow; it does not test
  the wrapped byte alone. Negative differences select $30. The actual chosen
  value is stored and published as the weapon-mode output.
- Weapon enable and qualification-message arms retain their preceding output.
  C33186 saves/restores D4 on its audio path and preserves it on context/mute
  skips. C25704 masks D0, saves/restores D1, and leaves D4 untouched.
- Next-target C1B1A4, throttle-mode C1B5DC, hook C1B616, qualification-target
  C1C052 and ECM C1C1F2 likewise preserve output through their memory updates,
  status tones and applicable message/toggle children.

FlightActionOutput carries radar range, weapon block and weapon mode separately
from the event later queued. Actual owner expressions provide these values.
Preserving actions retain the actual earlier selector/prior output. Accepted
publication supersedes action output with its signed translated index. No CPU
register shadow, captured output or fitted gameplay constant was added.

## Connected comparison

Twelve additional Free Flight cleanup bodies supply actual outputs to radar,
weapon and preserving toggle/target commands before first depleted recorder
input. The real full-queue gate exposes the action output. Radar cases exercise
all three nibble selections; weapon cases include the $80 overflow, zero,
ordinary decrement and blocked paths. The comparison retains all earlier cases
and independently composes original body -> original keyboard -> original
recorder parents. Native gameplay consumes only its own actual owner outputs.

151 complete bodies, 168 recorder parents and 48 intervening keyboard parents
match original compared RAM/drawing and defined returns. No HUD/drawing mask
or clock/output seeding was introduced. Passing raw captures remain temporary.

4,416 selected command parents match all compared non-stack RAM and defined
low-byte returns: 1,438 preserve prior output, 368 publish selection, 28 publish
queue indices, 1,296 publish flight-action values and 1,286 publish view-action
values. None is unresolved within this bounded suite. The additional 2,240
parents include 512 radar cases covering all byte values at two selected-record
offsets, 1,280 pending weapon cases covering all byte values across ordinary,
blocked, mode-two enable and both qualification gates, and 448 target/toggle
cases across mission, aircraft-kind, context and tone-mute conditions.

Native Release/Debug and both reference MSVC runners build. Existing 2,512
input parents and sixteen Delete parents also pass. These component results
are separate from the complete-body runtime integration above.
Nine affected native CTests pass: host keys, scene exit, frame body, frontend,
frame tail, input, game input, qualification and artifact cleanup.

The full-port goal stays active; other command families, earlier HUD outputs,
normal full mission/combat outcomes, independent scenario acceptance, readable
typed state, audio fidelity and measured 20 ms frame performance remain open.
