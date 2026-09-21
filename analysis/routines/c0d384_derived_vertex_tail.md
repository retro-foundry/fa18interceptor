# `$C0D384-$C0D521`: renderer-workspace derived-vertex tail

Meaning level: **behavioral**, frame-1966 trace-backed.

The routine processes a workspace selected in `A3`. In the frame-1966 trace it first has `A3=$C46228`, the transformed input block consumed by the `$C34C` faces. It writes the previously unresolved slots 22 onward from offsets inside that same workspace; it is not a direct read of `$C351E2-$C35234`.

The first tail store is `$C0D396`:

```asm
movem.w d3-d5,$84(a3)
```

With `A3=$C46228`, that is `$C462AC`, slot 22. The following stores fill derived triples at offsets `$8A`, `$90`, `$96`, `$9C`, `$A8`, `$AE`, `$B4`, `$C0`, `$CC`, `$D8`, and `$DE`; the recorded routine then repeats with `A3=$C48390` for a separate renderer workspace.

The values are calculated from earlier workspace offsets using word arithmetic, additions/subtractions, and shifts. The trace begins with the tail still zero and ends with the `$C462AC-$C46301` records populated. The direct trace is `build/run031_frame1965_c0d396_tail_derivation_trace/trace.jsonl`; it is bounded by the caller's `$FC0D14` breakpoint and returns to `$C0D380`.

Consequently, the coordinate resemblance between the `$C351E2` static continuation and this runtime tail cannot be promoted to a static vertex-transform claim. It may encode a related template, but it is not the observed producer path.
