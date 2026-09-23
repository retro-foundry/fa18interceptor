# Segment-68 M-map packet trace coverage

Coverage counts only exact pair payload bytes reached in these bounded map traces. It excludes packet headers, control words, unselected alternate streams, and all untraced segment data; it is not complete terrain-map coverage.

- Segment: `$C42CA8`--`$C444F7` (6224 bytes)
- Unique direct packet headers: 24
- Unique selected stream starts: 28
- Unique consumed coordinate pairs: 387
- Unique exact pair-payload bytes: 1548 (24.87% of segment bytes)

| Inventory | Direct headers | Selected streams | Unique pairs |
| --- | ---: | ---: | ---: |
| `analysis\data\run003_m_map_polygon_static_packets.json` | 21 | 21 | 333 |
| `analysis\data\run035_m_map_polygon_static_packets.json` | 8 | 8 | 73 |
| `analysis\data\run035_m_map_stable_polygon_static_packets.json` | 5 | 5 | 37 |
| `analysis\data\run037_m_map_stable_polygon_static_packets.json` | 23 | 23 | 343 |

The JSON companion retains every reached header, selected stream, and pair address.
