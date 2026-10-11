# Run035 panned M-map landmarks

This original artifact had the panning sign reversed and is superseded by the
[corrected traced-marker map](../visuals/run035_m_map_projected_polygon_vectors_landmarks_traced_marker.svg).

The Golden Gate and mountain-candidate anchors are transferred from run003
using the measured fixed-coastline relation `run035 = run003 + (142,36)` host
pixels. The run035 source-vector coastline is independently extracted; this
step only transfers fixed landmark positions, rather than claiming a new
run035 bridge/terrain control trace. The corrected artifact also has the
separately traced state-dependent flight-object marker.

The corrected Golden Gate position is independently checked against the exact
`#880000` bridge-pixel bounds in both run003 and run035; see
[`m_map_golden_gate_anchor_validation.json`](m_map_golden_gate_anchor_validation.json).
The corrected map also renders the two corresponding `$C3559A/$C355D2` red
bridge vectors at that anchor, rather than representing the bridge only with
a callout marker.

The earlier Mountain ? callout was removed. `$C3B720/$C3B6B0` is a reusable
trace-backed component path, so its occurrence near the city cannot establish
a unique mountain landmark or an original semantic name.
