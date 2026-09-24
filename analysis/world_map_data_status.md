# World-map and LOD status

This is the evidence boundary for the game's large physical flight area.  It
separates the observed *scene-selection and render inputs* from an as-yet
unidentified authoritative world/map dataset.  No extracted range below is
called the complete world map.

The current source-backed path from live selector origin through immutable
template streams to the flat placement cache is summarized in the
[terrain template pipeline reconstruction](routines/terrain_template_pipeline_reconstruction.md).

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
| Does `M` identify map data? | Partly. Raw `$37` reaches `$C1BF8C`; its now-completed 5,619-instruction command helper is transition/page-control work and does not execute the terrain template selector/copy path. The later map-display renderer has both a bounded transform/control index (`$C35BDE -> $C35BF0`, `$C35BAA -> $C35BB8`, `$C35BC2 -> $C35BD0`, `$C36220 -> $C36232` twice, and `$C3B720 -> $C3B73E`) and a separate static-packet path inside verified original segment 68 (`$C42CA8-$C444F7`): its `$C42CA8-$C42D27` prefix is a traced 8×8 relative-offset directory, 12 cells of which select map packets in run003; 55 completed transforms consume 353 exact signed pairs before the polygon display stage. This is not a complete world-terrain extraction or a coastline-pixel mapping. | [`M` command contract](routines/c1bf8c_map_command.md), [partial export](data/run003_m_map_partial_geometry_export.md), [renderer census](data/run003_m_map_display_renderer_census.md), [directory lookup](routines/c2ad80_map_segment68_directory_lookup.md), [static packet path](routines/c2af00_map_static_pair_packet.md), [line component](data/c36220_c36232_map_line_component.md), and [component boundary](data/c3b720_c3b6b0_static_component_boundary.md) |
| Does `M` visibly show an in-game map? | Yes. The sealed run003 `M` event changes the cockpit to a stable green/blue coastline-style grid display within 30 frames. A controlled run035 end-of-flight `M` view shows the same coastline panned by `(142,36)` screen pixels with 98.8008% blue-raster agreement. Copper evidence identifies it as a 320x200, four-bitplane, double-buffered Chip-RAM display. Its prepared pending page receives 124 direct CPU blitter jobs during the transition (44 span jobs and 76 line-plane jobs), while the instruction trace reaches 42 finalized polygon wrappers and 19 line emitters. Thus this map bitmap is renderer-produced, not an identified dedicated coastline asset. The stable map interval also runs terrain-template selection and sends 103 copied static records to X/0/Z placement outputs, grouped into 83 descriptor-field candidates; a separate map-mode collector reaches static 3D control streams at the projection walker. This is map-mode terrain/control dataflow, not yet a placement-to-coastline-pixel proof or complete extraction. | [`M` visual probe](data/run003_m_map_visual_probe.md), [renderer census](data/run003_m_map_display_renderer_census.md), [template-placement handoff](data/run003_m_map_template_placement_handoff.md), [target catalog](data/run003_m_map_template_target_catalog.md), [control streams](data/run003_m_map_control_streams.md), [pan comparison](data/run003_run035_m_map_pan_comparison.md), and [screenshots](visuals/run003_m_map_display.png) |
| Does the visible M-map reuse the flight terrain-template selector? | Yes, in one same-replay comparison. Run037's cockpit band walk selects two static streams / 40 records; after its `M` command the stable visible map band walk selects six streams / 71 records, retaining both cockpit streams and adding four. This establishes shared, state-dependent template input, not a pixel-to-template mapping, terrain-cell identity, or LOD cause. | [run037 flight-to-map selector comparison](data/run037_flight_to_m_map_template_selection.md) |
| Does an independent M-map position reach flat terrain placements? | Yes. In run037's stable map frames 5,717--5,719, 100 of 110 traced immutable template copies reach `$C1DD36` and emit descriptor-qualified X/0/Z placements; all 100 middle words are zero and group into 82 descriptor `+8` control-field candidates. This is bounded map-mode placement/cache evidence, not a complete terrain mesh or a coastline-pixel mapping. | [run037 template-placement handoff](data/run037_m_map_template_placement_handoff.md) |
| Does a full run037 M-map placement-cache snapshot remain flat? | In the bounded snapshot, yes: 111 descriptor-qualified `$C4E9AA` records have a zero middle word, spanning X `-1024..15872` and Z `-768..10576`. The cache is mutable, so this supports the sampled flat placement plane but does not measure the physical map extent or prove a universal elevation rule. | [run037 placement snapshot](data/run037_m_map_runtime_placement_snapshot.md) |
| Are terrain entries coordinate pairs rather than triples? | For the sampled terrain/map placement source, yes: each immutable template supplies a header plus two words, and the independent segment-68 M-map packet reader consumes exactly two words. The builder produces an X/0/Z cache entry. Later renderer-local components remain three-word triples, so the pair source and triple geometry are distinct formats. | [planar tuple/local triple boundary](data/planar_tuple_and_local_triple_boundary.md) |
| How do M-map source pairs become renderer triples? | The byte-exact `$C2AF9C-$C2AFF7` loop combines each immutable signed pair with live translation/matrix state and writes three signed words through `A5`. A run003 trace-state arithmetic calculation covers 353 pairs/55 display-stage batches; all 298 within-batch adjacent writes advance by the expected six bytes. Its calculated third word ranges `384..6144`; the source routine proves it is transform depth, not a stored terrain-elevation word. | [pair-transform outputs](data/run003_m_map_pair_transform_outputs.md) |
| Can M-map source packets be tied to visible-page rendering? | Yes, for the sealed run003 map-transition trace. All 55 packet-bearing `$C2AFE2 -> $C246A0` calls are bounded by their observed `$C2AFE8` returns; 38 reach `$C2FF48` polygon submission, and 36 reach 72 ordered `$C304F4` pending-map-page `BLTSIZE` jobs before that return. The report retains each exact source pair address and raw pair. This is direct packet-to-renderer/page-work attribution, not a complete terrain mesh, global map coordinate system, or pixel ownership claim. | [packet-to-screen attribution](data/run003_m_map_packet_screen_attribution.md) |
| Is any actual 3D terrain-like geometry proven in M-map mode? | Yes, one compact reusable component. An active flat placement/template route reaches immutable `$C3B720-$C3B73D`, five local triples with four traced triangular sides under `$C3B6B0`; the M-map transition then submits its line/span primitives to the prepared map page. Its fifth local vertex has middle component `1024`, so this proves local 3D component geometry while **not** contradicting the separately sampled X/0/Z placement plane. It is not a complete terrain mesh, global elevation model, or named map feature. | [component boundary](data/c3b720_c3b6b0_static_component_boundary.md), [template-to-polygon path](data/run037_c44970_template_to_polygon_path.md), and [3D inspection plot](plots/c3b720_shared_bridge_pylon_static_topology.png) |
| Does a longer stable M-map window reveal additional matrix-input components? | No, in the sealed no-input extension. The original 13-frame (133,001 instruction) and 20-frame (198,208 instruction) windows each execute the same six `$C1F4AC` transforms from the same five immutable sources: `$C35BAA`, `$C35BC2`, `$C35BDE`, `$C36220` twice, and `$C3B720`. This bounds the steady-state sample; it is not whole-map or LOD coverage. | [stable transform-source comparison](data/run037_m_map_stable_transform_source_comparison.md) |
| Do run037 map placements reach the renderer control interface? | Yes, at the descriptor-field boundary. In the stable map trace, 17 distinct descriptors execute `$C1CC70`, reading their `+8` longword and writing it to `$C45A36`; all 17 are independently present in the template-placement inventory. `$C1F6F8` later loads `$C45A36` as a control-stream pointer. Scheduling prevents a one-placement-to-one-walker assignment, so this is not pixel, model, or terrain-cell ownership. | [run037 descriptor-control handoff](data/run037_m_map_descriptor_control_handoff.md) |
| Are any active run037 descriptor targets in a newly verified immutable Hunk? | Yes. Original Hunk 69 is byte-exactly located at `$C44500-$C44877`, resolving 11 targets from the run037 template-placement catalog; three execute the observed `$C1CC70` field-to-`$C45A36` write. This promotes their original/runtime provenance, not their meaning to mesh or terrain-cell data. | [run037 Hunk-69 target mapping](data/run037_segment69_descriptor_target_mapping.md) |
| Does the adjacent partly mutable Hunk 70 contain stable active targets? | Yes, as bounded records only. All 24 catalog-selected 24-byte targets in `$C44880-$C45623` are snapshot-stable across five states and original-byte-identical outside their relocation operands. This identifies an immutable target-record subset inside a globally mutable/unclassified CODE Hunk; it is not a complete terrain mesh or LOD table. | [run037 Hunk-70 target inventory](data/run037_segment70_template_targets.md) |
| Does any active template target reach an actual 3D/polygon path? | Yes, one bounded path is now direct. Template `$C427AD` reaches descriptor `$C22890`, whose `$C44970` field is read at `$C1CC70`; its no-input continuation reaches `$C1F4AC` with static `$C3B720`, then four `$C2FF48` polygon submissions under `$C3B6B0`. The capped trace does not establish individual-pixel or whole-component ownership. | [run037 `$C44970` path](data/run037_c44970_template_to_polygon_path.md) |
| Are the newly linked controls a position-specific terrain mesh? | No evidence supports that. Run037 replays the same bounded `$C35BAA`, `$C35BC2`, and `$C36220` map-display components seen at an earlier map position, each through `$C1F4AC -> $C48390 -> $C212B0/$C2FA7E`. Their repeat identifies reusable map-renderer control data, not terrain-cell geometry. | [run037 control-component repeat](data/run037_m_map_control_component_repeat.md) |
| Do two sealed M-map runs select different `$C1F4AC` immutable source blocks? | Not in the directly comparable transition/stable windows. Run003 and run037 each execute the identical six transforms from five sources (`$C35BAA`, `$C35BC2`, `$C35BDE`, `$C36220` twice, `$C3B720`). This makes these observed blocks reusable map-renderer inputs rather than a position-specific terrain-cell mesh in those windows; it does not rule out other inputs at other positions. | [cross-run source comparison](data/run003_run037_m_map_transform_source_comparison.md) |
| Does a second independent stable M-map scenario change those local 3D inputs? | No. The 20-frame run035 stable sample has 40 descriptor-control updates and 12 `$C1F4AC` transforms, while run037 has 17 and 6 respectively; despite that different scheduling/control activity, both transform exactly the same five immutable sources. This separates the sampled changing control/page state from a stable reusable local map-renderer component set, without proving global-map or LOD invariance. | [run035/run037 stable source comparison](data/run035_run037_m_map_stable_transform_source_comparison.md) |

