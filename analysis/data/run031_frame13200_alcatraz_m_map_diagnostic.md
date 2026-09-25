# Run031 frame 13,200: Alcatraz-window M-map diagnostic

Classification: **authentic map renderer output reached through a diagnostic
UI-latch bypass; not an Alcatraz map label**.

The sealed frame-13,200 screenshot is the user-identified Alcatraz-window
flight state. In that state `$C4584B=$03`, which the byte-exact keyboard gate
uses to route raw input to the context-mode table before the direct M-key
table. The accepted run033 Golden Gate map state has `$C4584B=$00`.

Writing only `$C4584B=$00` into the deserialized emulator instance, then
delivering the normal relative M press/release, reaches the original M-map
renderer. It produces a 51-polygon `$C4B390`-before-`$C2FF48` pass; its direct
vector raster is
[`run031_frame13200_alcatraz_m_map_projected_polygon_vectors.svg`](../visuals/run031_frame13200_alcatraz_m_map_projected_polygon_vectors.svg).
No map geometry memory, map control stream, or projected polygon workspace was
written by the probe.

The pass aligns to run035 by `run031(x - 42, y - 6) == run035(x, y)`, with
98.1683% blue-water agreement over 102,856 pixels. It joins the normalized
mosaic at `(172,67)`.

This establishes that the mode latch is sufficient to block the command
upstream of `$C1BF8C`; it does **not** establish that any displayed component
is Alcatraz or that the map-object symbol belongs to the aircraft. The island
remains unlabelled until a direct map primitive or a proven player/object join
identifies it.

Authorities: `source_amiga/observed/gate_keyboard_command_dispatch.asm`,
`build/run031_frame13200_alcatraz_checkpoint/state.bin`,
`build/run031_frame13200_alcatraz_m_map_mode_latch_zero/`,
`build/run031_frame13200_alcatraz_m_map_mode_latch_zero_projected_polygons/`,
and [`run035_run031_frame13200_alcatraz_m_map_pan_comparison.json`](run035_run031_frame13200_alcatraz_m_map_pan_comparison.json).
