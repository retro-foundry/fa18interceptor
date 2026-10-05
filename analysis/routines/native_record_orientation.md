# Complete native record orientation

`native_record_orientation.c` implements complete `$C2D954` and `$C2D94E`
against the actual shared scene records. It publishes the original three
angles, the forward matrix at the former `+$80` fields and the inverse
matrix at the former `+$92` fields. The forward matrix is now a named owner
in the bank; the inverse remains the actual geometry object consumed by
native command transformations. Byte views read these owners live, and
their old packed positions hold no duplicate values. Record tails are preserved.

The source saves `D1/D4-D6/A1` and restores `D1/D5-D7/A1`. Thus the inverse
uses zero or `$7080 - original angle` for the same three original angles.
Incoming `D7` is irrelevant. Placement's `$C2D954` preserves the secondary
flags; `$C2D94E` adds the original bit-2 clear first. The older detached
`record_matrix_update` packet now derives these inputs correctly, while
retaining its documented `$C2D94E` flag-clear entry.

Both native and older packet callers share the rotation and attitude math
implementations. The original inverse composer subtracts the intermediate
product from the cosine product at `[0][0]`, and subtracts the intermediate
product from the sine product at `[2][1]`. The former native implementation
reversed both. The correction also truncates an intermediate to a signed
word before the second multiplication, wraps long accumulation explicitly,
and handles arithmetic shift of `INT32_MIN` without signed overflow. The
previous hand-written contract expectations mirrored those errors; they
have been corrected against the original instructions.

`FA18FlightTrigData` binds a live original asset and its quarter-table offset.
`fa18_load_native_trig_data` binds Hunk 63 directly. Signed displacements
outside this asset require bounded adjacent field owners, using the existing
`PortFieldByte` mechanism. The lookup preserves doubled-word wrapping,
signed quadrant tests and word negation. It neither clamps angles nor
fabricates sine/cosine data. Missing owners return failure and retain earlier
angle or matrix stores. The legacy quarter-table API now rejects unavailable
reads instead of indexing outside its supplied buffer.

Run `python tools/recomp/check_native_record_orientation.py`. At 4,096 cases
per record entry, 8,192 complete original calls match every game RAM byte
except the source CPU save stack `$C7FD00..$C7FF00`. All 292 source boundaries
are visited with no child contracts. The original `$C2E47A`, `$C2E514`,
`$C2E5F6`, and `$C2E6DA` children execute fully. Independent named matrix
owners are also checked for all sixteen records.

The standalone lookup compares all 65,536 input word angles against the
actual source with arbitrary table/adjacent words. Record fixtures include
original data, arbitrary signed words, `$8000` products, high-register and
incoming-D7 variation, and all zero-angle branches. All 1,802 original
quarter-table bytes match the ADF's Hunk 63, and a subsequent asset mutation
reaches the bound native lookup. Captured RAM and Musashi appear only in
the validation runner, never in these native owners.

Strict GNU contracts and symbol checks, the native MSVC build, 83 focused
scene/flight/matrix/terrain CTests and the unchanged 480-file native guard
pass. The build also restores the existing scene-entry test's missing
viewport-transition link dependency. Shared-record regression retains
24,576 calls/98 boundaries; bootstrap retains 16,384 calls/291 boundaries,
with the same three pending child contracts.

This completes a required placement dependency. Full `$C09266` placement,
`$C1C63E` record update, `$C1C860` context refresh, original data production,
and native main-loop composition remain open. The ROM-free runner requires
no Kickstart image but still uses the reference machine and CPU state;
this batch does not claim a complete playable game without emulation.
