# Native PCM buffer ownership - 2026-10-08

PCM playback now retains named host buffer spans rather than game-data
addresses. This moves the current/next sample buffers out of the per-byte
global address lookup path. The buffers remain live views of their owning
frontend's sample storage; sample bytes are not copied or substituted.

The connected path is `port/native/main.c` -> `native_audio_render` -> buffer
service -> `audio.c:request_voice_sample` (original C500D8). The original
handler still selects the voice, truncates its word length, updates repetition
and chaining, and produces period/volume. Native service resolves that
address/length once through its frontend-owned `native_storage_span` and
retains `NativePcmBuffer` pointer/byte count for current and next playback.
The byte loop uses that span and its existing cursor/phase. Priming, reloads,
coalesced requests, signed PCM, stereo, pitch, volume and PAL clock are unchanged.

`native_storage_span` takes an explicit storage owner. The existing global
`native_storage_range` keeps its checked interface for remaining addressed
game state. Frontend initialization supplies the audio resolver and owner;
an active request without a resolver fails explicitly. Sample storage lasts
for the frontend lifetime, as before.

## Evidence

Before the change, the accepted `adc019b1` Release executable recorded three
ordinary-key runs: intro/selection at 3,100 frames, Free Flight at 6,500, and
the newly enlisted final combat route at 24,200. The last run uses the actual
cruise-earned log and exercises all eleven flare/chaff presses, four aircraft
expiries and objective admission. The changed executable agrees on complete
WAV, exported 1 MiB data image, final pixels, saved log and every runtime
counter in all three runs. RAM/WAV files are temporary; only reports and hashes
are retained. This is preservation of the previously accepted native output,
not an independent original complete-flight or recorded-audio comparison.

The existing sample oracle still compares 256 complete original C500D8 calls
and their output payloads. Four loaded square-wave streams match an independent
rational sample-index formula across arbitrary block partitions. Its explicit
sample-owner resolver is called once per active request rather than per output
byte. The 145 voice-program sequences / 7,250 original callbacks also pass.

`fa18_native_pcm_buffers` uses the actual frontend resolver in a component
check. Both host storage banks preserve odd-address alignment, signed output,
live sample-byte edits and their own buffer while an unrelated global storage
owner is bound. It keeps this observation inside the current buffer: original
voice requests still use the game-data interface. Release assertions are
enabled. An initial component setup coalesced stop/start before draining the
empty-slot request, so its second bank correctly retained the first current
buffer. The corrected setup consumes the stop before starting the next bank;
game restart/priming semantics were preserved.

One same-host headless final-combat timing run without WAV/device output
measures mean audio work 15.1705 -> 10.5480 microseconds, and p95 26.0 -> 16.9.
Runtime counters agree. Timing is observational, not a CTest threshold or
complete visible-frame performance acceptance. WAV timing is recorded
separately and includes file publication; it is not used for this claim.

## Remaining scope

This removes addressed byte fetches from PCM playback only. Voice programs,
sample request state and other gameplay fields still use the source data arena.
Their typed-state migration remains open, along with original audio alignment,
hardware fetch/filter fidelity, independent complete original flights,
uninterrupted tour checks, remaining caller contracts and broader performance.
No newly encountered undecoded routine was found; C500D8 and the voice owners
were already readable. The source inventory is unchanged. The complete-port
goal stays active. CPU/chipset/translation/glue objects remain absent from the
playable native link.

Eight selected Release checks and six Debug checks pass: PCM ownership, original
sample/voice oracles, full earned tour and artifact policy/cleanup in both;
frontend/link and timing-instrumentation checks in Release. All nine tour/wrap
stages agree with the preceding accepted results and between configurations.
The refreshed canonical Release executable matches the validated build. The
build cache is 2.07 GiB after pruning; capture limits remain unchanged. Sealed
reports and hashes are in `figures/native_pcm_buffer_ownership_checkpoint.json`.
