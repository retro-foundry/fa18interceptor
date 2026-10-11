# `$C09498`: negative scene-table pose route

Classification: **scenario-backed direct pose contract**.

In run075 `$C0924A` reads Hunk-67 table-A entry three, whose first word is
`$800E`. The negative branch masks it to record index 14 and tests bit 6 of
that selected record. If clear, it clears `$C45848` and retries the enclosing
table selection. If set, the trace selects descriptor `$C39210`, masks its
`+$02` longword `$80000070` to `$70`, writes root `+$10=$77` and
`+$18=$7708`, carries the selected record's `$66/$68/$6A` triple, transforms
the source vector `(11,0,$68)` through `$C091E0`, and commits the resulting
root pose fields before calling `$C2D954`.

`port/scene_negative_pose.c` ports `$C09498-$C095BE` with typed selected
record and descriptor inputs. It preserves the negative-table guard, bit-6
retry result, descriptor sign guard, direct root field arithmetic, transform,
screen-field packing, and matrix-update handoff. The selected record/descriptor
producers and `$C2E47A/$C2E514` matrix owners remain caller boundaries.
