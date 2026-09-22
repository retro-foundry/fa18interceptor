# `$C3A94C/$C3A94E`: compact polyhedral renderer family

This family appears at the frame-12600 Golden Gate checkpoint. Its orthographic
sheet is a six-quad compact solid, roughly a tapered box/wedge in the sampled
orientation. No object name is assigned.

[Open the pre-cull orthographic sheet](../plots/frame12600_c3a94e_preclip_face_sheet.png).

## Renderer boundary

- The live record walker enters control stream `$C3A94C` four times during the
  checkpoint capture.
- The stream dispatches through `$C1F942` to `$C2005C`; all six observed faces
  return to `$C1F944`.
- The face controller is `$C3A94E`. Its distinct face-offset records begin at
  `$C3ABBA`, `$C3AC1A`, `$C3AC26`, `$C3AC74`, `$C3ACA0`, and `$C3ACAC`.
  Every observed face is a four-point polygon, yielding 24 renderer-observed
  polygon edges.
- `$C2005C` reads the points from the mutable `$C48390` transformed-vertex
  workspace. Consequently the sheet recovers topology before culling, but not
  immutable source coordinates.
- The bounded control trace enters `$C1F6F8` with `A1=$C3A986`,
  `A3=$C483A8`, and then loads `A5=$C3A94C`. `$C3A94C` resolves its stream
  base through `$C3A942` to `$C3A958`; that stream dispatches face control at
  `$C3ABB2`. The selected handler `$C20EC4` additionally writes derived
  workspace values at `$30(A3)`. Thus `$C483A8` is already a mutable input to
  the family and `$C20EC4` is a downstream workspace derivation, not its
  immutable model source.

## Status

The control stream and face records are a distinct, separable renderer input
family, not code. Their upstream immutable vertex source and game-object
ownership remain untraced. Treat this as a compact polyhedral component
candidate rather than a named model export. The next valid source trace must
find the writer that populates `$C483A8` before this `$C1F6F8` entry; tracing
`$C3A942`, `$C3A958`, or `$C20EC4` backwards alone would only rediscover
control or derived workspace state.

The nearby `$C3513C` dual-lane transform is explicitly excluded as that
writer: across eight sampled invocations it enters with `A5=$C3B9D6` (then two
with `$C3B9D0`), never `$C3A94C`, and its bounded follow-on trace reaches
`$C1F6F8` with `A1=$C3515A`, `A3=$C483A8`, and the C3B9 controller context.
Shared workspace position alone is therefore not source provenance.

An exact CPU write watch on `$C483A8` is an observed miss for 64 ordinary
frames from the sealed frame-12600 external checkpoint. The compact controller
can therefore reuse a pre-existing workspace record during that interval; its
first writer predates the checkpoint or belongs to an unobserved initialization
route. The next capture must begin before scene/entity initialization, not
inside this already-populated renderer loop.
