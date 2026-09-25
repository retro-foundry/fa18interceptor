# Panned M-map projected-polygon mosaic

[Open the source SVG](../visuals/m_map_panned_projected_polygon_mosaic.svg) or
[inspection PNG](../visuals/m_map_panned_projected_polygon_mosaic.png).

This wider view joins the run042 40-polygon, run037 47-polygon, run035
49-polygon, bounded moving run041 50-polygon, and run003 42-polygon M-map
passes. Every blue shape is a direct `$C4B390`
projected polygon captured immediately before `$C2FF48`; the green base is the
game’s observed land palette. In the normalized mosaic run042 starts at
`(0,0)`, run037 is translated by `(14,45)`, run035 by `(130,61)`, run041 by
`(172,72)`, and run003 by `(272,97)` host pixels. The run041/run035 join has
98.2697% blue-water agreement over 99,866 pixels; the run037/run035 join has 97.9997% blue-water
agreement over 84,888 pixels, while run035/run042 has 98.4816% over 59,670;
their measurements are
[`run041_run035_m_map_pan_comparison.json`](run041_run035_m_map_pan_comparison.json),
[`run037_run035_m_map_pan_comparison.json`](run037_run035_m_map_pan_comparison.json),
and
[`run035_run042_m_map_pan_comparison.json`](run035_run042_m_map_pan_comparison.json).

Golden Gate retains its evidence-qualified anchor.
The two red Golden Gate strokes are the transferred `$C3559A/$C355D2` map
vectors and coincide with the red-pixel-validated bridge anchor.
The display grid and flight-object marker are excluded: they are view/state
dependent renderer output, not fixed coastline geometry.

The reusable `$C3B720/$C3B6B0` component is also excluded: one observed map
occurrence does not prove it is a unique mountain or landmark.

This is wider observed source coverage, **not** an absolute coordinate system,
a complete world map, or a reconstructed global terrain model. Green space
outside either captured map rectangle represents absent capture coverage, not
asserted land.
