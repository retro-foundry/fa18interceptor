# run001 early control-word sampling

This is a normal full-frame replay measurement, not instruction stepping. The
artifact `build/run001_takeoff_control_samples.json` restores frame 2,696 and
replays frames 2,697--3,000 with the original events in that interval.

## Observed contract

Recorded input `J 0 5 1` is delivered at frame 2,697 and its matching release
`J 0 5 0` at frame 2,978. Between frames 2,703 and 2,948, `$C45778` rises from
`$0208` to its observed upper bound `$03C0` in `$20`-word increments. Its
first sampled value after the press is `$01E8`.

`$C4577C` equals `$C45778` in every sampled frame of this interval, including
each observed increment and the saturated state. `$C45776` remains `$FEF0`.
The earlier bounded `$C13E10->$C25D84` packet supplies the code-level edge
that writes `$C4577C`; this sampling supplies repeatable frame-level evidence
of the resulting lockstep state.

`J 0 5` is kept as a recording code. This run alone does not establish a
physical control name or an aircraft-axis name for either word.
