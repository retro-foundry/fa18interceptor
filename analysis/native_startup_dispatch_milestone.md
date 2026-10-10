# Original startup sound dispatch — 2026-10-10

The original startup stereo phase is now traced to its actual empty/start/prime
interrupt sequence. Of the observed **5,450 chip-clock** right-channel delay,
**5,438** are already present at DMA enable; the following channel fetch adds
**12**. These are measurements from one recorded hardware frame, not a delay
constant or a CPU cycle model for native playback. Exact original/native sound
onset and whole-waveform acceptance remain open.

The source authority is the independently started 5,000-call recording from
the [startup handoff evidence](native_startup_sample_handoffs_milestone.md).
Its loaded hunk 76 starts at `C502B8`, relocated from canonical `C500D8`.
The empty-voice branch at `C50314` silences the channel, sets period 124,
disables DMA, then executes `MOVE.W #400,D0; SUBI.W #1,D0; BNE` before
returning. The checker verifies the actual retained instruction bytes. The
earlier hunk manifest does not contain this loaded sound hunk and is not used
to substitute instructions.

All events below occur in original call/hardware frame **3,821**. Beam
positions are horizontal/vertical. The checker verifies 20 writes per channel,
including interrupt requests and acknowledgements, silence, minimum period,
DMA stop, initial pointer/length/period/volume and the priming publication.

| Event | Channel 0 | Channel 1 |
| --- | --- | --- |
| Empty request | 152/264 | 182/288 |
| DMA disabled | 187/265 | 198/289 |
| Start request | 117/283 | 126/307 |
| Start acknowledged | 88/284 | 78/308 |
| DMA enabled | 168/284 | 158/308 |
| First sample byte | 18/286 | 20/310 |

Both initial buffers are at `01CA90`, contain 2,044 bytes and use period 358
with volume 63. Their priming requests preserve the same payload. Same-frame
beam subtraction uses `vertical * 227 + horizontal`; it needs no assumption
about lines per frame or absolute boot duration. DMA enable to first sample
is 304 clocks on channel zero and 316 on channel one. The resulting 5,450-clock
phase agrees with **all 233,996 shared consumed-byte ordinals** across the
complete startup window.

DMA stop to the next software start request takes 4,016 / 4,014 clocks. These
spans include the wait, return, caller, interrupt and bus work; they do not
measure the loop in isolation. The source confirms a wait after disabling
DMA, but does not justify converting either recorded span into a portable
native sleep or a fixed stereo offset.

The actual native path remains `main -> native_frontend_tick -> play_sound
-> free_voice/clear_voice_interrupt -> native_audio_request_channel`, followed
by `main -> native_audio_render -> service -> request_voice_sample`.
An empty slot immediately stops its native stream. Both pending startup
channels are serviced at output frame zero, as the retained current-runner
trace confirms. Native has no DMA engine or interrupt dispatcher. No gameplay,
sample, filter, host clock or scheduling behavior changes in this batch.

The complete handoff reassessment preserves every preceding report field
exactly and adds dispatch evidence. Four mutations reject changed wait count,
interrupt source, DMA value and beam order. All 30 existing audio protocol
guards, Python compilation and the diff check pass. Compressed reference
storage is reused; no new emulator capture or raw RAM is retained.
The [checkpoint](figures/native_startup_dispatch_checkpoint.json) binds the
source bytes, event sequence, phase, helper and retained artifact hashes.

```powershell
python tools/native/check_music_handoffs.py --original build/native-audio/voice-layout/ready-capture --baseline build/native-audio/filter-response-cold/ready-filtered --pcm-reference build/native-audio/filter-response-cold/ready-filtered/original.wav --native-trace build/native-audio/startup-current-native/audio.jsonl.gz --native-report build/native-audio/startup-current-native/report.json --consumed-samples build/native-audio/startup-consumed --out build/native-audio/startup-consumed/dispatch-handoffs.json
```

Full-flight comparison acceptance and original/native sound timing remain
open. Campaign continuity is waived; named-state cleanup remains separate.
