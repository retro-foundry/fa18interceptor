# Run060 scheduler setup transition

Classification: **native checkpoint state transition; writer not yet
identified**.

Native boot-restore checkpoints independently show that the gate is still
uninitialized at GUI frame 9,208 (`+$6E=$0007`), reaches zero at frame 9,209
without changing scheduler fields, and completes setup on the following frame
9,210. The transition values are:

| Address | Before | After |
| --- | ---: | ---: |
| `$C45798` | `$00` | `$FF` |
| `$C4582A` | `$00` | `$03` |
| `$C4582C` | `$FF` | `$04` |
| `$C458AD` | `$00` | `$01` |

The signed byte at `$C4582C` subsequently samples as `$03`, `$02`, `$01`,
`$00`, and `$FF` at global frames 9,218, 9,227, 9,236, 9,245, and 9,254.
At frame 9,263 the bounded parent-tick trace observes its negative value and
the phase-three route that installs `$C11078`; that callback advances to
`$C110A4`.

The native snapshots prove state ordering only. They do not identify the CPU
instruction that performed the frame-9,210 stores or establish a qualification
predicate. The full preceding native state sequence is retained in
[`run060_native_scheduler_gate_timeline.md`](run060_native_scheduler_gate_timeline.md).
The older aligned direct-core sampler remains local diagnostic evidence, not
the validation oracle for this timing claim.
