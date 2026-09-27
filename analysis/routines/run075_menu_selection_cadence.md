# run075 demo-selection timing and Copper-state cadence

Classification: **scenario timing evidence; not a native scheduler contract**.

The sealed `captures/run075` replay was sampled after every Engine9000
full-frame call with:

```text
python scripts/sample_replay_memory.py --restore captures/run075/initial_state.bin \
  --playback captures/run075/playback.e9k --config captures/run075/config.uae \
  --frame-offset 200 --frames 80 --word 0xC45AD6 --word 0xC458A6 --word 0xC1820C \
  --word 0xC1820E --sample-every 1 --sample-first 228 --sample-last 280 \
  --input-kind K --output build/port_run075_menu_tick_words_offset.json
```

`Engine.frame` advances in its VBlank callback, so this is a presentation
boundary, not an instruction-step trace. The byte-exact `$C0F5F8` tail in
`source_amiga/observed/run_post_input_tick_tail.asm` decrements `$C45AD6` by
one on each invocation. Its only static direct call in the complete parent
update is `$C0EFE4`, but the enclosing CPU-paced loop can execute more than
once between these sampled presentation boundaries.

## Measured run075 countdown

The release at frame 234 installs `$00D2`; no post-input tick occurs in that
sampled presentation. The subsequent observed decrement counts are:

| sampled frame(s) | decrements in each sampled presentation | total |
| --- | ---: | ---: |
| 235 | 1 | 1 |
| 236--240 | 5 | 25 |
| 241--270 | 6 | 180 |
| 271 | 5 | 5 |

The total is 211. Thus `$00D2` becomes `$FFFF` at the frame-271 sample and
takes `$C0FECE`'s negative-delay path. This is evidence
for this replay/configuration, not a general relation between native video
frames and game updates. A native frame-number table or inferred 5/6 pattern
would encode measured emulator pacing rather than source-owned behavior.

After the transition arm, the sampled delay is four at frame 272 and remains
four through frame 280. Therefore the later entry-delay countdown cannot be assumed
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

An offset-aware no-future-input trace begins at `$C0FEEA` in replay frame 271
(`build/port_run075_c0feea_c4fe38_writes_25k/`). It records the following
ordered writes to the first two `$C4FE38` entries:

| trace instruction | writer | entry result |
| ---: | --- | --- |
| 8 | `$C17B1A` (`$C17B08`, index 0) | entry 0 cleared |
| 2,235 | `$C17B1A` (`$C17B08`, index 1) | entry 1 cleared |
| 5,218 | `$C17B80` (`$C17B2C`, source 8, target 0) | entry 0 = `$C06A18` |
| 6,229 | `$C17B80` (`$C17B2C`, source 9, target 1) | entry 1 = `$C06A58` |

This proves an indexed-record handoff sequence, but not that either record is
itself a display page or when a native chunky buffer should be cleared.

## Port consequence

The current native C menu route deliberately stops at frame 272. It needs a
user-approved, target-legal timing contract before it can advance the
countdown to the proved negative-delay transition. Separately, a source-backed
semantic interpretation of the Copper-state handoff is needed before the
frame-273 blank presentation is represented in the chunky framebuffer.
