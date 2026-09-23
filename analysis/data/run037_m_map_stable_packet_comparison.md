# run037 stable M-map packet comparison

Classification: **bounded second flight-position M-map packet sample**. This
records exact immutable packet inputs observed after the user's frame-5,414
`M` command. It is not a complete terrain export, a flight-world coordinate
system, or a flight-world LOD result.

The sealed replay remains in the cockpit at frames 5,400 and 5,425, then
shows the green/blue map at frame 5,700 and still at frame 6,200. A 13-chipset
frame trace from frames 5,688--5,700 executes 28 direct `$C2AF00` entries,
all on the inline route, completing 56 `$C2AFE2` batches and consuming 363
pair reads. The accompanying inventory retains each exact address and signed
pair: [run037 packet inventory](run037_m_map_stable_polygon_static_packets.md).

Compared with run003's stable map inventory, run037 contains every one of its
21 direct headers and adds `$C42D28` and `$C42E3E`; it reaches 343 unique pairs
versus run003's 333. Against the existing cumulative run003/run035 coverage,
only `$C42E3E`, its inline stream `$C42E42`, and the three payload pairs at
`$C42E44`, `$C42E48`, and `$C42E4C` are new. Cumulative exact pair-payload
coverage therefore rises from 1,536 to 1,548 of the segment's 6,224 bytes
(24.87%).

This is useful evidence that the M renderer selects different bounded static
packet content at a different ordinary-flight position. It does **not** prove
that those packets are a terrain-cell map, nor that their selection is caused
by flight distance: position, map state, projection, and culling remain
confounded in this comparison.

Authority: sealed `captures/run037`; keyframes in `build/run037_keyframes/`;
and `build/run037_m_map_stable_13f_trace/trace.jsonl` (133,001 instructions,
frames 5,688--5,700).
