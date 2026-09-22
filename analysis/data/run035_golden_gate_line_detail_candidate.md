# run035 Golden Gate line-detail selection candidate

Classification: **scenario-backed range/detail candidate; not proven LOD**.

The red Golden Gate landmark is measured only in the outside viewport. The
renderer then selects different static `$C355xx` line contexts as the observed
red raster grows during the recorded approach:

| sampled replay image | red landmark pixels | red bitmap bounds | collection arm frame | Golden Gate line context | captured lines | static line records |
| ---: | ---: | --- | ---: | --- | ---: | --- |
| 4,250 | 54 | `138..164,111..111` | 4,251 | `$C3559A`, `$C355D2` | 2 | `$C358B8` |
| 7,000 | 180 | `108..169,88..92` | 6,999 | `$C355CE` | 4 | `$C358A4,$C358A8,$C358AC,$C358B0` |
| 8,000 | 1,160 | `0..214,86..109` | 7,999 | `$C3558A` (17), `$C355BC` (1) | 18 | `$C35836..$C35876`, `$C357EA` |

The collections are armed one replay frame before their corresponding image
because the renderer work arrives at the subsequent debugger host frame. The
Golden Gate contexts return through `$C2F08E`, unlike the unrelated cockpit
emission caught in the off-by-one frame-8,001 collection.

This is positive evidence that the known Golden Gate line renderer selects
increasingly elaborate static line-record groups across this approach. It is
not yet an LOD claim: aircraft position, heading, roll, pitch, visibility, and
potential state changes all vary in the user recording. It also concerns line
detail only; no filled-face/terrain mesh replacement has been tied to the red
bridge pixels.

The contexts and their line-list records are static inline scene-control data
inside the verified segment-42 `$C35568-$C361FF` range. They must remain
control/topology evidence, not be decoded as a contiguous map-vertex table.

Authority: sealed `captures/run035`; red-pixel measurements in
`run035_golden_gate_red_viewport_interval.json`; and ignored replay-preserved
collector outputs `build/run035_red_{4251,6999,7999}_line_entries/`.

```text
python scripts/collect_blitter_line_entries.py \
  --restore captures/run035/initial_state.bin \
  --playback captures/run035/playback.e9k \
  --arm-frame 6999 --frames 6999 --max-lines 512 \
  --output build/run035_red_6999_line_entries
```
