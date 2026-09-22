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
| Is there a flat placement layer? | Yes, for the sampled runtime scene-placement records. Every descriptor-qualified 24-byte record in three run033 checkpoints has a zero middle coordinate word, with signed first/third words varying across the X/Z diagnostic. The stable `M`-map scenario independently carries 103 copied template records through the builder to X/0/Z placements. | [runtime placements](data/runtime_scene_placements.md) and [map-mode handoff](data/run003_m_map_template_placement_handoff.md) |
| Is that layer live data rather than an immutable mesh? | Yes.  A traced `$C1DC1C-$C1E0B0` bulk pass writes successive 24-byte placement records through `A2`; it precedes the `$C1CB74` selector. | [placement record builder](routines/c1dc1c_scene_placement_record_builder.md) |
| What feeds the placement builder's workspace scan? | The traced update sequence resets leading cell words to `$FFFF` at `$60`-byte stride between captured builder passes; `$C1D330-$C1D3E6` traverses `$C48390` in `$600`-byte bands and enters the next builder pass.  This proves mutable-cell lifecycle, not the original source. | [band-walk contract](routines/c1d330_scene_workspace_band_walk.md) |
| Is a static input known for that traversal? | Yes: the active cursor resolves to byte-stable, relocation-free segment 65 at `$C412EC`.  Its byte controls, combined with live row terms, select static template groups for workspace bands; no terrain-coordinate read from it is proven. | [band selector stream](data/workspace_band_selector_stream.md) |
| Where do the live row terms come from? | `$C1D10C` loads one of two mutable selector-context packs into its frame before the shared band walk. Both have traced/static writers from the current `$C46184` control record selected by `$C458DE`; the first can instead use `$C45C3E/$C45C46` under an alternate mode. Their spatial meaning remains unproven. | [selector-context setup](routines/c1d10c_scene_selector_context.md) |
| Is the template stream itself statically selected? | Yes.  `$C1D3F4` resolves relative offsets from `$C42390` to static group records, passes an observed bit gate, and selects static template streams that enter `$C1D442`.  The index is not yet tied to player position or distance. | [static selector groups](data/static_template_selector_groups.md) |
| Where does the observed page filter apply? | Upstream of workspace population: the same 41 `$C1D3F4` selector entries run under each controlled origin-bin probe, but the live row-key search reaches a subset of 16/10/14/10 static streams. This is the traced page/subsection filter, not an LOD table. | [origin group/stream comparison](data/origin_selector_mutation_probe.md) |
| Is any static terrain-cell directory decoded? | Partly. Accepted `$C42390` group records encode a sorted row-key list followed by a parallel static stream-pointer table; `$C1D4E4-$C1D50C` binary-searches the former and `$C1D43A` selects the latter. The index-to-world-page mapping remains untraced. | [decoded group-record directory](data/origin_selector_mutation_probe.md) |
| Is the group-selector directory bounded? | Structurally, yes: `$C42390-$C4258F` is a 256-entry signed-relative selector directory; `$C42590` starts the next count/threshold/pointer record. Only a bounded subset is scenario-proven terrain-page input, so the complete directory is not reclassified as a complete terrain map. | [full selector directory inventory](data/static_template_selector_directory.md) |
| Do the two origin components enter separate directory axes? | Yes, in the controlled selector window. Incrementing `$C45C3E` changes the call-time row key in 28/41 calls but no group index; incrementing `$C45C46` changes the group index in 28/41 calls but no row key. The combined probe composes both. | [independent directory axes](data/origin_selector_mutation_probe.md) |
| Are the two selector inputs separable and bounded beyond a one-bin probe? | Yes. For bins 0--31 in the same bounded update packet, every group-axis bin changes only `$C1D3F4` selector indices and every row-axis bin only row keys; bins 1--30 alter the same 28/41 call positions. Outer probes (32, 33, 63, 64, 127, 128, 255) exactly repeat `bin & $1F`, proving a 32×32 selector-bin lattice for this path. This is not yet an absolute world-coordinate or physical-map-size decode. | [controlled axis sweep](data/origin_selector_axis_sweep_00_1f.md) |
| Can the full bounded selector lattice be decoded to static streams? | Yes. The 32×32 cross-product of the independently captured group-record pointers and row keys decodes to 4--18 selected static streams per cell. Its bin `(16,16)` exactly reproduces all 41 runtime stream/no-stream choices. This is the first complete page-stream lookup export for the bounded update packet, not a terrain mesh or global map. | [template-selector lattice](data/terrain_template_selector_lattice.md) |
| Is the immutable template payload for that lattice exported? | Yes. The 21 distinct selected streams contain 116 non-terminator exact static header/two-word records; two selected streams are immediate `$FF` terminators. Their 32×32 selector-cell references are exported alongside the payload. The records remain reusable placement inputs, not global terrain vertices. | [lattice stream templates](data/terrain_lattice_stream_templates.md) |
| Does static page payload reach scene descriptors? | In the origin-control window, yes: 91 copied records establish one descriptor association for each observed header class, and the bounded lattice export annotates matching records. The same static source is known to choose different descriptors in a separate scene window, so these are control-window links rather than immutable mesh ownership. | [annotated lattice templates](data/terrain_lattice_stream_templates.md) |
| Is there a page-to-renderer-target content catalog? | Yes, for the control window. The exported template/descriptor links aggregate into static renderer-control targets with their template records, streams, and 32×32 selector-cell memberships. It is context-specific page-content evidence, not positioned objects or LOD. | [lattice target catalog](data/terrain_lattice_target_catalog.md) |
| Is a selector boundary observed? | Locally, on both axes. Zeroing either origin component changes only its own directory axis and suppresses the low-key stream set (16 to 4 streams), with no new wraparound streams. This is not a proven global map edge. | [zero-bin boundary probes](data/origin_selector_mutation_probe.md) |
| Does live flight select changing terrain pages? | Yes. In run034, ordinary flight reaches `$C1D3F4` with group/row `(16,16)` selecting `$C42BD4`, `(16,17)` selecting no row stream, and `(15,15)` selecting `$C42ADA`. This proves live bounded page-stream changes, not a global position map or LOD. | [run034 page transition](data/run034_terrain_page_selector_transition.md) |
| Does a static path reach coordinate-bearing placements? | Yes.  The trace copies 37 static entries from verified segments 66--67 into cells; 22 are later read by the builder and emitted as descriptor-qualified three-word placements before the trace ends.  In this joined sample every middle word is zero.  This is still not a raw global-coordinate or mesh record. | [copy inventory and X/Z diagnostic](data/workspace_template_copies.md) |
| Are selected page streams connected to their template contents? | Yes, for the controlled `(16,16)` origin window. Fifteen selected static streams produce 106 exact `$C1D488` template-record reads, preserving each source header and two input words before the mutable workspace copy. | [active stream-record inventory](data/active_terrain_template_stream_records.md) |
| Does that static-to-placement path repeat across updates? | Yes. A 30-frame extension observes two further refresh phases: 143 copies from segments 66--67, 101 later builder reads, and 101/101 zero middle output words. It is stronger flat-placement evidence, but remains a bounded workspace path rather than a full terrain export. | [extended copy inventory and X/Z diagnostic](data/workspace_template_copies_404_426.md) |
| Are segment-66/67 entries fixed global map positions? | No. Seven identical static entries reach the builder in both measured windows but emit different X/Z tuples. They are reusable templates combined with mutable placement context, so extracting their two words as a world-coordinate table would be wrong. | [refresh-window comparison](data/placement_refresh_window_comparison.md) |
| Does a live origin select terrain-template pages? | Yes. A same-breakpoint control/mutation pair changes only the `$C45C3E/$C45C46` origin values. The selected set drops from 106 to its 60-entry subset and common template outputs shift. This proves causal paging, not the original coordinate-to-page table. | [origin mutation probe](data/origin_selector_mutation_probe.md) |
| Is the terrain paging two-dimensional? | Yes. Independent +1 selector-bin probes shift the dominant common placement outputs by `(-512, 0)` and `(0, -512)` respectively; the combined probe composes to `(-512, -512)`. This proves a 512-unit cache-page lattice, not an absolute world-unit conversion. | [axis probe](data/origin_selector_mutation_probe.md) |
| Does page selection prove terrain chunks or LOD? | It proves a chunk/page-like active-template subset: each origin-bin perturbation removes static entries, while every shared source retains its descriptor association. It does not prove LOD because no same-item, distance-controlled model/topology replacement has been observed. | [chunk-versus-LOD comparison](data/origin_selector_mutation_probe.md) |
| Does a live page refresh copy static terrain content into bands? | Yes. Run034's frame-9,656 `$C1D330-$C1DC08` walk selects `$C42ADA` (8 records), `$C42706` (4), and `$C42BD4` (16) for three distinct mutable bands; three other searches emit no stream. This is active page-content evidence, not global placement or LOD. | [run034 band/content inventory](data/run034_live_band_walk_page_content.md) |
| Are descriptor substitutions observed elsewhere? | Yes: seven template sources select different descriptor records in widely separated flight windows. That is context-dependent selection, but page/control state and distance change together, so it is not LOD evidence. | [window descriptor comparison](data/placement_refresh_window_comparison.md) |
| Do those substitutions reach renderer control? | They change the repeated `+8` descriptor field used as `$C45A36` on `$C1CB74-$C1CCB6`'s generic route; `$C1F6F8` consumes `$C45A36` on the projection path. Type-specific gates can bypass that route, so projection use remains to be traced per descriptor. | [descriptor-control-stream field dataflow](data/placement_refresh_window_comparison.md) |
| Where does that live origin come from? | `$C29042` derives it through the active `$C46184+$C458DE` control record's matrix/transform path, stores all three components at `$C45C3E/$C45C42/$C45C46`, and later adjusts them through an accumulator. It is live derived state, not a static template copy. | [active-origin update](routines/c29042_active_origin_update.md) |
| Are the renderer's projected triples the source map? | No. `$C45630-$C48383` and `$C48390-$C4E76B` are mutable workspaces; static source extraction from either would be wrong. | [geometry boundary report](model_geometry_boundaries.md) |
| Does `M` identify map data? | Partly. Raw `$37` reaches `$C1BF8C`; its now-completed 5,619-instruction command helper is transition/page-control work and does not execute the terrain template selector/copy path. The later map-display renderer now has a bounded transform/control index: `$C35BDE -> $C35BF0`, `$C35BAA -> $C35BB8`, `$C35BC2 -> $C35BD0`, `$C36220 -> $C36232` twice, and `$C3B720 -> $C3B73E`. The final pair is independently bounded through `$C3B6B0` to both line and span primitives in the prepared map page. This is a partial local component/index, not a complete world-terrain extraction or a coastline-pixel mapping. | [`M` command contract](routines/c1bf8c_map_command.md), [renderer census](data/run003_m_map_display_renderer_census.md), and [component boundary](data/c3b720_c3b6b0_static_component_boundary.md) |
| Does `M` visibly show an in-game map? | Yes. The sealed run003 `M` event changes the cockpit to a stable green/blue coastline-style grid display within 30 frames. A controlled run035 end-of-flight `M` view shows the same coastline panned by `(142,36)` screen pixels with 98.8008% blue-raster agreement. Copper evidence identifies it as a 320x200, four-bitplane, double-buffered Chip-RAM display. Its prepared pending page receives 124 direct CPU blitter jobs during the transition (44 span jobs and 76 line-plane jobs), while the instruction trace reaches 42 finalized polygon wrappers and 19 line emitters. Thus this map bitmap is renderer-produced, not an identified dedicated coastline asset. The stable map interval also runs terrain-template selection and sends 103 copied static records to X/0/Z placement outputs, grouped into 83 descriptor-field candidates; a separate map-mode collector reaches static 3D control streams at the projection walker. This is map-mode terrain/control dataflow, not yet a placement-to-coastline-pixel proof or complete extraction. | [`M` visual probe](data/run003_m_map_visual_probe.md), [renderer census](data/run003_m_map_display_renderer_census.md), [template-placement handoff](data/run003_m_map_template_placement_handoff.md), [target catalog](data/run003_m_map_template_target_catalog.md), [control streams](data/run003_m_map_control_streams.md), [pan comparison](data/run003_run035_m_map_pan_comparison.md), and [screenshots](visuals/run003_m_map_display.png) |

