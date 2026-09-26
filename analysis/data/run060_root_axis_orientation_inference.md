# run060 root axis and pitch inference

Classification: **scenario-backed coordinate-convention inference**. The
field roles below are stronger than raw offsets but remain subject to the
program's transform sign convention.

Three facts align in the initial qualification-flight segment:

1. Root `+$18` is the proven feet-valued cockpit altitude, so it is the
   program's vertical coordinate.
2. With the root orientation matrix equal to fixed-point identity, frames
   701--901 advance `+$1C` while `+$14` and `+$18` stay fixed. Thus `+$1C`
   is the initial straight-flight/forward horizontal coordinate and `+$14`
   is the orthogonal horizontal coordinate.
3. The first live root matrix change preserves the first row/column axis and
   changes only the `+$18/+1C` plane:

```text
identity:       4000 0000 0000
                0000 4000 0000
                0000 0000 4000

first turn:     4000 0000 0000
                0000 3FFF 0039
                0000 FFC7 4000
```

This is the fixed-point small-angle form of a rotation in the vertical/forward
plane, leaving the first horizontal axis unchanged. On conventional aircraft
axes that is a **pitch-like** rotation. The sole changing root angle word in
this window is `+$66`, so `+$66` is a pitch-like angle state for this scenario.

Accordingly, the most useful program-coordinate names are:

```text
root +$14  horizontal X / lateral candidate
root +$18  vertical Y / cockpit altitude
root +$1C  horizontal Z / initial forward candidate
root +$66  pitch-like angle candidate
```

This does not yet prove the real-world sign (nose-up vs. nose-down), physical
units, or the identity of the second and third attitude controls. The
conclusion is about the observed transform basis and flight trajectory, not a
claim that the binary uses a documented aerospace coordinate standard.

Authority: `data/run060_root_pose_motion_timeline.md`,
`data/run060_root_attitude_matrix.md`, and the byte-exact
`compose_alternate_three_angle_matrix.asm` formula.
