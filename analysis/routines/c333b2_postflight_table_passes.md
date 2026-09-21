# `$C333B2` postflight table-pass configuration

Classification: **static-only dataflow**. This is an untraced continuation of
the postflight scaled-value stage.

`source_amiga/observed/configure_postflight_table_passes.asm` reproduces
`$C333B2-$C33417` (102 bytes). It routes byte `$62(a0) == $10` to `$C3341A`;
otherwise it initializes two work configurations and invokes `$C25A08`,
`$C32AA4`, and `$C32AB4` before transferring to `$C335B6`.

The work-table and helper meanings remain unproven.
