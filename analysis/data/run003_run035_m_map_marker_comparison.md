# Run003/run035 M-map object-marker comparison

Classification: **scenario-backed map-attached renderer-vector movement**.

The black M-map symbol uses the same `$C2B93E` three-record line-list entry
in both captures, but its directly traced `$C4C598` vectors differ:

| capture | vector points from `$C2FA7E` |
| --- | --- |
| run003 appearance trace | `(31,130)`, `(25,131)`, `(26,132)` |
| run035 stable 20-frame trace | `(103,166)`, `(97,167)`, `(98,168)` |

Converting the run003 points to host pixels and applying the independently
measured coastline translation `(142,36)` predicts run035 points
`(204,166)`, `(192,167)`, and `(194,168)`. The direct run035 points are
`(206,166)`, `(194,167)`, and `(196,168)`: a uniform two-host-pixel X
residual, with Y exact. This establishes that the symbol is map-attached, not
a screen-fixed reticle. Its upstream producer remains `$C45BEA`, the mutable
display context associated with the F/A-18-like `$C34C` face family.

It still does **not** prove the symbol is the player: a world-relative object,
another aircraft, a base, city, or selected object could all satisfy the
present trace. The residual must not be read as a global coordinate delta;
the measurement only compares screen-space map panning.

The independent run024 San Francisco cockpit map confirms the same relation:
its marker points are `(32,131)`, `(26,132)`, and `(27,133)`. The measured
run024-to-run035 coastline translation `(140,35)` predicts `(204,166)`,
`(192,167)`, and `(194,168)`, again leaving the same uniform two-host-pixel
X residual to the direct run035 points. Run024 differs from run003 by exactly
one logical pixel in each axis, matching the composed coastline translation.
This third state makes map attachment repeatable, but still does not identify
the object as a city, base, player, or aircraft.

The visible evidence artifact is
[`run035_m_map_projected_polygon_vectors_landmarks_traced_marker.svg`](../visuals/run035_m_map_projected_polygon_vectors_landmarks_traced_marker.svg)
with its inspection PNG alongside it. Golden Gate uses the existing
fixed-coastline panning transfer; MAP OBJECT ? and the five map grid lines are
direct run035 line traces.
