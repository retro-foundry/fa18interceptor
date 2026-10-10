# Interactive microsecond acquisition — 2026-10-10

Windowed `fa18_native` now acquires actual elapsed microseconds from SDL's
performance counter. The previous 20,000-microsecond PAL samples always cleared
the five low bits used by original scene initialization. The connected runtime
now preserves those bits. This fixes acquisition precision; independently
recorded complete-flight and sound acceptance remain open.

The active caller is `port/native/main.c -> native_frontend_tick ->
native_flight_tick -> timer_child/setup MC_TIMER_REQUEST ->
native_clock_sample/native_clock_request -> read_menu_time_sample (C16D04)`.
The runner installs a preallocated callback/context before locking gameplay
memory. Acquisition returns complete seconds and microseconds through the
existing timer.device request boundary. The original C16D04 copy, timer owners,
C28722 initialization, scene rules and RNG remain connected and unchanged.
The dependency removed is quantized PAL time as the interactive GetSysTime
input. Host time starts at the runner's counter origin after startup/prewarming;
it is neither a captured reference clock nor an offset fitted to a recording.

The default windowed clock is `host`; the default headless clock remains `pal`.
`--clock host|pal` declares either acquisition contract explicitly, and
`--clock-report PATH` records its actual requests outside the gameplay lock.
Historical visible comparison tools now select `--clock pal` explicitly so
their sealed routes retain the same inputs. Their existing strict comparisons
are unchanged. Their combat/camera measurements cover that diagnostic clock;
they do not establish equivalent coverage for new default host-clock inputs.

Validation covers the connected runtime and original rules separately:

- The C16D04 runtime test preserves the observed original 976,083-microsecond
  sample, all 32 low-bit patterns, second rollover and 32-bit seconds wrap.
  PAL diagnostic rollover retains the previous values.
- The external original-instruction timer oracle checks 72 cases, including
  precise fractions and rollover: complete compared non-stack RAM and poll
  counts match (49 cases yield). Two complete C28996 periodic record passes
  also match. Its final retained input is the actual visible run's RAM;
  controlled acquisition samples exist only in this external oracle.
- Default and explicit headless PAL runs have identical complete RAM and
  counters. Explicit headless host acquisition observes every low-bit pattern.
  That fast run reaches C10C08 with only about 2.59 seconds of host time;
  it is an acquisition check, not a flight qualification or parity claim.
- A 1,960-update qualification comparison preserves every RAM, PPM and WAV
  byte and all counters against the retained previous runner. Event ownership
  checks reject skipped/unreached events and all 12 malformed anchors.
- Final focused CTest passes 5/5: clock precision, actual host acquisition,
  replay, event replay and artifact cleanup. The separate original timer check
  passes. The normal native build completes and updates the playable executable.

A fresh visible default-host run starts its own pilot directory and enters
Free Flight through ordinary keys. Sound is live. All **6,502 rendered frames
are presented**, with **zero work measurements above 20 ms**; work mean is
1.5800 ms, p95 4.3277 ms, p99 7.5795 ms and maximum **15.3594 ms**. Its
2,252 clock requests retain **all 32 low-bit patterns**. It reaches C10DAE
with no postflight reset and retains 177 actual flight observations. This
bounded check stays on the ground, with gear down; it does not qualify takeoff,
mission completion, combat or other camera views. Every timing row and the
complete compressed flight trace/final RAM are retained.

The callback, context and counters use fixed storage. On this Windows host,
SDL's counter reads call QueryPerformanceCounter/QueryPerformanceFrequency
without an allocator. The visible run retains the 32 MiB SDL arena, 512 KiB
PCM rings, zero project gameplay heap violations and zero pool failures.
There are **two bounded SDL gameplay pool requests**, satisfied inside the
preallocated arena (71,152 bytes startup, 75,392 peak). OS/driver allocations
are outside the allocator report's observed scope.

Evidence lives in `build/native-flight/host-clock-visible/`,
`build/native-flight/host-clock-headless/`,
`build/native-flight/host-clock-final-default-guards.json` and the committed
[hash checkpoint](figures/native_host_clock_checkpoint.json). The visible
capture's retained executable is SHA-256
`3a19e795bf743d119dd540023d83a8c26c1ab92446dffe16bd93fc1e8b5b86ed`.
The final tested/distributed executable is
`36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`:
only a setup comment changed in runtime source between capture and relinking.
Capture identity is preserved rather than attributed to the later executable.
Passing raw duplicates are removed after verified compressed retention;
the existing build and CTest pruning hooks remain enabled.

The preceding [escort clock evidence](native_escort_scene_clock_milestone.md)
establishes why the precision matters and validates the original initializer
under both actual input sets. This change does not claim the subsequent escort
outcome matches. Broader default-host mission/combat/view measurements,
independent whole-flight comparisons and original sound onset/handoff/waveform
checks remain open. The uninterrupted campaign requirement stays waived;
named-state cleanup remains a separate task outside this goal.
