# `$C3515E -> $C34C06-$C34C48`: external-aircraft detail model

[Open the orthographic sheet](../plots/c34c_aircraft_detail_preclip_unique_face_sheet.png).

At the Golden Gate checkpoint, five distinct records in this range reach the
pre-clip polygon routine through `$C203C4`. They contain 26 total polygon
edges and are rendered with `$C45BEA` as the active transform/display context.
The tight-fit plots show a tapered fuselage/nose-like component. The visual
name remains provisional, but the source-to-face model path is traced.

The sampled records are `$C34C06`, `$C34C18`, `$C34C2A`, `$C34C38`, and
`$C34C48`. They repeat unchanged as a family over the capture. `$C45BEA` is
not the face source: it is the mutable context associated with their current
transformed display. The coordinates captured at `$C2469E` are likewise
transformed workspace coordinates, so this establishes renderer topology and
component ownership—not immutable vertex coordinates.

The face records reside in the byte-stable `$C34A50-$C3555F` Hunk-41 payload,
the same loaded data region that contains `$C3515E`. The frame-12,000 bounded
trace proves the stronger link: `$C1F100` begins with `A1=$C3515E` and
`A0=$C46228`; 22 transformed triples fill `$C46228-$C462A6`. `$C2035A` then
uses `A4=$C46228` while stepping through the `$C34C` records, and `$C203C4`
calls `$C2469E` for their polygons.

The same 64-frame checkpoint capture observes no `$C212B0` line submission
with `A5=$C45BEA`. The sheet's zero line count is therefore an observed
renderer result, not a missing-line inference.

The `$C34A9A/$C34A9C` polygon family is not merged into this model: it uses
the separate `$C48390` workspace in the sampled frame. Similar silhouette and
shared Hunk residency are insufficient to claim a shared object instance.
