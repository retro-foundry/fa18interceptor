# Panned M-map projected-polygon mosaic

[Open the source SVG](../visuals/m_map_panned_projected_polygon_mosaic.svg) or
[inspection PNG](../visuals/m_map_panned_projected_polygon_mosaic.png).

This wider view joins the run042 40-polygon, run035 49-polygon, and run003
42-polygon canonical M-map passes. Every blue shape is a direct `$C4B390`
projected polygon captured immediately before `$C2FF48`; the green base is the
game’s observed land palette. In the normalized mosaic run042 starts at
`(0,0)`, run035 is translated by `(130,61)`, and run003 by `(272,97)` host
pixels. The run035/run042 join has 98.4816% blue-water agreement over 59,670
pixels; its measurement is
[`run035_run042_m_map_pan_comparison.json`](run035_run042_m_map_pan_comparison.json).

Golden Gate and Mountain ? retain their existing evidence-qualified anchors.
The two red Golden Gate strokes are the transferred `$C3559A/$C355D2` map
vectors and coincide with the red-pixel-validated bridge anchor.
The display grid and flight-object marker are excluded: they are view/state
dependent renderer output, not fixed coastline geometry.

This is wider observed source coverage, **not** an absolute coordinate system,
a complete world map, or a reconstructed global terrain model. Green space
outside either captured map rectangle represents absent capture coverage, not
asserted land.
