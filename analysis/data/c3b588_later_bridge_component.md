# `$C3B588-$C3B5A5`: later-bridge component input boundary

Classification: **trace-proven five-triple transform input with two static
renderer face/control families**.  This is an exportable component boundary,
not a complete later-bridge reconstruction.

The alternate matrix entry `$C1F4AC` reads five consecutive triples beginning
at `$C3B588`, then reaches `$C1F6F8` with `A1=$C3B5A6` and
`A3=$C483AE`.  Thus `$C3B588-$C3B5A5` is exactly the five-input run (30 bytes);
the following bytes are control data, not a sixth coordinate.

The bytes are identical in the sealed run031 frame-14,500 checkpoint and all
four sampled run033 bridge checkpoints (5,250 / 5,500 / 6,000 / 6,250), and
the direct triples are exported in
[the portable payload](c3b588_later_bridge_component_static_payload.json).

## Bounded renderer association

`collect_matrix_instance_geometry.py` preserves each render interval from one
`$C1F4AC` entry up to the next.  Its four `$C3B588` occurrences show two
different static consumers of this same five-slot transform workspace:

| Occurrence | Static renderer context | Observed output |
| ---: | --- | --- |
| 1, 3 | `$C3B50A` | one four-point polygon |
| 2, 4 | `$C3B4FE` | two polygons plus a `$C212B0` two-segment line record |

Focused no-input traces confirm the ownership on the two alternate instances:

- occurrence 1 reaches `$C2005C` with `A5=$C3B50A`, `A2=$C3B574`, then
  final polygon submission `$C2FF48`;
- occurrence 2 reaches `$C2005C` four times with `A5=$C3B4FE` and static
  records `$C3B516`, `$C3B522`, `$C3B53C`, and `$C3B556`, then reaches
  `$C2FF48` and `$C212B0` with `A2=$C3B568`.

The direct input is therefore connected to both renderer families without
merging their separate face records or mutable `$C48390` poses.  These
families occur in the later-bridge checkpoint; this establishes a drawable
bridge component, but not its full landmark identity, instancing count, or
unseen faces.

[The static orthographic sheet](../plots/c3b588_later_bridge_component_static_topology.png)
maps the five observed face-offset records directly onto the five local input
triples.  It is a five-face pyramid/wedge topology in source coordinates;
that geometric description is evidence-based, while its scene identity is
still intentionally unnamed.

Authorities:

- `build/run031_frame14500_c3b588_{following_trace,occ2_following_trace}/trace.jsonl`;
- `build/run031_frame14500_c3b588_instance_geometry/instance_geometry.json`;
- `analysis/data/c3b588_later_bridge_component_face_topology.json`;
- `build/run031_frame14500_bridge_checkpoint/slow.bin` and the run033
  `frame{05250,05500,06000,06250}` checkpoint `slow.bin` snapshots.
