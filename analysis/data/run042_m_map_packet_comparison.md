# Run042 distinct-location M-map packet comparison

Classification: **location-sensitive bounded static map-packet selection**.
The sealed run042 post-`M` transition visibly produces a different coastline
map view and selects immutable segment-68 packets not reached in the earlier
stable run037 map window. This is not an absolute world-coordinate decode or
an LOD result.

The `M` key is pressed at replay frame 9,385. A replay-preserved checkpoint at
frame 9,392 was traced for 24 no-input frames. The map renderer entered 57
direct `$C2AF00` packet headers, completed 110 `$C2AFE2` pair-transform
batches, and consumed 749 exact static signed pairs from segment 68.

Its 25 distinct direct headers are:

```text
$C42D28 $C42DC4 $C42DFC $C42E1A $C42E3E $C42E52
$C439E8 $C43A78 $C43B1A $C43B34 $C43BAC $C43CBE $C43D1A
$C43E24 $C43ECA $C43F5E $C43FD0 $C4404C $C4407C $C4409E
$C440BC $C44160 $C4417A $C4420C $C4424A
```

Compared with the 363-pair stable run037 packet sample, eight direct headers
are newly observed in run042:

```text
$C43D1A $C43FD0 $C4404C $C440BC $C44160 $C4417A $C4420C $C4424A
```

Six run037 headers are absent from this bounded run042 window:

```text
$C43994 $C439A8 $C439CC $C43D34 $C43D84 $C44066
```

This establishes that two visibly distinct map positions select different
immutable packet content. The packet pairs are local renderer inputs whose
third/depth component is computed later, so the comparison does not establish
global terrain coordinates, coastline ownership, a terrain mesh, or
distance-driven LOD.

Authority: sealed `captures/run042`; ignored reproducible artifacts
`build/run042_post_m_checkpoint/`, `build/run042_m_map_transition_trace/`,
and `build/run042_m_map_static_packets.json`; comparison authority
`analysis/data/run037_m_map_stable_polygon_static_packets.json`.
