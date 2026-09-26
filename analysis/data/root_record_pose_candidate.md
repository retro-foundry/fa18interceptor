# Root control record pose candidate

Classification: **scenario-backed active flight-pose inference**. This is
deliberately not a final aircraft-position/orientation identification.

Three independently established facts now meet at the root selected record
`$C46184` in run060:

1. `$C13D84` selects it for a real joystick/control update and publishes it at
   `$C18210`.
2. `$C3201A` consumes root `+$18`, applies `>>10` then `*5`, and the
   deterministic end scenario correlates its stable result 145 with the
   cockpit `145 FT` display. Thus the middle component is scenario-backed
   vertical altitude.
3. `$C1C54E-$C1C63D` consumes root `+$14/+18/+1C` as a three-component base
   vector. It derives an offset from root `+$92..+$A2`, adds it to the base
   tuple, and publishes the transformed result for projection.

The byte-exact direct arm is:

```text
D0 += (long[root+$14] & $003FFFFF)
D1 +=  long[root+$18]
D2 += (long[root+$1C] & $003FFFFF)
D0,D1,D2 = -D0,-D1,-D2
```

## Inference

Because the middle member is altitude and the adjacent first/third members are
used as matching components of the same projection vector, root `+$14` and
`+$1C` are strong **horizontal pose-coordinate candidates**. The root
`+$92..+$A2` nine-word transform matrix is an active **flight
orientation-transform matrix**: live `$C2E514` calls update it from root
`+$66/+68/+6A` angle state during the run060 turn segment; see
`data/run060_root_attitude_matrix.md`.

The root tuple is sampled moving coherently during run060's qualification
flight and returns to its start value on the replay's reset event; see
`data/run060_root_pose_motion_timeline.md`. Direct stepped traces now identify
the committed vertical writer at `$C14D32` and paired horizontal writers at
`$C25E6E/$C25E72`; see `data/run060_root_pose_integrator.md`. In cockpit play
the pose can be the aircraft, camera, or a deliberately coincident
aircraft/camera context. The root is selected by the live control stage, which
supports (but does not prove) player ownership.

The initial identity-matrix trajectory supplies a provisional coordinate basis:
`+$14` is the lateral horizontal candidate, `+$18` is vertical altitude, and
`+$1C` is the initial forward horizontal candidate. The first changing angle
`+$66` rotates only the vertical/forward plane, making it pitch-like. See
`data/run060_root_axis_orientation_inference.md`.

## Early run060 initialization event

The deterministic per-frame sampler over root `+$14/+18/+1C` finds exactly
one mutation in frames 1--300: replay frame 189. There is no recorded input
event from frames 160--210. The mutation changes all three components
together:

```text
+$14  $10545920 -> $11982C00
+$18  $00000708 -> $00007708   (5 -> 145 under the cockpit-altitude scale)
+$1C  $10A404F0 -> $1059A000
```

The exact stepped writer trace now resolves the transition: `$C094FC` writes
`+$18`, and `$C09534/$C09538` write `+$14/+1C`. The latter pair is the output
of the root matrix/base transform entered at `$C091E0`. This is a deliberate
initialization or placement update, rather than ordinary per-frame renderer
scratch. See `data/run060_root_pose_initialization.md` for the bounded
instruction evidence. The same qualification transition resets `+$92..+$A2`
to a fixed-point identity matrix through `$C2E514`; this is direct matrix
writer evidence, not merely co-timed sampling. It is still not enough to
choose aircraft pose over a coincident camera pose, but it strengthens the
root pose interpretation.

## Not yet proven

- The real-world sign and units of the inferred lateral/forward axes.
- Whether root position is aircraft world position, camera position, or a
  shared player/camera pose.
- Whether the matrix is aircraft attitude, camera attitude, or a shared
  aircraft/camera orientation transform.
- The fixed-point units, wrap behavior, and writer/integrator for the two
  horizontal components and matrix.

The late-run stability of the triple does not reject this candidate: it is a
stable portion of this one qualification scenario. A controlled differential,
followed through world placement or camera output, is needed for promotion to
an aircraft-pose contract.
