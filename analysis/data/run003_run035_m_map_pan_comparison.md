# `M` map coastline pan across flight positions

Classification: **scenario-backed visual world-position result**.

Two deterministic in-game map renders show the same blue-water/green-land
coastline under a viewport translation:

| replay | visual | condition |
| --- | --- | --- |
| run003 frame 2,213 | [map image](../visuals/run003_m_map_display.png) | original sealed `M` probe |
| run035 frame 8,895 | [map image](../visuals/run035_m_map_display.png) | controlled `M` press after the sealed flight and turn-away |

Within screen rectangle `x=40..679`, `y=18..195`, exact blue RGB `#003366`
matches under `second(x+142, y+36) = first(x,y)` for 70,716 overlapping pixels
with 98.8008% agreement. The remaining mismatch is expected from grid,
readouts, marker pixels, and viewport clipping. This proves that the map view
pans a common coastline presentation in response to different flight state;
it is not a fixed decorative screen.

The result does **not** identify the static coastline producer, establish a
map-to-3D coordinate transform, or prove that the presentation asset is the
same data as the 3D terrain template lattice. It is a stronger visual oracle
for those future joins. Both views use the same standard RGB4 map palette
(`COLOR04=$0151` green and `COLOR06=$0036` blue); this is palette reuse, not
source-asset evidence.

A bounded trace after the run035 command sees a different repeated control
workload (`$C3515A`) that draws a display/grid-like line set, but no transform
entry. It is therefore not a terrain-source or LOD comparison; see the
[panned-map control probe](run035_m_map_panned_control_probe.md).

Authority: sealed `captures/run003` and `captures/run035`; ignored
reproduction artifacts `build/run003_m_visual_30/` and
`build/run035_postflight_m_8895/`; machine-readable measurement in
`run003_run035_m_map_pan_comparison.json`.

```text
python scripts/compare_map_viewports.py \
  --first build/run003_m_visual_30/screen.png \
  --second build/run035_postflight_m_8895/screen.png \
  --colour 003366 --left 40 --top 18 --right 680 --bottom 196 \
  --max-x 320 --max-y 80 \
  --output analysis/data/run003_run035_m_map_pan_comparison.json
```
