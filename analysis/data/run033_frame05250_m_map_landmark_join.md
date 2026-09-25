# Run033 frame 5,250: direct Golden Gate M-map join

Classification: **direct M-map vector capture with a named landmark context**.

The sealed run033 frame-5,250 checkpoint is the user-identified distant red
Golden Gate scene.  A correctly *relative* five-frame M-key event enters its
M-map display.  The resulting canonical pass has 49 `$C4B390` projected
polygons immediately before `$C2FF48`, rendered in
[`run033_frame05250_m_map_projected_polygon_vectors.svg`](../visuals/run033_frame05250_m_map_projected_polygon_vectors.svg).

The map's direct `$C2FA7E` line collection reaches the already established
bridge contexts:

| context | renderer endpoints (logical 320x180) |
| --- | --- |
| `$C3559A` | `(127,74)` to `(127,77)` |
| `$C355D2` | `(127,80)` to `(127,76)` |

Exact-colour blue-water alignment with run035 measures
`run033(x - 80, y - 18) == run035(x, y)`, with 97.8951% agreement over
89,600 pixels.  In the normalized source-vector mosaic this puts the run033
viewport at `(210,79)`.  Its bridge strokes therefore land at host-space
`x=464`, `y=153..160`, agreeing with the previously traced Golden Gate anchor
at `(466,156.5)` to within two host pixels.

This is now independent direct M-map confirmation of the Golden Gate label;
it is not a transfer from an unrelated flight screenshot.  It does not
identify any other landmark or convert the joined view into global map
coordinates.

Authorities: `build/run033_frame05250_checkpoint/state.bin`,
`build/run033_frame05250_m_press.e9k`,
`build/run033_frame05250_m_map_projected_polygons/projected_polygons.json`,
`build/run033_frame05250_m_map_line_entries/blitter_line_entries.json`, and
[`run035_run033_frame05250_m_map_pan_comparison.json`](run035_run033_frame05250_m_map_pan_comparison.json).
