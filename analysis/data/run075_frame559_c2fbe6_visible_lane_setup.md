# run075 frame559 `$C2FBE6` visible lane setup

The run075 breakpoint at `$C2FBE6` reaches the four lane renderer helper at
engine frame 561. The bounded trace shows the helper selecting each lane from
`$C456E7`, applying the optional `$C456E8` adjustment, loading the lane base
from the current plane table, and adding the computed row offset before
writing the blitter packet.

The lane packet is submitted through the same `$40/$42/$52/$48/$54/$74/$62/$58`
Custom register offsets used by the other renderer helpers. The first lane
starts from a pointer derived from the current table plus the row offset; the
following lanes repeat the operation with the next plane selection. This is
the visible page boundary after the `$C30678` temporary workspace pass.

The trace also shows the per lane enable tests at `$C456E7` and the optional
row adjustment at `$C456E8`, so the native operation needs a semantic active
page plus a lane mask and row offset. It must not use the temporary packet's
`0x6e..` values as screen coordinates.

Authority: `build/run075_frame559_c2fbe6_500/trace.jsonl`, captured from the
sealed run075 restore and playback.
