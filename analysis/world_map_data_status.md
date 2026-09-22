# World-map and LOD status

This is the evidence boundary for the game's large physical flight area.  It
separates the observed *scene-selection and render inputs* from an as-yet
unidentified authoritative world/map dataset.  No extracted range below is
called the complete world map.

## What is established

| Question | Evidence-backed answer | Authority |
| --- | --- | --- |
| Is there a static scene-selection dataset? | Yes.  The inline `$C223A8-$C227EB` table in original CODE segment 16 contains relocation-backed pointer triplets into scene-family segments 41--50. | [scene pointer triplets](data/c223a8_scene_pointer_triplets.md) |
| Which candidates survive runtime unchanged? | Hunks 41--47, 49, and 50 are byte-stable across the recorded snapshots after complete relocation operands are excluded.  They are immutable scene-family candidates, not reclassified data hunks or complete models. | [geometry boundary report](model_geometry_boundaries.md) |
| Where does the active scene path select a stream? | The capped flight-update packet walks 24-byte records and copies a descriptor's third longword to `$C45A36`; `$C1F6F8` then consumes that control-stream pointer on the projection path. | [`$C1CB14` handoff](routines/c1cb14_flight_update_stage.md) |
| Are the renderer's projected triples the source map? | No. `$C45630-$C48383` and `$C48390-$C4E76B` are mutable workspaces; static source extraction from either would be wrong. | [geometry boundary report](model_geometry_boundaries.md) |
| Does `M` identify map data? | Not yet. Raw `$37` reaches `$C1BF8C`, sets a request bit, and enters a long helper.  The post-helper static tail initializes display-transition state, but the helper has no completed return trace and no traced asset/data consumer. | [`M` command contract](routines/c1bf8c_map_command.md) |

## Flatness is not yet a data invariant

The displayed flight area may be operationally flat, but no current trace
establishes a single world-up axis, a terrain-height field, or an all-zero
height coordinate in the source dataset.  In fact, renderer-observed local
triples from scene-family inputs contain nonzero values in all three stored
components.  Those values may be model-local coordinates, transformed scene
inputs, or another coordinate convention; they must not be used to contradict
or confirm the gameplay-level flatness claim.

Consequently, a map export must retain all three stored components until a
producer-to-consumer trace proves which components encode global placement and
which (if any) encode elevation.

## LOD result

The three matched Golden Gate windows retain the same core `$C355D8` static
face family at both close samples and at the standard checkpoint.  This rejects
a *simple whole-family close-range replacement* for that one landmark and
scenario.  It does not reject culling, clipping, per-instance detail, or a
different range selector elsewhere in the world.

See the measured face/input overlap and its limits in the
[close-range LOD probe](data/golden_gate_close_range_lod_probe.md).

## Next evidence required to find the authoritative map

1. Capture a deterministic `M`-entry and map-exit scenario with screenshots,
   final RAM, and a bounded no-input trace after the long helper completes.
2. Record the first reads of the static scene pointer triplets and correlate
   each selected descriptor with the player/scene position state at that frame.
3. For two widely separated positions, trace the selected static source range
   through the mutable `$C45630`/`$C48390` workspaces to projection.  Compare
   descriptor identity, source triples, and position state.
4. For an LOD claim, hold an instance/camera orientation as constant as
   possible, vary measured distance, and trace the selector plus the chosen
   static family.  Face counts alone are insufficient because culling changes
   them.

Until those conditions are met, the project has a proven scene-control index
and immutable scene-family candidates, not a complete extracted 3D world map
or an LOD table.
