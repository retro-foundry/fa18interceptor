# M-map landmark evidence status

[Open the eleven-capture source-vector mosaic](../visuals/m_map_panned_projected_polygon_mosaic.png).

| map annotation | status | evidence boundary |
| --- | --- | --- |
| Golden Gate | verified landmark | `$C3559A/$C355D2` M-map line contexts are tied to the user-identified red bridge and transfer through the measured coastline pan. |
| MAP OBJECT ? | map-attached, identity unknown | The `$C2B93E` three-stroke symbol follows three independent coastline-map states; it is not a fixed reticle. Its identity can still be player, aircraft, base, city, or selected object. |
| `$C3B720/$C3B6B0` | excluded | A reusable local component; one map occurrence does not establish a mountain or landmark. |
| `$C3B5B6` | excluded | The same four-triangle component repeats in widely separated joined map regions. |
| C35BDE / C36220 lines | excluded | Direct source components, but no unique semantic or landmark join. |

The mosaic contains eleven direct `$C4B390`-before-`$C2FF48` polygon captures
from independent M-map states. Green/blue is source-vector land/sea inside
observed viewports; black is uncaptured space. It is not a complete global map.

The next sound route to another named landmark is a map-capable free-flight
checkpoint with a separate visual landmark oracle and a direct renderer/map
position join. The existing Golden Gate, Alcatraz-window, and later-bridge
demo snapshots cannot supply that join because their isolated M commands do
not enter map mode; see
[`run031_m_map_command_rejection.md`](run031_m_map_command_rejection.md).
