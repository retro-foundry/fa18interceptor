# `$C0924A`: nested root-setup helpers

Classification: **static direct helper contracts with a scenario-backed
caller**.

The first part of `$C0924A` calls `$C09620` and then `$C095C0` after its own
prefix stores. `$C09620` calls `$C0840E`; neither `$C0840E`, `$C09620`, nor
`$C095C0` has another unresolved callee in the covered ranges.

`$C0840E` sets a direct display pointer/value group and clears three adjacent
164-byte root-workspace blocks. `$C09620` clears root bit `+$21`, writes its
root control fields, invokes `$C0840E`, then initializes its direct latches
and limit values. `$C095C0` clears direct root transient fields, clears two
longwords and two words outside the root record, writes `$FFFF` to the final
word, and preserves only bit 15 clearing of root word `+0`.

`port/scene_root_setup.c` ports the ordered `$C09620` then `$C095C0` pair as
`FA18SceneRootSetupState`, retaining structural offset-based field names.
`scene_root_setup_contract_test` checks the direct values, masked root word,
conditional `$C45798` rewrite, and all three cleared workspace blocks.

The remainder of `$C0924A` selects table data, computes and commits a root
placement, invokes `$C091E0` and `$C2D954`, and is still separate. `$C091E0`
is now the standalone native `scene_vector_transform` arithmetic primitive;
the direct `$C2D954` matrix-update publication/order is now native
`record_matrix_update`; the caller's table selection and the two matrix
computation owners remain open.
The run060 writer evidence for that remaining placement portion is documented
in `analysis/data/run060_root_pose_initialization.md`.

The native Hunk-67 adapter now provides the caller's original `$C42A02`
eight-word records; run075 takes entry three, whose first word is `$800E`.
Its guarded negative pose route is now native `scene_negative_pose`; selected
record/descriptor producers and the positive-table route remain open.
