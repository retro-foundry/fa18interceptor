# Run035 panned M-map landmarks

[Open the panned landmark map](../visuals/run035_m_map_projected_polygon_vectors_landmarks.svg).

The Golden Gate and mountain-candidate anchors are transferred from run003
using the measured fixed-coastline relation `run035 = run003 - (142,36)` host
pixels. The run035 source-vector coastline is independently extracted; this
step only transfers fixed landmark positions, rather than claiming a new
run035 bridge/terrain control trace. The flight-object marker is intentionally
not transferred because its motion and player ownership are unresolved.
