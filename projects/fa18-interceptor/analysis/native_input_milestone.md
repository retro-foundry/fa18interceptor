# Native source input owner

The playable caller is `fa18_native` -> `native_frontend_tick` ->
`native_flight_tick` -> `native_input_process` -> C0F3C4
`process_pending_key_events`. It runs before C0F5F8 and record/view updates.
SDL/replay key events now enter a host queue and are consumed at that source
boundary, including releases. Timer/display waits do not repeat input work.

The existing C16EAE/C16BF2/C16C56 event owners and C1AD74/C1AC28 command
dispatchers replace the previous immediate manual flight-key composition.
Joystick bit decoding is shared with C16F1C through `latch_joystick_input`;
the hardware wrapper remains reference-only. Command argument snapshots retain
the event transformations used by status tones and hook/weapon messages.
The original C1B55C right direction is $04; C1B558 left is $08, despite their
historical child enum names. Indexed mode changes return the source value 2.

Validation:

- `tools/native/check_input.py`: four actual native checkpoints demonstrate
  queued press during a timer wait, unchanged game/root/HUD state during that
  wait, source-boundary consumption, correct right direction and release.
- `native_input_oracle.c`: 144 cases execute original C0F3C4 and command bodies
  against the native composition, comparing every non-stack RAM byte. Only
  keyboard-descriptor and physical button contracts are supplied by the host.
  Cases consume 142 keyboard events and include 48 recorder-drain cases,
  joystick decoding, filtered keys, view controls, gear/hook, weapon mode,
  throttle/stick, Space and indexed selections.
- Six affected native checks pass: startup, clock, actions, model strips,
  map limits and postflight/display reset. Frontend/link omission and menu
  regressions pass; all twelve reference CTests pass.

This batch is complete (100% of the keyboard/source-owner integration scope).
This does not measure whole-game completion or establish recorded-run parity.
Physical mouse/controller acquisition, modifier-event timing, inherited
countermeasure/recorder selection, map commands and other game modes remain
open. Unsupported reached children fail explicitly. No sample output or
substitute game behavior was added. Full sealed replays were not repeated;
Copper fade remains excluded.
