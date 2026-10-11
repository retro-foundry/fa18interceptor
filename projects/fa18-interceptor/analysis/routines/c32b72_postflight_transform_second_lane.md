# `$C32B72` second postflight transform-lane submission

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes this complete `$C32B72-$C32B8D` path.

`source_amiga/observed/submit_second_postflight_transform_lane.asm` is
byte-exact. It uses pointer slot `4(A5)`, combines the prepared transform
offset, selects `$0B0A` or `$0BFA` from bit 2 of `$C45955`, merges the table
word, and calls `$C330FE`.

Relative to `$C32B40`, this establishes the second distinct pointer slot and
enable bit; it does not assume a semantic plane ordering.
