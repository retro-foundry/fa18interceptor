# Golden Gate close-range renderer / LOD probe

The dense ordinary-replay contact sheet supplies close visual anchors for the
run031 Golden Gate pass.  The closest sampled view is frame 11,850, where the
suspension span crosses the full external-camera image directly above the
aircraft.  Frame 11,925 is a second low-altitude pass beside a tower/deck.

[Open the dense 25-frame contact sheet](../plots/golden_gate_dense_replay_contact_sheet.png).

## Matched no-input renderer windows

Each sealed checkpoint was replayed for 64 host frames, collecting accepted
face tuples at `$C2469E` before clipping.  This makes the comparison about
the renderer family, not a screen-pixel or inferred-mesh comparison.

| Ordinary replay anchor | Visual relation | `$C355D8` pre-clip faces | `$C355A0` pre-clip faces | `$C45BEA` / C34C detail faces |
| --- | --- | ---: | ---: | ---: |
| frame 11,850 | span overhead; altitude display 260 ft | 16 | 4 | 20 |
| frame 11,925 | low-altitude tower/deck pass; altitude display 205 ft | 16 | 2 | 20 |
| frame 12,000 | cockpit Golden Gate checkpoint | 14 | 3 | 25 |

The static `$C355D8` bridge face context therefore remains active at both
close external passes.  Its 16 accepted faces at the two closest samples are
not replaced by another observed bridge context.  `$C355A0` also remains
active, while the `$C45BEA` rows remain the separate C34C aircraft-detail
context rather than bridge geometry.

Line collection independently sees `$C355D8` eight times at frame 11,850,
versus seven times in the frame-12,000 window.  This supports persistence of
the same bridge render family across the close pass.

## Static face-record overlap

The `$C355D8` pre-clip controller selects `$C35732`, `$C35760`, and
`$C3576A` in all three windows.  These are exact static-record addresses, not
workspace coordinates or a visual similarity measure.  Frame 12,000 also
selects `$C35726`; the close windows select additional low-address transient
records.  Thus the visible/cull-accepted subset varies, but there is positive
evidence that the close passes retain part of the same static bridge face
family rather than replacing it wholesale.

## Result and limit

This probe finds **no evidence of a simple close-range LOD swap** between the
two close external views and the established Golden Gate checkpoint.  It does
not prove that the game has no distance, clipping, or instance-specific LOD
rule: the 64-frame collections cover only their exact replay windows, and
face count can vary with culling and camera orientation.  A future LOD claim
requires a traced selector whose value changes with a measured scene distance
and changes the selected static bridge family.

Authority:

- `build/run031_golden_gate_dense_keyframes/keyframes.json`;
- `build/run031_frame11850_golden_gate_close_preclip_faces_64f/preclip_faces.json`;
- `build/run031_frame11925_golden_gate_low_altitude_preclip_faces_64f/preclip_faces.json`;
- `build/run031_frame12000_golden_gate_preclip_faces_64f/preclip_faces.json`;
- `build/run031_frame11850_golden_gate_close_line_submissions_64f/line_submissions.json`;
- `build/run031_frame12000_golden_gate_line_submissions_64f/line_submissions.json`.
