# run035 Golden Gate red-pixel line correlation

Classification: **landmark-to-renderer spatial correlation**.

At the first sampled red-visible point (replay frame 4,250), the outside
viewport landmark is RGB `#880000`, bitplane index 1, and has bitmap bounds
`x=138..164`, `y=111`. The replay-preserved collector armed immediately after
that point catches these `$C2FA7E` line-emitter inputs:

| capture order | static `A5` context | input endpoints `(x0,y0)->(x1,y1)` | low four plane bits | return |
| ---: | --- | --- | ---: | --- |
| 0 | `$C3559A` | `(164,110)->(152,110)` | `$F` | `$C2F08E` |
| 1 | `$C355D2` | `(138,111)->(153,110)` | `$F` | `$C2F08E` |
| 2 | `$FFFFED` | `(151,112)->(169,113)` | `$F` | `$C302BA` |

The first two inputs jointly have bounds `x=138..164`, `y=110..111`; that is
the same horizontal span as the red landmark and directly reaches its sole
bitmap row. Their contexts are the existing `$C3559A/$C355D2` Golden Gate
line family, using static list `$C358B2` in the prior run033 proof. The third
line has a different call/return context and is not included in the bridge
correlation.

This corrects the earlier mixed-scene limitation: run035 now identifies the
red Golden Gate *line* family, rather than merely showing unrelated active
bridge contexts. It still does not assign every co-visible polygon/face to
Golden Gate, export immutable source vertices, or demonstrate LOD. The
emitter's `active_line_plane_mask` was `$FF` for the two bridge submissions
(low four bits `$F`); its write operation must not be simplified to a direct
one-plane colour assignment solely because the final landmark has index 1.

The collector resumes through a full host frame after each instruction step,
so its `host_frame` values 4,252 and 4,253 are debugger-stepped collection
timing, not a claim that the screenshot at replay frame 4,250 was drawn by a
single uninterrupted invocation. The static context plus exact screen-space
overlap are the evidence used here.

Authority: sealed `captures/run035`; `build/run035_red_4250/chip.bin`; the
landmark measurement in `run035_golden_gate_red_viewport_interval.json`; and
ignored collector output
`build/run035_red_4251_line_entries/blitter_line_entries.json`.

```text
python scripts/collect_blitter_line_entries.py \
  --restore captures/run035/initial_state.bin \
  --playback captures/run035/playback.e9k \
  --arm-frame 4251 --frames 4251 --max-lines 512 \
  --output build/run035_red_4251_line_entries
```
