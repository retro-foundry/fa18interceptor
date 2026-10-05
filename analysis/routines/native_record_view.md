# Native record-view owner

`port/native_record_view.c/.h` implements complete `$C23CA6` and its internal
shared tails, with direct `$C091E0` local-point placement and `$C2436A` in-sight
behavior. It replaces the scheduler's root-view callback. The authority is
the sealed original instruction graph, corroborated by
`port/game/control_records.c`; differences between the older memory-side
composition and the instructions are resolved in favour of the instructions.

The owner preserves the event gate, mode-eight table scan/advance/error exits,
linked view and angle easing, signed selector and zone lookup, local-point
selection, fixed-point placement, in-sight products and status-table output.
It shares the existing aircraft, position and inverse-matrix owners. Parameter,
status, primary-list and zone data are caller-bound field windows. The carried
axis word is an explicit input used by the source's signed byte/word gate;
the upstream root-control owner must supply it. It is not a CPU register file.

The `$FF` viewer selector skips the indexed viewer's mode store, active test
and flag publication. The indexed root selector takes those operations before
placement. An inactive viewer without the zone flag returns through `$C242DA`
without clearing pending; other status exits clear it. Signed-overflow branches
use the mathematical sign of the last word/long operation before subsequent
wrapped comparisons. Products wrap in source order. Negative status rows read
explicit preceding field owners. Fault children retain the published error word
and may change live pending state before the parent resumes.

`C2574A` normalization now invokes the actual native vector-math owner and
`C1D974` magnitude body. Only `C06C02` fault handling remains an explicit lower
boundary. Missing data/owners or a non-completing exhausted zone list fail
explicitly; no child substitute is installed. The caller's viewer work retains
a native record pointer for the record-consuming routes; source data-list
cursors are not represented as fabricated aircraft pointers or as a guest bus.

Validation: `python tools/recomp/check_native_record_view.py` passes 24,576
original-byte calls across the view and both normalization entries at all
481/481 graph boundaries, including actual placement, in-sight, normalization
and magnitude instruction children. Every Chip/Slow RAM byte matches
except CPU ABI stack `$C7FD00..$C7FF00`; typed aircraft/matrix/position owners are
checked independently. There are 9,104 actual normalization calls and 759
controlled fault calls. Fault contracts preserve source errors and vary pending
state and carried axis. They do not prove fault handling. Core normalization
entries also prove non-completing source factor loops; see `native_vector_math.md`.
Fixtures cover all sixteen callers/viewers, shared caller/viewer identity,
all parent routes, table advance/end/error, both zone lists, linked selection,
local-point modes, signed/overflowed coordinates, arbitrary matrices, in-sight
accept/reject and status rows. See
`analysis/figures/native_record_view_checkpoint.json`.

MSVC Release native game and affected contracts, strict GNU compilation,
ten affected CTests and the unchanged 513-file native build guard pass.
The subsequent `native_record_control` batch supplies the carried axis directly
and removes root/secondary control callbacks. The subsequent `native_record_pose`
batch makes pose and motion history direct. The subsequent placement/finish/regions/dispatch batches remove the outer
scheduler callbacks. View work now returns the actual companion identity and
full carried axis; actual normalization and the fault contract expose their returned axis.
The updated proof checks these outputs and independently compares typed
records against original output. Native
main still does not invoke this startup graph; full native runtime integration
and the actual fault owner remain open.
