# Model-data export boundaries

This is the conservative handoff list for separating drawable game geometry
from 68000 code and mutable renderer state. An entry is exportable only where
both its static data boundary and its renderer consumption path are traced.

| Family | Immutable data to retain | Renderer path | Export status |
| --- | --- | --- | --- |
| `$C3515E-$C351DC` / `$C34C06-$C34C48` | 22 local triples plus five Hunk-41 face records | `$C1F100/$C1F21C` -> `$C46228-$C462A6` -> `$C203C4` -> `$C2469E` | **exportable traced detail model**; five observed polygons, 26 edges, no matching line list in the 64-frame capture |
| `$C39D2A` / `$C3925C,$C3925E` | static source triples and face records in the traced candidate block | observed matrix transform -> `$C48390` -> `$C2005C` -> polygon path | exportable candidate topology; named aircraft-carrier deck/island only as the user's visual identification |
| Golden Gate batches `$C35932`, `$C3B720` | separately transformed immutable source batches and their record streams | batch transforms -> `$C355D8/$C355D6/$C3B6B0` face families | exportable **components**, not a single complete bridge mesh |
| `$C362A2` / `$C36298` | static source candidate and face stream | observed alternate transform -> final polygon submission | exportable unnamed bridge-like component candidate |

## Renderer families not yet exportable as models

- `$C34A9A/$C34A9C`: 42 pre-cull faces are useful topology evidence, but the
  sampled frame reads the separate `$C48390` workspace. At frame 12000 the
  captured points for `$C34A9A` collapse to an X-axis-only state, unlike the
  richer frame-7500 view. Do not attach them to `$C3515E` or export them as
  static coordinates without their own immutable-source trace.
- `$C3A94C/$C3A94E` and `$C38F98`: renderer-resident face/control families;
  their upstream immutable vertex sources are not established.

## Runtime data explicitly excluded

- `$C45BEA`: mutable transform/display context, not polygon or vertex data.
- `$C46228-$C462A6`, `$C48390`, `$C4BF90`, and `$C4E910`: transformed or
  pre-clip workspaces. Capture them only to reproduce a sampled pose; do not
  use them as model-source exports.

The [model identification catalogue](model_identification_catalog.md) links
each plotted family and its detailed evidence.
