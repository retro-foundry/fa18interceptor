# `$C316C0` postflight renderer submission tail

Classification: **static-only dataflow**. This is the final submission tail of
the untraced postflight loop.

`source_amiga/observed/submit_postflight_renderer_record.asm` reproduces
`$C316C0-$C31721` (98 bytes). It chooses one of two table limits, appends a
coordinate pair, selects the shared or adjacent renderer based on `d7` bit 0,
restores preserved registers, updates `d3` bit 5, and loops to `$C31410`.

The record/table role remains unproven; all labels are dataflow-level.
