# Native record update stage

The real runner reaches C1C63E through original C090B4/C0F016 calls. It now
schedules `advance_record_update_stage` and retains its requests, named child,
child result and C phase. The stage composes the native record loop directly;
origin and rate children still have original runtime boundaries. Source signed
thresholds, coarse keys, origin request gates and publication order are unchanged.
No guessed parent cycle charge was added. Thin entry/stack adapters remain.

Latest connected batch: native C1C63E retains game `RecordUpdateStageFrame`
and calls the C22C80 record loop directly. All 112 update-stage CPU cases are
deleted; the shared C22C80/C1C63E instruction body is now **100% removed**.
This connected chain has removed 892 instruction cases, retaining thin entries,
guest data and other original children. 4,096 original-child core cases match
all state; 4,096 production cases match registers/PC/SR and RAM outside old
CPU stack scratch. Strict all-RAM still fails (case 0 at C7FED1). Both compilers/
runners, twelve CTests and profiling pass. The MSVC/GNU demo matches. Bounded
recordings reach 43/15/75 native stage -> record-loop calls, with zero C22C80
CPU entry dispatch. Previous-build non-fade/index differences start at
381/446/263 (64/1,588/12,536 pixels); final RAM differs. Timing/parity stay open.
No full replay repeated. Bounded raw demo **69.8507%**, delta **-0.0008 pp**;
full raw **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw meter excludes CPU-style adapters; repair that measurement before using it
for further progress estimates. Inventory unchanged. Evidence:
`analysis/emulation_removal_update_stage_batch.json/.md`.

The original-child core test compares every register, PC, full SR and the
entire RAM image for 4,096 fixtures. The separate composed test activates only
C1C63E and traverses the native record-loop/dynamics subtree. It passes 4,096
held-event fixtures with the explicitly weaker RAM scope excluding CPU scratch
C7FD00..C7FEFF. The return word remains compared. The strict failed result is
retained in `build/update-stage/live-strict.log`; the successful scopes are in
`core.log` and `live-game-ram.log`. Events and IRQ timing remain separate gates.

The demo RGB frames match the prior batch, but 64 selected-index pixels differ
at frame 381. The comparator still rejects that difference under the existing
policy. Carrier/crash RGB/index differences and all final RAM differences also
remain open; Copper fade is excluded. Evidence is in
`build/update-stage/recording-results.json` and the captures beside it.

The profiling CTest now asserts both direct parent edges and zero C22C80/C25B66
CPU-entry dispatch, in addition to its existing output-invariance and child
checks. No acceptance assertion was relaxed.
