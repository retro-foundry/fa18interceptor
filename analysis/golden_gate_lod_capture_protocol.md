# Golden Gate distance-only detail capture

Purpose: distinguish distance-driven line/face selection from the camera
changes present in `run035`. This is one continuous recording; do **not** use
the game's Save, Restore, Rewind, Reset, or Warp controls while recording.

1. Start a normal free-flight recording and fly until the Golden Gate is the
   red structure in the **outside viewport**. Ignore red cockpit/HUD pixels.
2. Put the bridge approximately in the centre of the viewport, level the
   aircraft, then leave pitch, roll, and heading controls untouched.
3. Hold a constant throttle and fly straight toward it for roughly 20--30
   seconds. Do not turn away after the closest view; stop the recording then.
4. Close the recording session normally. It already contains every frame; no
   intermediate save is needed or wanted.

The useful result contains the same red landmark at visibly small, medium, and
large extents while the horizon and bridge bearing stay stable. The existing
analysis will compare `$C355xx` line contexts, filled-face contexts, selected
terrain-template streams, and projected screen bounds at those extents.

`run035` already establishes the red Golden Gate line sequence
`$C3559A/$C355D2 -> $C355CE -> $C3558A`, but its heading and attitude vary.
This protocol supplies the missing control needed to call that sequence LOD
rather than an approach-specific detail change.
