# Run005 focused flight-control and camera coverage

Run005 should cover controls missing or insufficiently separated in runs002--004.
It is a coverage experiment: the recording proves inputs and execution paths;
semantic labels still require bounded replay evidence.

The interactive run was superseded by a scripted, native Engine9000 replay:

```powershell
python scripts/build_scripted_flight_run.py
```

It restores the verified in-flight frame-600 state and emits native
`E9K_INPUT_V1` key transitions, avoiding the unreliable Windows frontend input
path. Results are in `captures/run013_scripted/` and
`analysis/run013_scripted.md`.

1. Press F1 through F10 once each, separately.
2. Press F10 once, wait, then press F10 again to cover the documented repeated
   afterburner route. Release each key promptly.
3. Tap Backspace twice with a gap.
4. Hold each cursor direction briefly and separately: Up, Down, Left, Right.
5. Press Shift+F once for the documented rescue-pod path, then leave a longer
   gap so its result can be replayed independently.
6. Continue straight flight for at least ten seconds, then land or crash only
   if convenient. This supplies an uninterrupted post-control update window.

Avoid Esc, Shift+Esc, F11, F12, Restore, Rewind, Reset, Warp, and configuration
changes. Do not repeat the well-separated gear, hook, HUD, map, radar, target,
ECM, chaff, flare, eject, Return, or Space experiments unless needed to keep
the aircraft controllable.

Numeric-keypad views are deferred. Enabling PUAE's Keyrah keypad mapping changes
the core configuration and makes the existing baseline savestate incompatible.
Do not substitute top-row digits: they would be a different raw-key experiment.
