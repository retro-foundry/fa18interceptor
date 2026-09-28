# `$C2FEDE-$C2FF45`: selected-table display stage

Authority: `build/attract_cockpit_c2fede_trace/`, a no-future-input attract
cockpit trace. A breakpoint at `$C2FEDE` hits on frame 4 and the packet returns
to `$C0D742` after 2,439 instructions. The imported raw P-code records 591
observed starts and 3,957 operations.

The routine saves `$C456E2`, replaces it with the longword at `12(A2)`, and
calls `$C0D752`. On the observed zero-status path it calls the already bounded
`$C301F6` submission routine. If `$C4589B` is nonzero, it additionally calls
`$C30466` with `D0=8`, `D3=0`, and `D4=0`. It restores `$C456E2` before the
next branch in every path.

When `$C45785` is clear and `$C0D74A` returns zero, the routine copies one word
and five longwords from `$C4B390` to `$C4B432`; otherwise it clears the first
destination word. The observed trace takes the copy path. The meaning and
ownership of both record areas remain unassigned.

The raw P-code authority is `pcode/raw/attract_cockpit_c2fede/`. The static
alternate branch is retained byte-exactly in
`source_amiga/observed/run_selected_table_display_stage.asm`.

The run075 return-bounded trace at `build/run075_prepared_c2fede/` confirms
that the live selected four-point list enters `$C301F6`, takes the far
`$C302DE` continuation, and completes the `$C30306` range/finalization path.
`port/selected_display_submission.{c,h}` now bridges that direct `$C301F6`
entry to the existing bounds/far-list implementation without incorrectly
introducing the separate `$C2FF48` DMA wrapper.
`fa18_flight_renderer_page_bind_projection_page_blitter` can now attach those
callbacks to a native page only after its caller supplies initialized inherited
state whose page identity matches the renderer's four lower planes. The live
`$C2FEDE` parent, its inherited blitter inputs, and normal replay scheduling
remain caller-owned and uncomposed.

`port/selected_table_display_stage.{c,h}` now ports the complete local
orchestration: saved-table substitution/restoration, prepared-list result
branch, direct submission, optional selector lane call, mode clear, and the
24-byte output copy. Its optional selector lane and required `$C0D74A`
post-pass are callbacks because their source-owned state has not been bounded.
In particular, a nonzero prepared-list result bypasses `$C301F6` but still
falls through to the restore/post-pass sequence, as in the source.
