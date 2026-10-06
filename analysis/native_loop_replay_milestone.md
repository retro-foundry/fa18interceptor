# Native recorded-input delivery

The playable caller is `port/native/main.c` -> `native_frontend_tick` ->
`native_replay_update` -> raw Amiga menu dispatch or the host keyboard queue ->
C0F3C4 `process_pending_key_events`. The native build links `replay.c` directly;
CPU, ROM, instruction adapters and chipset execution remain absent.

`port/recomp/loop_input.c` defines the existing `FA18_LOOP_INPUT_V1` contract:
the first column is the C0EFD4 main-loop iteration; the second is informational
video-frame metadata. Native delivery retains raw keys and same-iteration edge
order. It does not reinterpret recorded Amiga keys through SDL. Timer and outer
display continuations retain the current update iteration.

The three sealed recordings start at the main menu (C0FCB4, mode 0). Read-only
inspection of their saved states established this anchor; the native runtime
does not load those states. Replay starts on the first main-menu update after
ordinary ADF startup and pilot entry. Existing E9K input can supply the intro's
Space press. It remains a separate host-frame test input format.

Run a bounded crash-recording prefix:

```text
fa18_native --headless --frames 10000 --replay INTRO.e9k --input captures/native/qual_fail_crashes/input.fa18in --iterations 1960 --save-dir NEW_DIRECTORY
```

`INTRO.e9k` contains `E9K_INPUT_V1`, `F 1800 K 32 0 0 1` and
`F 1802 K 32 0 0 0`, each on its own line. Use a fresh save directory for
reproducible cold startup. `--iterations` defaults to the recording's `end`
iteration. Invalid rows, unsupported device events and a frame cap reached
before the requested iteration return errors. Counters expose update/replay
iterations, delivered events and anchor status.

Validation:

- `check_replay.py`: the sealed crash prefix reaches active qualification at
  iteration 1960, delivering 18 raw events, with 98 record updates and 97 scene
  frames. Replacing all video-frame metadata with zero preserves every exported
  RAM byte, every output pixel and all counters. Two edges within iteration 1961
  preserve their order through C0F3C4. Eleven malformed input cases and a missing
  menu anchor fail explicitly.
- The carrier-success prefix reaches active qualification at iteration 4760,
  delivering 37 events and executing 95 record updates. Bounded original
  C12098/C1C63E comparisons pass at the crash checkpoint; reached carrier
  descriptor/parent, face-result, hull and circle comparisons pass at the carrier
  checkpoint.
- Updated `check_input.py` confirms the update counter stays unchanged during
  a pending timer, alongside root/HUD/input state. Its 144 original-code input
  cases pass. Frontend, save/reload, SDL presentation and native link omission
  regressions pass.

This delivery milestone is complete (100% of this scope). It does not establish
identical baseline state, original menu/flight cadence or frame-by-frame parity.
The crash prefix extended to iteration 2150 reaches the unconnected C17F8C
collision-sound child; full outcomes remain open. Demo and other game modes,
mouse/controller events, remaining frame/HUD/record children and sample output
remain unfinished. No full reference replay suite was repeated. Copper fade
remains excluded from comparisons.
