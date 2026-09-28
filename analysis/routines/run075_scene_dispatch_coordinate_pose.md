# run075 scene-dispatch coordinate pose handoff

Classification: **sealed-replay, source-backed record-pose publication**.

The selected run075 record does not receive its `$6FB8` second angle from a
capture seed or from the root-placement copy.  The replay proves the following
source chain at global frame 272:

```text
$C28AFE -> $C28B34 -> $C28800
  -> $C123FA -> $C45AC2 = $6FB8
  -> $C288BC -> $C2D954
  -> slot 14 ($C47D84) +$66/+$68/+$6A = 0000/6FB8/0000
```

The first `$C2D954` entry in the replay is at local frame 72 (global frame
272), with `A1=$C47D84` and `D4/D5/D6=0/0/0`; it is the direct record-creation
arm returning to `$C28E02`.  Instruction stepping across that same dispatch
then reaches `$C28800`.  Its `$C2889E` call to `$C123FA` returns to `$C288A4`
after 237 instructions and leaves `$C45AC0/$C45AC2=0000/6FB8`.  `$C288AA-$C288BC`
loads that second word into `D5` and calls `$C2D954` again, which stores the
triple into slot 14.

The source-derived evidence is retained in
`build/run075_c288_coordinate_update/` and
`build/run075_slot14_angle_write_trace/`.  At the latter store, the immediate
predecessors are `$C288AC` (load `$C45AC2`), `$C288B6` (move it to `D5`), and
`$C288BC` (call `$C2D954`).

`port/scene_dispatch_coordinate_pose.{c,h}` now ports the bounded
`$C28800-$C288C4` nonnegative-link continuation: source high-byte selection,
bit-6 gate, placement-field copy, wrapped coordinate deltas, `$C123FA`
six-longword callback packet, and the `(0, output[1], 0)` `$C2D954`
publication all have a contract.  It is intentionally not scheduled by
`scene_dispatch_runtime` yet.  The existing `coordinate_update_negative_pair`
adapter covers a different observed `$C123FA` negative-only branch, so it must
not be used to synthesize this packet.  Port the actual `$C123FA` route
selected by `$C28800`, then compose the result with this dispatch continuation;
do not introduce `$6FB8` as a native default.
