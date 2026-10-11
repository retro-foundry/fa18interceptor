# Planar tuple source versus local triple geometry

Classification: **proven format boundary for the sampled terrain/map path**.
It supports two-coordinate terrain-placement inputs, but does not make all
renderer geometry two-dimensional.

## Evidence for two-coordinate placement inputs

The immutable terrain-template entries used in run037 are six-byte records:
one header byte, one padding/control byte, and **two signed 16-bit words**.
`$C1D488` reads the header and `$C1D4BC` copies exactly those two words into a
mutable workspace cell.  In the stable `M`-map interval, the later builder
turns those cells into descriptor-qualified placements whose observed shape is
`(X, 0, Z)`.  This is a source pair plus a derived zero middle component, not
a stored three-coordinate terrain tuple.

The independent static M-map packet path has the same source dimensionality:
`$C2AF9C` and `$C2AF9E` consume exactly two signed words from each immutable
segment-68 packet.  The third renderer workspace component is written from
live transform state rather than packet storage.

## Evidence for local triples

The control target selected by a placement is a separate layer.  In the
traced example, template `$C427AD` has the two words `$0800,$0800`; its
placement is `(192, 0, -448)` and its descriptor selects target `$C44970`.
That target eventually reaches the static component at `$C3B720`.  The
bounded `$C3B720-$C3B73D` component contains exactly five **three-word**
triples and feeds the projection/polygon route.

This does not establish that `$C427AD` owns all of `$C3B720`, or that every
local triple is a Cartesian model vertex.  It does establish that the
two-word placement payload and the later three-word renderer component are
different formats and must not be decoded as one common coordinate table.

## Result

The best-supported current model is:

```
immutable terrain template: header + (u, v)
        -> mutable placement cache: (X, 0, Z) + descriptor
        -> selected renderer control/component: local (a, b, c) triples
```

Thus the user's planar-terrain hypothesis is supported for the sampled
terrain/map **placement source**.  It does not prove that flight-world terrain
has no elevation everywhere, nor does it justify discarding the third value
from renderer-local geometry.

Authority: [run037 template-to-placement handoff](run037_m_map_template_placement_handoff.md),
[M-map packet source dimensionality](m_map_packet_source_dimensionality.md),
[static component boundary](c3b720_c3b6b0_static_component_boundary.md), and
[the `$C44970` target-to-polygon trace](run037_c44970_template_to_polygon_path.md).
