# Original startup mixer and linked native filter

The complete independently launched original recording keeps all **5,000
ordinary replay calls and 4,410,000 stereo PCM frames** unchanged. A bounded
read-only mixer observer covers calls **3,800–5,000**, including the first
music byte and every recorded startup music handoff. It stays within the
existing **512 MiB** diagnostic budget. Native gameplay is unchanged.

Within that interval, all **3,433,326 actual records** pass: one initial
accumulator snapshot, **1,906,036 integration intervals**, **1,059,282 emitted
averages** and **468,007 consumed bytes**. Every output budget, signed average,
logical boundary and existing consumed-byte record is checked. Seven mutations
of actual records are rejected. The complete original PCM, event log, RAM,
registers, final save state and video remain identical to the unmodified
reference. The original sample telemetry remains identical to the earlier
5,000-call observer. No DMA stream is claimed for this recording.

The same linked native A500/LED filter used by the playable port produces
**every recorded PCM sample exactly** from these observed original averages,
using blocks of 2,048 and 13 frames. This includes the preceding **3,350,718
recorded silent frames**. Their PCM and absence of consumed bytes are checked;
their mixer intervals were not observed. The comparison applies the original
`sample16si_anti_handler` / `FINISH_DATA(bits=15)` x2 finish and
`driveclick_mix` muted-click 2/3 gain explicitly in the reference adapter.
The playable port's gain, scheduling and clocks are unchanged. No shifted,
resampled, normalized or selected waveform section is used.

The complete PCM SHA-256 remains
`d372bb3238e20f6b2ba8b4adb92ab0e1b1f02a59b73e26ed6d66702c6c614d01`.
Dropping the first actual nonzero Paula average changes 16 PCM samples;
swapping the actual stereo averages changes 1,869,326. Both controls are
rejected. This verifies filter/output reconstruction on original inputs,
not complete native gameplay audio scheduling.

## Actual onset and handoffs

Both channels' first consumed bytes occur at call 3,821 and are zero. Their
logical and service times coincide; channel 1 follows channel 0 by
**2,790,400 raw units / 512 = 5,450 chip clocks**. The first nonzero byte is
7 on channel 0 at call 3,821 and on channel 1 at call 3,822, retaining exactly
the same logical gap. This directly observes the stereo phase difference;
it is not a fitted latency constant for native playback.

The first nonzero original Paula averages and filtered PCM occur at frames
**3,370,056** and **3,370,123**. The former averages are respectively 391 and
100. The **67-frame** output difference is already present before filtering.
The filter preserves the onset indices in this actual recording.

Every consecutive consumed byte uses **183,296 raw units / 512 = 358 chip
clocks**: 234,010 channel-0 gaps and 233,995 channel-1 gaps. This spans every
recorded startup buffer handoff without a sample-clock gap. Output averages
have distinct logical/service times, with service leads from 0 to 116,223
raw units. Consequently service timestamps alone are insufficient to
reconstruct output integration boundaries.

Native's simultaneous startup channel requests and different output cadence
remain unresolved. The newly verified logical timing and whole PCM are the
reference for fixing those owners. Neither faster native intro/loading nor
this filter result accepts the remaining native onset or whole waveform
differences. Flight/combat sound matching remains open.

## Connected reference path and bounded observation

The reference caller remains ordinary `retro_run` through original
`update_audio`, `anti_prehandler`, `samplexx_anti_handler` and `newsample`.
The final isolated DLL reads the original emitted averages directly. Its
SHA-256 remains
`18b8ba5e247f5808bc3e0cbaff589783e2adaaf6bcd5c8cb72af60f596814e47`.
No DLL rebuild or original source/object edit is needed for the bounded
window. The bridge enables the observer immediately before the selected
ordinary call, reads the actual current accumulators, and disables it after
the selected final call. All replay calls, PCM, sample and word telemetry
remain complete. Empty out-of-window mixer frames preserve complete call
framing. The snapshot explicitly declares the observed interval.

A real 94-call warm replay independently checks enable and disable boundaries
with calls 60–93 observed. All 97,686 ordinary mixer records in that interval
are identical to the earlier full-window observer; only the actual initial
accumulator snapshot is added. All 94 call/PCM boundaries, complete execution,
PCM, DMA, sample and word streams stay exact. Records outside the selected
window are empty. The checker rejects ranges that hide records or claim
unobserved PCM budgets. **41 audio protocol tests pass.** Default captures
continue to observe all calls.

The earlier `filter-response-cold/ready-filtered` event trace was rejected
because it lacks the established relocated voice-layout observer metadata.
The pinned, corrected `voice-layout/ready-capture` is the unmodified baseline;
its complete execution and event log match. The same newly captured recording
was checked against this baseline, without recapturing or relaxing equality.

No CPU, chipset, emulator clock or captured RAM is added to the playable
native runner. The unchanged native executable remains
`544bb244eb048b65436382a755cbf9bd9abd29f1337aec849967ef157fac3bd8`.
The linked Release filter test retains its previously validated SHA-256
`1581254e1c9e33a3ca3c96a89708e73af4154bc11648fad89553b7aa1e7aeb71`.

## Retention and reproduction

The [checkpoint](figures/native_original_audio_startup_mixer_checkpoint.json)
binds actual reports, tool identities, source/observer identities, compressed
retention and rejection controls. Passing raw captures are removed only after
decoded equality checks. Exact existing compressed storage is shared only
after both encoded and decoded equality. Rechecks use compressed artifacts.
The cache pruner remains enabled; canonical recordings and active flight
comparison inputs are preserved.

```powershell
$env:FA18_ENGINE_ROOT = (Resolve-Path build/native-audio/mixer-output-probe-engine).Path
# Decode the retained ready-initial/state.dat.gz into a temporary state.bin;
# its required SHA-256 is 221b8ddc9b010c59a4cf45f4714a3456a383221f4b31fb5eabf64e091f25b0e2.
python scripts/engine9000_bridge.py --frames 5000 --restore <temporary-state.bin> --playback build/native-audio/filter-response-cold/launch-ready.e9k --config build/native-audio/filter-response-cold/filtered.uae --output <new-capture-directory> --wav --audio-events --audio-samples --audio-mixer-call-range 3800 5000
python tools/native/check_original_audio_mixer.py --capture build/native-audio/mixer-startup-bounded --baseline build/native-audio/voice-layout/ready-capture --prior-samples build/native-audio/startup-consumed --reference-context startup --out build/native-audio/mixer-startup-bounded/recheck.json
python tools/native/check_original_mixer_pcm.py --capture build/native-audio/mixer-startup-bounded --timeline-report build/native-audio/mixer-startup-bounded/comparison.json --native-filter build/native-cmake/native/Release/fa18_native_pcm_filter_test.exe --out build/native-audio/mixer-startup-bounded/mixer-pcm-recheck.json
python -m unittest discover -s tools/native -p 'test_original_audio_*.py'
python scripts/prune_build_artifacts.py --quiet
```

Use a fresh capture directory and remove the temporary restore afterwards.
Unset `FA18_ENGINE_ROOT` to select the ordinary core again.
