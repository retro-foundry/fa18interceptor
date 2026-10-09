# Complete original Demo retained audio payloads - 2026-10-09

Every recorded original sample-buffer request in the complete Demo now has a
same-channel native payload witness. The check covers **8,281 requests**, not
only the earlier 48 startup-music requests. The original pointer and word
length come from its actual Hunk 76 handler writes; resolving those ranges in
retained final RAM gives 12 distinct payloads. All occur on the same channels
among native's 17 payloads. Relocated addresses are never compared as identities.

| Channel | Original requests | Native active requests |
|---|---:|---:|
| 0 | 254 | 173 |
| 1 | 368 | 217 |
| 2 | 19 | 24 |
| 3 | 7,640 | 7,670 |

The real original caller remains `engine9000_bridge.py`'s ordinary Custom-write
observer and boundary voice reader. Its immutable loaded Hunk 76 and relocation
equations identify the sample handler. The validator requires complete original
PCM/chunk/write/voice coverage, exact original execution preservation, and a
request count equal to all actual handler length writes. Native's complete
observer trace must pass its header/footer and every boundary; every active
request hash is independently recomputed from its retained owned sample span.
The native final RAM hash must match its complete filter-integration report.

Wrong payload length, corrupted payload identity, reassigned channel and a
missing request are rejected. No recording is shifted, trimmed or searched for
a matching sequence, and no gain or clock is changed. The original 48-request
startup comparison still passes byte payloads, ordering, length, period and
volume. It keeps the original 67-sample right-channel onset difference reported.

## Meaning and limits

This is complete **retained-payload coverage**. Reading final RAM does not prove
that every byte remained immutable at each hardware fetch, and catalog identity
does not establish handoff order, loop count, pitch, level, onset or complete
waveform parity. Those checks remain open. The different initial-state, startup
and elapsed-time contexts of these recordings remain explicit; their request
counts are not treated as equivalent or used to assert an audio fault.

The native evidence retains the historical filtered executable identity
`f085a617509d218decd742dca0a2594e6498d0dbf0545045d9f152d380d95480`.
It is not relabelled as a fresh six-point-model capture. No playable source,
allocation or executable changes in this batch. Existing current-build
qualification/mission-three evidence and preallocation results remain intact.
Component sample ownership, the bounded startup request comparison and broader
recorded-sound acceptance are reported separately.

The [checkpoint](figures/native_complete_audio_payload_checkpoint.json) binds
original authority, complete 21,069-call PCM/event coverage, actual sample
addresses/hashes/counts, native trace/RAM identity, tool hash, negative controls
and startup regression. Reusable original RAM remains compressed; the checker
creates only reports. Full-flight drawing and visible performance remain open;
the uninterrupted campaign is waived and named-state cleanup stays deferred.

## Reproduction

```powershell
python tools/native/check_music_handoffs.py --original build/native-audio/voice-layout/complete-demo --baseline build/native-audio/original-complete-demo-baseline --native-trace build/native-audio/native-complete-demo-events/audio.jsonl.gz --native-report build/native-audio/native-filter-demo/report.json --native-data build/native-audio/native-filter-demo/final.dat.gz --pcm-reference build/native-audio/original-complete-demo/original.wav --complete-payload-catalog --out build/native-audio/voice-layout/complete-payload-catalog.json
python tools/native/check_music_handoffs.py --original build/native-audio/voice-layout/ready-capture --baseline build/native-audio/filter-response-cold/ready-filtered --native-trace build/native-audio/native-complete-demo-events/audio.jsonl.gz --native-report build/native-audio/native-filter-demo/report.json --pcm-reference build/native-audio/filter-response-cold/ready-filtered/original.wav --out build/native-audio/voice-layout/music-handoffs-catalog-regression.json
```
