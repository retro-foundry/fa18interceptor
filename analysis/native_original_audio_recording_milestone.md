# Complete original demo PCM recording - 2026-10-09

The reference host now records the actual original emulator's complete batch
PCM output. This establishes a usable reference recording, not acceptance of
native sound onset, sample handoffs or filtering. The native executable and
audible implementation are unchanged.

The connected reference caller is `scripts/engine9000_bridge.py` -> pinned
`ami9000.dll:retro_run` -> libretro batch audio -> `Engine.on_audio` -> WAV and
per-block sample coverage. It removes the missing reference-PCM export step.
The original core remains unmodified; the native game still does not link a
CPU/chipset or this comparison host. `fa18_recomp` is not a full original audio
authority, so its output is not substituted here.

`--wav` records only the requested ordinary replay calls, after startup/restore
initialization. Rate comes from `retro_get_system_av_info`, not an assumed
native rate. Every stereo signed-16 PCM block is copied intact and logged with
its call, sample offset, length and hash. Callback write/budget errors propagate
to the replay caller. Instruction-stepped recording is rejected. The default
512 MiB capture budget reserves 8 MiB for other outputs and bounds PCM/log
writes; an oversized requested window fails before creating a WAV.

The complete sealed `captures/uae/run075` demo replay covers all 21,069 ordinary
calls and all 21,069 emitted blocks: 18,582,858 stereo frames at 44.1 kHz, or
421.38 seconds. Every block and the entire WAV are checked without trimming,
alignment, resampling, normalization or filtering. PCM SHA256 is
`76ffa4bef9b203c84e67565b41685b1e62d66c0710fa0c318d264fa2d8c2b3c0`;
WAV SHA256 is
`db14f6340ca2464d6f2fd441b8f007bc2eb21d4eac7d39990befcb0d37816745`.

An otherwise identical full nonrecording replay preserves every original
register, complete chip/slow RAM, serialized state, video and existing audio
hash. The real 32-call CTest also checks this and rejects missing blocks,
shifted offsets, modified PCM, instruction stepping and insufficient budget.
The capture validator continues to check RAM after verified lossless gzip
retention. Sealed recordings, source state and core are untouched.

The reference uses the pinned core's `anti` interpolation, `emulated` filter,
`auto` filter type and 100% stereo separation. Its source is
`tools/engine9000-src/ami9000/sources/src/audio.c` at commit
`ace4c3a9553e7005ed32c6fee8817126a53a8887`: A500 fixed filters use 6,200/20,000 Hz
coefficients, with the LED-controlled cascade using 7,000 Hz coefficients.
The sealed options also retain optional floppy sound at 80; any contribution
must be identified before attributing waveform differences to game samples.
The core's filtering is the recording authority here, not a claim of exact
analog hardware reconstruction.

Native `port/game/native/audio.c` currently uses integer sample holding at
48 kHz and no corresponding output filter. Its consumed Demo replay candidate
has 12,172,800 stereo frames (253.6 seconds) including the native intro key.
That recording has a different startup/control/timing context from the sealed
full emulator replay. The two recordings have not been aligned or accepted as
equivalent. Next work must establish original voice/onset and buffer-handoff
events, the actual LED/filter state, and an evidence-backed native response.
No convenience timing offsets or guessed filter defaults are added.

The candidate retains zero project gameplay heap violations and zero SDL pool
failures. Original RAM is compressed; the complete WAV and block log remain in
bounded build storage for this unresolved sound investigation. The compact
report, exact authority hashes, options and candidate counters are retained in
`figures/native_original_audio_recording_checkpoint.json`. Release CTest and
artifact cleanup pass; the active Release native executable retains SHA256
`1684a3fcb297249f16880eb082403094a6a83f25784d6ab4890cf823fad0bfd2`.
The complete-C-port goal remains active with audio fidelity, broader drawing
and visible performance acceptance still open. State cleanup remains deferred
and the uninterrupted campaign requirement remains waived.
