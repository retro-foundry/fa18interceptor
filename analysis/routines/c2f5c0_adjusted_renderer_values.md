# `$C2F5C0` adjusted renderer wrapper

Classification: **static-only dataflow**. No available P-code export enters
this `$C2F5C0-$C2F5F3` span.

`source_amiga/observed/submit_adjusted_renderer_values.asm` is byte-exact for
the complete 52-byte wrapper. It adds `$C45988` to `D0`, rejects a negative
result or one at least `$140` through `$C2F622`, then adds `$C458D8` to `D1`.
It selects the `$C456B6` pointer block and the primary renderer tables
`$C2F766/$C2F786`, saves the adjusted pair, calls shared body `$C2F688`, and
restores the pair before returning.

This establishes only the wrapper's arithmetic, range gate, and call/dataflow;
the coordinate and buffer meanings remain unassigned without runtime evidence.
