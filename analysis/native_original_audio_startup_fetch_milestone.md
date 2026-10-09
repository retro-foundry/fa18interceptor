# Original sound startup prefetch — 2026-10-10

The channel-2 zero word at call 2,178 is an original DMA startup prefetch.
The unchanged source state machine discards that word; it does not emit an
extra silent sample. This closes the classification of one of the two
uncatalogued words in the complete original Demo recording. Channel 1 at
call 94 remains unresolved. Native onset, handoff timing and complete recorded
waveform acceptance remain open.

The complete recording is revalidated first: all 21,069 original calls,
2,939,342 actual DMA words, complete PCM and unchanged original execution.
The initial retained channel-2 state is idle with a zero pointer. There is
no preceding channel-2 fetch, DMA enable or manual data write in the recorded
history. The actual first enable writes `8204` to DMACON at beam 8/63. The
published pointer is `01A8C0`, length 2,062 words, period 358 and volume 10.

| Call | Beam h/v | Address | Fetched word | Source treatment |
|---|---|---|---|---|
| 2,178 | 21/64 | `000000` | `0000` | Startup state 1 → 5; discarded |
| 2,178 | 21/65 | `01A8C0` | `21FD` | Next word loads the sample buffer |

`audio_getpt` returns the old pointer before resetting it to the published
pointer. Source state 1 acknowledges the first DMA word and enters state 5
without loading `dat2` or changing the current/previous sample. The next word
loads the buffer; in the harness's unmodulated playback case its bytes are
`21`, then `FD`. The source case-1 discard also holds for every attachment-bit
setting, independently of the unmodulated first-payload-byte check.

The reference harness compiles 16 functions and both channel structures
extracted verbatim from the pinned original `audio.c`. It uses the existing
compiler and checks the original cycle-unit, channel-count and DMA-master
constants. Nonzero current/previous-sample and `dat2` sentinels prove that the
first word is discarded rather than replaced with an emitted zero. All
65,536 possible priming words and all four channels × 256 attachment settings
pass: **66,560 cases**. Five record-context controls reject a playing initial
channel, lost old-pointer fetch, changed payload word, earlier DMA enable and
manual data before startup.

This harness checks ordered pointer/state/sample use. Its host-output adapter
is already active; interrupts are recorded without delivery. Unexpected
scheduler, logging and sample-ripper paths abort. It does not simulate the
whole chipset or establish beam-to-PCM timing. The original DLL and native
runtime are unchanged; this removes no playable runtime dependency. The zero
word stays in the complete DMA stream and outside the asset catalogue.

The [checkpoint](figures/native_original_audio_startup_fetch_checkpoint.json)
retains source, recording, stream, compiler, extracted-code, harness and report
identities, actual startup context and all control results. Temporary header
and executable files are removed automatically; no duplicate recording or
raw RAM is retained.

```powershell
python tools/native/check_original_audio_startup_fetch.py --capture build/native-audio/dma-complete-demo --baseline build/native-audio/original-complete-demo-baseline --pcm-reference build/native-audio/original-complete-demo/original.wav --out build/native-audio/dma-complete-demo/startup-fetch.json
python scripts/prune_build_artifacts.py --quiet
```
