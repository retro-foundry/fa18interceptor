# Run003/run035 M-map flight-object marker comparison

Classification: **scenario-backed renderer-vector movement**.

The black M-map symbol uses the same `$C2B93E` three-record line-list entry
in both captures, but its directly traced `$C4C598` vectors differ:

| capture | vector points from `$C2FA7E` |
| --- | --- |
| run003 appearance trace | `(31,130)`, `(25,131)`, `(26,132)` |
| run035 stable 20-frame trace | `(103,166)`, `(97,167)`, `(98,168)` |

This establishes that the symbol is not a screen-fixed reticle across these
two M-map states.  It is rendered at a state-dependent M-map position.  Its
upstream producer remains `$C45BEA`, the mutable display context associated
with the F/A-18-like `$C34C` face family.

It still does **not** prove the symbol is the player: a world-relative object,
another aircraft, or a selected object could all satisfy the present trace.
The shift also must not be read as a global coordinate delta: the separately
measured coastline panning translation has different magnitude and direction.

The visible evidence artifact is
[`run035_m_map_projected_polygon_vectors_landmarks_traced_marker.svg`](../visuals/run035_m_map_projected_polygon_vectors_landmarks_traced_marker.svg)
with its inspection PNG alongside it.  Golden Gate and Mountain ? use the
existing fixed-coastline panning transfer; FLIGHT OBJECT ? is a direct run035
line trace.
