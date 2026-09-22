# `$C39D2A → C3925C/C3925E`: static-model candidate

Classification: **trace-proven static vertex transform followed by renderer polygon contexts; long-component visual identity unresolved**.

The 43 immutable triples and ten renderer-observed faces are separately
extractable as [`c39d2a_c3925_static_payload.json`](c39d2a_c3925_static_payload.json).
Regenerate that payload with:

```powershell
python scripts\export_static_topology_payload.py --topology analysis\data\c39d2a_c3925_face_topology.json --slow build\run031_frame12000_golden_gate_checkpoint\slow.bin --output analysis\data\c39d2a_c3925_static_payload.json
```

At the Golden Gate checkpoint, `build/run031_frame12000_c39d2a_following_trace/trace.jsonl` begins at `$C1F100` with `A1=$C39D2A`. The same transform pass consumes 43 consecutive source triples in the `$C39Dxx` range. It then enters the renderer walker, selecting static contexts `$C3925C` and `$C3925E`; their face records reach `$C2469E` and `$C24CFE`.

The independent external-camera frame-7,500 final-polygon collector records three `$C3925C` and three `$C3925E` submissions. The pre-cull face collector observes five unique static face-record addresses in each context; [the complete pre-cull PNG sheet](../plots/c39d2a_c3925_preclip_complete_face_sheet.png) draws all ten selected faces. It is deliberately named a candidate: the Golden checkpoint trace proves the source-to-face contexts, while the external checkpoint supplies the transformed face geometry under a different camera/scenario.

The source set is substantially larger-scale than the `$C3515E` flight-object candidate. Its source-coordinate spans are X=3520, Y=2336, and Z=14976: it is a tall component, not a broad flat hull. The five `$C3925E` faces form the observed sides/end of a narrow eight-vertex upright block (indices 31--38), while the `$C3925C` records provide a lower slab/deck-like group (indices 0--20). In the Golden Gate context, a **bridge pylon/tower plus deck component** is therefore the best current visual reading.

This remains an identification hypothesis rather than a decoded object name.
[The bounded cadence capture](c39d2a_transform_cadence.md) observes one
`$C39D2A` transform per simulation update, so it does not establish whether
another controller instantiates a mirrored tower or repeated road segment. It
does, however, make an aircraft-carrier interpretation substantially less
consistent with the recovered aspect ratio and observed topology.

## External-camera correlation and limit

The sealed frame-7,500 external-camera replay now supplies that same-window
source correlation: its `$C1F100` inventory contains six `$C39D2A`
transforms, and the first bounded source trace reaches `$C1F6F8` with
`A1=$C39E2C`, `A3=$C4848C`, and `A5=$C3B6B0`.  Thus the 43-triple source is
live in the external-aircraft view and then dispatches through `$C3B6B0`.

That is still **not an object-name proof**.  `$C3B6B0` is a reusable
four-triangle controller: the separate `$C3B720` five-triple terrain/mountain
component also dispatches through it.  Controller reuse means this observation
does not merge `$C39D2A` with that mountain component, nor does it establish
that the full 43-triple source is the player aircraft.  Keep the external-view
correlation, the Golden Gate `$C3925x` face evidence, and the visual bridge
reading as distinct evidence; do not rename the component as a carrier,
bridge tower, or aircraft solely from the screenshot.

Authorities: `build/run031_frame7500_c1f100_matrix_inventory/` and
`build/run031_frame7500_c39d2a_following_trace/` (sealed replay), plus the
previous Golden Gate bounded trace cited above.

[The static-coordinate topology candidate (SVG)](../plots/c39d2a_c3925_static_topology_candidate.svg) and [PNG sheet](../plots/c39d2a_c3925_static_topology_candidate.png) bind the ten observed face-offset lists directly to the 43 traced `$C39D2A` triples. They retain only those renderer-observed edges and make no extra deck, hull, island, or road connections.
