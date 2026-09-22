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
| Is there a flat placement layer? | Yes, for the sampled runtime scene-placement records.  Every descriptor-qualified 24-byte record in three run033 checkpoints has a zero middle coordinate word, with signed first/third words varying across the X/Z diagnostic. | [runtime placements](data/runtime_scene_placements.md) |
| Is that layer live data rather than an immutable mesh? | Yes.  A traced `$C1DC1C-$C1E0B0` bulk pass writes successive 24-byte placement records through `A2`; it precedes the `$C1CB74` selector. | [placement record builder](routines/c1dc1c_scene_placement_record_builder.md) |
| What feeds the placement builder's workspace scan? | The traced update sequence resets leading cell words to `$FFFF` at `$60`-byte stride between captured builder passes; `$C1D330-$C1D3E6` traverses `$C48390` in `$600`-byte bands and enters the next builder pass.  This proves mutable-cell lifecycle, not the original source. | [band-walk contract](routines/c1d330_scene_workspace_band_walk.md) |
| Is a static input known for that traversal? | Yes: the active cursor resolves to byte-stable, relocation-free segment 65 at `$C412EC`.  Its byte controls, combined with live row terms, select static template groups for workspace bands; no terrain-coordinate read from it is proven. | [band selector stream](data/workspace_band_selector_stream.md) |
| Where do the live row terms come from? | `$C1D10C` loads one of two mutable selector-context packs into its frame before the shared band walk. Both have traced/static writers from the current `$C46184` control record selected by `$C458DE`; the first can instead use `$C45C3E/$C45C46` under an alternate mode. Their spatial meaning remains unproven. | [selector-context setup](routines/c1d10c_scene_selector_context.md) |
| Is the template stream itself statically selected? | Yes.  `$C1D3F4` resolves relative offsets from `$C42390` to static group records, passes an observed bit gate, and selects static template streams that enter `$C1D442`.  The index is not yet tied to player position or distance. | [static selector groups](data/static_template_selector_groups.md) |
| Does a static path reach coordinate-bearing placements? | Yes.  The trace copies 37 static entries from verified segments 66--67 into cells; 22 are later read by the builder and emitted as descriptor-qualified three-word placements before the trace ends.  In this joined sample every middle word is zero.  This is still not a raw global-coordinate or mesh record. | [copy inventory and X/Z diagnostic](data/workspace_template_copies.md) |
| Does that static-to-placement path repeat across updates? | Yes. A 30-frame extension observes two further refresh phases: 143 copies from segments 66--67, 101 later builder reads, and 101/101 zero middle output words. It is stronger flat-placement evidence, but remains a bounded workspace path rather than a full terrain export. | [extended copy inventory and X/Z diagnostic](data/workspace_template_copies_404_426.md) |
| Are segment-66/67 entries fixed global map positions? | No. Seven identical static entries reach the builder in both measured windows but emit different X/Z tuples. They are reusable templates combined with mutable placement context, so extracting their two words as a world-coordinate table would be wrong. | [refresh-window comparison](data/placement_refresh_window_comparison.md) |
| Does a live origin select terrain-template pages? | Yes. A same-breakpoint control/mutation pair changes only the `$C45C3E/$C45C46` origin values. The selected set drops from 106 to its 60-entry subset and common template outputs shift. This proves causal paging, not the original coordinate-to-page table. | [origin mutation probe](data/origin_selector_mutation_probe.md) |
| Is the terrain paging two-dimensional? | Yes. Independent +1 selector-bin probes shift the dominant common placement outputs by `(-512, 0)` and `(0, -512)` respectively; the combined probe composes to `(-512, -512)`. This proves a 512-unit cache-page lattice, not an absolute world-unit conversion. | [axis probe](data/origin_selector_mutation_probe.md) |
| Where does that live origin come from? | `$C29042` derives it through the active `$C46184+$C458DE` control record's matrix/transform path, stores all three components at `$C45C3E/$C45C42/$C45C46`, and later adjusts them through an accumulator. It is live derived state, not a static template copy. | [active-origin update](routines/c29042_active_origin_update.md) |
| Are the renderer's projected triples the source map? | No. `$C45630-$C48383` and `$C48390-$C4E76B` are mutable workspaces; static source extraction from either would be wrong. | [geometry boundary report](model_geometry_boundaries.md) |
| Does `M` identify map data? | Not yet. Raw `$37` reaches `$C1BF8C`, sets a request bit, and enters a long helper.  The post-helper static tail initializes display-transition state, but the helper has no completed return trace and no traced asset/data consumer. | [`M` command contract](routines/c1bf8c_map_command.md) |

## Flatness is not yet a data invariant

The sampled runtime placement layer supplies positive, scenario-backed support
for a flat world plane: its middle coordinate word is zero for all 121, 123,
and 123 descriptor-qualified records at the three run033 checkpoints.  This
does not yet establish the original static source table, prove that this word
is a global height axis under every game mode, or rule out separate terrain
geometry.  Renderer-observed local triples from scene-family inputs still
contain nonzero values in all three stored components.  Those values may be
model-local coordinates, transformed scene inputs, or another coordinate
convention; they must not be conflated with placement height.

Consequently, a map export must retain all three stored components until a
producer-to-consumer trace proves which components encode global placement and
which (if any) encode elevation.

## LOD result

The three matched Golden Gate windows retain the same core `$C355D8` static
face family at both close samples and at the standard checkpoint.  This rejects
a *simple whole-family close-range replacement* for that one landmark and
scenario.  It does not reject culling, clipping, per-instance detail, or a
different range selector elsewhere in the world.

The terrain-placement builder does contain a dynamic output-shift table: it
chooses a shift from live magnitude terms before emitting placement words.
This is adaptive coordinate precision/range handling, not LOD evidence, since
no different mesh, face list, or template topology is selected by that path.
The same static template has observed shift counts 7 and 2 in two replay
windows.  It further rules out decoding template words or final cache tuples
as a fixed global-position table.  See the
[placement builder's shift boundary](routines/c1dc1c_scene_placement_record_builder.md).

See the measured face/input overlap and its limits in the
[close-range LOD probe](data/golden_gate_close_range_lod_probe.md).

## Next evidence required to find the authoritative map

1. Capture a deterministic `M`-entry and map-exit scenario with screenshots,
   final RAM, and a bounded no-input trace after the long helper completes.
2. Trace the first selector pack (`$C45948/$C4594A`) and run a controlled
   player/scene-position experiment that correlates changed static sources
   with changed placement records; `$C458DE` is already established as a
   current control-record offset, not a terrain-coordinate proof.
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
