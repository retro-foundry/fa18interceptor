# Region-flight validation diagnostics — 2026-10-08

The existing region-flight regression still lacks its required spawn, zone-exit
and NPC-missile-slot-thirteen coverage. These diagnostics narrow the investigation;
they do not demonstrate a successful mission or resolve that regression.

## Fixture and playable backend

`fa18_native_mode_two_test` uses the playable `native_frontend_tick` path.
It now also calls `native_audio_render` after each tick, including the outcome
restart loop, with 960 stereo frames at 48 kHz, matching `port/native/main.c`.
This services the real audio owner without changing game behavior or source
comparison masks. The established region trajectory and its failing coverage
are unchanged by PCM servicing.

Two optional environment variables support comparisons at an identical tick:

- `FA18_MODE_END_TICK`: positive unsigned terminal tick; malformed, zero,
  negative and overflowing values fail before startup. Defaults are unchanged.
- `FA18_MODE_FINAL_DATA`: output prefix for one final `PREFIX.before.dat` RAM
  snapshot, written before evaluating the existing acceptance guards.

Clear both variables before acceptance runs. A shortened diagnostic can write
its snapshot and still exit 1 because the normal flight assertions remain
unsatisfied. Use temporary RAM storage and prune after manual captures.

## Aligned evidence and rejected pilot experiment

With the established region inputs and fresh pilot directories, the canonical
playable runner and Release mode fixture have zero differences across all
1 MiB of RAM at ticks 16,003, 18,500 and 22,000. The local report is
`build/native-flight/region-runner-fixture/comparison.json`. RAM was temporary.
This is a runner/fixture comparison using the same native runtime, not an
independent original-game flight comparison. An earlier apparent trajectory
difference compared different ticks; the aligned snapshots rule it out at
these sampled points.

An exploratory ordinary-input pilot pulled up at tick 10,615 for 35 ticks and
held right rudder from tick 11,200 to 11,600. Its 61 input/stage intervals and
62 sampled bodies match original compared RAM/drawing, with no config write.
It returns to the menu with only 4,033 scene frames and fails the existing
acceptance guards. Reports are in
`build/native-flight/matrix-cursor-region-investigation/`; passing RAM was
removed. This unsuccessful pilot is not adopted: the fixture retains its
established keys and all strict guards.

Those source comparisons start original instruction execution from native
before-states. They establish sampled boundary agreement, not independent
complete original-flight parity. Next work remains ordinary-input region/zone/
NPC-missile coverage, followed by mode-four mission success and the other
whole-port work listed in `NEW_PORT_HANDOFF.md`.

## Validation

Debug and Release mode-fixture builds pass. Release mode-two and mode-eight
combat source checks plus artifact cleanup pass (3/3); Debug ordinary mode-four
and artifact cleanup pass (2/2). Logs are
`build/native-flight/mode-diagnostics-release-ctest.log` and
`build/native-flight/mode-diagnostics-debug-ctest.log`. All fourteen invalid
tick-value/configuration combinations fail before startup. The three aligned
RAM comparisons above also pass with the final fixture code. Passing capture
storage is removed and the 4 GiB build-cache pruner remains enabled.

The canonical playable and Release playable remain identical, SHA256
`d66c5ea2578453ebe9e444a314b5c4345dfd46bed19440b25fafa0a6c1a64a50`.