| Is there a visual inspection export of the traced planar map source? | Yes. The cumulative source-pair visual preserves 59 deduplicated ordered packet paths across four sealed M-map traces, containing 387 exact pair addresses (24.87% of segment-68 bytes as pair payload). It joins only points from the same traced batch and makes no terrain-mesh, placement, or LOD claim. | [cumulative packet source visual](data/m_map_packet_source_coverage.md) |
| Can unrendered packet data be structurally separated from unknown bytes? | Yes, structurally. Twenty-four direct `$C2AF00` headers, two selector samples, and 45 non-reject targets from the byte-exact wide directory root all 71 grammar-compatible headers. Their 79 inline/alternate streams decode to 817 pair records and `$FFFF` terminators, covering 52.51% of segment 68 as pair payload. This is packet-format/source coverage, not evidence that every pair rendered or is terrain. | [static reachable packet streams](data/static_m_map_packet_streams.md) |
| Do grammar-compatible headers still lack a static producer path? | No, in the current segment-68 scan. All 71 are dynamically observed or referenced by a non-reject selector-directory cell; 45 remain directory-only (not dynamically executed in the sealed samples). This promotes their packet structure, not terrain identity, complete rendering coverage, or LOD meaning. | [directory-backed header coverage](data/static_m_map_packet_streams.md#directory-backed-header-coverage) |
| Does a stable map sample select additional packet headers? | Yes. The earlier run035 stable-map selector sample reaches `$C43FD0` and `$C440BC` at `$C2AF40`, in addition to the 24 headers with a direct `$C2AF00` trace. Their inline streams structurally contain 28 and 37 pairs respectively. This promotes their header/stream provenance, not a terrain-cell or LOD meaning. | [static reachable packet streams](data/static_m_map_packet_streams.md) |
| Does the stable run037 M-map window execute candidate-only headers? | No, in the extended no-input sample. Across 300,000 stepped instructions, 56 `$C2AF40` visits repeat 23 already known headers and execute none of the remaining 45 candidate-only addresses. This rejects them for that stable map draw, not for other map positions or modes. | [run037 long selector sample](data/run037_m_map_packet_long_window.md) |
| Does the run035 map-appearance variant execute candidate-only headers? | No, in its matching 300,000-instruction no-input sample. Forty visits split 20 inline/20 alternate selections over eight already known headers, with no remaining candidate-only header. This strengthens depth-driven stream variation for an existing packet family, but does not establish flight-world LOD. | [run035 long selector sample](data/run035_m_map_packet_long_window.md) |
| Do candidate headers have a simple immutable external-pointer table? | No evidence of one. An even-aligned longword scan across the baseline and run037 stable snapshots finds zero stable external references to all 71 headers (segment-68 self-references excluded). Relative-offset tables and computed pointers remain outside this negative result. | [header-reference scan](data/m_map_packet_header_references.md) |

| Is the M-map packet selector itself reconstructed? | Yes, for `$C2AD80-$C2AE59`. It indexes signed byte-pair controls at `$C29F00`, bounds them, chooses a 16- or 64-byte row stride, and resolves a positive 16-bit offset from the caller-supplied static base into `A3`. The adjacent 18-word detail-limit lookup is also byte-exact. This proves selector mechanics, not global axes, terrain ownership, or LOD. | [relative-offset selector](routines/c2ad80_map_segment68_directory_lookup.md) |
| Does the M-map selector use a local neighborhood stencil? | Yes, in the sampled renderer path. All 25 observed `$C29F00` selector entries are the full signed offset lattice `{-2..2} × {-2..2}` before the bounded relative-offset lookup. This supports local page-neighborhood selection, not a map-size, world-axis, or LOD claim. | [selector byte-pair inventory](data/m_map_selector_byte_pairs.md) |
| Does the traced 8×8 packet directory encode known blank selections? | Yes. Thirty-one cells resolve to `$C42E6A`, which starts `$FFFF0800`; `$C2AEFC` immediately rejects a negative loaded header longword. These are no-packet selections in this directory, while the remaining non-negative targets still need content semantics. This proves a bounded selection mask, not a global coastline boundary or LOD layout. | [segment-68 directory inventory](data/run003_m_map_segment68_directory.md) |
| Does the selector reach more than the initial 8×8 directory? | Yes. Across four M-map traces, 104 `$C2ADCE` reads resolve to 44 observed slots at two immutable bases: `$C42CA8` and `$C42E6C`. The second base contributes 28 unique slots, with both non-negative packet entries and 8 immediate-reject slots. This is direct multi-directory/chunk-selection evidence, not a decoded world-grid scale or terrain identity. | [observed directory accesses](data/m_map_observed_directory_accesses.md) |
| Are both selector row strides used in recorded map draws? | Yes. The frame-local mode selects 16- or 64-byte rows at both observed bases. In run035 and run037, bounded initializer traces show a normal pass followed by a wide pass on the next frame, so the layouts are sequential map-render preparation passes rather than mutually exclusive global map modes. This does not establish physical-distance LOD. | [initializer sequence](data/m_map_directory_initializer_sequence.md) and [selector stride modes](data/m_map_selector_modes.md) |
| Are the two directory layouts explicitly initialized? | Yes. `$C2AB34` byte-exactly initializes wide mode `1`, base `$C42E6C`, shift `8`, and maxima `31/31`; its adjacent `$C2AB5A` sibling initializes normal mode `0`, base `$C42CA8`, shift `12`, and maxima `7/7`. This establishes two intentional selector layouts, not their world-scale or LOD meaning. | [wide directory initializer](routines/c2ab34_wide_map_packet_directory_setup.md) |
| Is the wide directory structurally decoded? | Yes. Its explicit `32×32` bounds decode `$C42E6C-$C4366B` as 1,024 relative-offset cells resolving 103 reused targets: 498 with non-negative target words and 526 guarded as negative targets. This is complete static selector-layout coverage, not a claim that every target renders terrain or that local indices are global map coordinates. | [wide directory inventory](data/wide_m_map_packet_directory.md) |
| Does projection depth select the 16- versus 64-byte directory layout? | Not in the sampled evidence. Run035 appearance (`1113..1124`) and stable map (`196608`) both retain the non-zero/64-byte mode, while their packet stream routes differ. The metric drives stream/coordinate-detail behavior within that layout; its layout-selector cause remains unproven. | [detail gate](routines/c2ad00_map_control_record_detail_gate.md) and [live selector samples](data/run035_m_map_appearance_packet_runtime_state.md) |
| Does the wide pass have its own depth-threshold control-stream selector? | Yes. `$C2AC1A` selects bases `$C2A0C2`, `$C2A072`, or a low-metric filter path at `$C2AC3E` around `$10000/$9000`. Their 25-code/9-code first records feed `$C29F00`: the high stream selects the full 5×5 local offset neighborhood, while the middle stream selects its inner 3×3. This is a verified projection-depth-driven M-map detail mechanism, not measured physical-flight-distance or mesh-LOD. | [wide directory initializer](routines/c2ab34_wide_map_packet_directory_setup.md), [stable branch sample](data/run035_m_map_stable_wide_depth_branches.md), and [middle-band probe](data/run035_m_map_stable_wide_depth_mid_probe.md) |
| Do those local offsets resolve to static packet families? | Yes, in the joined map traces: all 25 observed signed offsets occur, and 77 of 104 selector entries reach `$C2AEFC` and a static packet header before the next selector entry. Individual offsets select different headers across frames/scenarios, consistent with local, state-dependent selection rather than a fixed global coordinate table. A bounded miss is not evidence of empty content. | [selector-to-header join](data/m_map_selector_pair_header_join.md) |

## M-map packet detail result

The separately traced segment-68 static packet renderer has a real,
depth-driven geometry-variant mechanism.  `$C2AAD2-$C2AB0C` derives its metric
from the transformed projection component `$C45A78`; `$C2AD00` applies the
`$400`/`$C80` bands; and `$C2AF40` selects either the inline packet stream or
the header's alternate stream.  The run035 appearance trace dynamically takes
both routes, and matched headers prove different immutable coordinate data
(three matched alternate first batches reduce from `13` to `7`, `9` to `8`,
and `6` to `3` pairs).  This establishes an LOD-style geometry mechanism for
the M-map renderer.  Its depth is a transformed renderer component, so it
does not prove that physical flight-world distance selects a terrain model.

The independently recorded run037, after ordinary straight flight, visibly
enters the same map and contributes one further direct packet header
(`$C42E3E`) with three previously unobserved exact source pairs. It confirms
position-sensitive bounded M-map packet content, while leaving the terrain
cell and flight-distance interpretations unproven. See the
[run037 packet comparison](data/run037_m_map_stable_packet_comparison.md).

Authority: [packet path and depth provenance](routines/c2af00_map_static_pair_packet.md),
[variant comparison](data/run003_run035_map_packet_variant_comparison.md), and
[raw coordinate visual](plots/run003_run035_map_packet_variants.png).

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

The new format boundary refines that caution: the sampled immutable
terrain/map placement **sources** themselves are two-word pairs, but they
select separate renderer-local three-word components. A useful export should
preserve that separation rather than pad source pairs into invented triples or
drop a real component from local geometry. See the [planar tuple/local triple
boundary](data/planar_tuple_and_local_triple_boundary.md).

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

The later sealed `run038` does not remove that limitation. Its cockpit-only
red-landmark scan expands from 30 pixels at frame 4,000 to 790 at 6,750, but
the landmark bearing moves materially across the viewport and disappears by
the sampled frame 7,250. Its medium-distance trace also does not reach the
known `$C1F4AC` Golden Gate batch entry. It is retained as an auditable
negative distance-only probe, not LOD evidence. See the
[run038 distance probe](data/run038_golden_gate_distance_probe.md).

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
