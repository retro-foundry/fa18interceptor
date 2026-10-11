# run060 first pitch-like control path

Classification: **sealed-replay, scenario-backed input-to-orientation chain**.
This is the first complete executed path from a recorded joystick event to the
active root orientation update. It establishes program data flow and a
pitch-like transform effect; it does not name a physical joystick direction or
claim a general aircraft-control mapping outside this scenario.

```text
frame 939  recorded J 0 5 1
  -> frame 942 $C16F1C derived-bit route
  -> $C16F88 calls $C1B50C
  -> $C1B516/$C1B538 merge $20 into root +$65: $01 -> $21
  -> frame 949 $C1B410 consumes +$65 bits 5:4
  -> root signed +$28: -1 -> -2
  -> $C1342C selects $C3D690 word lane +4 = $002D
  -> target = -$002D = -45; quarter-step working +$56: -6 -> -15
  -> $C2DEE0 returns D4/D5/D6 = $7070/$0000/$0000
  -> $C2D954 publishes root +$66/+68/+6A
  -> $C2E514 composes root +$92..+$A2 attitude matrix
```

The resulting matrix first changes the vertical/forward plane while preserving
the lateral basis axis, so the published `+$66` component is pitch-like for
the run060 coordinate basis. The active root pose then uses that matrix in the
already traced flight-pose update chain.

Authority: `routines/c16f1c_joy0dat_bit_pair_state.md`,
`routines/c1b410_update_three_axis_control_bytes.md`,
`routines/c1342c_matrix_side_leaf.md`,
`routines/c2dee0_matrix_transform_stage.md`,
`data/run060_root_axis_orientation_inference.md`.
