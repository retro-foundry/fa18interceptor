# `$C2AA9C-$C2AB33`: map packet depth stage

This source prefix initializes four renderer words, negates the full projection
depth at `$C45A78`, optionally normalizes it, and chooses the normal and wide
map packet passes.

The full depth is the value published at `$C1C636`, represented by
`FA18ProjectionPacket.depth_metric`; it is not the packet's low `y` word.
When `$C457DD` is clear and the negated value is at most `$7FFF0`, the source
arithmetic-shifts by four, multiplies its low word by `$8000 / max(2,C45A42)`,
then arithmetic-shifts by four again. A metric above `$3F8` runs the normal
pass; the wide pass always follows.

The frame-382 run075 parent trace has `$C45A78=-125` and `$C457DD=$80`, so it
skips normalization, derives metric `125`, skips normal, and enters the wide
pass. It then reaches six `$C246A0` calls and three `$C2FF48` submissions
before returning to `$C0F082`.
