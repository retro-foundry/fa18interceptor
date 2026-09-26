# run060 root attitude-matrix update

Classification: **runtime writer trace plus static transform dataflow**. This
establishes the root matrix as active orientation-transform state during the
qualification flight. It does not establish whether the orientation belongs
to the aircraft, cockpit camera, or a shared transform.

At the first attitude-change window (checkpoint frame 950), stepped replay
catches `$C2E514` stores to root `$C46184+$92..+$A2` with `A1` advancing from
`$C46216` through `$C46226`. Consecutive calls replace the matrix words; for
example, the first traced call changes the lower-right 2x2 terms from:

```text
4000,0000, 0000,4000
    ->
3FFF,0039, FFC7,4000
```

The 25-frame run060 timeline shows these updates beginning near frame 951 and
continuing through the recorded turn/input segment. They reset to the identity
matrix at the qualification reset event, alongside the root pose tuple.

Static `$C2D94E-$C2D99B` provides the producer contract for this exact
destination: it publishes three record words to `+$66/+68/+6A`, advances the
same record pointer by `+$92`, derives a three-angle input tuple, and calls
`$C2E514` (`compose_alternate_three_angle_matrix`). Therefore the matrix is
not renderer scratch: it is a live three-angle fixed-point transform derived
from state stored in the moving root record.

This promotes root `+$92..+$A2` from a pose-orientation candidate to an active
**flight orientation-transform matrix** for run060. The pose tuple and matrix
are consumed together by the projection path. Naming it aircraft attitude
would still exceed the evidence because a cockpit camera can use the same
orientation transform.

Authority:
`build/run060_frame00950_root_attitude_writer/memory_writes.json`,
`build/run060_root_record_first4000_25_frame_samples`,
`source_amiga/observed/publish_record_matrix_update_triple.asm`, and
`source_amiga/observed/compose_alternate_three_angle_matrix.asm`.
