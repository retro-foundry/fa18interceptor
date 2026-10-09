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

## Complete original audio-event trace

`--wav --audio-events --restore STATE` now also records the pinned core's
existing Custom-register logger for audio/control writes, including actual
writer PC and beam position. It reads the four complete active voice records
and master volume from ordinary game RAM at each replay boundary. Both logs
share the bounded recording budget; dropped Custom writes or callback failures
reject the recording. This extends the actual original host, not the native
game or an invented audio-device model.

The complete sealed demo has 406,753 event rows, including all 21,069 voice/PCM
boundaries. Buffer-pointer low-word writes per channel are 254/368/19/7,640
(8,281 total), and active slot changes are 37/34/10/160. The actual C500D8
handler's writes, C501E0 period/volume publications, source PCs and every voice
record remain retained in the trace. The checker validates contiguous call
coverage and sample offsets against every original PCM block, complete record
sizes and the whole event-log hash. Its full nonrecording comparison again
preserves every RAM/state/register/video/audio field and the entire earlier
WAV hash. Native onset and handoff matching are still open.

Filter endpoints are decoded from the sealed initial state and the normal
final serialization using pinned `cia.c:save_cia` and `audio.c:save_audio`
layouts. Both have CIA PRA 192 / DDRA 3: the LED pin is on. This proves these
two endpoints only. Intermediate CIA writes and within-frame LED duty remain
unproven; the trace does not force a guessed native filter policy.

A rejected 32-frame experiment serialized state at every boundary. Its five
changed bytes belonged to CPU/CHIP/CHPD chunks, even though PCM and RAM still
matched. The strict complete-state check caught it, and repeated serialization
was removed. The production trace uses safe RAM reads and existing write logs,
with no additional mid-run serialization or live CIA reads. Failed probe RAM
is retained compressed with exact differences and hashes in the event report.

The real 32-frame CTest now includes event tracing and rejects a missing voice
boundary or incorrect PCM offset even when the event descriptor's checksum is
updated. Explicitly reusing a retained reference WAV still verifies every
block and complete hash; corrupt reusable PCM is rejected. CTest, Python syntax
checks and artifact cleanup pass. Native source/output/executable are unchanged.

`figures/native_original_audio_events_checkpoint.json` retains the compact
coverage, complete source/register counts, first slot transitions, endpoint
states and exact commands. Reusable original RAM is compressed. The generated
duplicate full WAV was removed after complete identical-hash verification;
`--pcm-reference build/native-audio/original-complete-demo/original.wav` checks
the retained canonical WAV against the event capture's full block log. The
event log remains in bounded build storage for native alignment/filter work.
