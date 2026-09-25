# Run003 M-map projected polygon vectors

[Open the source-vector M-map SVG](../visuals/run003_m_map_projected_polygon_vectors.svg).

This is the map extraction requested for land and sea: it starts with the
game's green land palette and draws the 42 captured blue water polygons from
`$C4B390` immediately before `$C2FF48` submits them to the area renderer.  It
also draws the captured grey grid vectors and the independently traced Golden
Gate marker.

The paired PNG is only a local rasterisation used to measure agreement against
the sealed display oracle; neither the SVG polygons nor their vertices are
derived from bitmap runs.  The generated JSON records the resulting agreement
metric and exact evidence inputs.

Reproduce after collecting the bounded polygon capture:

```powershell
python scripts/collect_run003_m_map_projected_polygons.py `
  --output build/run003_m_map_projected_polygons
python scripts/render_run003_m_map_projected_polygon_vectors.py
```
