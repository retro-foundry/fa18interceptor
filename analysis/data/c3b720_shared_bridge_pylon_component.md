# `$C3B720-$C3B73D`: shared bridge-scene pylon component

Classification: **trace-proven five-triple input and four static triangular
faces**.  It is an exportable partial bridge-scene component, not a complete
landmark mesh.

The run033 frame-5,250 red Golden Gate checkpoint enters `$C1F4AC` with
`A1=$C3B720` and transforms five consecutive triples.  `$C1F6F8` is then
reached with `A1=$C3B73E` and `A3=$C483AE`, fixing the direct input boundary at
`$C3B720-$C3B73D` (five six-byte triples).  The following bytes are renderer
control data, not a sixth vertex.

The same bounded trace enters `$C2005C` four times under static controller
`$C3B6B0`, with records `$C3B6BA`, `$C3B6C6`, `$C3B6E0`, and `$C3B6FA`.
Their offset lists map exactly to source slots:

| Record | Slot indices |
| --- | --- |
| `$C3B6BA` | 0, 1, 4 |
| `$C3B6C6` | 1, 2, 4 |
| `$C3B6E0` | 2, 3, 4 |
| `$C3B6FA` | 3, 0, 4 |

This proves four triangular sides around slot 4.  No record selecting a base
polygon was observed, so the [local-coordinate sheet](../plots/c3b720_shared_bridge_pylon_static_topology.png)
does not invent one.  The component is active in the user-identified red
Golden Gate window and is kept semantically unnamed despite its pylon-like
topology.

The source triples and control contract are available in
[the portable payload](c3b720_shared_bridge_pylon_static_payload.json).

Authorities:

- `build/run033_frame05250_c3b720_following_trace/trace.jsonl`;
- `build/run033_frame05250_face_preparations/face_preparations.json`;
- `build/run033_frame05250_checkpoint/slow.bin`.
