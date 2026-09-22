# `$C3515E -> $C34A9A/$C34A9C`: full flight-object face family

Classification: **trace-proven procedural source-to-face model path**.

Machine-readable separation contract:
[`c351_flight_object_export_manifest.json`](c351_flight_object_export_manifest.json).

At the frame-7,500 external-camera checkpoint, the two related object-control
contexts `$C34A9A` and `$C34A9C` submit 21 pre-cull records each through
`$C2005C`.  They share 20 records; `$C34A9A` uniquely submits `$C34D02` and
`$C34A9C` uniquely submits `$C34D0E`.  There are therefore **22 distinct
static face records** and 82 observed polygon edges, not 42 distinct faces.
All records are in the byte-stable Hunk-41 payload from `$C34C8A` through
`$C34E66`.

Their offsets address vertex slots 0 through 39 in the mutable `$C48390`
workspace (slot 16 is simply not used by these observed faces).  The complete
construction path is now bounded:

- `$C1F100/$C1F21C` reads the immutable `$C3515E-$C351DC` triple stream and
  produces slots 0--21 in both the C34C and C34A lanes; `$C45BD8` supplies the
  C34A-lane transform.
- The frame-1965 `$C0D384` trace then operates with `A3=$C48390`.  Its stores
  cover the tail slots 22--39, including paired writes at offsets `$9C`,
  `$B4`, `$C0`, `$CC`, and `$E4`.  The before/after workspace snapshots show
  every tail slot used by the C34A faces changing during that invocation.
- `$C2005C` resolves the static face offsets against this completed C34A
  workspace before orientation and clipping.  The external-camera collector
  is the topology authority; the triples it records are view-dependent output,
  not immutable vertices.

This removes the prior "source linkage pending" boundary for this family:
the source coordinates, derived-tail procedure, static faces, and renderer
consumer are all connected.  An export must retain the `$C351xx` input,
`$C0D384-$C0D521` derivation, the two coordinate transforms, and the separate
`$C34A9A/$C34A9C` face-context choices.  It must not serialize `$C48390` as
source geometry.

The independent external-camera oracle makes the complete family visibly
F/A-18-like.  That is visual identification evidence, not a decoded original
object name; neither this trace nor the C34C admission gate proves an LOD rule
or a missile identity.

Authorities:

- `build/run031_frame7500_external_face_preparations_64f/face_preparations.json`
- `build/run031_frame12000_c3515e_transform_trace/trace.jsonl`
- `build/run031_frame1965_c0d396_tail_derivation_trace/{trace.jsonl,slow.bin,final_slow.bin}`
