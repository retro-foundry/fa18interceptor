# Original warm audio restore state — 2026-10-10

The original channel-1 extra word is already encoded in the requested warm
save. A separate observer now verifies all four actual `restore_audio`
assignments and reads the live state before the first ordinary replay call.
The subsequent complete 94-call PCM, DMA, sample, word, RAM and video outputs
remain exact. This resolves the relationship between the requested save and
the first observed word budget; it does not establish a cold sample-padding
rule or accept native sound onset and complete waveforms.

The reference caller is `retro_unserialize -> restore_audio`, followed by the
bridge's observer initialization and ordinary `retro_run` calls. The new hook
reads each channel immediately before the original restore function returns.
An exported getter then copies the four live channel states without invoking
audio processing, serialization or an emulator step. Its two sets of four
39-byte records occupy 312 preallocated bytes. The isolated DLL preserves the
original source, link objects and existing sample/word observer DLLs.
Nothing is linked into or changed in the playable native runner.

The requested state contains length `4001` words and base `01DA70` on both
music channels. The last declared word is therefore `025A70`.

| Channel | Actual restored pointer | Restored remaining | Pre-replay pointer | Pre-replay remaining | Projected last word |
|---|---|---:|---|---:|---|
| 0 | `0210F4` | 9,407 | `0211BC` | 9,307 | `025A70` |
| 1 | `0210E4` | 9,416 | `0211AC` | 9,316 | `025A72` |

Every recorded restore field agrees with its requested value: pointer, base,
length, count, period, data, pipeline word, state, volume, interrupt flag,
request flags, event time and horizontal request position. The pointer/count
relationship survives a 100-word address advance on each channel before the
observer is enabled. The first actual pointer reads agree with these live
pre-replay budgets. Execution between the two snapshots was not observed;
this batch does not claim a trace of those intervening transfers.

The existing [word lifecycle evidence](native_original_audio_word_lifecycle_milestone.md)
remains authoritative for the actual subsequent 18,656 word lifecycles and
channel-1 zero at call 94. Its entire word descriptor and assessment remain
unchanged and are bound to the sealed checkpoint. The new capture preserves
all 82,908 stereo PCM frames, 37,311 consumed bytes, all events and DMA data,
complete RAM/state and the final video. The preexisting assessment also
revalidates unchanged in every field with the extended tooling.

Six mutations of the actual snapshots reject changed restored/live pointers
and counts, advanced snapshot time and a swapped phase. Thirteen sample
contract tests pass, including complete snapshot framing, phase/channel order
and flag validation. Python compilation and the diff check pass. The new
comparison passes again from compressed storage. All nine compressed PCM,
RAM, DMA, sample, word and log artifacts share the earlier exact storage after
both encoded and decoded byte equality checks; raw passing copies are removed.
The normal 4 GiB pruner runs without disabling its protections.

The [checkpoint](figures/native_original_audio_restore_state_checkpoint.json)
binds the actual snapshots, complete preserved execution, observer build,
tools and retained artifacts. The unchanged native executable remains
`544bb244eb048b65436382a755cbf9bd9abd29f1337aec849967ef157fac3bd8`.

```powershell
python tools/native/build_original_audio_probe.py --word-state --restore-state --out build/native-audio/restore-state-probe-engine
$env:FA18_ENGINE_ROOT = (Resolve-Path build/native-audio/restore-state-probe-engine).Path
python scripts/engine9000_bridge.py --frames 94 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/restore-state-loop-zero-new --wav --audio-events --audio-dma --audio-samples
Remove-Item Env:FA18_ENGINE_ROOT
python tools/native/check_original_audio_restore_state.py --capture build/native-audio/restore-state-loop-zero --baseline build/native-audio/word-state-loop-zero-baseline --prior-words build/native-audio/word-state-loop-zero-complete --dma-reference build/native-audio/dma-complete-demo --restore captures/uae/run075/restored-state.bin --out build/native-audio/restore-state-loop-zero/comparison.json
```

Select a new output for a new capture. The last command revalidates the retained
capture. Complete native sound acceptance and broader independent flight
drawing histories remain open. Campaign continuity is waived, and named-state
cleanup remains outside this goal.
