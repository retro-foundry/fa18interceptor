# Run034 diagnostic M-map projected polygon vectors

Classification: **diagnostic direct renderer-vector map coverage**.

The sealed run034 replay records an `M` key release at frame 5,680 but lacks
the corresponding key-down event.  Restoring its frame-5,675 flight state and
supplying the standard single-frame `M` key-down produces a repeating
43-submission `$C4B390 -> $C2FF48` pass.  Its renderer-produced polygons were
captured before the area blitter and rendered with the established fill masks:
green land base and blue mask-2 water.

The resulting [SVG](../visuals/run034_diagnostic_m_map_projected_polygon_vectors.svg)
and [PNG](../visuals/run034_diagnostic_m_map_projected_polygon_vectors.png)
are useful coverage inspection of this flight position.  They are not added to
the normal M-map mosaic, receive no landmark label, and do not prove a normal
run034 M-map screen because the input was supplied diagnostically.

Authority: `build/run034_near_m_5675/state.bin`,
`build/run003_m_press_only.e9k`, and
`build/run034_diagnostic_m_map_projected_polygons_tagged/projected_polygons.json`.
The collector invocation uses `--diagnostic-input`; the rendered JSON retains
that classification.
