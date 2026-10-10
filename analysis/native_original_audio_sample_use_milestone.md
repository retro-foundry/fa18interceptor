# Complete original audio sample consumption - 2026-10-10

The complete original Demo now retains **5,878,371 actual sample bytes**
delivered to the original mixer across all **21,069 ordinary replay calls**.
Complete PCM, DMA, RAM, serialized state, registers, video and event logs
remain exact against the unmodified reference core. Both uncatalogued zero
fetches are classified: channel 1 emits both bytes during playback; channel 2
discards its startup word. Native onset, handoff timing and waveform acceptance
remain open. The origin of the extra channel-1 word is still unexplained.

| Channel | Observed sample bytes | Initial bytes with unknown address |
|---|---:|---:|
| 0 | 2,793,875 | 1 |
| 1 | 2,782,386 | 2 |
| 2 | 61,300 | 0 |
| 3 | 240,810 | 0 |

All **5,878,368 bytes with known provenance** match actual same-channel
DMA-fetched words. The first three bytes in the restored, already-running
pipeline have recorded values and words but explicitly unknown addresses
(`FFFFFFFF`). They are included in the trace and total; their provenance is
not inferred. The address/word lookup establishes contents, not DMA delivery
time. Byte ordering comes directly from the sample observer.

## Actual zero-word use

Channel 1 fetches `0000` at address `025A72`, call 94, beam 19/277. The
observer records its high and low bytes at sample indices 37,265 and 37,267,
states 2 and 3, beams 185/279 and 89/281. Both values are zero. Their service
cycles differ by 183,296 internal units, exactly **358 chip-clock units**.
This is actual playback, rather than the startup discard. Filtering and other
channels still contribute to PCM; these two bytes do not imply two zero PCM
output samples. Neither extra padding nor a restore-artifact explanation has
been added to native playback.

Channel 2's address-zero prefetch at call 2,178 contributes **no sample bytes**
throughout the complete trace. Its first emitted byte is `21` from word `21FD`
at `01A8C0`, sample index 637,016, beam 22/65. The actual fetch is at 21/65;
the one-chip-clock data handling agrees with the unchanged source startup
check and independently stopped idle endpoint.

## Connected observer and preservation

The real reference caller is ordinary `retro_run` through `update_audio`,
the channel state machine and `newsample`. An isolated copy of original
`audio.c` adds three observer hooks: incoming data-address provenance,
provenance when `dat` becomes `dat2`, and the actual byte after `newsample`.
It links against hashed original cached objects using the original build
commands. The original source, original objects and original reference DLL
remain untouched. No native runtime dependency is added or removed.

The reference DLL identities are deliberately distinct:

- Unmodified core: `5750be527458423407ec293c43cdf312cfd2248a3f06fcfb70c43561cb5549f1`.
- Isolated observer: `a4715a3cbe58e89c2f9de79aef7b0e194efb6dc50830305d54af1def81f2a0f7`.
- Unchanged native executable: `7ebadbd5cb2ac17df99751dda3dc3f804f57792d0762b0a32f7accac08a815a0`.

The observer preallocates 65,536 records of 20 bytes (1,310,720 bytes). The
largest ordinary call emits 834 records. Records drain only at existing replay
boundaries, without stepping or intermediate serialization. The bridge shares
the existing **512 MiB** diagnostic budget. Timestamps are original service
cycles/beam positions, not an independent reconstruction of logical mixer
integration time. Debugger boundaries use the source's 512-unit divisor; no
fitted clock offset is applied.

Real 93-call and 94-call pairs establish observer preservation before the
full run. The complete pair retains **18,582,858 stereo PCM frames at 44.1 kHz**,
with every byte and chunk equal. All snapshot fields are exact except the
declared observer DLL identity, observer descriptor and wall-clock duration.
The sample stream has 118,262,741 decoded bytes and SHA-256
`744ec20ad91f20bfef5eb4389566feccf96c5f434d33dcd7098514e492cda4a8`.
The complete WAV SHA-256 remains
`db14f6340ca2464d6f2fd441b8f007bc2eb21d4eac7d39990befcb0d37816745`.

Thirty audio guards and the real 32-call original-audio CTest pass. The full
comparison passes again from compressed WAV, RAM, DMA, samples and JSONL
storage. During cleanup the pruner removed the older raw canonical WAV;
the already-validated observer recording supplied identical bytes, retained
as the canonical `original.wav.gz`. Readers accept raw or losslessly compressed
references and check decoded hashes without resampling or normalization.
Passing raw duplicates are removed after verified equality. The 4 GiB cache
hooks remain enabled. A first short capture's debugger/internal-clock unit
error is retained as a failed diagnostic; the source-derived conversion fixes
it without changing original playback.

The [checkpoint](figures/native_original_audio_sample_use_checkpoint.json)
binds the complete report, retention manifest, observer build and tools.
This establishes original byte consumption, not native sound acceptance.

```powershell
python tools/native/build_original_audio_probe.py
$env:FA18_ENGINE_ROOT = (Resolve-Path build/native-audio/sample-probe-engine).Path
python scripts/engine9000_bridge.py --frames 21069 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/consumed-complete-demo-new --wav --audio-events --audio-dma --audio-samples
python tools/native/check_original_audio_samples.py --capture build/native-audio/consumed-complete-demo-new --baseline build/native-audio/dma-complete-demo --pcm-reference build/native-audio/original-complete-demo/original.wav --out build/native-audio/consumed-complete-demo-new/comparison.json
python -m unittest discover -s tools/native -p 'test_original_audio_*.py' -v
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_original_audio_capture$' --output-on-failure
python scripts/prune_build_artifacts.py --quiet
```

Use a new capture directory. Verify and compress reusable evidence before
running cache cleanup; unset `FA18_ENGINE_ROOT` to select the ordinary core
again. Existing retained evidence is under `build/native-audio/consumed-complete-demo`.
