# Matrix output and following-record sight reference - 2026-10-08

The mode-four missile pursuit diagnostic exposed a missing matrix output in
the playable record loop. Passing that output to the following record fixes
the mismatch: all 26 input/stage intervals and 59 sampled bodies match original
compared RAM/drawing, including the crash continuation and natural reset.
The flight still crashes at tick 22,148, with no weapon hit, objective or result
save and completion count unchanged at 3. Mode-four success remains open.

## Original contract and connected caller

The active path is `native_frontend_tick` -> `native_flight_tick` ->
`native_records_update` -> `update_control_records` -> `record_child` ->
`dynamics`. `DY_RECORD_MATRIX` invokes `update_dynamics_record_matrix` and
C2DEE0's matrix transform through the existing record-matrix owner. C2DFF6
leaves its final product cursor at `MATRIX_TRANSFORM_PRODUCT + 32` (C45BC2).
The source carries that reference into the next record's C2436A sight update.

Native matrix extraction already produced the correct angles and divisor,
but the native record loop discarded this reference output. The isolated
CPU adapter knew the cursor; that alone did not integrate it in the playable
runner. `MatrixTransformAngleState.next_product` now exposes the output from
the extraction owner. The non-class-30 dynamics caller carries it through
`DynamicsState.scene` into `RecordLoop.viewer`. The class-30 tracking branch
retains its incoming reference. The comparison adapter uses the same output.

The original failing body is capture 57, body 8,773, iteration 13,232, stage
C11788, host ticks 21,983 -> 21,986 and saved tick 3,536. The player is already
in crash phase 4. The original C2436A call for record C46984 uses viewer C45BC2;
C257C2 then writes normalized vector [-46, -160, 97] at C45A4C. The old native
result was [22, -1, -191]. Five compared RAM bytes differed, with no drawing
difference. The same ordinary input now passes that body and C11830's reset.
No matrix arithmetic, motion, AI, damage, outcome rule or comparison mask changes.

## Repeatable evidence

`fa18_native_mission_four_combat_reset` is a serial CTest using the existing
mission fixture's `4-mission` argument. The validation pilot reads state and
sends ordinary keys for takeoff, target selection, pursuit and radar fire.
There are 42 flight fire presses, including retries; the first three are at
ticks 12,617, 13,123 and 13,629. Fire presses do not imply successful launches
or hits. The checker requires the incomplete flight, unchanged completions,
no objective/hit/save, the reached continuation and original boundary parity.
The retained consumed input is `tools/native/fixtures/mission-four-combat-reset.e9k`.

The records component oracle checks 576 complete C2DEE0 cases against original
angles, divisor, returned cursor and non-stack RAM. Its existing 256 record
publication, 128 signed-angle matrix and 256 settling cases also pass, along
with the captured record/view composition. These component checks establish
the producer contract; the actual flight comparisons establish its integration.

```powershell
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mission_four_combat_reset$' --output-on-failure
```

Passing RAM remains temporary and each passing pair is removed immediately.
The resolved failing before/after/source case is retained locally as three
compressed `.dat.gz` files with hashes and timing in
`build/native-flight/mission-four-climb-probe/failure/resolved.json`.
The existing 4 GiB build-cache pruning and capture limits remain enabled.

## Validation and remaining region coverage

Debug and Release playable, mission, qualification-sequence and mode-two
fixture builds pass. Six selected Debug checks pass across the initial run
and the isolated mode-five sequence retry. The latter exceeded its former
60-second native capture deadline twice with all complete input rows matching
the retained sequence. Debug sequence CTests now allow 90 seconds through an
explicit checker option; Release and standalone defaults retain 60 seconds.
The completed Debug sequence matches all original boundaries and saved/input
hashes. This deadline is separate from gameplay performance acceptance.

The new mode-four regression passes in Release. Nine of ten additional Release
checks pass: artifact policy/cleanup, mode-five combat/reset, both successful
result sequences, carrier restart, ordinary mode four, frontend/link omission
and independent-camera depth. Both successful sequences retain their accepted
input/save hashes and result transitions. Logs are
`build/native-flight/matrix-cursor-debug-ctest.log`,
`matrix-cursor-debug-sequence-ctest.log`,
`matrix-cursor-mode-four-release-ctest.log` and `matrix-cursor-release-ctest.log`.
Hashes and individual statuses are in `figures/native_matrix_sight_reference_checkpoint.json`.

The existing `fa18_native_region_flight` CTest still fails its required region
coverage guards. Its original key sequence now crashes with no observed spawn
or zone exit and only NPC missile slots nine/eleven, rather than slot thirteen.
All 58 input/stage intervals and 47 sampled bodies nevertheless match original
compared RAM/drawing, with no config write. A validation-only shorter pull-up
also misses these events and passes its same-size source comparison; the
original test inputs are restored. Its strict coverage guards remain intact.
Restore region/zone/NPC-missile coverage with ordinary input before claiming
that regression accepted. These sampled before-state comparisons do not
establish independent complete original-flight behavior.

These comparisons execute original instructions from native before-states.
Independent complete original-flight parity remains open. Successful modes
four/six/seven/eight, further play after the accepted result menu, remaining
caller contracts, typed state, audio fidelity and wider performance remain
open. The complete-port goal stays active.
