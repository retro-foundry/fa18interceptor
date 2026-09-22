# Renderer model extraction status

Updated: 2026-09-22.

## Flight object and `$C45BEA`

`$C45BEA` is not model data. It is mutable display state used while the five
`$C34C06-$C34C48` detail faces enter the pre-clip renderer. The complete
procedural flight-object boundary is `$C3515E -> $C34A9A/$C34A9C`:

- 22 immutable coordinate triples;
- 22 distinct static face records / 82 observed polygon edges;
- direct slots 0--21 plus code-derived slots 22--39;
- separate C34A and C34C face layers with separate transform lanes.

Use [the payload](data/c351_flight_object_static_payload.json) and
[the export manifest](data/c351_flight_object_export_manifest.json). Do not
export `$C48390`, `$C46228`, or `$C45BEA` as source geometry.

The complete family is visibly F/A-18-like in the external-camera oracle.
That is visual identification, not a decoded original name; the C34C detail
layer is not proven to be a missile or an LOD mesh.

## Golden Gate pylon/deck candidate

`$C39D2A -> $C3925C/$C3925E` is separately extractable as 43 static triples
and ten observed faces. Its tall source aspect ratio and upright-block plus
deck topology favour a bridge pylon/deck component over the former carrier
interpretation. Semantic name and instancing are still unproven. See
[the candidate evidence](data/c39d2a_c3925_static_model_candidate.md) and
[static payload](data/c39d2a_c3925_static_payload.json).

## Remaining bridge batches

The `$C1F4AC` route reuses `$C48390` and mixes raw source triples with control
packets. Consult [the packet inventory](data/c1f4ac_golden_gate_source_packet_inventory.md)
before extracting any of its inputs. In particular, `$C35932` and `$C361E4`
are mixed packets, while `$C3AD0E`, `$C3B0CE`, `$C3B720`, `$C3B9B2`, and
`$C3A96E` are direct triple blocks for the sampled route.

[Close-range Golden Gate evidence](data/golden_gate_close_range_lod_probe.md)
now covers the overhead frame-11,850 and low-altitude frame-11,925 views.
The same `$C355D8` face family remains active in both, so no simple
close-range LOD-family replacement is observed.

The user-identified red Golden Gate view in run033 frame 5,250 adds a
distance-specific renderer fact: `$C3559A` and `$C355D2` submit 24 total
`$C212B0` line segments through static record `$C358B2` (selector 1; workspace
offsets 0 and 6).  The endpoints are selected from mutable `$C48390`; retain
the static record as bridge line topology but do not export those workspace
coordinates as source geometry.  The co-visible `$C3B6B0` and `$C3B504`
polygons remain separately classified terrain/pyramid components.  See the
[line-family evidence](data/c35932_c355d8_bridge_model_candidate.md).

## Other separated external component

`$C39072-$C390EF` is a 21-triple immutable slender-flight-object input.  A
source-bounded capture associates each observed instance with the eleven
`$C38F98` external-scene polygons before the next matrix input begins.  The
face records can still select mutable bases, so this is an associated source
packet plus renderer family—not a flattened static-mesh export.  See
[its boundary](data/c39072_slender_flight_object_boundary.md) and the
[`$C38F98` contract](data/c38f98_external_face_control_boundary.md).
