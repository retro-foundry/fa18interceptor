# run075 frame 392 cockpit-entry boundary

This note records the first visible change after the verified menu/demo clear
sequence. It is limited to runs 60 and above and uses the canonical run075
replay oracle.

## Pixel boundary

Frames 287 through 391 are identical to the all-black frame 286 page. Frame
392 changes 361 of the 64,000 RGB444 pixels. The changed-pixel bounding box is
native coordinates `x=7..318, y=101..199`. The pixels are sparse and dark;
this is the first visible cockpit or flight-scene construction boundary.

The native port must remain locked before this boundary until the calls that
produce these pixels are identified. A static frame fixture would not explain
the replay-driven transition.

## Runtime evidence

The replay reaches `$C2FD22` at Engine frame 291. The complete entry
register record is retained in `build/run075_c2fd22_trace/report.json`.

The first instructions load five display-plane pointers from `$C456BE`, then
clear four active plane streams with a `DBRA` loop. The routine therefore
clears the planar visual buffers; it is not the source of the frame-392
cockpit pixels. The existing native `fa18_renderer_clear_planar_words`
contract models this operation as a semantic display-buffer clear.

The trace was started from the canonical restored state with the recorded
run075 input events applied at their original frame boundaries. The bounded
instruction trace advances debugger frames while stepping, so it is used for
callee behavior and register evidence only; frame 392 pixel ownership still
requires a normal replay trace around the first visible change.

## Next port boundary

Trace the normal frame-391/392 renderer and flight-state calls, then model the
first cockpit or scene records as C structs. Do not add the flight model or
advance the native frame gate until the frame-392 pixels and their input/state
cause are accounted for.
