# `$C3515E -> $C34C06-$C34C48`: reduced flight-object face-layer candidate

[Open the orthographic sheet](../plots/c34c_aircraft_detail_preclip_unique_face_sheet.png).

[Open the orthographic + isometric sheet](../plots/c3515e_c34c_aircraft_detail_isometric_sheet.png).

[Open the static-coordinate topology candidate](../plots/c351_c34c_static_topology_candidate.svg).

[Open the static-coordinate topology PNG sheet](../plots/c351_c34c_static_topology_candidate.png).

[Open the complete frame-1966 renderer-workspace topology sheet](../plots/c351_c34c_workspace_topology_frame1966.png).

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

## Completeness boundary

`$C2035A` reads the actual static face offsets against `$C46228`. The five
faces reference vertex indices `0` through `35` (36 slots total). The bounded
`$C3515E` transform supplies slots `0` through `21` only, ending at
`$C462AC`. Slots `22` through `35` first initialize together at replay frame
1966. Their direct writer is now traced: `$C0D384-$C0D521` calculates and
stores the tail from earlier `$C46228` workspace values, beginning with
`$C0D396: movem.w d3-d5,$84(a3)` when `A3=$C46228`. See [the routine evidence](../routines/c0d384_derived_vertex_tail.md).
The contiguous `$C351E2-$C35234` triples have a related numerical pattern but
are not the observed input of that writer.

This model has renderer-proven topology and a partial static-vertex path, but
is not yet a complete source-model export. The fourteen unresolved indices
align with the contiguous static `$C351E2-$C35234` triple run, making that
range the next source-trace target rather than inferred geometry. The offset collector is
`scripts/collect_c203_face_indices.py` and its frame-12000 report is the
authoritative topology capture.

The `$C34A9A/$C34A9C` polygon family is not merged into this model: it uses
the separate `$C48390` workspace in the sampled frame. Similar silhouette and
shared Hunk residency are insufficient to claim a shared object instance.
They are nevertheless [co-rendered in one composite candidate pass](c34c_c34a_co_rendered_composite.md): that association is useful for visual identification, while the separate immutable-source boundaries remain mandatory.

## Reduced-detail interpretation

The shared `$C351xx` transform path means C34C and C34A reuse one flight-
object template, while the C34C layer has only five observed faces compared
with 42 for C34A. The independent external-camera frame shows an unmistakably
F/A-18-like aircraft when the C34A family is active. Together with the
spatially separate C34C instance at frame 12000, this supports a **reduced-
detail alternate layer for another flight-object instance**. It is not proof
of a distance/LOD selector, and it does not support naming C34C a missile.
