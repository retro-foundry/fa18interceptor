# run003 second control-lane packet

Classification: **scenario-backed behavioural dataflow**.

Authority: sealed `captures/run003/playback.e9k`, restored from
`captures/run003/initial_state.bin`.  The bounded trace is retained in ignored
build output at `build/run003_frame5360_axis_y_update/`; it hit `$C1B410` on
absolute replay frame 5,360, executed 23 instructions, and stopped at the
local return (`$C1B4CE`).  Input through the breakpoint was delivered by the
ordinary deterministic replay; no later input was delivered while stepping.

The preceding raw-key contract shows the comma held command (`raw $38`) writes
`$80` to the high two bits of active control byte `$C461E9` (`+$65` of root
`$C46184`).  At the bounded update entry, the instruction stream loads that
byte, clears the first-field path, isolates `$80` with `andi.b #$C0,d3`, and
takes the non-`$40` decrement branch.  The final slow-RAM snapshots establish
the only lane change:

```text
$28  $00 -> $00
+$29  $00 -> $FF  (signed 0 -> -1)
+$2A  $00 -> $00
```

Thus `$80` is a second-lane decrement command, complementary to `$40` by the
static branch condition.  It is intentionally not promoted to a physical
rudder/yaw claim: the trace proves input-mask-to-lane dataflow and bounded
arithmetic, not an aircraft coordinate convention or visible motion outcome.
