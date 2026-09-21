# `$C34C06-$C34C48`: flight-object detail candidate

[Open the orthographic sheet](../plots/c34c_aircraft_detail_preclip_unique_face_sheet.png).

At the Golden Gate checkpoint, five distinct records in this range reach the
pre-clip polygon routine through `$C203C4`. They contain 26 total polygon
edges and are rendered with `$C45BEA` as the active transform/display context.
The tight-fit plots show a tapered fuselage/nose-like component. This is a
visual candidate only; no object name is assigned.

The sampled records are `$C34C06`, `$C34C18`, `$C34C2A`, `$C34C38`, and
`$C34C48`. They repeat unchanged as a family over the capture. `$C45BEA` is
not the face source: it is the mutable context associated with their current
transformed display. The coordinates captured at `$C2469E` are likewise
transformed workspace coordinates, so this establishes renderer topology and
component ownership—not immutable vertex coordinates.

The face records reside in the byte-stable `$C34A50-$C3555F` Hunk-41 payload,
the same loaded data region that contains the `$C3515E` static aircraft-vertex
candidate. This establishes a shared immutable *resource region*, although a
direct transform trace from `$C3515E` to these five records has not yet been
captured.

The `$C3515E` transform trace and this family occur in the same bounded
rendering pass, but a same-frame overlay shows their captured transformed
coordinates are not co-located. They must therefore remain separate object
instances until a shared transform/output range is directly proved.
