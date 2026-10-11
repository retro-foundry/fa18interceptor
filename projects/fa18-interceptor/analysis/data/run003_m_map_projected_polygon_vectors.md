# Run003 M-map projected polygon vectors

[Open the source-vector M-map SVG](../visuals/run003_m_map_projected_polygon_vectors.svg).

This is the direct-vector map extraction: it starts with the game's green
land palette and draws all 42 captured polygons from `$C4B390` immediately
before `$C2FF48` submits them to the area renderer. Thirty-eight submissions
use active fill mask `2` and are blue water; four `$C3B6B0` submissions use
mask `15` and are the separately captured dark-green filled map overlay.
It also draws every captured line submission: grey ordinary overlays, red
`$C3559A/$C355D2` Golden Gate strokes, the grey grid, and the black conditional
symbol.

The paired PNG is only a local rasterisation used to measure agreement against
the sealed display oracle; neither the SVG polygons nor their vertices are
derived from bitmap runs.  The generated JSON records the resulting agreement
metric and exact evidence inputs.

The small black symbol at host-crop bounds `x=50..70, y=130..132` is drawn
from three `$C4C598` renderer line vectors. Its producer context remains
conditional and its game identity is unknown, so the visual carries no label.

The dark-green `$C3B6B0` fill and its two line segments, along with the
`$C35BD4`, `$C36212`, and `$C36216` line components, are rendered but not
called buildings, roads, cities, or landmarks. Their direct renderer presence
is proved; their semantic identities and global placements are not.

Reproduce after collecting the bounded polygon capture:

```powershell
python scripts/collect_run003_m_map_projected_polygons.py `
  --output build/run003_m_map_projected_polygons
python scripts/render_run003_m_map_projected_polygon_vectors.py
```
