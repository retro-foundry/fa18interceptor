# Run042 new M-map headers: directory and packet grammar

Classification: **scenario-observed header selection joined to byte-decoded
wide-directory provenance and static packet grammar**.

Run042 directly enters the eight listed headers at `$C2AF00`; none is a
direct entry in the bounded run037 stable-map sample. Each has one or more
nonnegative targets in the byte-decoded 32×32 wide directory, and each inline
and alternate stream completes under the exact `$C2AF46` count/threshold grammar.
The cell coordinates are local selector indices, not world coordinates. This
does not identify terrain, coastline ownership, physical extent, or distance LOD.

Authority: `analysis/data/run042_m_map_polygon_static_packets.json`, `analysis/data/run037_m_map_stable_polygon_static_packets.json`,
`analysis/data/wide_m_map_packet_directory.json`, and `analysis/data/static_m_map_packet_streams.json`.

| Header | run042 entries / frames | route | wide cells | static streams (role: pairs; terminator) |
| --- | --- | --- | ---: | --- |
| `$C43D1A` | 3 / 34, 38, 48 | inline×3 | 14 | `$C43D1E` (alternate/inline: 4; `$C43D30`) |
| `$C43FD0` | 2 / 32, 46 | inline×2 | 1 | `$C43FD4` (alternate/inline: 28; `$C4404A`) |
| `$C4404C` | 1 / 37 | inline×1 | 13 | `$C44050` (alternate/inline: 4; `$C44062`) |
| `$C440BC` | 2 / 31, 44 | inline×2 | 1 | `$C440C0` (alternate/inline: 37; `$C4415E`) |
| `$C44160` | 1 / 36 | inline×1 | 13 | `$C44164` (alternate/inline: 4; `$C44176`) |
| `$C4417A` | 1 / 36 | inline×1 | 1 | `$C4417E` (alternate/inline: 33; `$C44208`) |
| `$C4420C` | 1 / 36 | inline×1 | 1 | `$C44210` (alternate/inline: 13; `$C44248`) |
| `$C4424A` | 1 / 36 | inline×1 | 1 | `$C4424E` (alternate/inline: 13; `$C44284`) |

## Exact wide-directory cells

Only cells with a nonnegative target word are listed. A cell may share a
header with other cells; this reuse is static selector provenance, not a
measured map area.

| Header | `(x,y)` cells |
| --- | --- |
| `$C43D1A` | `(18,17)`, `(19,17)`, `(20,17)`, `(21,17)`, `(22,17)`, `(23,17)`, `(24,17)`, `(25,17)`, `(26,17)`, `(27,17)`, `(28,17)`, `(29,17)`, `(30,17)`, `(31,17)` |
| `$C43FD0` | `(18,18)` |
| `$C4404C` | `(19,18)`, `(20,18)`, `(21,18)`, `(22,18)`, `(23,18)`, `(24,18)`, `(25,18)`, `(26,18)`, `(27,18)`, `(28,18)`, `(29,18)`, `(30,18)`, `(31,18)` |
| `$C440BC` | `(18,19)` |
| `$C44160` | `(19,19)`, `(20,19)`, `(21,19)`, `(22,19)`, `(23,19)`, `(24,19)`, `(25,19)`, `(26,19)`, `(27,19)`, `(28,19)`, `(29,19)`, `(30,19)`, `(31,19)` |
| `$C4417A` | `(15,20)` |
| `$C4420C` | `(18,20)` |
| `$C4424A` | `(19,20)` |

## Evidence boundary

All run042 entries select the inline route with `D7 = 0` in this bounded
transition trace. The report records complete static alternate streams because
the byte-exact selector can choose them in other observed scenarios; it does
not claim that run042 rendered those alternates. Pair counts describe decoded
packet format, not faces, paths, or map geometry.
