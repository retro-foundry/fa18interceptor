# Runtime code/data/graphics boundary inventory

This generated inventory is the separation authority.  It preserves the original
Amiga Hunk declaration and adds only evidence-backed runtime classifications.
A CODE hunk that has not run is **not** called data: it remains unclassified.

## Result

- Original segments: 185.
- Initialized DATA hunks: 32 (1,500 bytes); BSS hunks: 27 (11,512 bytes).
- CODE hunks with positive data-only evidence: 11.
- Executed CODE hunks: 38; unresolved/ambiguous CODE hunks: 76.
- Known visual memory is listed as mutable display targets, not immutable graphics assets.

## Separation rules

1. Extract `initialized_data` from original HUNK_DATA payloads and allocate `uninitialized_data` from HUNK_BSS sizes.
2. Keep every `code_hunk_*` region in the executable image.  The `data_in_code_hunk` verdict only licenses treating that whole hunk as data where its positive references support it.
3. Split the exact `inline_data_regions` out before disassembly; their enclosing CODE hunks stay executable.
4. Keep `active_display_regions` outside both program-code and static-asset exports. They are mutable Chip-RAM renderer destinations in the named capture state.

## Exact inline data boundaries

| Range | Bytes | Conservative role | Evidence |
| --- | ---: | --- | --- |
| `$C2209C`–`$C220A7` | 12 | scene_pointer_prefix | `analysis/data/c2209c_scene_pointer_prefix.md` |
| `$C223A8`–`$C227EB` | 1092 | scene_pointer_triplet_table | `analysis/data/c223a8_scene_pointer_triplets.md` |
| `$C33258`–`$C332B3` | 92 | font_compositor_offset_mask_pairs | `analysis/data/c33258_postflight_transform_table.md` |
| `$C37EA0`–`$C37EA5` | 6 | projected_edge_list_candidate | `analysis/data/c37ea0_bridge_silhouette_projected_edge_list.md` |
| `$C38B0A`–`$C38B1F` | 22 | projected_edge_list_candidate | `analysis/data/c38b0a_external_view_projected_edge_list.md` |
| `$C35584`–`$C35589` | 6 | golden_gate_control_stream_words | `analysis/data/c35584_c355d8_golden_gate_control_stream.md` |
| `$C355D8`–`$C355DD` | 6 | golden_gate_control_stream_words | `analysis/data/c35584_c355d8_golden_gate_control_stream.md` |
| `$C3925A`–`$C3925F` | 6 | external_view_control_stream_words | `analysis/data/c3925a_external_view_control_stream.md` |
| `$C3751A`–`$C3752F` | 22 | projected_edge_list_candidate | `analysis/data/traced_runtime_edge_lists.md#c3751a` |
| `$C393C4`–`$C393D1` | 14 | projected_edge_list_candidate | `analysis/data/traced_runtime_edge_lists.md#c393c4` |
| `$C3985A`–`$C39883` | 42 | projected_edge_list_candidate | `analysis/data/c3985a_projected_edge_list.md` |
| `$C39886`–`$C3988F` | 10 | projected_edge_list_candidate | `analysis/data/traced_runtime_edge_lists.md#c39886` |
| `$C3989A`–`$C398A3` | 10 | projected_edge_list_candidate | `analysis/data/traced_runtime_edge_lists.md#c3989a` |
| `$C483BA`–`$C48455` | 156 | consecutive_projection_records | `analysis/data/c483ba_golden_gate_projection_records.md` |
| `$C34A56`–`$C34A6D` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C34A5E`–`$C34A75` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C3556E`–`$C35585` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C36038`–`$C3604F` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C36210`–`$C36227` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C36244`–`$C3625B` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C36276`–`$C3628D` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C3672A`–`$C36741` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C36914`–`$C3692B` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C36A70`–`$C36A87` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C36EB2`–`$C36EC9` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C37E60`–`$C37E77` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C384CC`–`$C384E3` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |
| `$C39264`–`$C3927B` | 24 | live_renderer_control_stream_prefix | `analysis/live_control_streams.md` |

## Original HUNK data allocation

| Kind | Hunks | Bytes | Boundary authority |
| --- | ---: | ---: | --- |
| Initialized DATA | 32 | 1,500 | Original `HUNK_DATA` declarations and payload offsets in `analysis/hunk_inventory.json` |
| Uninitialized BSS | 27 | 11,512 | Original `HUNK_BSS` declarations and allocation sizes in `analysis/hunk_inventory.json` |

Zero-length Hunk declarations are retained in the counts: they preserve segment numbering and relocation identity, but contribute no bytes to extraction/allocation.

## Snapshot-backed static data

| Range | Bytes | Role | Qualification |
| --- | ---: | --- | --- |
| `$C3ED00` to `$C41127` | 9256 | preloaded_menu_and_mission_text_pool | Snapshot-backed static data within a mutated CODE hunk; not a byte-identity claim for the extracted executable. |

## Runtime display state

| Range | Bytes | Classification | Evidence |
| --- | ---: | --- | --- |
| `$C1AA9C` to `$C1AB07` | 108 | mutable_palette_and_display_pointer_state_in_BSS | `analysis/runtime_display_pointer_state.md` |

## Mutable geometry workspace

| Range | Bytes | Classification | Evidence |
| --- | ---: | --- | --- |
| `$C45630` to `$C48383` | 11604 | mutable_geometry_record_workspace_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C48390` to `$C4E76B` | 25564 | mutable_geometry_working_table_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |

## Byte-stable scene-family candidates

| Range | Bytes | Classification | Evidence |
| --- | ---: | --- | --- |
| `$C34A50` to `$C3555F` | 2832 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C35568` to `$C361FF` | 3224 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C36208` to `$C36A1B` | 2068 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C36A28` to `$C3720F` | 2024 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C08718` to `$C089EB` | 724 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C37218` to `$C37677` | 1120 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C37680` to `$C37983` | 772 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C37990` to `$C37F77` | 1512 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |
| `$C37F80` to `$C383E3` | 1124 | byte_stable_scene_family_candidate_in_CODE_hunk | `analysis/model_geometry_boundaries.md` |

## Active display memory

| Capture state | Range(s) | Classification | Evidence |
| --- | --- | --- | --- |
| attract frame 1800 | `$012BC0–$014AFF`, `$014B00–$016A3F`, `$016A40–$01897F`, `$018980–$01A8BF` | mutable_chip_ram_display_target | `analysis/cockpit_bitplane_assets.md` |
| run029 frame 993/994 | `$04DB30–$04FA6F`, `$04FA70–$0519AF`, `$0519B0–$0538EF`, `$0538F0–$05582F`, `$055830–$05776F` | mutable_chip_ram_display_target | `analysis/run029_active_cockpit_bitplanes.md` |

## Blitter channel state

`analysis/attract_cockpit_blitter_jobs.json` records the complete register state at each observed CPU `BLTSIZE` write. It is the source/destination boundary for the frame-1800 cockpit trace; do not infer an immutable source asset from an active plane pointer.

## Immutable disk graphics

The ADF's `pix/frnt5`, `pix/inst5`, and `pix/splsh` ILBM resources are independently inventoried in `analysis/disk_graphics_assets.md`. They can be exported verbatim by the recorded command and are separate from code and mutable display memory.
The broader game-disk inventory at `analysis/disk_game_resources.md` separately hashes the executable, ILBMs, text resources, configuration, and icon metadata.

The complete machine-readable segment list, including original payload offsets and resolved runtime addresses, is `analysis/runtime_region_manifest.json`.
