# `$C1FEF2`: counted `$34`-byte record-stream skip

Classification: **runtime-backed behavioral stream-cursor helper**. This is
a memory-read-only indirect target of the `$C1F942` record dispatcher. It
does not inspect or modify the skipped record bodies; its observable effects
are register and cursor changes.

The helper takes `n = word[$C458DA] & $000F`, sets `D0=$34`, and advances
`A2` by exactly `n * $34` bytes using `DBF`. The initial branch enters the
`DBF` test before the first `ADDA.W`, so **n=0 skips zero records** and
**n=15 skips fifteen**, not sixteen. On every return it sets `D0=0,Z=1`;
the low word of `D1` ends `$FFFF`, while its upper word is preserved. At
`$C1F944` the dispatcher treats this as a nonnegative zero-status result
and continues its control-stream walk.

The frame-1802 attract trace
`build/attract_focus_1800/trace.jsonl` enters from `$C1F942` with
`A2=$C393DE`. `$C458DA.w & $F` is `$F`, and the helper returns with
`A2=$C396EA`: delta `$30C = 15 * $34`. Its eight distinct instruction
starts also occur in `pcode/raw/attract_1800/`. Byte-exact source is
`source_amiga/observed/adjust_c1fef2_record_pointer.asm`.

This assigns a specific stream-skip operation to one dispatch-table target.
It does not identify what the `$34`-byte records depict or why `$C458DA`
selects the count.
