# Model-data export boundaries

This is the conservative handoff list for separating drawable game geometry
from 68000 code and mutable renderer state. An entry is exportable only where
both its static data boundary and its renderer consumption path are traced.

| Family | Immutable data to retain | Renderer path | Export status |
| --- | --- | --- | --- |
| `$C351xx` / `$C34C06-$C34C48` and `$C34A9A/$C34A9C` face layers | local triples plus Hunk-41 face records | `$C1F100/$C1F21C` emits `$C46228` through `$C45BC6` and `$C48390` through `$C45BD8`; `$C0D384-$C0D521` derives tails; both reach pre-clip renderers | **exportable procedural flight-object template** with two separately transformed renderer layers; retain face-context choices and derivation, not mutable outputs |
| `$C39D2A` / `$C3925C,$C3925E` | 43 static source triples and ten observed face records in the traced candidate block | observed matrix transform -> `$C48390` -> `$C2005C` -> polygon path | exportable long-component candidate topology; carrier, road, and bridge identities remain unproven |
| Golden Gate batches `$C35932`, `$C3B720` | separately transformed immutable source batches and their record streams | batch transforms -> `$C355D8/$C355D6/$C3B6B0` face families | exportable **components**, not a single complete bridge mesh |
| `$C362A2` / `$C36298` | static source candidate and face stream | observed alternate transform -> final polygon submission | exportable unnamed bridge-like component candidate |

## Renderer families not yet exportable as models

- `$C34A9A/$C34A9C` is no longer in this category: its complete 0--39 slot
  construction is traced from `$C351xx` plus `$C0D384`; see
  [the full face-family evidence](c34a_full_flight_object_face_family.md).
- `$C3A94C/$C3A94E` and `$C38F98`: renderer-resident face/control families;
  their upstream immutable vertex sources are not established.

## Runtime data explicitly excluded

- `$C45BEA`: mutable transform/display context, not polygon or vertex data.
- `$C46228-$C46301`, `$C48390`, `$C4BF90`, and `$C4E910`: transformed or
  pre-clip workspaces. Capture them only to reproduce a sampled pose; do not
  use them as model-source exports.

The [model identification catalogue](model_identification_catalog.md) links
each plotted family and its detailed evidence.
