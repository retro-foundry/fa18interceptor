# run060 root pose initialization writer

Classification: **runtime writer trace / dataflow**. This identifies the
instructions that replace the root pose-candidate triple during run060's early
lifecycle. It does not identify the game-state label of that lifecycle.

The sealed run060 sampler first observes the root triple change during replay
frame 189. An exact Engine9000 checkpoint immediately before that event
(`frame 187`) retained the old bytes. Stepping from that checkpoint with no
future input finds these three writes to `$C46184+$14`:

| Instruction | Effect | Value written |
| --- | --- | ---: |
| `$C094FC: MOVE.L D1,$18(A2)` | root `+$18` | `$00007708` |
| `$C09534: MOVE.L D0,$14(A1)` | root `+$14` | `$11982C00` |
| `$C09538: MOVE.L D2,$1C(A1)` | root `+$1C` | `$1059A000` |

At every write, the resolved address register is `$C46184`. The before and
after tuple is therefore:

```text
$14/$18/$1C: 10545920,00000708,10A404F0
          -> 11982C00,00007708,1059A000
```

The first writer lies in the `$C0924A` early setup packet. Its `$C09498`
route selects a table-indexed record, derives a value for root `+$18`, then
calls the root transform entry `$C091E0` with `(D3,D4,D5) = (11,0,$68)`.
The transform reads root matrix `+$92..+$A2` and root base `+$14/+18/+1C`;
its transformed `D0` and `D2` are committed by `$C09534/$C09538`.

## Matrix reset in the same transition

The root's adjacent nine words at `+$92..+$A2` also change exactly once in
frames 1--300, on frame 189. From the same pre-write checkpoint, the stepped
writers are `$C2E536`, `$C2E558`, `$C2E588`, and `$C2E5A8`, all stores in
`$C2E514` (`compose_alternate_three_angle_matrix`). They replace the prior
matrix with:

```text
$92..+$A2 = 4000,0000,0000, 0000,4000,0000, 0000,0000,4000
```

The live composer inputs are `D0.w=D2.w=D4.w=0`; its documented fixed-point
formula therefore produces the identity matrix. `$C095B8` calls the record
matrix-update tail `$C2D954` immediately after committing the root tuple,
which leads to this composer. This proves a qualification-transition reset of
the root transform matrix alongside the placement update. It strengthens the
orientation-matrix candidate, but does not distinguish an aircraft attitude
matrix from a camera/render transform.

The same bounded trace shows `$C1011E: JSR $C0924A` immediately before this
packet. Its active path reaches `$C10102` through `$C0FFE2`; the existing
qualification-transition trace proves that this is the mode-9 post-gate
branch selected by the qualification menu path. Thus this is a
**qualification-transition placement update**, not unobserved renderer
scratch or an input-driven per-frame integrator.

It does **not** yet prove whether the selected record is the aircraft, camera,
or a shared aircraft/camera pose. It also does not show that this is the only
initialization route for the record.

Authority: `build/run060_early_root_pose_mutations/memory_region_mutations.json`,
`build/run060_frame00187_root_pose_writer_stepped/memory_writes.json`, and
`build/run060_frame00187_root_pose_writer_full_context/memory_writes.json`,
`build/run060_early_root_matrix_mutations/memory_region_mutations.json`, and
`build/run060_frame00187_root_matrix_writer_full/memory_writes.json`,
the byte-exact source around `$C091E0-$C09249`, and
`analysis/routines/c0fece_delayed_menu_transition.md`.
