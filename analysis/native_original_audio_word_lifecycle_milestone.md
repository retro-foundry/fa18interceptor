# Original warm audio word lifecycle — 2026-10-10

The channel-1 zero word is now traced through actual pointer selection, DMA
delivery, counter reload and sample consumption. A new 94-call original pair
preserves every PCM byte, event, DMA record, register, RAM/state byte and video
byte. **18,656 running word lifecycles** pass. This explains the observed loop
boundary; the origin of the initial pointer/count relationship and complete
native sound acceptance remain open.

The real reference caller is ordinary `retro_run` through original audio DMA,
`audio_getpt`, `AUDxDAT_addr` and `event_audxdat_func` in
`tools/engine9000-src/ami9000/sources/src/audio.c`. The separate observer DLL
adds read-only hooks for pointer selection, incoming data, completed counter
handling and length-register writes. It also retains the existing consumed-byte
observer. No native runtime dependency is added or removed, and no original
source, cached original object or reference DLL is replaced.

| Evidence | Complete coverage |
|---|---:|
| Ordinary replay calls | 94 |
| Stereo PCM frames at 44.1 kHz | 82,908 |
| Consumed bytes | 37,311 |
| Live pointer reads / arrivals / counter events | 18,656 each |
| Length-register writes | 2 |
| Word-state records | 55,970 |
| Largest ordinary-call record count | 599 |
| Damaged-real-stream rejection controls | 17 |

## Actual boundary

Channel 1's final read occurs at call 94, beam 19/277. Immediately before
selection its pointer is `025A72` and remaining count is one. Original
`audio_getpt` returns that address and resets the pointer to `01DA70`.
`AUDxDAT_addr` receives word `0000` with the count still one.
`event_audxdat_func` reloads the count to `4001` one chip clock later, beam
20/277. The next actual pointer read is `01DA70`.

Both zero bytes reach the original mixer, exactly as in the retained complete
Demo: sample indices 37,265 and 37,267, beams 185/279 and 89/281, states 2 and
3. Their service cycles differ by 358 chip clocks. The filter and other
channels still contribute to the stereo output.

The first live read already determines the boundary. Every pointer read up to
the first reset preserves `pointer + 2*(remaining - 1)`:

| Channel | First pointer | First remaining count | Reads through reset | Final read |
|---|---|---:|---:|---|
| 0 | `0211BC` | 9,307 | 9,307 | `025A70` |
| 1 | `0211AC` | 9,316 | 9,316 | `025A72` |

Both declare start `01DA70` and length `4001`, making `025A70` the last
declared word. Channel 1's first observed pointer/count relationship therefore
already projects one word beyond that range. There is no newly invented
length rule during these 94 calls. How this relationship arose before the
first ordinary replay call is still unverified. The requested restore payload
and the independently stopped call-93 endpoint remain different contexts;
neither is substituted for these live transitions. This evidence does not
establish a restore artifact or a cold-start padding rule.

## Preservation and retention

The unmodified new 94-call capture also equals the matching prefix of the
retained complete original Demo. The observer's paired stopped DMA stream has
18,654 grid records and remains byte exact. Actual live word hooks record
18,656 reads, including reads outside that completed grid. As in earlier
sample checks, an explicit pinned complete DMA recording supplies same-channel
address/word contents for buffered words outside the stopped grid. That lookup
does not infer their delivery time. All three initial sample bytes of unknown
address provenance remain explicitly `FFFFFFFF`.

The isolated word ring is preallocated: 65,536 records of 38 bytes, or
2,490,368 bytes. It drains only at existing ordinary call boundaries and shares
the existing 512 MiB capture budget. Native gameplay and its allocation
behavior are unchanged. Both sample-footer and word-footer budget failures
disable and close both observers.

The 17 controls mutate the real stream, recomputing local binary hashes.
They reject truncated/trailing data, lost calls, incorrect footer counts,
changed PCM boundaries, invalid channel/beam/kind/flags, changed pointer reset,
destination, delivered address, counter reload/state, zero word and missing
pointer/counter events. The original assessment remains exact. The 12 sample,
seven prefix and 13 DMA unit checks pass, along with the real 32-call original
audio preservation check. All new checks pass again from compressed storage.

The premature first launch failed with WinError 193 while the separate DLL
was still linking, before game initialization. That failure is retained.
Attempts using a pruned old baseline or only the stopped DMA content grid
remain recorded as rejected; neither becomes accepted evidence. A new ordinary
unmodified baseline and explicit complete fetch witness resolve those evidence
requirements. Passing RAM, PCM, DMA, samples, word records and logs are retained
losslessly compressed; raw copies are removed and the normal pruner runs.

Identities:

- Original reference DLL: `5750be527458423407ec293c43cdf312cfd2248a3f06fcfb70c43561cb5549f1`.
- Existing sample observer, preserved: `a4715a3cbe58e89c2f9de79aef7b0e194efb6dc50830305d54af1def81f2a0f7`.
- Separate word observer: `978f2e54205f695803018829724e3a25e43e3b8a1646dc1d4717ca2dae89a4ac`.
- Unchanged native executable: `148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.

The [checkpoint](figures/native_original_audio_word_lifecycle_checkpoint.json)
binds source/build identities, reports, tools, failures and compressed retention.
Native onset, handoff timing and whole recorded filter output remain open;
no measured delay, waveform offset or padding is added to the native game.

```powershell
python tools/native/build_original_audio_probe.py --word-state --out build/native-audio/word-state-probe-engine
$env:FA18_ENGINE_ROOT = (Resolve-Path build/native-audio/word-state-probe-engine).Path
python scripts/engine9000_bridge.py --frames 94 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/word-state-loop-zero-complete-new --wav --audio-events --audio-dma --audio-samples
Remove-Item Env:FA18_ENGINE_ROOT
python scripts/engine9000_bridge.py --frames 94 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/word-state-loop-zero-baseline-new --wav --audio-events --audio-dma
python tools/native/check_original_audio_word_states.py --capture build/native-audio/word-state-loop-zero-complete --baseline build/native-audio/word-state-loop-zero-baseline --dma-reference build/native-audio/dma-complete-demo --out build/native-audio/word-state-loop-zero-complete/comparison.json
python tools/native/test_original_audio_word_states.py --capture build/native-audio/word-state-loop-zero-complete --out build/native-audio/word-state-loop-zero-complete/guards.json
```

Capture directories must be new; the last two commands revalidate the existing
compressed evidence. For new captures, select their paths explicitly and verify
and compress them before cache cleanup.
