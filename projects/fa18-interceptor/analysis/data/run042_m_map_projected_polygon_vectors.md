# Run042 M-map projected polygon vectors

[Open the source-vector SVG](../visuals/run042_m_map_projected_polygon_vectors.svg)
or its [inspection PNG](../visuals/run042_m_map_projected_polygon_vectors.png).

This is the first direct coloured source-vector render for the preserved
run042 M-map location. The collector stops at `$C2FF48` and records its
`$C4B390` projected polygon pairs before the game’s area-fill implementation.
The first repeated polygon bounds a 40-submission canonical pass.

Replaying the same no-input 100-frame window from
`build/run042_post_m_checkpoint/state.bin` provides the screen oracle. For
land/sea-classifiable pixels, the rendered vectors agree at **110,856 /
113,126 = 97.9934%**. The remaining discrepancy is retained as the known
area-fill edge-rule boundary rather than patched into the source vectors.

Run042 is visibly and packet-wise a different map location, but no safe
coastline translation to the run003/run035 mosaic is established. It is
therefore a validated additional source view, not a forced extension of the
panned mosaic or a complete global map.
