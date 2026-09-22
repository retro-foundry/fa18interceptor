# `$C36214` map-mode control-to-geometry trace

Classification: **scenario-backed static control stream to mutable geometry and projected-segment path**.

In the no-input post-`M` collection, `$C1F6F8` enters with `$C45A36=$C36214`, an immutable segment-43 control stream. Its first control word is `$A020`. The walker recognizes bit `$2000`, masks the low selector `$0020`, dispatches through the static `$C1FCE8` table, and reaches the handler at `$C21310`.

That handler reads the stream's list/count fields, then uses its selected offsets (`0` and `6` in the observed first item) against the transformed triple workspace `$C48390`. It copies the selected triples into temporary projection workspace `$C4C592`, then calls `$C2EE4A`, the projected-segment path leading to blitter line setup.

Proven bounded contract:

```text
$C36214 static control word/list
  -> $C1F6F8 class dispatch
  -> $C21310 selected offsets
  -> $C48390 transformed triples
  -> $C4C592 projection scratch
  -> $C2EE4A projected-segment renderer
```

The triples remain mutable transformed workspace values, not immutable source vertices. The trace does not match this segment to a coastline pixel, establish that `$C36214` represents a named landmark, or extract the complete segment-43 model.

The two traced `$C2FA7E` calls have endpoints `(118,66)->(125,60)` and
`(125,60)->(132,59)`. Both endpoints hold final map-display bitplane index 6
in the independently video-hash-matched frame-25 bitmap; see the
[endpoint bitplane check](run003_m_map_c36214_line_endpoints.md). This is
display-space overlap, not proof that either specific line invocation wrote
those pixels.

Authority: sealed `captures/run003`; `build/run003_m_visual_5/state.bin`; and ignored `build/run003_m_map_c36214_stream_trace/selected_stream_trace.jsonl` (800 instructions from the exact `$C36214` entry).
