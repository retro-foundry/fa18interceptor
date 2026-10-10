# Actual original audio endpoints — 2026-10-10

Later [complete sample-consumption evidence](native_original_audio_sample_use_milestone.md)
records both bytes of the channel-1 zero word and confirms no channel-2
prefetch bytes are emitted. The earlier endpoint/startup results below
remain unchanged; native sound timing and waveform remain open.

Two independently stopped original replays now retain actual serialized audio
state immediately before the calls containing the uncatalogued zero words.
Every PCM byte, chunk row, event row and actual DMA record in each stopped
replay equals the corresponding prefix of the complete Demo recording.
Channel 2 is idle before its startup word; channel 1 is already playing before
its zero word. The channel-2 startup classification passes using the actual
entry state. The channel-1 word and native sound timing remain open.

| Last replay call | Stereo PCM frames | Actual DMA words | Relevant actual endpoint |
|---|---:|---:|---|
| 93 | 82,026 | 18,455 | Channel 1 state 2; pointer `0259C6`; 88 remaining words |
| 2,177 | 1,920,114 | 318,527 | Channel 2 state 0; pointer, length and data zero |

The original `retro_unserialize` runs `m68k_go` until restoration completes,
with frontend/audio callbacks disabled during that work. The audio-event log's
initial hardware fields are decoded from the requested save payload; its
initial voice fields are read from RAM after that work. These fields describe
different contexts. Neither the requested payload nor a completed DMA frame
view should be substituted for the actual live replay endpoint.

The stopped replays use the unchanged original DLL, original configuration,
save state and complete input recording. Each run serializes only at its final
endpoint, after recording closes. No intermediate snapshot or stepping changes
the recorded output. The complete recording and all existing evidence remain
unchanged. The checks bind the source core/options, complete WAV and stream,
all prefix bytes and retained endpoint RAM/state hashes.

The source startup harness now requires the independently stopped call-2,177
prefix. It revalidates the prefix before using the actual idle channel state;
all 66,560 poison/attachment cases and five context controls still pass. The
call-93 state records a playing channel, so the startup rule does not classify
that zero. No restore-artifact or audible-loop explanation is asserted.

Seven prefix guards pass. They reject changed PCM, events and DMA words even
when local descriptors are rehashed, changed source authority, changed
serialized state and a missing PCM chunk. Lossless compressed retention and
removal of the duplicate WAV also pass. Original state/Chip/Slow RAM and compact
DMA are retained compressed. The duplicate WAVs are removed only after every
byte equals the complete recording's matching prefix. Both real checks pass
again from compressed storage using the explicit canonical complete WAV.
The normal capture budget and build pruner remain enabled.

This is reference evidence, not a native runtime change or complete native
sound acceptance. The [checkpoint](figures/native_original_audio_endpoint_checkpoint.json)
binds both real captures, their endpoint state, reports, tools and tests.

```powershell
python scripts/engine9000_bridge.py --frames 93 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/dma-before-loop-zero --wav --audio-events --audio-dma
python scripts/engine9000_bridge.py --frames 2177 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/dma-before-startup-zero --wav --audio-events --audio-dma
python tools/native/check_original_audio_prefix.py --prefix build/native-audio/dma-before-loop-zero --complete build/native-audio/dma-complete-demo --pcm-reference build/native-audio/original-complete-demo/original.wav --retain-compressed --out build/native-audio/dma-before-loop-zero/comparison.json
python tools/native/check_original_audio_prefix.py --prefix build/native-audio/dma-before-startup-zero --complete build/native-audio/dma-complete-demo --pcm-reference build/native-audio/original-complete-demo/original.wav --retain-compressed --out build/native-audio/dma-before-startup-zero/comparison.json
python -m unittest discover -s tools/native -p test_original_audio_prefix.py -v
python scripts/prune_build_artifacts.py --quiet
```

Capture output directories must be new. Subsequent checks omit
`--retain-compressed` to preserve the existing retention records.
