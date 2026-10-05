# Native control-record scheduler

`port/native_control_record_update.c/.h` implements complete
`$C22C80-$C230AE` against `FA18NativeSceneRecords`. Its behavioral authority is
the readable source in `port/game/record_update_stage.c` and the sealed
original-instruction proof documented in `native_c_record_update_stage.md`.

The owner decrements the word at `+$04` in all sixteen 32-byte work records,
runs the periodic child, clears bit zero in record secondary flags for slots
0 through 14, updates the two signed-positive byte counters, and releases a
lost selection. It then executes the exact root, primary group, secondary
group, standalone and paired slot schedule. Slot 7 is prepared and tested but
never dispatched. Active slots 14 and 15 receive forced bit two; inactive
records do not.

Child completion and source decisions are separate outputs. Ready children can
decline a record without failing the parent, and dispatch can decline the pose
child. The scheduler tracks the semantic companion-record slot that source A2
retains across standalone records: root uses slot 4; primary groups use root;
the secondary and paired groups advance through 4, 6, 8, 10 and 12. No CPU
register or guest pointer is exposed.

The focused contract uses a mixed active/inactive bank to cover periodic work,
both ready decisions, both placement groups, dispatch/pose routing, slot 7,
forced flags, workspace wrapping, root countdown and a failed child after
preceding stores. The parent is now a direct child of
`native_record_update_stage`. Its individual control/view/marker/pose,
ready/place, dispatch and finish children remain explicit boundaries; this
contract does not establish them.
