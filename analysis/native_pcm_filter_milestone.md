# Native A500 output filter - 2026-10-09

The playable native runner now filters its interval-averaged stereo PCM with
the original A500 fixed and enabled LED cascades. The dependency removed is
unfiltered host output. The real caller is `port/native/main.c` ->
`native_audio_render` -> `native_pcm_filter_process` in
`port/game/native/audio.c` -> the existing PCM device/WAV writer. The native
game does not run CPU or chipset emulation to produce these samples.

## Authority and storage

The authority is `audio.c:filter`, `rc_calculate_a0` and their original
SoftFloat tangent implementation at source commit
`ace4c3a9553e7005ed32c6fee8817126a53a8887`. The complete `audio.c` SHA-256 is
`d9371ba84ce7b42aea93ae3da10f648bc7b919e61a9f3e60b66de11d62a92390`.
The pinned original core and independent cold-start recording identities are
retained in [the preceding filter evidence](native_original_filter_state_milestone.md).
Its complete Demo reports enabled LED filtering at all 21,069 audio boundaries.
The first notification and the complete cold-start response establish this
output profile; neither endpoint-only observations nor silence-only captures
are accepted as filter evidence.

`NativePcmFilter` owns 56 fixed bytes: five float histories per stereo channel,
three original coefficients and the output rate. Startup sets it once, before
the frame loop, at the runner's existing 48 kHz output rate. Histories persist
through title, menu, flight, stops, silence and menu return. Processing modifies
the existing PCM buffer in place. There are no allocations, growing buffers,
transcendental calls or filter resets in gameplay. Rates 44.1 and 48 kHz have
source-validated coefficients; other rates fail explicitly. The original
ordered float operations, truncation and denormal offset are retained, with
MSVC `/fp:strict` or GNU contraction/fast-math disabled for this source.

Low-level unconfigured PCM components continue to expose the pre-filter signal
for their existing sample/averaging oracles. The playable entry always
configures filtering. The music transition test now configures the same profile
and renders it through title acknowledgement, menu ducking, music-free flight
buffers and the original Shift-Escape return to menu.

The game's original stereo x2 gain is unchanged. The reference emulator's
floppy mixer also scales its PCM by 2/3 even with drive clicks muted. Unique
conversion around that gain exists only in the recording comparison tool;
neither optional drive noise nor that host mixer gain is added to the game.

## Component and complete recording validation

The new linked-native component test compares every sample against extracted,
unchanged original filter functions and actual original SoftFloat sources.
Both Release and Debug pass 32,768 stereo input frames at each supported rate,
including signed full-scale impulses, separate stereo channels, DC, full-scale
alternation, deterministic component vectors and 8,192 trailing silent frames.
Blocks of 2,048, 13 and one frame produce identical output. LED-off, A1200 and
fixed-only outputs differ and are rejected; unsupported rates are rejected.
The component generator does not modify game RNG or scheduling.

Both builds also pass every one of the 13,230,014 stereo frames in the original
independently cold-started raw/filtered recording pair at 44.1 kHz. Actual
native filtering, with the test-only reference gain conversion, gives the
complete original filtered PCM hash
`3306c6e282474025dbfd0a0a1c44409d99bfd8013883678510ab9f00c19e7016`.
No offset, sample trimming, excluded silence, segment selection or alignment
is used. This proves the native filter's response to original input; it does
not prove native game sound onset or buffer handoffs against that recording.

## Playable integration

The 3,100-frame intro, 6,500-frame Free Flight and 24,200-frame final-combat
runs preserve complete prior RAM, final pixels, saved pilot bytes and all
non-PCM counters. Filtered WAVs and nonzero-output counts intentionally change.
The prior averaging/preallocation checkpoints remain retained. The new strict
preallocation/output checkpoint requires complete WAV/state/save equality;
Debug passes it against Release over all 33,800 frames. Both builds report zero
guarded gameplay heap violations and SDL pool failures. OS/driver private heaps
remain outside the measurement.

The ordinary strict output CTest also passes in Release. PCM filter,
averaging, buffer-ownership and audio-trace CTests pass in both builds; the
filtered title/menu/flight music checks cover modes 1, 2, 5 and 7.

A visible 6,500-frame intro/menu/Free Flight run uses a real audio device and
presents every frame. Complete WAV, RAM, pixels, counters and save status match
the headless checkpoint. There are zero gameplay heap violations or SDL pool
failures; two SDL requests use the fixed startup arena, whose peak is 75,952
bytes. Audio work peaks at 517.9 microseconds, including the filter. All timing
rows are retained. At frame 799, still in the intro, input polling costs
405,592.3 microseconds and total frame work reaches 406,230.9 microseconds.
This single row fails the 20 ms gate; it is neither excluded nor attributed
to the filter. Only mode 0/1 and view 0 are measured here. The complete failed
performance case remains under `build/native-audio/native-filter-visible`;
broader mission/view performance acceptance stays open.

A complete playable Release Demo independently checks all 12,680 boundaries,
8,248 requests and 554 stops. Every request/stop/voice/cursor/phase trace byte
and every final game-data byte remain unchanged. Every one of its 12,172,800
stereo output frames matches the unchanged original filter applied to the
preceding native averaged PCM at 48 kHz. Actual and expected PCM SHA-256 are
`3fb1c48d9602a48622a8237db7a7ea84cd2736b75c7e8e1edbe66a99e9ca9867`.
No gain conversion, offset, trimming or exclusions are used in this runtime
comparison. The entire filtered native WAV is retained for the remaining
onset/handoff investigation; duplicate passing PCM/traces and raw RAM are
removed after verification, with reusable RAM compressed.

The checkpoint, compiler/executable identities, complete recording response
and playable Demo reports are in
[native_pcm_filter_checkpoint.json](figures/native_pcm_filter_checkpoint.json).
Local reports are under `build/native-audio/native-filter-reference-Release`,
`native-filter-reference-Debug`, `native-filter-Release`, `native-filter-demo`
and the active CMake preallocation output folder. The validated Release is
copied to `build/native/fa18_native.exe`.

Original game onset and handoff timing, remaining full-flight drawing and
broader visible performance acceptance remain open. Complete native/original
Demo WAVs begin in different timing contexts and are not claimed identical.
The user waived an uninterrupted campaign and deferred named-state cleanup
outside this goal; individual earned mission evidence remains valid.
