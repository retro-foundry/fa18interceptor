# run060 control-mask transition at the initial turn

Classification: **normal-replay timing evidence**. This records the root
record's byte `+$65` (`$C461E9`) around the first pitch-like update; it does
not by itself identify the writer or prove a recording-event causal edge.

Sampling sealed run060 after every replay frame gives:

```text
frames 930-934  $00
frames 935-941  $01
frame 939       recorded J 0 5 1
frames 942-948  $20
frames 949-954  $21
frames 955-961  $20
frames 962-966  $21
frames 967-970  $20
```

The live `$C1B410` packet at frame 949 consumes `$21`; its `$20` direction
field decrements root signed byte `+$28` from −1 to −2, while its low two bits
are not used by that three-axis updater. The `$01 -> $20` transition is
therefore already present before the packet that produces the first
pitch-like `$7070` angle tuple.

The recorded `J 0 5 1` is three replay frames before the observed `$20`
transition, but its direct `$C1718E` callback has zero JOY0DAT delta and
leaves the callback accumulators unchanged. The timeline supports correlation
only; the concrete writer of `$C461E9` in frames 940-942 remains untraced.

Authority: `build/run060_root65_930_970.json`,
`build/run060_frame0947_three_axis_control_writer/`.
