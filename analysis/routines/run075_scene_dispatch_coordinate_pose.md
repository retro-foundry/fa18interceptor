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
publication all have a contract.  `coordinate_update_positive_pair.{c,h}`
now ports the exact `$C123FA-$C1294E` route used by this packet: the source
inputs are `(0,0,$00800000,0,$0B000000,-1)`, shift 14 is selected, rounded
ratio 745 shifts to Hunk-63 word index 11, and its source word 25 gives
`$6FB8`.  The adapter remains intentionally unscheduled by
`scene_dispatch_runtime` until the source `$C28800` target-record ownership
and dispatch continuation are composed: the observed target is `$C47D84`,
while selector `$4000` resolves its linked source record at `$C4E184`, beyond
the 17-slot native dispatch bank.  `scene_dispatch_coordinate_pose` therefore
requires its linked record explicitly rather than treating the selector as a
local index.  The existing
`coordinate_update_negative_pair` adapter covers a different `$C123FA`
branch and is not used to synthesize this packet.
