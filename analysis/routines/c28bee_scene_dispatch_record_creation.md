# `$C28BEE-$C28E08`: scene-dispatch record creation arm

Classification: **static direct record-construction contract**.

This is the creation arm within `$C28B34`, itself reached through `$C28AFE`
from `$C28722`. It clears exactly 41 longwords (164 bytes) at the selected
`$C46184 + (index << 9)` slot: it must not be represented as clearing the
entire 512-byte record. It then initializes source-derived header flags,
record type/index, dispatch coordinates and components, default control
fields, fixed-point `$14/$18/$1C` placement, and hands `(0,0,0,type&$F0)` to
`$C2D954`. In particular, `ORI.W #$0140,+0` establishes bit 6; this is the
same later `$C28B6A` selection gate.

`port/scene_record_dispatch.c` implements this direct arm as
`fa18_create_scene_dispatch_record`. Its contract test verifies the clear
extent, bit fields, wrap-safe fixed-point placement, default fields, and
ordered matrix publication. `$C28B34`'s enclosing pointer-table iteration,
the `$C295E0` geometry lookup, and the `$C28F16` coordinate helper remain
outside this narrow adapter.

In the frame-370 continuation trace, `$C28B34` is reached three times with
table cursors `$C2987A`, `$C29884`, and `$C2988E`. None reaches `$C28BEE`:
each selected slot already has bit 6 at `+1`, so that trace proves dispatcher
reachability and the skip condition, not creation timing. The selected
run075 pose record is therefore still a producer/integration boundary for
the first cockpit frame.

`$C28B34` first forms its template pointer as `$C22048 + word(A2)`, so the
source word is an offset within Hunk 16, not a relocation offset itself. Its
`MOVEM.L` copies all five pointer words to `$C22188 + index*20`, but this
bounded path consumes only the second word to derive the descriptor class.
The native dispatcher therefore resolves only words that have Hunk
relocations; untouched source pointer words remain opaque rather than causing
the otherwise valid dispatch to fail.
