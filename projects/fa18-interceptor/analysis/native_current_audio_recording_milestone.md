# Current playable audio recording - 2026-10-10

The current native executable (`7ebadbd5...`) independently starts from ADF
and reproduces all **12,680 audio boundaries, 8,248 requests and 554 stops**
of the complete native Demo. Every trace row, complete filtered PCM byte,
final game-data byte and runtime counter equals the previously accepted
filtered run (`f085a617...`). Both runs retain their own executable identities.
The 12,172,800 native stereo frames have PCM SHA-256
`3fb1c48d9602a48622a8237db7a7ea84cd2736b75c7e8e1edbe66a99e9ca9867`.

The actual caller remains `port/native/main.c` -> `native_audio_render` ->
the existing request/stop observer and PCM writer. No playable code, runtime
dependency, game clock or allocation behavior changes. The run reports zero
guarded gameplay heap violations and SDL pool requests/failures. Private
driver allocations are not measured by the headless capture.

The complete original Demo is revalidated from compressed references. All
**8,281 original requests** resolve to current native-owned sample payloads
on their actual channels; wrong length, bytes, channel and missing-request
controls reject. A separate original game launch also checks **all 48 initial
music requests** against the current native ordered startup requests, including
payloads, lengths, period and volume. Both onset WAV readers now accept exact
losslessly compressed files; the event reader streams compressed JSONL.

This current-build check preserves the earlier measured onset difference:
original right-channel onset follows left by **67 samples at 44.1 kHz**;
native stereo channels start together. The independently booted original
has its own elapsed boot context, while the full original Demo begins from a
restored mid-music state. Neither context is aligned or used to infer a fault
from differing full request counts. Full original/native handoff timing and
waveform acceptance remain open. The complete original byte-consumption trace
is [separate evidence](native_original_audio_sample_use_milestone.md).

The [checkpoint](figures/native_current_audio_recording_checkpoint.json) binds
the current command, reports, complete trace/RAM/PCM identities, original
authority, startup comparison and lossless retention. Reports and verified
compressed recordings are under `build/native-audio/current-native-complete-demo`.
Raw passing copies are removed after whole-file equality, and the existing
cache budget remains enabled.

```powershell
python tools/native/check_music_handoffs.py --original build/native-audio/dma-complete-demo --baseline build/native-audio/original-complete-demo-baseline --native-trace build/native-audio/current-native-complete-demo/audio.jsonl.gz --native-report build/native-audio/current-native-complete-demo/report.json --native-data build/native-audio/current-native-complete-demo/final.dat.gz --pcm-reference build/native-audio/original-complete-demo/original.wav --complete-payload-catalog --out build/native-audio/current-native-complete-demo/original-payload-comparison.json
python tools/native/check_music_handoffs.py --original build/native-audio/voice-layout/ready-capture --baseline build/native-audio/filter-response-cold/ready-filtered --pcm-reference build/native-audio/filter-response-cold/ready-filtered/original.wav --native-trace build/native-audio/current-native-complete-demo/audio.jsonl.gz --native-report build/native-audio/current-native-complete-demo/report.json --out build/native-audio/current-native-complete-demo/startup-handoffs.json
```
