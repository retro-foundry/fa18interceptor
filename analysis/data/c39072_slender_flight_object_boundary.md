# `$C39072-$C390EF`: slender flight-object input boundary

Classification: **trace-proven 21-triple immutable transform input with a
bounded `$C38F98` renderer-family association; probable slender flight object.**

At the sealed run031 frame-7,500 external-camera checkpoint, `$C1F100`
enters with `A1=$C39072` and `A3=$C48390`.  The subsequent `$C1F6F8`
record-walker entry has `A1=$C390F0`, `A3=$C48408`, and `A5=$C34A9C`.
That fixes `$C39072-$C390EF` as 21 six-byte static triples and excludes
`$C390F0` as the next control pointer.  The `A5=$C34A9C` value is the
record-walker entry state, not by itself a face-family contract.

The preserved [portable payload](c39072_slender_flight_object_static_payload.json)
is intentionally coordinates-only.  Its points describe a thin body along
local Z (from -1530 to +486) with transverse groups at radii 22, 103, and
126.  That supports a **missile/slender-flight-object visual candidate**, but
is not a decoded game object name.

The source-bounded geometry collector supplies the downstream association that
the focused instruction trace did not reach: all four observed `$C39072`
instances submit the eleven polygon records of the static `$C38F98` family
before the next matrix input begins (normally `$C39D2A`).  The first three
instances also contain unrelated later renderer work, so only those eleven
`context_a5=$C38F98` polygons are attributed to this source.  This corrects
the initial apparent `$C34A9x` association.  It does not yet prove a static
slot-to-face index map, because `$C38F98` records can select mutable bases;
do not serialize `$C48390` or borrow the F/A-18 `$C34A9x` topology.

Authorities:

- `build/run031_frame7500_c1f100_matrix_inventory/matrix_transform_entries.json`;
- `build/run031_frame7500_c39072_following_trace/{trace.jsonl,trace_summary.json}`;
- `build/run031_frame7500_c39072_instance_geometry/instance_geometry.json`;
- `build/run031_frame7500_external_checkpoint/slow.bin`.
