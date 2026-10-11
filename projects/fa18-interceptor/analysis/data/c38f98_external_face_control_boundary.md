# `$C38F98`: external-scene face-control boundary

`$C38F98` is a byte-stable Hunk-51 renderer-control family, not an immutable
coordinate table.  At the sealed run031 frame-7,500 external-camera
checkpoint, pre-cull collection at `$C2005C` observes this same context on four
render passes.  The passes select these eleven static face records:

| Face record | Role established by the trace |
| --- | --- |
| `$C38FA6` | first record after the control header |
| `$C38FB0`, `$C38FBA`, `$C38FC6`, `$C38FD2` | renderer-observed face records |
| `$C38FDC`, `$C38FE6`, `$C38FF0`, `$C38FFA` | renderer-observed face records |
| `$C39004`, `$C3900E` | final renderer-observed face records |

The first invocation has `A2=$C38FA6` and `A3=$C38F92`; later records use the
same static `A2` addresses but select transient bases such as `$C45970` and
other per-pass workspace values.  The eleven unique records make the 11-face,
36-edge pre-cull sheet reproducible as renderer topology, but **not** as a
static model export.

The parent record walk enters `$C1F6F8` with `$C45A36=$C384CC` during the same
external-camera replay.  A no-future-input trace confirms that `$C384CC` is
the upstream control stream; it does not establish a first writer for the
selected triple bases.  In particular, `$C48390`, `$C45970`, and the sampled
`A3` values remain mutable renderer state and are excluded from source data.

Nearby `$C38E2C-$C38E4F` and `$C38E54` are independently trace-proven static
matrix-input packets, but their focused traces only establish the `$C39260` /
`$C384CC` path.  They remain [separate transform-packet evidence](c38e_external_transform_packets.md).

`$C39072-$C390EF` is the first bounded source association for this family:
the source-bounded collector sees exactly eleven `$C38F98` polygons after
each of four `$C39072` input transforms and before the next matrix input.
That associates the immutable 21-triple packet with this renderer family,
while retaining the caveat that the individual face records can use mutable
workspace bases.  The resulting payload is therefore a source packet plus
renderer-family contract, not a flattened static mesh.

## Status

Retain `$C38F98-$C3900E` as static **face/control data** and retain the
renderer record-walker path when recreating this draw family.  Do not export
it as a standalone model until a transform writer is traced from an immutable
source range to the slots selected by these records.  No object identity is
assigned.

Authority: `build/run031_frame7500_external_face_preparations_64f/face_preparations.json`
and `build/run031_frame7500_c384cc_stream_trace/{control_stream_entries.json,selected_stream_trace.jsonl}`;
the source association is in
`build/run031_frame7500_c39072_instance_geometry/instance_geometry.json`.
