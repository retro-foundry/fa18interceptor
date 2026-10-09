# Original audio filter state and response - 2026-10-09

The original reference's published filter state is now measured throughout the
complete sealed Demo, and its filter response is checked against an entire
audible cold-start recording. These establish the reference needed for native
filter work. The playable native executable and gameplay are unchanged.

## Published filter state

The real reference caller remains `scripts/engine9000_bridge.py` -> pinned
`ami9000.dll:retro_run`. Its existing libretro LED interface is installed only
for `--audio-events`, from core initialization onward. The authority is
`libretro-core.c:retro_led_interface`: LED 0 publishes `gui_data.powerled`.
`cia.c:led_vsync` derives that boolean from accumulated pin duty, using the
original brightness threshold of 96, and calls `audio.c:led_filter_audio`.
The LED callback therefore observes the filter's published boolean without
reading live CIA registers or taking additional serialized snapshots.

All 21,069 sealed Demo boundaries report power/filter state 1. There is one
power notification, at replay call 1, before that call's PCM upload. The initial
published interface cache is zero because the core initializes its change
cache to zero; this is not substituted for the sealed state's known enabled
CIA pin. No subsequent power transitions occur. The trace retains all 406,754
rows, including every earlier audio write and voice boundary.

The full nonrecording baseline again agrees on every register, complete RAM,
serialized state, video and PCM block, including the complete earlier WAV
hash. This closes the previously unmeasured published Demo filter-state path.
It does not identify the exact sample at the first within-block transition,
every CIA write or analog pin duty. The source updates the published state at
vsync; the host reports its actual callback time without shifting samples.

The active real 32-frame CTest passes. Missing notifications, missing LED
fields, changed LED boundaries and shifted LED sample offsets are rejected,
even when the event descriptor's row count and checksum are updated. Earlier
PCM/event corruption, missing boundaries, stepping and budget rejections still
pass. Old event captures without the new interface fields remain valid within
their earlier, explicitly narrower scope.

## Complete cold-start filter response

Two independently initialized original instances boot the unchanged disk,
open its Workbench game icon and reach the original credits through 15,000
ordinary replay calls. Their configs differ only in `puae_sound_filter`:
`off` versus `emulated`. Both keep the original `anti` interpolation, auto
A500 filter model and 100% stereo separation. Floppy click volume is explicitly
muted at 100 in both; no disk, ROM, game code or sealed recording is changed.
Complete final registers, RAM, serialized state and video are identical.

`check_original_filter_response.py` validates every recorded block, call,
offset and complete WAV hash. It extracts unchanged `audio.c:filter` and
`rc_calculate_a0` bodies and compiles a test-only reference with the original
SoftFloat conversions and tangent implementation. It does not link a CPU or
chipset, and it is not part of the native game. At 44.1 kHz the coefficients
are 0.48603487, 0.931495488 and 0.521334589 for the original 6200/20000/7000 Hz
stages. The original compiler's double-precision `M_PI` definition matters:
forcing the fallback float constant produced a one-sample later difference
in a rejected warm-state probe. The accepted reference uses the original
GNU math-header precision and original SoftFloat rounding.

The reference core's `driveclick.c:driveclick_mix` applies a two-thirds gain
even when its initialized click resources are silent. The checker uniquely
recovers the even pre-filter samples produced by original x2 `FINISH_DATA`,
rejects any ambiguity and applies that same post-filter mixer gain. This is
source-backed inversion of a known reference stage, not fitted normalization
or a proposed native volume policy.

All 13,230,014 stereo frames and 26,460,028 signed samples match exactly;
12,611,199 samples are nonzero. Nothing is trimmed, shifted or excluded.
Wrong LED-off, A1200 and fixed-only responses are rejected. Earlier cold-start
probes that never launched the game contained only silence and are rejected
by the checker, rather than accepted as filter evidence. A warm-state probe
had an unknown initial filter history and is not accepted as a whole recording.

The source authority remains commit
`ace4c3a9553e7005ed32c6fee8817126a53a8887`, with `audio.c` SHA-256
`d9371ba84ce7b42aea93ae3da10f648bc7b919e61a9f3e60b66de11d62a92390`.
The pinned core remains SHA-256
`5750be527458423407ec293c43cdf312cfd2248a3f06fcfb70c43561cb5549f1`.
The compact report, source identities, exact commands and cold-start playback
identity are in
[native_original_filter_state_checkpoint.json](figures/native_original_filter_state_checkpoint.json).
The generated cold-start input is retained separately from sealed recordings
in `tools/native/fixtures/original_audio_filter_launch.e9k`.

Reusable RAM is retained as verified lossless gzip. The duplicate full Demo
WAV is removed after every original block and complete hash agree with the
canonical recording. The controlled cold-start pair stays in bounded build
storage for the native filter comparison; rejected silent copies are removed
after retaining their disposition. The ordinary pruner remains enabled.

Native still has interval averaging and no corresponding output filter. The
next connected change is an evidence-backed native filter response with fixed
storage, followed by complete native preservation and original comparisons.
Original/native onset and handoff alignment remain open. This cold-start
reference covers boot through credits, not a complete native mission or
filtered whole-flight acceptance. Broader drawing and visible performance
remain open; named-state cleanup stays outside the active goal.
