# Original voice ownership and first music handoffs - 2026-10-09

The original audio trace now resolves its actual loaded voice owner instead of
assuming the sealed recording's CPU addresses. A separate original launch from
a retained Workbench state loads sound Hunks 74/75/76 at a different location.
The earlier fixed-address trace reported empty voices despite audible music;
those voice rows are rejected. Its complete PCM, register writes and execution
remain valid reference evidence, independently reproduced without changes.

## Authority and connected reference caller

The original executable is `local/extracted/f18_interceptor`, identified by
`analysis/hunk_inventory.json`. The reader verifies the 420-byte original Hunk
76 against its disk hash
`ee2e47870bb7d698441f797a2a6bc38ede72e3e6f88d39eadd3fd0ee19ffd01c`.
Every non-relocated byte and every relocation equation must agree. References
to Hunks 74, 75 and itself identify the actual voice table and master volume.
Raw file-buffer copies, inconsistent relocations and ambiguous loaded matches
are not accepted as loaded code.

The real reference caller is `scripts/engine9000_bridge.py` -> the existing
original Custom-write callback -> `VoiceReader.observe_write`, then the normal
per-frame boundary -> `VoiceReader.voices`. It scans ordinary RAM once at
restore and can resolve newly loaded code at actual original handler writes.
There are no live chipset reads, extra mid-run serializations, breakpoint
retiming or source-code changes. Until ownership is resolved, voice records
and master volume are explicitly unknown, rather than falsely empty/zero.
The existing 512 MiB recording budget still bounds the complete PCM and trace.

The sealed game uses Hunk 74 at `C4FE28`, slots `C4FE38`, master `C4FF26` and
Hunk 76 at `C500D8`. The separate launch uses Hunk 74 at `C50008`, slots
`C50018`, master `C50106` and Hunk 76 at `C502B8`: a 480-byte relocation.
The validator independently resolves retained final RAM, checks every reported
layout and rejects an unknown owner after an observed actual handler write.
Legacy voice traces are accepted only when their fixed addresses match the
verified loaded owner. The real 32-frame test rejects wrong, missing and
falsely unknown ownership even with updated trace checksums.

## Complete reference preservation

The complete sealed Demo replay passes all 21,069 ordinary calls, 18,582,858
stereo PCM frames and 406,754 trace rows. Every preceding Custom write, LED
notification and complete voice record remains identical after removing the
new layout annotation. All registers, both complete RAM banks, serialized
state, video, audio counters and every WAV byte remain unchanged. All 21,069
voice boundaries have verified ownership. The original canonical WAV remains
the exact recording with SHA-256
`db14f6340ca2464d6f2fd441b8f007bc2eb21d4eac7d39990befcb0d37816745`.

Regenerating the Workbench state from the independent cold boot reproduces
its exact SHA-256
`221b8ddc9b010c59a4cf45f4714a3456a383221f4b31fb5eabf64e091f25b0e2`.
The separate 5,000-call launch then preserves every prior PCM byte, register,
RAM/state byte and video result. Every register write and LED notification is
also unchanged. The first verified voice boundary is call 3,821; there are
1,180 known and 3,820 explicitly unresolved boundaries. This is a discovery
boundary, not a claimed code-load timestamp. Its earlier fixed-address voice
rows are explicitly rejected; full-wave preservation does not validate them.

## Initial music requests and remaining timing differences

`tools/native/check_music_handoffs.py` compares all 48 actual original music
requests in that separate launch, 24 per stereo channel, against native's
ordered startup requests. Relocated pointers are compared through the actual
sample bytes. Every payload hash, word-truncated length, period 358 and volume
63 agrees, including initial priming, repetitions and chain handoffs. The
request count agrees with every actual handler length write; both complete
traces and original PCM blocks are validated first. Wrong payloads, lengths,
periods and volumes are rejected. No searched subsequence, WAV shift, trimming,
gain conversion or adjusted game clock is used to obtain this result.

Native refill times obey its rational source-clock accumulation; rounding is
3.185..70.284 PAL chip cycles, below one 48 kHz output interval. Original beam
timestamps are reported separately. A 313-line interpretation gives interval
residuals -145..196 cycles on channel 0 and -38..58 on channel 1; 312/314-line
interpretations diverge by up to roughly 37,900 cycles. This is diagnostic
inference, not a new timing acceptance. The reference source's
`retrodep/sounddep/sound.c:update_sound` derives PCM intervals from the clock
supplied by `custom.c:compute_vsynctime`, which includes its actual scan
lengths and frontend refresh policy. Native retains the original PAL constant
3546895. Reference clock/latency effects remain to be assessed.

The original WAV first becomes nonzero at stereo frames 3,370,056/3,370,123,
at 44.1 kHz: its right channel follows the left by 67 frames, about 1.519 ms.
Native starts both at frame 19 at 48 kHz. Original initial/priming requests
also have distinct beam times; native primes at the same output-frame index.
These differences remain measured and unresolved. The accepted faster native
intro does not by itself establish exact audio onset or stereo phase parity.
This batch accepts the sample/level request comparison, not identical complete
sound recordings or all flight/combat handoffs.

## Retention and scope

The compact checkpoint is
[native_original_voice_layout_checkpoint.json](figures/native_original_voice_layout_checkpoint.json).
Reports and reusable compressed original RAM remain under
`build/native-audio/voice-layout`. Passing duplicate WAVs were removed only
after whole-file identity checks; validators continue to check every block
against explicitly supplied retained originals. The misleading older separate
launch trace has a rejection note; sealed recordings and media are untouched.

The active Release original-audio CTest and artifact cleanup pass. Native
Release was rebuilt after the test prerequisite update and remains byte-for-
byte identical to the preceding validated executable, SHA-256
`f085a617509d218decd742dca0a2594e6498d0dbf0545045d9f152d380d95480`.
This reference-host correction adds no allocation, CPU/chipset dependency,
timing or audible change to playable native gameplay. Whole-flight sound,
remaining drawing and visible performance acceptance stay open. State cleanup
remains deferred and the uninterrupted campaign requirement remains waived.
