# Native sample requests and output

The playable path is `port/native/main.c` -> `native_frontend_tick` -> original
sound consumers -> native channel requests, then `native_audio_render` ->
`request_voice_sample` -> `amiga_pcm_write`. Game behavior remains in
`port/game/`; the SDL/WAV publication service lives in `port/amiga/`.

## Original behavior

C4FFB4 writes each descriptor's +$14 INTREQ payload. Despite the historical
`clear_voice_interrupt` name, the actual hunk descriptors contain $8080,
$8100, $8200 and $8400: these **request** audio-channel service. C500D8
acknowledges through the separate +$16 payload and selects the real slot.
The previous native no-op lost both sample starts and stops. It now posts
a coalesced request to the current frontend's audio owner.

C500D8 publishes the sample pointer, ASR.L length truncated to a word,
C501E0's period/volume, and enabled output. Infinite repetition $FFFFFFFF
is unchanged. Other counts decrement with longword wrap; a negative result
publishes the voice's +$20 link to the slot and resets the old count from
+$14. An empty slot publishes volume zero, period 124 and stops output.
The shared `request_voice_sample` preserves that game RAM owner.

The host stream preserves a playing and next buffer, initial priming and
reload requests. The initial request is supported by the reference audio
implementation's state 1 -> 5 transition; it is not counted as a played
loop. Reload requests run the same source handler. Signed disk PCM is held
at the PAL clock 3546895 / period (reference `custom.h` CHIPSET_CLOCK_PAL),
with rational phase retained between host blocks. Pitch publication applies
at the next source byte. Channels 0/3 route left, 1/2 right. The host publishes
stereo s16 at 48000 Hz, 960 frames per existing native PAL frame.

`amiga_pcm_write` queues that PCM to SDL in the windowed runner. Headless
runs execute the same game/sample path without opening a device. `--wav PATH`
captures the exact published PCM with a finalized stereo RIFF header. Device
and file failures are reported; no substituted samples or muted failure path
were added. CPU, custom-register/DMA engines, ROM, translations and glue remain
absent from the native link.

## Evidence

`python tools/native/check_samples.py` checks:

- 256 complete original C500D8 calls and their C501E0 child, matching all
  non-stack RAM below C7FC00 and pointer/length/period/volume payloads. Cases
  cover empty channels, all four outputs, infinite and finite counts, signed
  wrap, links/reset counts, duplicate/odd/zero lengths and output limits.
- Four actual disk-backed square-wave streams against an independent rational
  sample-index formula, including signed PCM, stereo isolation and arbitrary
  host block partitions producing identical samples, phase, requests and RAM.
- Actual native intro/menu/selection WAV: stereo, s16, 48000 Hz, exactly 960
  frames per PAL tick, and counted nonzero PCM matching runtime diagnostics.
- SDL device creation and nonzero queued credits PCM through its dummy driver;
  this checks host publication, not speaker fidelity. The normal SDL frontend
  check also opens the default device successfully.
- A missing WAV parent fails with a specific output error.

The 7,250 PAL voice-update comparisons and real wait/fade/tone-completion check
still pass. Menu/frontend, complete native demo, carrier success/save/restart/
reload, crash/re-entry and twelve focused reference contracts pass. The native
map excludes CPU/chipset/translation/glue symbols. No original full replay is
repeated; existing source data is reused.

Actual Free Flight at PAL frame 6100 has 409 cockpit/HUD frames, 10,017 sample
requests and 5,856,000 stereo PCM frames (4,045,715 nonzero). Review artifacts:
`build/native-flight/sample-freeflight6100.png`, `.wav`, `.dat`.
The loaded menu sequence reaches sounds 21/22 rather than staying on 13/14,
demonstrating connected chain traversal in the actual runner.

## Scope

The three native audio integration boundaries are connected: PAL programs,
sample requests/repetition/chaining, host PCM publication (**3/3, 100% of
this connection inventory**). This is functional native playback, not
bit-exact recorded audio. Hardware fetch/interrupt latency, analog filtering
and exact alignment with original recorded audio remain unverified.
The whole-game goal remains open: startup lead is still 36 game ticks;
full recorded-frame parity remains 0/3 accepted, with Copper fade excluded.
