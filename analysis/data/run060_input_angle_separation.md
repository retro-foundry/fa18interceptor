# run060 input accumulator / root-angle separation

Classification: **scenario-backed negative dataflow evidence**. This limits a
previously plausible control claim; it does not show that run060 has no player
input path.

The sealed replay was sampled at every frame from 930 through 1050 for the
two `JOY0DAT`-derived accumulators and root orientation angles. At recorded
events `[J,0,5,1]` (frame 939), `[J,0,5,0]` (977), `[J,0,4,1]` (1005), and
`[J,0,4,0]` (1013), all three words remain unchanged:

```text
$C45776 = $01BF
$C45778 = $03C0
$C4577C = $03C0
```

Meanwhile root `+$66` begins changing at frame 949 and evolves
`$7070 -> $7048 -> $7018 -> ... -> $6EB0`; root `+$68/+6A` remain zero in
this window. Thus the active pitch-like angle update is **not explained by a
sampled change** to these accumulator words during those recorded events.

The bounded `$C13D84` packet remains a verified consumer of `$C45778` and a
structural route to root-motion updates. That does not prove that this route
owns the observed run060 angle evolution. Possible explanations include an
unchanged held control state, another input path, or scripted/control state;
the current evidence does not choose among them.

Authority: `build/run060_input_to_root_angle_frame0930_1050` and
`analysis/routines/c13e10_control_state_stage.md`.
