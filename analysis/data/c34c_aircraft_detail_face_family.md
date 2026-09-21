# `$C34C06-$C34C48`: aircraft-detail face family

[Open the orthographic sheet](../plots/c34c_aircraft_detail_preclip_unique_face_sheet.png).

At the Golden Gate checkpoint, five distinct records in this range reach the
pre-clip polygon routine through `$C203C4`. They contain 26 total polygon
edges and are rendered with `$C45BEA` as the active transform/display context.
The tight-fit plots show a tapered fuselage/nose-like component, corroborating
the external-aircraft identification without assigning a new object name.

The sampled records are `$C34C06`, `$C34C18`, `$C34C2A`, `$C34C38`, and
`$C34C48`. They repeat unchanged as a family over the capture. `$C45BEA` is
not the face source: it is the mutable context associated with their current
transformed display. The coordinates captured at `$C2469E` are likewise
transformed workspace coordinates, so this establishes renderer topology and
component ownership—not immutable vertex coordinates.

This family is therefore kept as a linked aircraft component, separate from
the `$C3515E -> $C34A9A/$C34A9C` static-vertex candidate until their shared
upstream source is directly traced.
