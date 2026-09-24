# Model-data export boundaries

This is the conservative handoff list for separating drawable game geometry
from 68000 code and mutable renderer state. An entry is exportable only where
both its static data boundary and its renderer consumption path are traced.

| Family | Immutable data to retain | Renderer path | Export status |
| --- | --- | --- | --- |
| `$C351xx` / `$C34C06-$C34C48` and `$C34A9A/$C34A9C` face layers | local triples plus Hunk-41 face records | `$C1F100/$C1F21C` emits `$C46228` through `$C45BC6` and `$C48390` through `$C45BD8`; `$C0D384-$C0D521` derives tails; both reach pre-clip renderers | **exportable procedural flight-object template** with two separately transformed renderer layers; retain face-context choices and derivation, not mutable outputs |
| `$C39D2A` / `$C3925C,$C3925E` | 43 static source triples and ten observed face records in the traced candidate block | observed matrix transform -> `$C48390` -> `$C2005C` -> polygon path | exportable tall bridge-pylon/deck candidate topology; semantic name and instancing remain unproven |
| `$C3A94C/$C3A94E` | four immutable triples at `$C3A96E-$C3A985`, static stream/face records, and six observed quads | `$C1F4AC/$C1F528` -> `$C48390` slots 0--3; `$C20EC4` derives slots 4--9 -> `$C2005C` | exportable procedural compact polyhedral component; its [manifest](c3a94c_compact_polyhedron_export_manifest.json) and [portable direct-input payload](c3a94c_compact_polyhedron_static_payload.json) exclude the workspace |
| `$C363EC/$C3628A` | four static triples plus `$C366D4` two-segment line record; one source-bounded finalized five-vertex face has no immutable face-record boundary yet | `$C1F4AC` -> `$C48390` slots 0--3 -> `$C212B0`; run034 also reaches `$C2469E -> $C2FF48` | exportable unnamed close-range **line** component with an associated filled-face observation; retain the source triples and line record, but do not export the mutable face workspace as topology. [Trace, payload, and face sheet](c363ec_c3628a_close_range_line_component.md) retain the boundary without assigning an LOD role |
| `$C3BE4C/$C3BBF6` | five static triples, `$C20F10-$C20F76` derived slots 5--8, and five observed quad records | `$C1F4AC/$C1F528` -> `$C48390` -> `$C3BBF4` control stream -> `$C20F10` -> `$C2005C` | exportable unnamed procedural scene component in the run034 retreat checkpoint; [source/control/topology evidence](c3be4c_run034_context_selection.md) explicitly rejects an LOD claim |
| Golden Gate `$C1F4AC` batches | separately transformed immutable source batches and their record streams; raw/mixed boundaries are inventoried | batch transforms -> `$C355D8/$C355D6/$C3B6B0` face families | exportable **components**, not a single complete bridge mesh; see [packet inventory](c1f4ac_golden_gate_source_packet_inventory.md) |
| `$C362A2` / `$C36298` | static source candidate and face stream | observed alternate transform -> final polygon submission | exportable unnamed bridge-like component candidate |

## Renderer families not yet exportable as models

- `$C34A9A/$C34A9C` is no longer in this category: its complete 0--39 slot
  construction is traced from `$C351xx` plus `$C0D384`; see
  [the full face-family evidence](c34a_full_flight_object_face_family.md).
- `$C38F98`: renderer-resident [face/control family](c38f98_external_face_control_boundary.md);
  its upstream immutable vertex source is not established.

## Runtime data explicitly excluded

- `$C45BEA`: mutable transform/display context, not polygon or vertex data.
- `$C46228-$C46301`, `$C48390`, `$C4BF90`, and `$C4E910`: transformed or
  pre-clip workspaces. Capture them only to reproduce a sampled pose; do not
  use them as model-source exports.

The [model identification catalogue](model_identification_catalog.md) links
each plotted family and its detailed evidence.
