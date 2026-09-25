# Run035 panned M-map projected polygon vectors

[Open the panned source-vector map](../visuals/run035_m_map_projected_polygon_vectors.svg).

The second sealed map state independently renders from 49 projected polygon
submissions captured immediately before `$C2FF48`. Blue source polygons over
the green land base agree with 110,412 of 111,944 comparable land/sea pixels
in the distinct run035 display oracle (98.6315%).

This validates that the extraction is not a run003-only screen fit: it follows
the game's panned map geometry in a different flight state. It deliberately
has no transferred landmark callouts; the current Golden Gate and terrain
anchors are directly proven only in the run003 renderer trace.

```powershell
python scripts/collect_run003_m_map_projected_polygons.py `
  --restore build/run035_postflight_m_8895/state.bin `
  --config captures/run035/config.uae --no-playback `
  --output build/run035_m_map_projected_polygons
python scripts/render_run003_m_map_projected_polygon_vectors.py `
  --input build/run035_m_map_projected_polygons/projected_polygons.json `
  --no-lines --no-annotations --expected-submissions 49 `
  --oracle analysis/visuals/run035_m_map_display.png `
  --svg analysis/visuals/run035_m_map_projected_polygon_vectors.svg `
  --png analysis/visuals/run035_m_map_projected_polygon_vectors.png `
  --output analysis/data/run035_m_map_projected_polygon_vectors.json
```
