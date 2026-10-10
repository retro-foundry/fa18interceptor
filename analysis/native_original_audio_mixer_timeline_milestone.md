# Original audio logical mixer timeline — 2026-10-10

The complete 94-call reference window now has a verified logical mixer
timeline. All **82,908 emitted Paula averages** and **37,311 consumed-byte
transitions** agree with **149,858 actual integration intervals** and the
initial four-channel accumulator state. All 270,078 records are covered.
The complete existing PCM, samples, word states, RAM, serialized state,
registers, events, DMA data and video remain unchanged.

This resolves the distinction between sample service timestamps and output
averaging timestamps in this warm window. It does not accept native sound
onset, whole-recording handoffs or complete waveforms.

The reference caller is `retro_run -> update_audio -> anti_prehandler ->
samplexx_anti_handler`, with `newsample` recording byte transitions. The
observer records the actual logical boundary `get_cycles() - n_cycles +
best_evtime` inside `update_audio`. It retains each prehandler's actual
integer duration and accumulator budgets. Emitted values are copied from
the original function's `datasp` after its calculation, before the caller
uses them. Its initialization snapshot reads the actual inherited areas
and times. Observation changes no source channel, clock or accumulator.

The logical clock uses raw original units, **512 per chip clock**. Every
observed interval agrees with the source's integer division by this unit.
Independent arithmetic carries the areas and budgets through all calls,
uses signed division toward zero at each output, and resets only when the
original average owner does. Each output call covers exactly its recorded
PCM-frame count. Every transition matches the existing byte observer's
service clock, channel and sample.

The measured clocks have different roles:

- Every channel-zero and channel-one consumed byte has equal logical and
  service timestamps in this window. All 18,655 / 18,654 consecutive gaps
  are exactly **358 chip clocks**.
- Emitted averages can precede their service timestamp by up to **116,217
  raw units**. Only one of the 82,908 average events coincides with service.
- There are **1,652 zero-duration prehandler intervals**. Source integer
  division discards 35,662,336 subcycle units across the observed intervals;
  these units do not contribute to its actual averaging budgets.

These are measurements of the reference scheduler and arithmetic. They are
not a native delay, gain, clock offset or sample alignment rule. The earlier
cold-start stereo phase evidence and complete native recordings keep their
own separate input contexts.

The optional observer occupies one fixed 65,536-record ring of 53-byte
records. It lives only in a separate reference DLL. The default sample,
word and restore observers generate their previous source unchanged.
Thirty-nine protocol tests pass. Seven mutations of real records reject a
changed initial area, duration, one-chip logical boundary, average, budget,
sample and missing transition. Python compilation and diff checks pass.

An initial diagnostic computed the observed average from the input
accumulators. The final observer reads the original emitted array directly
and independently preserves the same complete execution and mixer stream.
Only the final observer qualifies the emitted-value evidence. Its build
manifest records the original object hashes and exact compiler/link command.

The complete assessment passes again from compressed storage, with an
identical report. All ten compressed output artifacts share storage only
after encoded and decoded byte equality checks; their logical retained size
is **4,141,189 bytes**. Passing raw copies are removed and the normal cache
pruner runs. Native gameplay, allocations and executable remain unchanged.

The [checkpoint](figures/native_original_audio_mixer_timeline_checkpoint.json)
binds the actual timeline, paired execution preservation, final observer,
controls, tools and retained artifacts. Full native audio acceptance and
broader independent flight drawing remain open. Campaign continuity stays
waived; named-state migration stays outside this goal.

```powershell
python tools/native/build_original_audio_probe.py --word-state --restore-state --mixer-timeline --link-jobs 4 --out build/native-audio/mixer-output-probe-engine
$env:FA18_ENGINE_ROOT = (Resolve-Path build/native-audio/mixer-output-probe-engine).Path
python scripts/engine9000_bridge.py --frames 94 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/mixer-output-loop-zero-new --wav --audio-events --audio-dma --audio-samples
Remove-Item Env:FA18_ENGINE_ROOT
python tools/native/check_original_audio_mixer.py --capture build/native-audio/mixer-output-loop-zero --baseline build/native-audio/word-state-loop-zero-baseline --prior-samples build/native-audio/restore-state-loop-zero --dma-reference build/native-audio/dma-complete-demo --out build/native-audio/mixer-output-loop-zero/recheck.json
```
