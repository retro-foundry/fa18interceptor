# run060 third control-lane path

Classification: **sealed-replay, scenario-backed input-to-orientation chain**.
This documents the complete executed numerical path from the frame-1751
`J 0 6 1` event to a non-zero third root orientation component. It does not
assign a physical joystick direction or an aerospace name to that component.

```text
frame 1751  J 0 6 1
  -> frame 1754 input phase calls $C1B558
  -> root +$65 bits 3:2 become $08
  -> $C1B410: signed root +$2A 0 -> -3
  -> $C1342C: abs(-3) * 2 = table offset +6
  -> $C3D690[+6] = $003F; negate to $C45B62 = -63
  -> local target: (63 >> 1) + (63 >> 3) = 38
  -> half-step working root +$5A: 0 -> 19 ($0013)
  -> $C2DEE0 input D0/D2/D4 = 0/0/$0013
  -> output D4/D5/D6 = $7038/$0000/$0010
  -> $C2D954 publishes root +$66/+68/+6A
```

The same frame’s composed orientation matrix is the downstream consumer of
the published triple. The relationship establishes an independent third
control-to-orientation path alongside the initial pitch-like `+$28` path;
the two components' real-world axis labels remain unassigned.

Authority: `routines/c1b540_write_mid_nibble_control_field.md`,
`routines/c1b410_update_three_axis_control_bytes.md`,
`routines/c1342c_matrix_side_leaf.md`,
`routines/c2dee0_matrix_transform_stage.md`,
`routines/c2d94e_record_matrix_update_tail.md`.
