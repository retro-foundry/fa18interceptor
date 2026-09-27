# run075 demo-selection timing and Copper-state cadence

Classification: **scenario timing evidence; not a native scheduler contract**.

The sealed `captures/run075` replay was sampled after every Engine9000
full-frame call with:

```text
python scripts/sample_replay_memory.py --restore captures/run075/initial_state.bin \
  --playback captures/run075/playback.e9k --config captures/run075/config.uae \
  --frames 280 --word 0xC45AD6 --word 0xC458A6 --word 0xC1820C \
  --word 0xC1820E --sample-every 1 --sample-first 228 --sample-last 280 \
  --input-kind K --output build/port_run075_menu_tick_words.json
```

`Engine.frame` advances in its VBlank callback, so this is a presentation
boundary, not an instruction-step trace. The byte-exact `$C0F5F8` tail in
`source_amiga/observed/run_post_input_tick_tail.asm` decrements `$C45AD6` by
one on each invocation. Its only static direct call in the complete parent
update is `$C0EFE4`, but the enclosing CPU-paced loop can execute more than
once between these sampled presentation boundaries.

## Measured run075 countdown

The release at frame 234 installs `$00D2`; the same frame's post-input tail
leaves `$00D1`. The subsequent observed decrement counts are:

| sampled frame(s) | decrements in each sampled presentation | total |
| --- | ---: | ---: |
| 235--239 | 5 | 25 |
| 240--259 | 6 | 120 |
| 260 | 5 | 5 |
| 261--270 | 6 | 60 |

The total is 210. Thus the post-release `$00D1` becomes `$FFFF` at the
frame-270 sample and takes `$C0FECE`'s negative-delay path. This is evidence
for this replay/configuration, not a general relation between native video
frames and game updates. A native frame-number table or inferred 5/6 pattern
would encode measured emulator pacing rather than source-owned behavior.

After the transition arm, the sampled delay is four and remains four through
frames 272--280. Therefore the later entry-delay countdown cannot be assumed
to run once per presentation either.

## Observed indexed Copper-state handoff

Full replay snapshots (`build/port_frame269_replay/` through
`build/port_frame273_replay/`) give this state for the first two entries of
the `$C4FE38` indexed pointer table:

| snapshot frame | entry 0 | entry 1 |
| ---: | --- | --- |
| 269--270 | `$C07678` | `$C076B8` |
| 271 | `0` | `$C076B8` |
| 272--273 | `$C06A18` | `$C06A58` |

The frame-271 zero is consistent with the direct `$C17B08` clear of entry 0,
followed by its `$C4FFB0` `COPJMP2` strobe (`$8080`). The former menu label is
still visible through frame 272 and the blank page first presents at frame
273. The pointers are mutable runtime records; the current evidence does not
establish their record type, their page ownership, or a native double/triple
buffer mapping. They must not be copied as address-based native state.

## Port consequence

The current native C menu route deliberately stops at frame 272. It needs a
user-approved, target-legal timing contract before it can advance the
countdown to the proved negative-delay transition. Separately, a source-backed
semantic interpretation of the Copper-state handoff is needed before the
frame-273 blank presentation is represented in the chunky framebuffer.
