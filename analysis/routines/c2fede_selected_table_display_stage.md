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
