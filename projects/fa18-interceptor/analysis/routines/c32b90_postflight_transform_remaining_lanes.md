# `$C32B90` remaining postflight transform-lane submissions

Classification: **static-only continuation**. The run024 frame-23000 trace
reaches the prior second-lane call but does not enter this tail.

`source_amiga/observed/submit_remaining_postflight_transform_lanes.asm` is
byte-exact for `$C32B90-$C32BD1`. It repeats the `$C330FE` submission for
pointer slots `8(A5)` and `12(A5)`, gated by bits 1 and 0 of `$C45955`, then
uses `DBRA D0` to return to `$C32B0E` for the next offset/mask pair.

Together with the observed first two lanes, this statically establishes a
four-pointer-slot/bit-3-to-bit-0 compositor pattern. Ordering and plane
semantics remain unassigned pending runtime execution of this tail.
