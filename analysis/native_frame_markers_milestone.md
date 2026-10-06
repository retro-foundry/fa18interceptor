# Connected native map grid and aircraft markers

`native_frontend_tick` -> `native_flight_tick` now calls
`native_frame_grid_and_markers` after lost-selection cleanup and before timer
sampling, exactly at C0F2F0. It connects complete C2B564 and its C2AFFA,
C2B928 and C2B952 children: both original grid axes, visible number labels,
16 control-record scans, class-$20 markers, orientation buckets and cached
coordinate nibbles. Native rendering uses the existing source projection,
clipped segment, line and C32A44 number owners. Source gates and tables remain
authoritative. Saved values are C locals; five loop words use ordinary host
scratch at $4B06-$4B0F. No CPU stack, opcode dispatch or chipset enters the
native runner. Reference observer/hooks remain compatible.

A real Free Flight keyboard probe found map entry's unconnected voice child.
Both CONTEXT_COMMAND_MAP_VOICES and the existing REQUEST_VOICES invoke source
C0F4A6, so map entry now uses the same connected four-channel release.
With Space at 1800, Free Flight at 3000, Return at 4100, location/aircraft
keys at 5000/5400 and M at 6200, the runner opens its map. At PAL 6500 both
source grid gates are active, the five loop words show completed X/Z runs,
the source coordinate labels are drawn, and marker state is published.
Local preview: `build/native-flight/frame-markers-map6500.png`.

`native_frame_markers_oracle.c` executes original complete C2B564 and all
children against the actual native wrapper. 385 cases agree in all non-stack
RAM/drawing planes, excluding only the ten native scratch bytes. 224 cases
change visible pixels. Cases vary inactive gates, modes, all 16 record slots,
three marker types, caching/blink/selection, heading buckets, origin offsets,
height and five cardinal matrices. The final case consumes the actual runner
checkpoint without fixture overrides. The actual map checkpoint also passes.

`check_frame_tail.py` checks all three frame oracles (961 cases) from one
native demo checkpoint, then opens the map in the real frontend and compares
its full grid owner. Legacy reference owners C2AFFA/C2B3C2/C2B564/C2B928/C2B952
each pass 128 complete register/PC/SR/all-RAM cases. Native carrier
qualification, save/restart/reload and 15 result cases pass using existing
original consumed-input evidence. The full native demo still completes
4,892 updates with unchanged final RAM. Native link omission, both compiler
reference builds and all 12 focused reference contracts pass. No full
original replay was run.

C2B564's native child tree is **1/1 complete (100% of this source scope)**.
Of six identified missing frame owners, **5/6 are connected (about 83%)**.
Only C30A00 stores icons remain in that inventory. This counts connections,
not equal effort or whole-game completeness. Recorded-frame parity remains
0/3 accepted, startup lead 37 ticks, and Copper fade remains excluded.