## Flatness is not yet a data invariant

The sampled runtime placement layer supplies positive, scenario-backed support
for a flat world plane: its middle coordinate word is zero for all 121, 123,
and 123 descriptor-qualified records at the three run033 checkpoints. The
separate run035 Golden Gate/coast approach repeats that result for 123 records
at each of frames 5,500, 7,000, and 8,500; see the
[run035 placement diagnostic](data/run035_runtime_scene_placements.md).
This
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

The sealed user-supplied Golden Gate approach `run035` adds two later
checkpoints for batch member `$C3B0CE`, both reaching projection/submission.
It does not tie that individual source to `$C355D8`, so its longer/shorter
bounded paths are not interpreted as LOD. The user-defined red Golden Gate
landmark is independently visible only in the outside-world viewport at the
sampled frames 4,250--8,250; this excludes cockpit/HUD pixels and the other
bridge from the landmark definition. See the [viewport interval measurement](data/run035_golden_gate_red_viewport_interval.md)
the [red-pixel line correlation](data/run035_golden_gate_red_line_correlation.md),
and [run035 approach context](data/run035_golden_gate_approach_lod_context.md).

Within that measured interval, the Golden Gate line contexts change from the
two distant `$C3559A/$C355D2` submissions to four `$C355CE` submissions and
then 17 `$C3558A` submissions as the red raster expands. This is a genuine
range/detail-selection candidate, not distance-only LOD proof, because the
user's approach changes camera state. See the [line-detail candidate](data/run035_golden_gate_line_detail_candidate.md).

