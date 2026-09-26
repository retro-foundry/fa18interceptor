# Root control record pose candidate

Classification: **dataflow-backed pose inference**. This is deliberately not a
final player-position/orientation identification.

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
`+$92..+$A2` nine-word transform matrix is a strong **pose-orientation
candidate**: it transforms a selected local seed before that result is added
to the position-like triple.

In cockpit play the pose can be the aircraft, camera, or a deliberately
coincident aircraft/camera context. The root is selected by the live control
stage, which supports (but does not prove) player ownership.

## Not yet proven

- Which horizontal component is which world axis.
- Whether root position is aircraft world position, camera position, or a
  shared player/camera pose.
- Whether the matrix is aircraft attitude, camera attitude, or an adjacent
  rendering transform.
- The fixed-point units, wrap behavior, and writer/integrator for the two
  horizontal components and matrix.

The late-run stability of the triple does not reject this candidate: it is a
stable portion of this one qualification scenario, not a controlled motion
experiment. A controlled earlier-flight differential, followed through world
placement or camera output, is needed for promotion to a player-pose contract.
