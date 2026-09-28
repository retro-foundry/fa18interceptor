# `$C093BE-$C095BE`: positive root scene-pose route

Classification: **scenario-backed root initialization contract**.

Authority: the cold-boot root-creation instruction trace at
`build/cold_boot_menu_init_root_creation_instruction_trace/trace.jsonl`.
After `$C0924A` selects source-table entry zero, `$C093B4` loads five words
`(16,16,6,1184,2528)`.  The first word is nonnegative, so `$C093BA` falls
through to `$C093BE`; it does not take the existing `$C09498` negative route.

The bounded positive route:

- publishes the first two words and byte three through its grid-adjustment
  tables into root `+$06`, `+$08`, and `+$0B`;
- scales the table words and the following three-word tail, writes root
  `+$14`, `+$18=$708`, and `+$1C`, and publishes the corresponding three
  global delta longwords;
- derives root `+$0C/+0E` by arithmetic right shift; and
- forms `D4..D7=(0,$08C0,0,$08C0)` from tail word three and calls `$C2D954`.

The trace's concrete root result is `+$14=$10545920`, `+$18=$00000708`,
`+$1C=$10A404F0`, `+$06=$0041`, `+$08=$0042`, `+$0C=$1459`, and
`+$0E=$2404`.  It returns after the existing record-matrix boundary.

`port/scene_positive_pose.{c,h}` ports this exact positive branch with the
original table values and adjustment pairs supplied by its caller.  It does
not seed a capture, name the source tables, or schedule/present a frame.  The
focused contract verifies the recorded arithmetic and the `$C2D954` register
shaped matrix input.
