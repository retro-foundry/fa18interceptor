# `$C3B5B6` repeat across M-map positions

Classification: **direct projected-polygon recurrence; not a landmark**.

Three direct M-map polygon collectors contain four `$C3B5B6` submissions with
the same small four-triangle topology. In the normalized coastline mosaic,
their joined coordinates are:

| capture | normalized triangle region | relation |
| --- | --- | --- |
| run002 frame 4,302 | `x=210..218`, `y=184..186` | Base-4 initial map state |
| run002 frame 5,171 | `x=208..216`, `y=184..186` | nearby later Base-4 state |
| run038 frame 6,750 | `x=766..774`, `y=183..185` | independently joined flight/map state |

Each submission is captured from the game-produced `$C4B390` list immediately
before `$C2FF48`; it is not inferred from map bitmap pixels. The two Base-4
samples preserve the component under their small measured coastline pan. The
run038 occurrence is about 556 host pixels away in the joined observed map
frame while retaining the same `$C3B5B6` four-triangle family.

This proves repeated use of the renderer component across observed map
positions. It rules out treating a single occurrence as a unique mountain,
city, or landmark marker. It does not establish whether the component denotes
terrain decoration, a repeated world object, or a renderer primitive.

Authority: direct polygon captures
`build/run002_m_map_projected_polygons/projected_polygons.json`,
`build/run002_late_m_map_projected_polygons/projected_polygons.json`, and
`build/run038_m_map_projected_polygons/projected_polygons.json`; joined output
[`m_map_panned_projected_polygon_mosaic.svg`](../visuals/m_map_panned_projected_polygon_mosaic.svg).
