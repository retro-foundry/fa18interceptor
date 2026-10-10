# Native output rate and startup allocation ownership

The playable runner now accepts `--audio-rate 44100|48000` at startup. The
default remains 48000 Hz. This makes native recording at the original reference
recordings' 44100 Hz possible without resampling captured output. The existing
native A500/LED filter already has source-validated coefficients at both rates;
the runner now configures the filter and PCM device consistently before gameplay.

The connected path remains `port/native/main.c` -> `native_audio_render` ->
`request_voice_sample` (original C500D8), followed by `amiga_pcm_write`. The
existing 960-frame stack span covers both integral 20 ms blocks: 960 frames at
48000 Hz and 882 at 44100 Hz. Native byte periods, rational PAL clock, voice
programs, buffer owners and sample selection are unchanged. No new game routine,
CPU/chipset dependency, fitted clock or delay is introduced.

## Complete state and physical sound timing

Release and Debug independently exercise intro/selection (3100 ticks), Free
Flight (6500) and the actual earned final-combat pilot (24200) at both rates.
Complete final RAM, pixels, saved pilot and every non-PCM counter match. Every
20 ms audio boundary retains identical voice records, levels, playing status,
cursor and exact rational byte phase in physical time. The complete per-channel
request/stop sequences contain respectively **2086, 5890 and 55357 events**.
Payload hashes, buffer lengths, pitch, volume, channel and game tick agree on
every ordinal. Each event's enclosing sample intervals agree within their
declared output sample widths. Neither trace nor PCM is shifted or searched.

Five mutations of actual observations reject changed byte phase, event time,
payload, channel and omission. Invalid startup rates also reject. Explicit
48000 Hz reproduces the default exactly. Default Release WAV, RAM, pixels,
pilot and complete runtime counters preserve the preceding qualified
`b5eead93...` executable across all three scenarios.

The first checker rejected an interleaving at final-combat tick 5333. Independent
channel-0 and channel-3 requests occupy one enclosing sample at 48000 Hz and
adjacent samples at 44100 Hz. Every channel's ordinal sequence and all 24200
boundary states/phases remained exact. The corrected check compares those actual
per-channel owners and sample intervals, without sorting events on a channel or
discarding rows. The rejected original assumption, complete recordings and
diagnostic context remain compressed and retained.

## Device allocation startup

The first real 44100 Hz window revealed two SDL arena requests after gameplay
started. SDL's audio thread performs conversion after returning from the user's
callback. `SDL_audiocvt.c:EnsureStreamBufferSize` lazily reserves conversion
storage on that first pass. Device opening now waits for its second callback,
which proves that the first conversion completed. This uses the empty PCM ring
before gameplay and has a bounded startup failure. It advances no game voices
and adds no captured WAV frames.

That handshake absorbed the conversion request. The remaining 112-byte request
was SDL's initial polling sentinel: `SDL_events.c` pumps with a sentinel for
`SDL_PollEvent`, while `SDL_PumpEvents` alone does not. Startup now calls
`SDL_PollEvent(NULL)`, whose source contract preserves queued real input.
Both changes move allocation ownership to startup, using the existing fixed
SDL arena and fixed PCM ring.

Final real Direct3D/WASAPI windows in Release and Debug present all **180 frames**
and preserve every 44100 Hz PCM byte and final RAM byte. Gameplay heap violations,
SDL arena requests and arena failures are **zero**. Measured title-screen frame
work peaks at **2.8578 ms** in Release and **3.2739 ms** in Debug. These short
checks qualify startup/device use, not combat performance. Automated hidden
dummy audio/video checks at both rates also preserve complete startup outputs
against independently launched headless games and report zero requests.

Six final Debug CTests pass, covering output rates, allocation, trace ownership,
PCM buffer ownership, original averaging and cleanup. Release passes the
connected complete rate/default-preservation tool and the five affected
allocation/trace/averaging/buffer/cleanup checks. Combined temporary capture
peaks are **341415473 bytes** for the Release baseline comparison and
**244744907 bytes** in Debug, within 512 MiB. Passing raw copies are removed;
failed-case recordings and executable identities are losslessly compressed.

The final Release runner also reproduces the complete qualified Mission Five
recording from ordinary controls and the actual enlisted pilot. All 13763
complete bodies, 41289 full-MiB snapshots, complete trace, timing, counters,
final RAM, earned pilot and menu return remain exact. This extends the existing
original body/drawing qualification to the current executable without rewriting
the original reports or supplying captured RAM to gameplay.

At this milestone, the presentation pacer was initialized before device startup. The
new callback readiness wait exposes initial presentation debt: the first few
window frames can arrive close together before normal 20 ms pacing resumes.
Move that pacer's origin to the completed startup boundary in a follow-up and
verify visible presentation again. This is separate from the PCM/state and
zero-allocation results above. That follow-up is now completed in the
[startup pacing evidence](native_startup_pacing_milestone.md), with its own
current executable identities and complete Mission Five preservation.

See the [checkpoint](figures/native_audio_output_rate_checkpoint.json) for
executable/source identities, complete reports, terminal tests, retained
rejections and current complete Mission Five preservation. Evidence is under
`build/native-audio/output-rate`.

This batch removes the runner's fixed-rate recording limitation and two lazy
startup allocations. It does not accept native sound onset or the complete
original/native flight/combat waveform. The original frontend's display-derived
output clock and sequential startup channel enables still differ from native;
the [original clock evidence](native_original_audio_clock_milestone.md) retains
their actual owners. Named-state cleanup stays outside this goal; uninterrupted
campaign completion stays waived.
