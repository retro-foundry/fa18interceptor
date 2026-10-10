# Complete original sound fetch contents - 2026-10-10

Later [actual sample-consumption evidence](native_original_audio_sample_use_milestone.md)
classifies both zero words across the full recording: channel 1 emits both
bytes and channel 2 discards its prefetch. The channel-1 word's origin and
native sound timing/waveform remain open; the fetch evidence below is unchanged.

The complete original Demo now retains **2,939,342 actual DMA-fetched words**
across all **21,069 completed core frames**. Every fetched word equals its
retained original RAM bytes. The observer preserves complete original PCM,
RAM, serialized state, registers, video, and the earlier audio-event log.
It closes the prior fetch-content uncertainty for this recording; it does
not accept native sound onset, handoff timing, loop counts or the full waveform.

| Channel | Actual fetched words | Words matching current native assets | Uncatalogued words |
|---|---:|---:|---:|
| 0 | 1,396,972 | 1,396,972 | 0 |
| 1 | 1,391,226 | 1,391,225 | 1 |
| 2 | 30,655 | 30,654 | 1 |
| 3 | 120,489 | 120,489 | 0 |

All **2,939,340 catalogued fetched words** match sample bytes in the current
native executable's sealed mission-three input snapshot, iteration 22,142.
All native spans in the previous same-channel payload catalogue are unchanged
there. Original RAM is comparison data only and never initializes native
gameplay. The historical request trace remains explicitly identified by its
`f085a617...` executable; current asset bytes come from the `7ebadbd5...` runner.
This establishes current asset contents, not current-native request sequencing.

Two actual zero words remain outside the request catalogue: channel 1, call 94,
address `025A72`, beam 19/277; and channel 2, call 2,178, address `000000`, beam
21/64. Their exact beam positions are retained in the checkpoint. They are
not discarded or treated as accepted native handoffs. Their source context
still needs assessment during ordering/timing work. The subsequent
[startup check](native_original_audio_startup_fetch_milestone.md) classifies
the channel-2 word as a discarded original startup prefetch using unchanged
source functions. Channel 1 remains unresolved; both words stay retained.

## Connected capture and checks

The existing `scripts/engine9000_bridge.py` ordinary replay gains optional
`--audio-dma`, requiring `--wav --audio-events --restore`. It reads the pinned
core's existing completed DMA frame view in collect-only mode 6. The original
source path is `custom.c:dmal_func` -> `dmal_emu` -> `audio_getpt` -> `record_dma_read` ->
`chipmem_wget_indirect` -> `record_dma_read_value` -> `AUDxDAT_addr`. The view's
actual fetched value is exported, never reconstructed from final RAM.
No core patch, instruction stepping, intermediate serialization or additional
live-chipset register reads are introduced. The core remains SHA256
`5750be527458423407ec293c43cdf312cfd2248a3f06fcfb70c43561cb5549f1`.
This changes reference telemetry only; no native runtime dependency is removed.

The packed stream retains call/core-frame identity, PCM boundary, ordered slot,
address, fetched word, register/channel and beam coordinates. Every replay call
has a boundary, including frames with no audio fetch. Consecutive core frames,
the pinned 288-by-1,000 grid, all 88-byte raw ABI records, channel/register,
word size/address, slot ordering, terminal counts and complete hashes are
checked. Conflicting audio slots fail rather than silently losing an access.
NumPy reads the existing raw view using offsets from the pinned ctypes ABI;
the compact output is explicitly packed and independent of host padding.

PCM still covers all **18,582,858 stereo frames at 44.1 kHz**. The complete WAV
is byte-identical to the prior original recording, with SHA256
`db14f6340ca2464d6f2fd441b8f007bc2eb21d4eac7d39990befcb0d37816745`.
The previous event-log hash is identical. All original endpoint bytes and
registers remain exact against the nonrecording baseline.

The stream is 53,856,301 decoded bytes, SHA256
`4f9913ae1b6199455f7c4db01f168eb495a7f14392d5f791d4ead022eba1f77e`.
It is retained as 22,536,785 gzip bytes; original RAM/state are also verified
and compressed. Passing raw copies and the duplicate WAV are removed only
after exact equality. The validator again passes using compressed storage and
the explicit canonical WAV. The shared 512 MiB capture budget and 4 GiB build
pruner remain enabled.

The real 32-frame native CTest validates this caller alongside ordinary,
PCM-only and event-only original runs. It preserves complete state and rejects
damaged DMA coverage/source frames/PCM boundaries/registers, even with rewritten
stream checksums. Thirteen guard tests also cover conflicts, corrupt words,
changed native payloads, lost frames, duplicate slots, invalid fields, footer
truncation, budget failure and lossless compressed retention. Affected CTest
and artifact cleanup pass. No playable source or executable changes.

The [checkpoint](figures/native_original_audio_dma_checkpoint.json) binds the
complete recording, source authority, stream/report/tool identities, asset
witness, explicit uncatalogued words, tests and retention. Original-rule drawing
and visible qualification/mission-three performance remain accepted. Broader
flights, sound timing/waveform and visible combat/other missions/views remain
open; campaign continuity is waived and named-state cleanup is deferred.

## Reproduction

```powershell
python scripts/engine9000_bridge.py --frames 21069 --restore captures/uae/run075/restored-state.bin --playback captures/uae/run075/playback.e9k --config captures/uae/run075/config.uae --output build/native-audio/dma-complete-demo --wav --audio-events --audio-dma
python tools/native/check_audio_dma_payloads.py --capture build/native-audio/dma-complete-demo --baseline build/native-audio/original-complete-demo-baseline --pcm-reference build/native-audio/original-complete-demo/original.wav --catalog build/native-audio/voice-layout/complete-payload-catalog.json --native-delta build/native-flight/frame-delta-complete-flight/Release/frames.delta.gz --native-report build/native-flight/frame-delta-complete-flight/Release/report.json --out build/native-audio/dma-complete-demo/payload-witnesses-retained.json
python -m unittest discover -s tools/native -p test_original_audio_dma.py -v
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_original_audio_capture$' --output-on-failure
python scripts/prune_build_artifacts.py --quiet
```

The capture output must be a new directory. After verifying the newly generated
WAV, compress original state/RAM and the DMA stream losslessly and retain the
canonical WAV explicitly for subsequent checks.
