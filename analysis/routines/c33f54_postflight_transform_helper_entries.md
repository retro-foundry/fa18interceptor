# `$C33F54` postflight transform-helper entries

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

Three entries at `$C33F54`, `$C33F70`, and `$C33F8A` set distinct fixed
register/table configurations, then share a submission tail at `$C33FA2`.
That tail copies `d7` to `d2`, turns `d5` into `a4`, packs `d6` below the
former high word of `d0`, and calls `$C32AA6`.

The three configurations use fixed values `$10/$858/1/2`, `$A/$C33264/2/2`,
and `$1A/$C33258/2/2`, respectively. Their user-visible purpose remains
unproven.
