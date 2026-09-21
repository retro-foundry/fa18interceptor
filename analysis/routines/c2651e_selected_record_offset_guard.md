# `$C2651E`: selected-record offset equality guard

Classification: **observed guard/dataflow with an unobserved continuation**.
The exact `$C2651E-$C2652B` prefix is reconstructed in
`source_amiga/observed/guard_selected_record_offset.asm`.

It loads word `$C459B6` into `D0`, compares it with `$C4FDD2`, and on
inequality branches backwards to the adjacent `$C2651C` return stub. The
following entry at `$C2652C` is deliberately outside the slice: no available
packet establishes its continuation behavior.

The guard entry is observed in five independent update-stage exports, through
the call at `$C2627E`. The roles of the two compared words are not assigned;
the names describe only their access pattern and address relationship.
