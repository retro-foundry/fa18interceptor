# `$C3A94C/$C3A94E`: compact polyhedral renderer family

This family appears at the frame-12600 Golden Gate checkpoint. Its orthographic
sheet is a six-quad compact solid, roughly a tapered box/wedge in the sampled
orientation. No object name is assigned.

[Open the pre-cull orthographic sheet](../plots/frame12600_c3a94e_preclip_face_sheet.png).

## Complete source-to-face boundary

- The live record walker enters control stream `$C3A94C` four times during the
  checkpoint capture.
- The stream dispatches through `$C1F942` to `$C2005C`; all six observed faces
  return to `$C1F944`.
- The face controller is `$C3A94E`. Its distinct face-offset records begin at
  `$C3ABBA`, `$C3AC1A`, `$C3AC26`, `$C3AC74`, `$C3ACA0`, and `$C3ACAC`.
  Every observed face is a four-point polygon, yielding 24 renderer-observed
  polygon edges.
- A later `$C3A96E` transform occurrence proves the immutable input range:
  four triples at `$C3A96E-$C3A985` are transformed by `$C1F4AC/$C1F528`
  through `$C45BD8` into `$C48390-$C483A7` (slots 0--3). `A3=$C483A8` at
  `$C1F6F8` is the next free destination, not a pre-existing input record.
- `$C1F708` then loads `A5=$C3A94C`. `$C3A94C` resolves its stream base
  through `$C3A942` to `$C3A958`; that stream dispatches face control at
  `$C3ABB2`.
- The selected `$C20EC4` handler derives slots 4--9 from the direct block.
  Its stores are `$30/$36($C48390)` for slots 8/9, followed by stores at
  `$6($C483A2)`, `$6/$C($C483A8)`, and `$12($C483A8)` for slots 4--7.
- `$C2005C` reads the completed slots 0--9. The six static face records use
  every one of those slots; their pre-cull sheet is therefore a complete
  renderer-topology reference for this procedural component.

## Status

The four static input triples, `$C20EC4` derivation, static control/face
records, and `$C2005C` consumer are now connected. This is an exportable
**procedural compact polyhedral component**, not a contiguous static mesh:
retain the four triples, face/control streams, matrix transform, and derivation
routine; never export `$C48390` as source geometry. Its game-object identity
remains unnamed.

The nearby `$C3513C` dual-lane transform remains a separate C3B9 controller
path despite sharing the mutable workspace; it is not this component's source.

Authority: `build/run031_frame12600_c3a96e_occ2_trace/trace.jsonl`,
instructions 0--290, plus
`build/run031_frame12600_face_preparations_64f/face_preparations.json`.
