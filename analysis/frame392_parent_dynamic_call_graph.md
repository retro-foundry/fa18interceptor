# Dynamic frame call graph

Authority: `build/run075_frame392_parent_callgraph_trace/trace.jsonl`.

Only observed `JSR`/`BSR` transitions with a verified 68000 stack push are edges.
Green nodes have an address-linked native `port/` source reference; red nodes do not.
That classification is a porting triage signal, not proof that a green routine is fully ported.

- Instruction rows: 31800
- Verified game calls: 174
- Distinct dynamic callees: 49
- Address-linked native callees: 26
- Callees without native address evidence: 23

## Callee ledger

| Callee | Calls | Callers | Native address evidence |
| --- | ---: | ---: | --- |
| `$C2EE4A` | 16 | 2 | — |
| `$C2005C` | 15 | 1 | — |
| `$C2FA7E` | 13 | 2 | `port/flight_renderer_page.h`, `port/line.c`, `port/line.h`, `port/projection_grid.h`, `port/scene_render_fixture.c` |
| `$C247C0` | 12 | 2 | `port/clip_segment_pair.h`, `port/polygon_clip_pipeline.h` |
| `$C248B2` | 11 | 2 | `port/clip_segment_pair.c`, `port/clip_segment_pair.h`, `port/polygon_clip_pipeline.h`, `port/tuple_cache_stage.h` |
| `$C246A0` | 8 | 2 | `port/map_packet_polygon_display.h`, `port/map_packet_stage.h`, `port/map_packet_transform.h`, `port/polygon_clip_pipeline.h`, `port/polygon_clip_pipeline_contract_test.c`, `port/polygon_display_pipeline.h` |
| `$C501E0` | 8 | 1 | — |
| `$C50212` | 8 | 1 | — |
| `$C1FB82` | 7 | 2 | — |
| `$C24996` | 6 | 3 | `port/negated_tuple_emit.h`, `port/polygon_clip_pipeline.h`, `port/tuple_cache_stage.h` |
| `$C305AA` | 6 | 2 | `port/projection_grid.h` |
| `$C212B0` | 4 | 1 | — |
| `$C24FE8` | 4 | 1 | — |
| `$C30466` | 4 | 4 | `port/blit_job.c`, `port/blit_job.h`, `port/run036_polygon_oracle_test.c`, `port/selected_table_display_stage.h` |
| `$C2469E` | 3 | 1 | `port/polygon_clip_pipeline.c` |
| `$C2F60A` | 3 | 1 | `port/flight_renderer_page.h`, `port/planar_pixel.c`, `port/planar_pixel.h`, `port/projection_grid.h` |
| `$C2FF48` | 3 | 2 | `port/flight_renderer_page.h`, `port/polygon_display_pipeline.h`, `port/polygon_submission.h`, `port/projection_grid.h`, `port/projection_grid_contract_test.c`, `port/selected_display_submission.h` |
| `$C301F6` | 3 | 1 | `port/flight_page_handoff.h`, `port/flight_renderer_page.h`, `port/line.h`, `port/polygon_submission.h`, `port/projection_grid.c`, `port/projection_grid.h`, `port/projection_page_blitter.c`, `port/selected_display_submission.h` |
| `$C02836` | 2 | 1 | — |
| `$C1D0B6` | 2 | 1 | `port/alternate_flight_scale.h`, `port/scene_alternate_record.h`, `port/scene_component_accumulation.h` |
| `$C1D91A` | 2 | 1 | `port/scene_fixed_point_stage.h` |
| `$C1F99A` | 2 | 1 | — |
| `$C1FC42` | 2 | 1 | — |
| `$C25876` | 2 | 1 | `port/record_delta_scan.h` |
| `$C2F0F4` | 2 | 2 | — |
| `$C2F5F4` | 2 | 1 | `port/flight_renderer_page.h`, `port/planar_pixel.h`, `port/projection_grid.h` |
| `$C53EC0` | 2 | 2 | `port/viewport_palette.h` |
| `$C0D04C` | 1 | 1 | — |
| `$C1518C` | 1 | 1 | `port/record_scan_renderer_pass.h` |
| `$C1CB14` | 1 | 1 | `port/scene_placement.h` |
| `$C1CB26` | 1 | 1 | `port/scene_placement.h` |
| `$C1CCBC` | 1 | 1 | `port/flight_followup_pipeline.h`, `port/flight_followup_record.h`, `port/scene_placement.h` |
| `$C1ED4C` | 1 | 1 | `port/scene_stream_gate.h` |
| `$C1EE14` | 1 | 1 | `port/scene_stream_cursor.h`, `port/scene_stream_entry.h` |
| `$C1FB8C` | 1 | 1 | — |
| `$C1FE20` | 1 | 1 | — |
| `$C1FEF2` | 1 | 1 | — |
| `$C1FF0A` | 1 | 1 | — |
| `$C20002` | 1 | 1 | — |
| `$C207FE` | 1 | 1 | — |
| `$C20A40` | 1 | 1 | — |
| `$C20D68` | 1 | 1 | — |
| `$C22AC0` | 1 | 1 | — |
| `$C265E8` | 1 | 1 | `port/flagged_slot_scan.h` |
| `$C279D0` | 1 | 1 | `port/default_scene_render_pass.h`, `port/default_scene_render_pass_contract_test.c`, `port/flight_renderer_packet.c`, `port/flight_renderer_packet.h`, `port/flight_renderer_page.h`, `port/flight_scene_pipeline.h`, `port/game.h`, `port/projection_grid.h`, `port/scene_render_fixture.h` |
| `$C2F0C6` | 1 | 1 | — |
| `$C2F156` | 1 | 1 | — |
| `$C2F490` | 1 | 1 | `port/record_scan_renderer_pass.h` |
| `$C304B2` | 1 | 1 | `port/blit_job.c`, `port/blit_job.h`, `port/blit_job_contract_test.c`, `port/run036_polygon_oracle_test.c` |
