# Panned M-map projected-polygon mosaic

[Open the source SVG](../visuals/m_map_panned_projected_polygon_mosaic.svg) or
[inspection PNG](../visuals/m_map_panned_projected_polygon_mosaic.png).

This wider view joins the run042 40-polygon, run002 43-polygon, later run002
43-polygon, run037 47-polygon, run035 49-polygon, run038 58-polygon, bounded
moving run041 50-polygon, diagnostic run031 Alcatraz-window 51-polygon,
diagnostic run031 later-bridge 54-polygon, run033's Golden Gate state
49-polygon, run001 36-polygon, run024 42-polygon, run003 42-polygon, and
bounded run004 22-polygon M-map passes. Every blue shape is a direct `$C4B390`
projected polygon captured immediately before `$C2FF48`; the green base is the
game’s observed land palette. In the normalized mosaic run042 starts at
Each visible green rectangle is an observed 640×180 viewport; black space is
uncaptured map area, not sea or land. In the normalized mosaic run042 starts at
`(0,0)`, run002 is translated by `(16,131)`, later run002 by `(10,122)`,
run037 by `(14,45)`, run035 by
`(130,61)`, run038 by `(154,71)`, run041 by `(172,72)`, diagnostic run031
Alcatraz-window state by `(172,67)`, diagnostic run031 later-bridge state by
`(194,70)`, run033 frame 5,250 by `(210,79)`, run001 by `(194,121)`,
run024 by `(270,96)`, run003 by `(272,97)`, and run004 by `(356,170)` host
pixels. The run004/run035 join has 99.3979% blue-water agreement over 28,566
pixels; the run001/run035
join has 97.9020% blue-water agreement over 67,968 pixels; the later
run002/run035 join has 98.8856% blue-water agreement over 60,840 pixels; the
run002/run035 join has 97.9792%
blue-water agreement over 56,808 pixels; the run024/run035 join has 98.1594% blue-water
agreement over 71,500 pixels; the
run038/run035 join has 98.2684% blue-water agreement over 103,488 pixels;
the run041/run035 join has
98.2697% blue-water agreement over 99,866 pixels; the run037/run035 join has 97.9997% blue-water
agreement over 84,888 pixels, while run035/run042 has 98.4816% over 59,670;
their measurements are
[`run038_run035_m_map_pan_comparison.json`](run038_run035_m_map_pan_comparison.json),
[`run002_run035_m_map_pan_comparison.json`](run002_run035_m_map_pan_comparison.json),
[`run002_late_run035_m_map_pan_comparison.json`](run002_late_run035_m_map_pan_comparison.json),
[`run001_run035_m_map_pan_comparison.json`](run001_run035_m_map_pan_comparison.json),
[`run004_run035_m_map_pan_comparison.json`](run004_run035_m_map_pan_comparison.json),
[`run024_run035_m_map_pan_comparison.json`](run024_run035_m_map_pan_comparison.json),
[`run041_run035_m_map_pan_comparison.json`](run041_run035_m_map_pan_comparison.json),
[`run037_run035_m_map_pan_comparison.json`](run037_run035_m_map_pan_comparison.json),
and
[`run035_run042_m_map_pan_comparison.json`](run035_run042_m_map_pan_comparison.json).
The direct run033/run035 join has 97.8951% blue-water agreement over 89,600
pixels; its measurement is
[`run035_run033_frame05250_m_map_pan_comparison.json`](run035_run033_frame05250_m_map_pan_comparison.json).
The diagnostic Alcatraz-window/run035 join has 98.1683% blue-water agreement
over 102,856 pixels; its measurement is
[`run035_run031_frame13200_alcatraz_m_map_pan_comparison.json`](run035_run031_frame13200_alcatraz_m_map_pan_comparison.json).
The diagnostic later-bridge/run035 join has 98.2885% blue-water agreement over
97,344 pixels; its measurement is
[`run035_run031_frame14500_later_bridge_m_map_pan_comparison.json`](run035_run031_frame14500_later_bridge_m_map_pan_comparison.json).

Golden Gate retains its evidence-qualified anchor. The two red
`$C3559A/$C355D2` strokes are now directly captured in both a run033
user-identified Golden Gate M-map state and the prior map state; they coincide
at the measured join.

The Alcatraz-window capture is intentionally not labelled `ALCATRAZ` on the
map: only `$C4584B` (the upstream keyboard context-mode latch) was changed to
reach the original map renderer, and no emitted map primitive identifies the
user-observed island.

The later-bridge diagnostic is likewise coverage only: its flight-view bridge
has no user-confirmed map primitive and is not labelled in the mosaic.
The display grid and map-object marker are excluded: they are view/state
dependent renderer output, not fixed coastline geometry.

The reusable `$C3B720/$C3B6B0` component is also excluded: one observed map
occurrence does not prove it is a unique mountain or landmark.

This is wider observed source coverage, **not** an absolute coordinate system,
a complete world map, or a reconstructed global terrain model.
