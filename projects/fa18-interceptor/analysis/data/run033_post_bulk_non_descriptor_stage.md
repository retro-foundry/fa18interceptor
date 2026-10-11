# Run033 post-bulk non-descriptor stage result

Classification: **scenario-backed negative route evidence**. This is a
no-input continuation from the final state of the bounded run033 placement
bulk-refresh trace; it does not establish a complete builder-to-record join.

Authority:

- `build/run033_placement_bulk_404_trace/trace_final_state.bin`, the final
  state after its three-frame `$C1DC1C-$C1E0B0` bulk-refresh interval.
- `build/run033_post_bulk_to_c1cc70_trace/`, which restores that state and
  reaches `$C1CC70` on its eleventh ordinary chipset frame.

At the selected `$C1CC70` iteration, `A1=$C22334` and the instruction stores
that descriptor field to `$C45A36`. The placement loop then calls
`A2=$C096CA`, rather than `$C1EE14`. The target takes its comparison branch,
sets `D0=0`, and returns to `$C1CC88` after ten traced instructions.

This proves that a natural call following the observed placement-cache refresh
need not enter the descriptor/control renderer stage. Therefore the priority
builder-to-renderer capture must filter for an actual `$C1EE14` target (or its
documented `$C1ED3C/$C1ED4C` entry route), while retaining the record-building
stores in the same uninterrupted scenario. It neither rejects a later
builder-to-`$C1EE14` chain nor assigns a semantic identity to `$C096CA`.
