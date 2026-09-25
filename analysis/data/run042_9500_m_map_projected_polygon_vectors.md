# Run042 frame-9500 M-map projected polygon vectors

[Open the source-vector SVG](../visuals/run042_9500_m_map_projected_polygon_vectors.svg)
or its [inspection PNG](../visuals/run042_9500_m_map_projected_polygon_vectors.png).

This checkpoint is a moving map state: no byte-identical polygon recurs in the
128-submission collection. The first 34 submissions form one observed renderer
schedule cycle before its `$C4BFE8` lead context returns with changed projected
coordinates. The renderer therefore uses an explicit bounded-pass selection,
not a false static-repeat boundary.

Those 34 direct `$C4B390` projected polygons match the checkpoint map screen
at **111,822 / 113,098 = 98.8718%** for land/sea pixels. Its coast aligns at
zero translation with the earlier run042 map sample (99.6875% water agreement
over 113,920 pixels), so it does not add independent mosaic coverage.
