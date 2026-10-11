# Run001 M-map projected polygon vectors

[Open the source-vector SVG](../visuals/run001_m_map_projected_polygon_vectors.svg)
or its [inspection PNG](../visuals/run001_m_map_projected_polygon_vectors.png).

From the sealed run001 frame-3195 Base-1 free-flight checkpoint, one `M` key
press produces a 36-submission map pass. The direct `$C4B390` polygon export
matches its M screen at **111,583 / 112,630 = 99.0704%** for land/sea pixels.

The blue-water coastline joins run035 as `run035(x+64,y+60)=run001(x,y)` at
97.9020% agreement over 67,968 pixels, placing this pass at `(194,121)` in the
normalized mosaic. It contributes observed lower/right coverage; it does not
establish absolute map coordinates or a named landmark.

Authority: sealed run001 checkpoint, with ignored reproduction captures in
`build/run001_m_map_projected_polygons/` and `build/run001_m_map_oracle/`.
