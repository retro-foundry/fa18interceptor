# Run024 M-map object marker

Classification: **third scenario-backed observation of the map-attached
object marker**.

From the sealed run024 frame-23000 San Francisco cockpit checkpoint, one `M`
press reaches the same `$C2B93E` three-record line-list path used by the
run003 and run035 map captures. Its direct `$C4C598` line-emitter entries are:

| segment | logical endpoints |
| --- | --- |
| 1 | `(32,131) -> (32,131)` |
| 2 | `(26,132) -> (36,132)` |
| 3 | `(27,133) -> (35,133)` |

Run024's exact blue-water join says `run035(x+140,y+35)=run024(x,y)`. After
converting the marker points to host pixels, that relation predicts run035
points `(204,166)`, `(192,167)`, and `(194,168)`. The traced run035 points
are `(206,166)`, `(194,167)`, and `(196,168)`: the same uniform two-host-pixel
X residual already measured from run003. The run003-to-run024 marker shift is
one logical pixel in both axes, matching the coastline joins composed through
run035.

This establishes repeatable map attachment across three M-map states. It does
not identify the object: the San Francisco scenario label is flight-screen
context, not a city-coordinate proof, and the marker can still represent the
player, another aircraft, a base, city, or selected object.

Authority: ignored reproduction collector
`build/run024_m_map_line_entries/blitter_line_entries.json`; direct entries
are submissions 24--26 and 41--43, with `A5=$C4C598` and return PC `$C2BAE6`.
