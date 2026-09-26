# Run060 scheduler setup transition

Classification: **aligned checkpoint sampling; writer not yet identified**.

Starting from the sealed global-frame-9,200 checkpoint, the recording suffix
was replayed with frame alignment and sampled after each restored frame.  The
first observed scheduler setup mutation is at restored frame 10 (global frame
9,210):

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

The sampler records a frame-level mutation only.  It does not identify the
CPU instruction that performed the frame-9,210 setup stores or establish a
qualification predicate.  Its retained authority is
`build/run060_frame09200_scheduler_field_samples/memory_region_mutations.json`.
