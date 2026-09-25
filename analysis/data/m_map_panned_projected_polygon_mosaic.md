# Panned M-map projected-polygon mosaic

[Open the source SVG](../visuals/m_map_panned_projected_polygon_mosaic.svg) or
[inspection PNG](../visuals/m_map_panned_projected_polygon_mosaic.png).

This wider view joins the run003 42-polygon and run035 49-polygon canonical
M-map passes. Every blue shape is a direct `$C4B390` projected polygon
captured immediately before `$C2FF48`; the green base is the game’s observed
land palette. In the normalized mosaic run035 starts at `(0,0)` and run003 is
translated by `(142,36)` host pixels, matching the measured relation
`run035 = run003 + (142,36)` on the shared coastline.

Golden Gate and Mountain ? retain their existing evidence-qualified anchors.
The two red Golden Gate strokes are the transferred `$C3559A/$C355D2` map
vectors and coincide with the red-pixel-validated bridge anchor.
The display grid and flight-object marker are excluded: they are view/state
dependent renderer output, not fixed coastline geometry.

This is wider observed source coverage, **not** an absolute coordinate system,
a complete world map, or a reconstructed global terrain model. Green space
outside either captured map rectangle represents absent capture coverage, not
asserted land.
