# `$C36220 -> $C36232`: map-mode three-triple line component

Classification: **scenario-backed immutable transform input to map-line
renderer path**.

In the video-hash-matched map-transition trace, `$C1F4AC` enters at trace
indices 86,803 and 87,724 with `A1=$C36220` and destination `A3=$C48390`.
Each transform processes exactly three consecutive signed triples; the next
control-walker entries at indices 86,922 and 87,843 have `A1=$C36232`.

| source slot | raw triple |
| ---: | --- |
| 0 | `(4096, 172, -5248)` |
| 1 | `(-2816, 172, 1152)` |
| 2 | `(-8960, 0, 2816)` |

The direct source boundary is therefore `$C36220-$C36231` (18 bytes). The
following `$C36232` begins static control data, not a fourth triple.

Each walker interval reaches two `$C2FA7E` calls. The first uses static
`A5=$C36216`, the second `A5=$C36212`; both intervals produce the same two
logical screen segments:

```text
(118,66) -> (125,60)
(125,60) -> (132,59)
```

These are the segments already drawn over the hash-matched map frame in the
[map line overlay](../visuals/run003_m_map_c36214_line_overlay.png). Thus the
bounded dataflow is:

```text
$C36220-$C36231 immutable triples
  -> $C1F4AC -> $C48390 mutable transformed slots
  -> $C36232 static control entry
  -> $C36216/$C36212 line records
  -> $C2FA7E -> prepared M-map bitplanes
```

This proves a second small 3D component contributes visible map-line geometry.
It does not establish global coordinates, a coastline-pixel ownership match,
or a complete terrain model. Its nonzero middle components are local geometry,
not evidence against the separately proven flat placement layer.

Authority: sealed `captures/run003`; ignored
`build/run003_m_map_appearance_trace/{trace.jsonl,slow.bin}`; and the
[reproducible renderer census](run003_m_map_display_renderer_census.md).
