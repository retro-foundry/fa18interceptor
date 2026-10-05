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

`C2574A` normalization and `C06C02` fault handling remain explicit lower
boundaries. Missing data/owners or a non-completing exhausted zone list fail
explicitly; no child substitute is installed. The caller's viewer work retains
a native record pointer for the record-consuming routes; source data-list
cursors are not represented as fabricated aircraft pointers or as a guest bus.

Validation: `python tools/recomp/check_native_record_view.py` passes 8,192
complete original-byte calls at all 383/383 graph boundaries, including actual
placement and in-sight instruction children. Every Chip/Slow RAM byte matches
except CPU ABI stack `$C7FD00..$C7FF00`; typed aircraft/matrix/position owners are
checked independently. There are 912 controlled normalization calls and 759
controlled fault calls. Normalization contracts check the scale, full component
inputs and true return PC, then vary returned words. Fault contracts preserve
source errors and vary pending state. They do not prove the lower children.
Fixtures cover all sixteen callers/viewers, shared caller/viewer identity,
all parent routes, table advance/end/error, both zone lists, linked selection,
local-point modes, signed/overflowed coordinates, arbitrary matrices, in-sight
accept/reject and status rows. See
`analysis/figures/native_record_view_checkpoint.json`.

MSVC Release native game and affected contracts, strict GNU view/scheduler
contracts, eleven affected CTests and the 497-file native build guard pass.
The subsequent `native_record_control` batch supplies the carried axis directly
and removes root/secondary control callbacks. Six scheduler children remain:
periodic, pose, primary/secondary placement, dispatch and finish. Native
main still does not invoke this startup graph; full native runtime integration
and the actual normalization/fault owners remain open.
