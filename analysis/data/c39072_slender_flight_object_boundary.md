# `$C39072-$C390EF`: slender flight-object input boundary

Classification: **trace-proven 21-triple immutable transform input; probable
missile/slender-flight-object geometry, with renderer topology not yet joined.**

At the sealed run031 frame-7,500 external-camera checkpoint, `$C1F100`
enters with `A1=$C39072` and `A3=$C48390`.  The subsequent `$C1F6F8`
record-walker entry has `A1=$C390F0`, `A3=$C48408`, and `A5=$C34A9C`.
That fixes `$C39072-$C390EF` as 21 six-byte static triples and excludes
`$C390F0` as the next control pointer.  The same sealed 64-frame inventory
observes this input four times, initially under `$C34A9C` and then under
`$C34A9A`.

The preserved [portable payload](c39072_slender_flight_object_static_payload.json)
is intentionally coordinates-only.  Its points describe a thin body along
local Z (from -1530 to +486) with transverse groups at radii 22, 103, and
126.  That supports a **missile/slender-flight-object visual candidate**, but
is not a decoded game object name.  Although it dispatches to the static
`$C34A9x` face-controller family, the focused trace does not reach individual
`$C2005C` face records before its 30,000-instruction cap.  Do not assign the
full F/A-18 face topology, or serialize `$C48390`, to this source until a
bounded source-to-face trace supplies that slot contract.

Authorities:

- `build/run031_frame7500_c1f100_matrix_inventory/matrix_transform_entries.json`;
- `build/run031_frame7500_c39072_following_trace/{trace.jsonl,trace_summary.json}`;
- `build/run031_frame7500_external_checkpoint/slow.bin`.
