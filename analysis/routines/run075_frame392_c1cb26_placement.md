# run075 frame-392 alternate placement child

The frame-392 `$C1CB26` child is bounded by
`build/run075_frame392_c1cb26/trace.jsonl`: it hits at local frame 192 and
returns to `$C0F0BA` after 1,402 instructions. Input before the trace follows
normal run075 replay; later input is deliberately suppressed while stepping.

It takes the alternate fixed entry: `$C1CB26` writes `$01` to `$C45865`, copies
`$C459AE` to `$C459AA`, and the common loop selects `$C4F03A`. The depth packet
is `$C45A78=FFFFFF83`; negation, word `>>7`, and the signed `$C41130` lookup
publish `$4000` to `$C459B2`.

The trace walks records at offsets `$0000`, `$0018`, `$0030`, `$0048`, and
`$0060`; the last is the `$FFFF` terminator. The first record has header low
nibble three, reaches `$C1CC86`, and enters `$C1EE14`; its returned nonpositive
result is stored at the source record's `+20` field. The remaining records
exercise the existing signed countdown/skip paths. This is concrete evidence
for the already-native alternate-entry placement stage, but does not identify
the placement data as terrain or establish a scheduler cadence.

The sibling `$C1CB14` call is independently bounded by
`build/run075_frame392_c1cb14/trace.jsonl`. It performs the same
`$C45A78=FFFFFF83` depth-table calculation and publishes `$4000`, but keeps
the primary selector byte clear, chooses `$C4E9AA`, and immediately finds the
`$FFFF` terminator at offset zero. It returns to `$C0F0AC` after 25
instructions without descriptor work. Thus the frame-392 parent invocation's
only observed placement-descriptor traversal is the following alternate
`$C1CB26` child.
