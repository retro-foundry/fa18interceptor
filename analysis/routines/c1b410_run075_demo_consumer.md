# `$C1B410` run075 demo consumer packet

This packet extends the existing `$C1B410` structural contract with a run075
demo invocation. It was captured with the sealed `build/engine9000-replay`
runner from `captures/run075/initial_state.bin`, applying the sealed replay
events in order. The bridge breakpoint reached the routine at debugger frame
703; the bounded instruction trace began at frame 704.

## Entry evidence

At entry, `A1=$C46184` and the caller stack returns through `$C25C76`,
`$C0F016`, `$C1C6B6`, `$C22D88`, and `$C25C70`. The first instruction reads
`$65(A1)`. The observed byte is `$01`, so all three two-bit fields are zero.
The routine therefore writes zero to `$28(A1)`, `$29(A1)`, and `$2A(A1)` and
returns after the three bounded lane paths.

The following caller checks `$C459B4`, then reads the same active record's
flags and dispatch state. This connects the lane updater to the ordinary
demo update chain, while the trace does not assign physical axis names or
prove a nonzero control sample at this invocation.

## Native consequence

`fa18_flight_update_control_lanes` already represents this packet: packed
value `$01` produces `(0,0,0)` in the semantic lane struct. The caller chain
and the conversion from replay joystick fields to the packed `$65` control
byte remain open and must be traced before adding flight motion behavior.

Authority: `build/run075_c1b410_700/{report.json,trace.jsonl}`.
