# run035 Golden Gate red-viewport interval

Classification: **visual landmark measurement**.

The user identifies the Golden Gate bridge by its red pixels in the outside
world viewport. This measurement searches only rectangle `x=40..679`,
`y=18..159`, deliberately excluding the cockpit and HUD. It finds exact RGB
`#880000` (`136,0,0`) in 17 of 31 replay keyframes scanned every 250 frames.

| replay frame | red pixels | bounding rectangle, inclusive |
| ---: | ---: | --- |
| 4,000 | 0 | -- |
| 4,250 | 54 | `316,127`--`369,127` |
| 5,500 | 68 | `296,107`--`373,107` |
| 7,000 | 180 | `256,104`--`379,108` |
| 8,000 | 1,160 | `40,102`--`469,125` |
| 8,250 | 2,730 | `40,113`--`583,151` |
| 8,500 | 0 | -- |

Thus the replay supplies a bounded Golden Gate landmark interval from sampled
frame 4,250 through 8,250. Its absence at 8,500 is consistent with the
recorded turn-away, but the 250-frame sampling does not locate the precise
first or last visible frame.

## Display encoding at frame 4,250

The matching 54 screen pixels decode through the active Copper bitplanes as
index 1 only. The measured screen-to-bitmap relation is
`bitmap_x=(screen_x-40)/2`, `bitmap_y=screen_y-16`; all red pixels therefore
select `COLOR01=$0800`, the RGB4 palette value rendered by the host as
`#880000`. This establishes the relevant visible plane/index and excludes the
red cockpit instrument pixels from the measurement.

The scan measures raster output only. It does **not** identify a particular
face record, terrain/control-stream record, bridge component, or LOD level.
Those require a renderer submission correlated to this world-viewport region.

Authority: sealed `captures/run035`, ignored reproduction images in
`build/run035_viewport_scan/`, the command below, and the machine-readable
measurements in `run035_golden_gate_red_viewport_interval.json`.

```text
python scripts/analyze_viewport_landmark.py \
  --images build/run035_viewport_scan --colour 880000 \
  --left 40 --top 18 --right 680 --bottom 160 \
  --chip build/run035_red_4250/chip.bin --chip-frame 4250 \
  --bitmap-x-origin 40 --bitmap-y-origin 16 --bitmap-x-scale 2 \
  --output analysis/data/run035_golden_gate_red_viewport_interval.json
```
