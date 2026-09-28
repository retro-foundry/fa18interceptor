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

The full `$C28800` trace corrects the earlier ownership inference: run075
loads `$04(A2)=$8D0E`, takes the negative branch at `$C28806`, masks it to
`$0D00`, then arithmetic-shifts it by seven to the `$C295E0` word-table index
26.  That table resolves the five-word tuple `(70,112,6144,6144,0)`; `$C28F16`
copies it to slot 14 `+$2C..+$34`, `$C28824` writes `+$38=$FF`, and
`$C2882A-$C2883A` expands it to the two longword coordinate deltas.  Against
the created slot's `+$14/+$1C`, those are exactly
`(0,0,$00800000,0,$0B000000,-1)` for `$C123FA`.

`scene_dispatch_negative_coordinate_pose.{c,h}` ports that actual bounded
negative `$C28808-$C288C4` route with a required geometry-table resolver.
`scene_dispatch_coordinate_pose.{c,h}` remains the separately ported
nonnegative `$C2883E-$C288C4` linked-record route; it must not be described as
the run075 path.  `coordinate_update_positive_pair.{c,h}` ports the exact
`$C123FA-$C1294E` route used by the negative packet: shift 14 is selected,
rounded ratio 745 shifts to Hunk-63 word index 11, and source word 25 gives
`$6FB8`.  Neither route is scheduled by `scene_dispatch_runtime` yet; the
dispatch loop must supply the live target and original Hunk-27 geometry-table
resolver without capture-derived state.  The existing
`coordinate_update_negative_pair` adapter covers a different `$C123FA` branch.
