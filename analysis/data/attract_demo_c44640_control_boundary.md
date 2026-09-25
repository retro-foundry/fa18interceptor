# Demonstration-flight `$C44640` control boundary

Classification: **event-faithful bounded negative ownership evidence**.

The frame-3,600 no-input demonstration refresh reaches `$C1CC70` with
descriptor `$C22834` field `$C44640`.  In the selected 12,000-instruction
continuation, `$C1CC70` next reads `$C3B5A6` from descriptor `$C22870` before
any `$C1EF10` cursor publication, `$C1F6F8` walker, `$C1F4B0` immutable-source
read, or primitive submission attributable to `$C44640`.

The subsequent observed `$C3B5A6 -> $C3B5B4 -> $C3B628-$C3B645 -> $C3B5B6`
chain therefore belongs to the later selected store, not to `$C44640`, in this
bounded execution.  `$C44640` may reach a renderer path elsewhere; this trace
does not establish that and assigns no feature identity.

Authority: `build/attract_demo_c44640_selected_trace/trace.jsonl` and
`trace_summary.json`.
