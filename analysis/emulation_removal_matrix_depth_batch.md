# Matrix-depth C ownership batch

Scope: the playable C2D408 path, with an 800-frame isolated demo01 replay and
the preceding full-suite caller coverage. No full replay was rerun for this
batch, following the user's instruction. Machine-readable evidence and hashes:
`emulation_removal_matrix_depth_batch.json`.

Runner entry -> machine frame -> recomp hook -> `glue_C2D408` ->
`update_record_nonclass_matrix()` -> `adjust_matrix_record_depth()` ->
`settle_record()` / `mark_record_pending()`. Each game call uses ordinary C
arguments/results. The parent's `used_depth` observation records the actual
single C2DD4E call after the game owner returns, without consulting guest PC.

Known original callers are C2D408 -> C2DD4E -> C2DE96/C2DEA2. None of the three
leaf adapters dispatches independently in the preceding five full-suite ON
profiles. Remove their registry/prototype entries and `glue_matrix_depth.c`;
retain domain functions and generated reference implementations. This is scoped
caller ownership, not proof that every original dynamic caller is discovered.

The 800-frame path executes 69 direct depth calls and 56 side calls in 70 parent
invocations. Existing counters and all RGB, selected-index and RAM bytes match
the earlier runner. Parent source shadow: 65 matches, five incomplete, zero
mismatches; sandbox: 70 matches, zero incomplete/mismatches. Both active runners
build with GNU/MSVC; all twelve CTests pass, including profiling invisibility
and actual side/depth call coverage. GNU profiling invisibility also passes.

Current inventory: 603 readable CPU registrations plus 11 direct C entries;
readable reconstruction remains 614. The last full-suite raw minimum remains
**38.4011%**, with **zero observed counter delta** in this bounded probe. Its
combined parity is still failing, so accepted CPU removal remains unavailable.
Memory/chipset/boot **0%**, deletion **0/4**. Access sites remain 8,952 with
**zero** converted to native state. Parent CPU/fixed timing and guest memory
remain. The isolated parent's known frame-584 source timing difference is not
resolved by retiring unused child adapters.
