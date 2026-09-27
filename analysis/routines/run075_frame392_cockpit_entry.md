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

## Normal-frame renderer boundary

The corrected normal-replay PC profiles distinguish the two adjacent host
frames. They restore the canonical state after replay frame 200, deliver
recorded input by global frame label, and profile exactly one subsequent
frame:

```text
python scripts/profile_window.py --restore captures/run075/initial_state.bin \
  --playback captures/run075/playback.e9k --first-frame 391 --last-frame 391 \
  --frame-offset 200 --output build/run075_profile_391_offset
python scripts/profile_window.py --restore captures/run075/initial_state.bin \
  --playback captures/run075/playback.e9k --first-frame 392 --last-frame 392 \
  --frame-offset 200 --output build/run075_profile_392_offset
```

The frame-392 profile newly samples `$C27B32-$C27C9A` and `$C2F68A`, while
the frame-391-only profile instead includes `$C249…/$C24D…` and
`$C302…/$C306…` paths. The `$C27B…` range is the observed `$C279D0`
projection-to-polygon traversal: it transforms selected records, fills the
`$C4B392-$C4B39D` screen-pair buffer, and submits through `$C2FF48`.
`$C2F68A` is within the four-plane pixel primitive. This makes a renderer
submission path a direct candidate for the first visible scene change, but
does not yet identify the submitted records as terrain or assign the 361
pixels to a particular primitive.

## Next port boundary

Trace the normal frame-392 `$C279D0 -> $C2FF48` submissions and their display
writes, then model the first cockpit or scene records as C structs. Do not add
the flight model or advance the native frame gate until the frame-392 pixels
and their input/state cause are accounted for.
