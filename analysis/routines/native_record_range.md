# Native selected-record range owner

`port/native_record_range.c` implements the complete `$C244E2` owner,
including its shared `$C245AA-$C2467C` classification tail, directly over
the sixteen-record native bank. It replaces the scheduler's root-marker
callback. The actual `$C1D974` child uses the existing portable magnitude
core; its new field-window entry supports signed lookup offsets and explicit
adjacent data owners without changing the bounded word-table API.

The source gate publishes the redraw byte before sound program four. Its
explicit sound child may change the selected record, which is read afterwards.
Counter updates, signed-byte cadence comparisons, selected target identity,
coarse-word sign extension before `SUB.W/SWAP`, signed-overflow branch results,
wrapped long height differences, magnitude publication, mode changes and class
nibbles preserve the original order. Invalid native record offsets and missing
data or sound owners fail with preceding writes retained.

Validation: `python tools/recomp/check_native_record_range.py` passes 8,192
complete original-byte calls at all 137/137 source boundaries, including the
actual magnitude child. All Chip/Slow RAM matches except the CPU ABI save
stack, with typed native aircraft/matrix/position owners checked independently.
There are 3,277 controlled sound calls; their original argument and return PC
are checked, and they change selection in both implementations. This is an
explicit sound contract, not evidence for sample playback. Fixtures cover all
sixteen caller slots, signed coarse coordinates, negative and overflowed height
differences, range/class boundaries, counter rollover and magnitude table
mutation including the clamp. The checkpoint is
`analysis/figures/native_record_range_checkpoint.json`.

MSVC Release game/affected contracts, strict GNU native range contract, five
affected CTests and the 495-file native build guard pass. Native main still
does not invoke the startup graph. Root control/view/pose, lower sound,
scheduling, renderer and input/audio integration remain open.
