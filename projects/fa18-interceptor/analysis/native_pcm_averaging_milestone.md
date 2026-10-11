# Native PCM interval averaging - 2026-10-09

The playable renderer now averages the signed, volume-scaled source signal over
each host output interval. Previously it held the sample at the interval's
start, which differs at source-byte boundaries. The authority is the original
reference core's `audio.c` functions `anti_prehandler` and
`samplexx_anti_handler`, from source commit
`ace4c3a9553e7005ed32c6fee8817126a53a8887`; the complete file SHA-256 is
`d9371ba84ce7b42aea93ae3da10f648bc7b919e61a9f3e60b66de11d62a92390`.

The real caller remains `port/native/main.c` -> `native_audio_render` in
`port/game/native/audio.c`, inside the gameplay heap guard. The renderer
integrates in integer rational PAL-clock units, truncates each channel's
average before stereo summation and applies the existing original x2 gain.
Source-byte advances, buffer requests, stops, period publication and voice
sequencing retain their preceding timing and ownership. The change removes
point-sample holding; it adds no emulator, allocation, growing buffer or
guessed filter/LED policy to the playable runner.

The component CTest extracts the two unchanged original function bodies into
a generated test-only header. An independent global rational timeline splits
output intervals at source-byte boundaries and feeds those functions. In both
Release and Debug, all 196,608 intervals across 48 streams match: four channels,
periods 124/300/358/32767 and output rates 44100/48000/709379. Signed samples,
byte transitions and buffer wraps are included. Arbitrary output block splits
produce identical PCM. The test detects 2,276 intervals where the old holding
algorithm differs. Volume 21 keeps the original 32-bit accumulator in range;
the largest tested positive period is 32767 because the original voice owner
clamps negative signed periods. This establishes pre-filter averaging math,
not full Paula fetch/interrupt or filtered recording parity.

The active sample oracle also passes all 256 original C500D8 sample-handler
cases, and independently checks actual loaded square-wave PCM using periodic
prefix areas. Release audio-trace, PCM-buffer and averaging CTests pass; Debug
passes the same three checks. The updated sample oracle passes in Release.

The complete intro (3,100 frames), Free Flight (6,500) and final combat (24,200)
preserve all preceding RAM, final pixels, saved pilot bytes and non-PCM
counters. Their WAVs intentionally change, as do counts of nonzero samples.
The old preallocation checkpoint is retained. The new checkpoint is accepted
only after the original-function comparison and unchanged-state checks; the
ordinary preallocation CTest still requires exact complete WAV/state equality.
Debug passes that strict check against the new Release checkpoint for all three
runs. Both builds report zero gameplay heap violations and SDL pool failures.
Project and SDL storage remains preallocated; OS/driver heaps are unobserved.

A complete Release Demo separately preserves every request, stop, voice,
cursor and phase trace byte across all 12,680 boundaries, 8,248 sample requests
and 554 stops, together with complete final game RAM and all non-PCM counters.
Its averaged WAV is retained for the unresolved recording comparison; passing
duplicate traces and raw RAM are removed after verification. Trace coverage
per channel is 173/217/24/7670 active requests. This runtime result is distinct
from the original-function component check.

The reproducible checkpoint and executable identities are in
[native_pcm_averaging_checkpoint.json](figures/native_pcm_averaging_checkpoint.json).
Diagnostic reports remain under `build/native-audio/pcm-averaging-Release`,
`build/native-audio/pcm-averaging-demo` and the active CMake Debug output folder.
Canonical `build/native/fa18_native.exe` is refreshed from validated Release.

Complete original/native sound onset and handoff alignment, Amiga fixed/LED
filtering and intermediate LED state remain open. The original and native
complete Demo recordings start in different contexts and are not accepted as
matching WAVs. Broader full-flight drawing and visible performance also remain
open; named-state cleanup stays outside the active complete-C-port goal.
