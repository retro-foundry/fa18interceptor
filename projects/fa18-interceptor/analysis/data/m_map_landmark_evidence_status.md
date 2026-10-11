# M-map landmark evidence status

[Open the seventeen-capture source-vector mosaic](../visuals/m_map_panned_projected_polygon_mosaic_v4.png), the [rotatable land-and-sea evidence plane](../visuals/m_map_land_sea_3d_evidence.html), or the [free-look WebGL remake terrain viewer](../visuals/m_map_modern_remake_viewer.html).

| map annotation | status | evidence boundary |
| --- | --- | --- |
| Golden Gate | verified landmark | `$C3559A/$C355D2` M-map line contexts are tied to the user-identified red bridge, including a direct M-map capture from run033's Golden Gate state. |
| MAP OBJECT ? | map-attached, conditional, identity unknown | The `$C2B93E` three-stroke symbol follows three independent coastline-map states and is absent from the Alcatraz-window diagnostic map. It is not a fixed reticle or universal static city/island mark; its identity can still be player, aircraft, base, or selected object. |
| `$C3B720/$C3B6B0` | excluded | A reusable local component; one map occurrence does not establish a mountain or landmark. |
| `$C3B5B6` | excluded | The same four-triangle component repeats in widely separated joined map regions. |
| C35BDE / C36220 lines | excluded | Direct source components, but no unique semantic or landmark join. |

The v4 mosaic contains seventeen direct `$C4B390`-before-`$C2FF48` polygon captures
from independent M-map states. Green/blue is source-vector land/sea inside
observed viewports; black is uncaptured space. It is not a complete global map.

The remake terrain viewer consumes an ordered direct-triangle mesh from that
same source-vector mosaic, not its PNG or SVG rasterisation. Its `28 × 10.36`
plane ratio preserves the `1000 × 370` joined map geometry, and its red ground
segments are the verified `$C3559A/$C355D2` Golden Gate vectors at `(466,156.5)`.
This makes the recovered terrain useful in a modern renderer without inventing
a heightmap, global-distance scale, roads, buildings, or unproven landmark
identities.

One pass, run034, is explicitly diagnostic because its sealed input recording
preserved the `M` release but not its key-down; its supplied one-frame press
has a 98.253% direct coastline registration match and is retained as coverage,
not normal-replay landmark evidence.

The additional run024 frame-16,075 pass is an ordinary recorded M-key replay
and registers at `(12,131)` with 99.143% direct coastline agreement; it adds
normal-replay coverage but no new landmark identity.

The next sound route to another named landmark is a map-capable free-flight
checkpoint with a separate visual landmark oracle and a direct renderer/map
position join. Run033 now supplies that join for Golden Gate; its direct
evidence is retained in
[`run033_frame05250_m_map_landmark_join.md`](run033_frame05250_m_map_landmark_join.md).
The Alcatraz-window state now supplies diagnostic map coverage, but its
island/map primitive is not yet identified and is deliberately unlabelled.
