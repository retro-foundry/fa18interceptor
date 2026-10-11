# Connected native audio trace - 2026-10-09

The playable runner now exposes complete sample-request and stop events,
payload hashes and per-frame voice/stream state through `--audio-trace PATH`.
This removes the missing native event evidence needed for comparison with the
original full demo's audio writes. It does not change the audible algorithm:
native still uses sample holding, and original interpolation/filter acceptance
remains open.

The actual caller is `port/native/main.c` -> `native_audio_render` -> service ->
original C500D8's `request_voice_sample` -> optional `NativeAudioObserver` ->
`port/native/audio_trace.c`. Original empty-slot stops also publish an optional
event. Requests retain their sample address/length, effective period/volume,
channel and exact native output-frame index. Hashes use the already resolved
host sample span; stream ownership remains unchanged. Native playback does not
receive source RAM or original clock values and adds no CPU/chipset dependency.

Each frame records four complete active voice records, master volume, actual
channel outputs, stream cursor/phase, stage and update identity. Header/footer
counts reject incomplete evidence. The sink owns a startup-configured 4 KiB
stdio buffer and fixed 4 KiB row workspace, with no gameplay allocation.
The existing capture budget bounds every write, including header/footer.
Audio tracing requires a separate run from flight/RAM capture so their budgets
cannot silently overlap. Without the optional sink no trace rows or sample
hashes are produced. This is diagnostic CLI behavior, not a new player flow.

The complete consumed 4,892-iteration Demo has 12,680 boundaries, 8,248 requests
and 554 stops. Active requests by channel are 173/217/24/7,670. Every recorded
sample hash agrees with its actual retained native payload; all boundaries and
footers are checked. Complete WAV, both RAM banks and every runtime counter
preserve the preceding accepted native recording. The Debug run agrees with
every Release trace row and the entire WAV/RAM/counters/saved pilot. Both have
zero project gameplay heap violations and SDL pool failures.

Release/Debug `fa18_native_audio_trace` CTests exercise real intro/menu keys,
compare traced and untraced PCM/RAM/counters, verify payloads, and reject
truncated, exhausted-budget and conflicting-capture runs. Preallocation,
complete preallocation output and PCM-buffer CTests pass in both builds. These
include full intro, Free Flight and final-combat native preservation. Passing
test captures remain temporary; the complete demo trace and native RAM are
verified and retained compressed. Duplicate passing full WAVs and Debug trace
are removed after whole-file equality checks. The earlier native full WAV
remains the reusable exact PCM reference.

The original full replay has 21,069 boundaries and active buffer loads
254/368/19/7,640. It starts partway through menu music from its sealed state;
native starts fresh from ADF/title and consumed update inputs. These runs have
different startup/control/timing contexts. Their counts are not accepted as
equivalent or used alone to assert an audio fault. Native/source onset and
handoff alignment, intermediate LED/filter state, interpolation and complete
filtered waveform comparisons remain required. No timing offsets, clock
substitution or guessed filter defaults are added.

`figures/native_audio_trace_checkpoint.json` retains exact commands, executable
identities, coverage, full-preservation hashes and counters. The final Release
executable differs from the full Release capture binary only in CLI help text
adding the trace option; Debug's complete capture uses its final binary. The
canonical Release runner is refreshed after validation. The whole-C-port goal
remains active; state cleanup is deferred and uninterrupted campaign waived.
