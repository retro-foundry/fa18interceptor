# run037 flight-to-M-map terrain-template selection

Classification: **same-replay flight-versus-map workspace-selector comparison**.
It proves that both observed display states execute the established static
terrain-template selector and copier. It does not establish a complete terrain
map, a coastline-pixel ownership map, global coordinates, or flight-world LOD.

The sealed run has no `M` event before frame 5,414. At frame 5,217, in the
normal cockpit interval, a full `$C1D330`--`$C1DC08` workspace-band walk takes
1,455 instructions and reaches two static streams: `$C42B66` (18 records) and
`$C42646` (22 records). The bounded inventory therefore captures 40 exact
static template records before they are copied to mutable bands:
[flight inventory](active_terrain_template_stream_records_run037_flight_pre_map_band_walk.md).

After the map is visibly stable, the same complete band-walk route is reached
at frame 5,717 and runs for 2,954 instructions. It selects six static streams
and copies 71 records: `$C42ADA` (8), `$C42956` (3), `$C42BD4` (16),
`$C42706` (4), `$C42646` (22), and `$C42B66` (18). The exact group/row choices
and source records are preserved in the [map selector inventory](static_template_selector_groups_run037_m_map_band_walk.md)
and [map template inventory](active_terrain_template_stream_records_run037_m_map_band_walk.md).

The two streams present before `M` are retained during the map walk, while
four additional streams appear. That is direct state-dependent static
template selection in one replay, but it cannot distinguish map-mode policy,
changed flight state, or culling. The evidence therefore supports a live
chunk/page-like template input pipeline shared with the map display, not an
LOD conclusion.

Authority: `build/run037_flight_pre_map_band_walk_trace/trace.jsonl` (frame
5,217) and `build/run037_m_map_band_walk_trace/trace.jsonl` (frame 5,717),
each terminated at `$C1DC08`.
