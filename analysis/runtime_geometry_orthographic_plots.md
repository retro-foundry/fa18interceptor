# Runtime geometry orthographic plots

These are X–Y, X–Z, and Y–Z plots of coordinate triples that reached the perspective pipeline in the cited captured frames. Lines represent the traced endpoint pairings. Axis directions have not been semantically assigned; SVG Y inversion is display-only.

They are deliberately labelled as **mutable workspace geometry**, not source-model exports. They should nevertheless make recognizable outlines easier to identify.

Each line is labelled with its zero-based edge index. Exact endpoint triples are available in [the packet inventory](runtime_geometry_orthographic_packets.json), so an identification can cite a packet and one or more edges unambiguously.

## Traced packets

- [Attract packet `$C3985A`, frame 600](plots/attract_packet_c3985a_orthographic.svg) — ten traced edge-list pairs; [captured frame](../build/attract_focus_600/screen.png).
- [Attract packet `$C39886`, frame 600](plots/attract_packet_c39886_orthographic.svg) — two traced edge-list pairs; [captured frame](../build/attract_focus_600/screen.png).
- [Attract packet `$C3989A`, HUD trace frame 450](plots/attract_packet_c3989a_orthographic.svg) — two traced edge-list pairs; [captured frame](../build/attract_hud_trace_450/screen.png).
- [Attract packet `$C393C4`, HUD trace frame 450](plots/attract_packet_c393c4_orthographic.svg) — three traced edge-list pairs; [captured frame](../build/attract_hud_trace_450/screen.png).
- [Run001 packet `$C3751A`](plots/run001_packet_c3751a_orthographic.svg) — five traced edge-list pairs.
- [External-view packet, frame 7,500](plots/external_view_packet_orthographic.svg) — five `$C38B0A` edge-list pairs resolving through `$C48390`; [checkpoint frame](../build/run031_frame7500_external_checkpoint/screen.png).
- [Golden Gate packet, frame 12,000](plots/golden_gate_packet_orthographic.svg) — thirteen consecutive six-word records starting at `$C483BA`; [checkpoint frame](../build/run031_frame12000_golden_gate_checkpoint/screen.png).
- [Golden Gate closed polygon, frame 12,000](plots/golden_gate_polygon_c4b990_orthographic.svg) — four `$C4B990` triples closed as a loop, immediately before `$C24CFE → $C2FF48`; [projection evidence](data/run031_frame12000_polygon_projection_sample.md).
- [Golden Gate `$C355D8` polygon subset](plots/golden_gate_c355d8_polygons_orthographic.svg) — two closed polygons collected at `$C2FF48` before the active stream changes; [raw submission inventory](../build/run031_frame12000_polygon_submissions_12f/polygon_submissions.json).
- [Golden Gate external `$C35584` polygon subset](plots/golden_gate_external_c35584_polygons_orthographic.svg) — two closed polygons in the same `$C355xx` static scene family during the external Golden Gate pass; [raw submission inventory](../build/run031_frame12600_external_polygon_submissions_v2/polygon_submissions.json).
- [External-view `$C3925C` candidate subset](plots/external_view_c3925c_polygons_orthographic.svg) — three closed polygons; external camera makes this a player-aircraft candidate, not an assignment.
- [External-view `$C3925E` candidate subset](plots/external_view_c3925e_polygons_orthographic.svg) — four closed polygons; likewise an unassigned player-aircraft candidate.
- [External-aircraft `$C3925C`, frame 7,500](plots/external_aircraft_c3925c_frame7500_orthographic.svg) — the same context under the independent player-aircraft oracle.
- [External-aircraft `$C3925E`, frame 7,500](plots/external_aircraft_c3925e_frame7500_orthographic.svg) — the same context under the independent player-aircraft oracle.
- [Later bridge `$C36298` polygon subset](plots/later_bridge_c36298_polygons_orthographic.svg) — three closed polygons in static scene Hunk 43; [raw submission inventory](../build/run031_frame14500_bridge_polygon_submissions/polygon_submissions.json).
- [Bridge-silhouette packet, frame 14,500](plots/bridge_silhouette_packet_orthographic.svg) — the single `$C37EA0` pair resolving through `$C48390`; [checkpoint frame](../build/run031_frame14500_bridge_checkpoint/screen.png).

Regenerate with `python scripts\plot_runtime_geometry.py`. Identifications from these plots should cite the packet and frame, rather than treating the whole mutable `$C48390` region as one model.

## Coverage qualification

An ordinary replay from the sealed demonstration state, with a `$C212B0` breakpoint armed immediately before frame 3,600 and observed through frame 3,900, did not hit this submitter. These plots therefore cover traced early/specialized line packets, not a claim that all later scenery uses `$C212B0` or is represented here.
