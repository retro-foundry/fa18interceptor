# `$C34BF6`: traced C34C selection descriptor

Classification: **runtime-proven selector input for the five-face `$C34C` layer at the frame-12,000 Golden Gate checkpoint**.

The bounded replay trace enters `$C1F942` with `A5=$C34A9A`,
`A1=$C34ACE`, and `A2=$C34BF6`.  The dispatcher selects `$C201A6` and
preserves that context.  At `$C20260`, the handler reads the leading word at
`$C34BF6` (`$0A00`), compares it with runtime state at `$C4619C`, and, on the
observed path, advances `A4` from `$C46184` to `$C46228`.

It then reads the selector payload at `$C34BF8`: count `$0005`, an associated
word `$0001`, followed by five offsets beginning `$0006, $000C, $00C6,
$0078, $00C0`.  `$C2035A` uses those offsets with `A4=$C46228` and submits
the corresponding `$C34C06`, `$C34C18`, `$C34C2A`, `$C34C38`, and `$C34C48`
records through `$C203C4 -> $C2469E`.

This proves that the C34C layer is dispatched in the same `$C34A9A` object
context, rather than being an unrelated road or carrier segment.  It also
supplies an actual conditional-selection mechanism for this frame.  The trace
does **not** yet establish the meaning of `$0A00`, the units of the compared
runtime value, or every alternative descriptor path; calling it a distance
threshold or an LOD rule would therefore exceed the evidence.

Authority: `build/run031_frame12000_c1f942_to_c34c_selector_trace_long/trace.jsonl`,
instructions 15882--16135.  The trace begins from the recorded checkpoint and
is stepped without later input delivery after its breakpoint.
