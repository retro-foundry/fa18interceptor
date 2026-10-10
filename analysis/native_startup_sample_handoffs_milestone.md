# Independent startup sample handoffs — 2026-10-10

The independent original startup recording now has a complete consumed-byte
trace. All **468,007 bytes** follow the original published buffers, addresses,
high/low-byte order and periods exactly. All **48 original startup requests**
match the current native executable's ordered payloads, lengths, periods and
volumes. Original/native stereo onset and whole-waveform timing remain
unaccepted; no delay or clock adjustment is introduced to force equality.

This closes a context gap in the earlier audio evidence. The complete Demo
starts from restored, already-playing music, whereas this recording restores
the independently reproduced idle Workbench and launches the game through its
ordinary controls. Its pinned state, configuration, input and reference core
identities are the existing independent-startup authority. They are not
interchangeable with the sealed Demo authority.

The reused isolated observer records actual `newsample` bytes, cached word
addresses, states and service cycles. The real caller remains
`scripts/engine9000_bridge.py -> retro_run -> update_audio -> newsample`.
Every one of the **5,000 ordinary calls**, all **4,410,000 stereo PCM frames
at 44.1 kHz**, complete RAM/serialized state, registers, video, event rows and
PCM chunks preserve the unmodified reference. The observer DLL remains
`a4715a3cbe58e89c2f9de79aef7b0e194efb6dc50830305d54af1def81f2a0f7`;
the ordinary core remains
`5750be527458423407ec293c43cdf312cfd2248a3f06fcfb70c43561cb5549f1`.
No observer rebuild was needed.

| Observed startup consumption | Channel 0 | Channel 1 |
| --- | ---: | ---: |
| Consumed bytes | 234,011 | 233,996 |
| Complete buffers | 22 | 22 |
| Bytes consumed from the following buffer | 4,735 | 4,720 |
| Unknown initial addresses | 0 | 0 |
| Interval between every consecutive byte | 358 chip clocks | 358 chip clocks |

Channels two and three emit no sample bytes in this window. The checker
walks each published buffer from byte zero, checks every actual cached word
and byte against its retained payload, checks the exact expected word address
and high/low state, then advances only at that buffer's published length.
It performs no sequence search, trimming or fitted alignment. The 24 requests
per active channel include the queued buffer after the 23rd buffer that is
only partly played at the recording end; they are not all complete playback.

The first actual sample byte is zero on both channels. Channel zero begins
at call **3,821**, beam **18/286**; channel one begins at the same call,
beam **20/310**. Their service cycles differ by **5,450 chip-clock units**.
The same phase difference holds for **all 233,996 shared byte ordinals**,
including every completed buffer handoff. The first nonzero byte is seven
at sample-buffer address `01CA94`: channel zero at call 3,821, beam 88/292;
channel one at call 3,822, beam 90/3. These are observed source timestamps,
not a delay constant supplied to native playback.

The original filtered WAV becomes nonzero at stereo frames
**3,370,056 / 3,370,123**: right follows left by **67 output samples**.
Native starts both at output frame **19** at 48 kHz. Faster loading changes
absolute boot duration, but does not by itself assess this relative stereo
phase. Source sample periods and original service-cycle phase are now known
across the entire startup window. Ownership of the original request/interrupt
latency and the mapping to output intervals still require assessment before
accepting native onset or complete recorded waveforms. No fitted phase, gain,
resampling or reference clock enters the playable runtime.

An initial attempt to collect the startup DMA grid fails at call **4,294**:
the original debugger reports an audio slot conflict. The partial stream
retains 4,293 complete DMA calls; its 4,294 PCM chunks equal the canonical
recording's prefix, but no complete execution capture exists for that failed
attempt. It remains explicitly rejected and retained. Conflicting accesses
are not silently discarded or reconstructed from final RAM.

Consumed-byte recording now requires `--audio-events` and explicit selection
of the verified observer, independently of `--audio-dma`. The complete
startup byte trace therefore does not claim separate DMA fetch coverage.
The default sealed-Demo comparison still requires its complete paired DMA
evidence and retains its address-zero/extra-word checks. Its latest complete
report remains semantically identical after revalidation of all **21,069
calls and 5,878,371 bytes**. The new `--reference-context startup` is bound to
the existing pinned 5,000-call startup authority; using the Demo context for
this recording rejects before acceptance.

The current native runner is independently re-exercised over the complete
Demo: **12,680 boundaries, 8,248 requests, 554 stops**, all PCM, complete RAM
and every runtime counter exactly preserve the preceding native capture.
It reports zero gameplay heap violations and SDL pool failures. Its actual
caller remains `port/native/main.c -> native_audio_render -> service ->
request_voice_sample`; no playable dependency changes, and its executable
remains `36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.
This native preservation is separate from original waveform acceptance.

Four actual consumed-record controls reject changed word/byte contents,
word address, service interval and omission of the first byte at a real
buffer handoff. The existing four ordered-request controls also pass.
Thirty audio protocol guards, Python compilation and the real original-audio
CTest plus artifact cleanup pass. The full handoff comparison passes again
from compressed storage, with a report identical to the raw-capture assessment.

Original RAM, samples, events and the failed DMA window are retained
losslessly compressed. Verified identical PCM/RAM/trace files share existing
compressed storage; raw passing copies are removed. Cache cleanup removed an
older raw canonical chunk log; its already-verified identical compressed copy
restores the reference. The default 512 MiB capture and 4 GiB cache budgets
remain enabled. The [checkpoint](figures/native_startup_sample_handoffs_checkpoint.json)
binds actual source/native outcomes, guard results, failure scope, retention,
commands and artifact/helper identities.

```powershell
python tools/native/check_music_handoffs.py --original build/native-audio/voice-layout/ready-capture --baseline build/native-audio/filter-response-cold/ready-filtered --pcm-reference build/native-audio/filter-response-cold/ready-filtered/original.wav --native-trace build/native-audio/startup-current-native/audio.jsonl.gz --native-report build/native-audio/startup-current-native/report.json --consumed-samples build/native-audio/startup-consumed --out build/native-audio/startup-consumed/handoffs-retained.json
python -m unittest discover -s tools/native -p 'test_original_audio_*.py' -v
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_original_audio_capture$' --output-on-failure
```

Full-flight comparison acceptance and original/native sound timing remain
open. Campaign continuity remains waived; named-state cleanup stays outside
this goal.
