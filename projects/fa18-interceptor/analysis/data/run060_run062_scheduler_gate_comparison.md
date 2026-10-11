# Run060/run062 mode-9 scheduler-gate comparison

Classification: **sealed replay branch comparison**.

Both qualification scenarios reach the mode-9 arm `$C09E78 -> $C0A2F0`, but
their observed scheduler-gate entries diverge before any physical meaning is
assigned to the record fields.

| Scenario | Gate snapshot | Observed path |
| --- | --- | --- |
| run060, global frame 9,210 | `$C45798=$00`; `$C46184` first four bytes `$11C8C082` | Tests bit 6 at `+$01`, masked word `+$02`, and word `+$6E`; all pass, then writes the scheduler state. |
| run062, restored frame 6 from global-frame-2,000 checkpoint | `$C45798=$04`; `$C46184` first four bytes `$11C8208A` | `$C0A2F0` takes its first nonzero-selector exit at `$C0A2F2`, returning through `$C09E82/$C09E94` in four instructions. The record-header tests are not reached. |

The run062 trace is retained at
`build/run062_frame2000_c0a2f0_gate_trace/`; the run060 gate writer trace is
`build/run060_frame09200_c0a2f0_caller_probe/`.  This proves a scheduler-route
distinction and rules out using this run062 invocation to infer the result of
the later record-header tests. It does not identify a landing or qualification
predicate.
