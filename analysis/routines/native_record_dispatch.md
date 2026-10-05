# Native record dispatch

`port/native_record_dispatch.c/.h` implements complete `$C23A7E`, the
`$C23F4A` unclassified path, `$C243F2` tracking and `$C24458` control-target
selection. It replaces the final outer callback in the native record scheduler.
`FA18NativeControlRecordOps` is removed. The sealed original instructions are
authoritative; `game/main_loop_flight_controls.c` corroborates the behavior.

Dispatch runs the actual native `$C24568` view-range classifier, `$C23CA6`
record-view owner and `$C1BEE8` context publication. `$C1B7A6` sets detail four
and invokes actual queue publication. The classifier shares the existing
selected-range tail and `$C1D974` magnitude primitive. Its optional axis output
retains DIVU's remainder high word and subsequent word operations. Existing
scalar APIs continue to use the same body without requesting that output.

Class gates, lifecycle changes, heading limits, motion damping, shared sequence
updates and sticky target flags retain source widths and write order. Selected
control choices are signed bytes after decrement, scaled as a wrapped word.
Their field window must expose original data and actual adjacent live owners;
some choices alias record fields. Both coarse words are read before either is
published. Later point reads see preceding stores through the same owners.
Only native references inside the supplied sixteen-record bank resolve; missing
owners or unsupported references fail with preceding writes retained.

Completion and the source pose decision remain separate. View work returns the
numeric axis and companion-record identity. Heading and linked-selection paths
can change the companion; the scheduler preserves that returned identity for
subsequent groups. Projection, zone-table and linked-record paths also retain
their actual returned axis. Normalization and fault contracts now expose their
axis output instead of assuming it is preserved.

`python tools/recomp/check_native_record_dispatch.py` compares eleven complete
entries, including shared view/link and publication entries, in 90,112 calls.
All 1,035 reachable instruction boundaries are covered. Each call compares all
Chip/Slow RAM except the CPU ABI stack `$C7FD00..$C7FF00`, the carried axis,
returned companion identity and applicable decisions/events. Typed record
owners are compared independently against original output after the RAM check.
The mode-zero branch is pruned only after checking the sealed instructions that
clear and publish zero; no queue or child can change it before its comparison.
Actual view, magnitude, detail, zoom, redraw and queue bodies run throughout.
Normalization `$C2574A` and fault `$C06C02` are the only lower contracts.

The composed scheduler contract exercises actual expiry and pose suppression,
source redraw, previous-work retention and invalid shared bindings. The existing
view and selected-range proofs are rerun for the shared changes. Strict GNU
compilation, affected MSVC Release builds/CTests and the unchanged 511-file
native guard pass. Exact counts and source hashes are recorded in
`analysis/figures/native_record_dispatch_checkpoint.json`.

Full startup bindings, original control/view assets, remaining lower owners and
native main integration are still required. The playable ROM-free reference
runner continues to use CPU/chipset state; this batch does not complete the
emulation-free game.
