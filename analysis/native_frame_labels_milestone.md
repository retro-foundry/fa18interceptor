# Connected native scene-position labels

`fa18_native` now calls C2B3C2 after the game counter on the drawing branch,
and at the same join on the idle branch, before debug overlays/final message.
The call is `native_frontend_tick` -> `native_flight_tick` ->
`native_frame_scene_labels` -> the existing `draw_scene_position_labels`.
It removes the remaining CPU-child dependency for this frame owner.

Source mode/detail/height, blink/selection and record-active gates are retained.
Scene rows use original C42A02, C1D7E2 and C45BD8 data. Native children pass
ordinary point values to the existing C2ECA8 projection/pixel implementation
and C32A44 number layout: four glyph shifts, even byte alignment, wrapped
40-byte rows and existing BCD/text plotting. The two scene cursors are saved
locally rather than through a CPU stack. Reference hooks remain compatible.

`native_frame_labels_oracle.c` executes complete original C2B3C2 and its
projection/number children against the actual native wrapper. All non-stack
RAM and drawing planes agree in 256 cases; 41 draw visible pixels and publish
numbers. Fixtures retain the original scene stream, varying the observer
around its first source coordinate, mode/detail/height, both origin routes,
blink/selection, and an explicit identity matrix for visible/clip positions.
These test-only observer values never seed production gameplay.

The legacy C2B3C2 owner also passes 128 complete cases comparing registers,
PC, SR and all RAM, using its existing bus/write-log build replacements.
`check_frame_tail.py` runs both oracles from one native 2,400-update checkpoint:
all 576 cases pass. Native link omission passes. Both compiler reference
builds and 12 focused reference contracts pass. The 4,892-update native demo
completes with final RAM identical to the previous frame-tail batch; its gates
suppress labels. Visible rendering is demonstrated by the focused cases.
No full original replay was run.

This owner is **1/1 complete (100% of this source scope)**. The six identified
missing frame owners are **4/6 connected (about 67% of that inventory)**:
lost-selection cleanup, page mark, numeric overlays and scene-position labels.
Stores icons C30A00 and grid/record markers C2B564 remain. Connections do not
measure equal effort or whole-game completion. Recorded-frame parity remains
0/3 accepted, startup lead 37 ticks, Copper fade excluded.