The earlier leading primitive-selection hypothesis was **distant line lists
followed by nearer filled polygons**, rather than a replacement mesh. Run036
does not establish it: at its largest controlled red Golden Gate span, the
`$C35596/$C355CE` line groups again cover the full measured red-raster extent.
The replay-preserved `$C2FF48` polygon sample has no filled-face-to-raster
association. This retains possible coexisting filled detail as an open
question, but rejects presenting a line-to-polygon transition as a result.
See the [run036 primitive-transition probe](data/run036_red_primitive_transition.md).

One traced frame-7,000 finalized polygon takes a direct-blitter span-style
route rather than `$C2FA7E`, but its fully converted bounds are disjoint from
the red Golden Gate raster. It proves that the renderer has a non-line route
without linking it to the landmark or LOD; see the [polygon span-path
probe](data/run036_polygon_span_path_probe.md).

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
   them. The ready-to-record Golden Gate protocol is in
   [`golden_gate_lod_capture_protocol.md`](golden_gate_lod_capture_protocol.md).
5. Correlate a **filled-face** raster submission to the measured red Golden
   Gate viewport region, then trace it upstream to its face/control record.
   The `$C3559A/$C355D2` line-family correlation is complete, but it cannot
   assign the co-visible filled faces.

Until those conditions are met, the project has a proven scene-control index
and immutable scene-family candidates, not a complete extracted 3D world map
or an LOD table.
