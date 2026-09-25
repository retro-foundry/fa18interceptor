# Run031 frame 14,500: later-bridge M-map diagnostic

Classification: **authentic map renderer coverage through the established UI
mode-latch bypass; no landmark primitive identified**.

The user-visible run031 frame-14,500 state contains the later bridge encounter.
Its `$C4584B=$03` command-mode latch bypasses the direct M-key table. Clearing
only that latch before the ordinary relative M press/release reaches the
original M-map renderer and yields a bounded 54-polygon vector pass, rendered
in
[`run031_frame14500_later_bridge_m_map_projected_polygon_vectors.svg`](../visuals/run031_frame14500_later_bridge_m_map_projected_polygon_vectors.svg).

Exact-colour blue-water alignment measures
`run031(x - 64, y - 9) == run035(x, y)` with 98.2885% agreement over 97,344
pixels. The pass is placed at `(194,70)` in the normalized mosaic.

The user has not supplied a confirmed name for this bridge and no direct map
component identifies it. The capture therefore expands source-vector coverage
only; it does not create a bridge label.

Authorities: `build/run031_frame14500_bridge_checkpoint/state.bin`,
`build/run031_frame14500_later_bridge_m_map_mode_latch_zero/`,
`build/run031_frame14500_later_bridge_m_map_projected_polygons/`, and
[`run035_run031_frame14500_later_bridge_m_map_pan_comparison.json`](run035_run031_frame14500_later_bridge_m_map_pan_comparison.json).
