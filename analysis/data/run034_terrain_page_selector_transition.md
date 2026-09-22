# run034 live terrain-page selector transition

Classification: **ordinary-flight static terrain-page selector evidence**. It
proves live selector/group/row transitions and selected static streams; it is
not a global world-coordinate map or LOD table.

`$C1D3F4` was armed during ordinary replay of run034 and traced only from its
entry to its observed caller return at `$C1D3B8`. The call stack in all three
captures is `$C0F042 → $C1C920 → $C1D3B4 → $C1D3F4`, tying the selector to the
live flight page-update route rather than a synthetic origin mutation.

| Replay hit | live selector index | live row key | static group | group thresholds | selected stream |
| ---: | ---: | ---: | --- | --- | --- |
| 4,252 | 16 | 16 | `$C42608` | 9, 13, 15, 16 | `$C42BD4` |
| 8,040 | 16 | 17 | `$C42608` | 9, 13, 15, 16 | none: row search does not select a stream |
| 9,656 | 15 | 15 | `$C425F4` | 12, 14, 15 | `$C42ADA` |

The first two calls keep group 16 but move the live row key beyond its largest
sampled threshold; the latter call changes both group and row to group 15's
highest sampled threshold. This is direct evidence that the two live values
select bounded static page/template streams. The 8,040 no-stream result is a
row-threshold outcome in this invocation, not a proven map boundary or a
missing terrain region.

No same-item alternate model or face topology is selected by these short
selector calls. The result therefore strengthens the terrain chunk/page model
while adding no positive LOD evidence.

Authorities:

- `build/run034_page_4252_trace/trace.jsonl`;
- `build/run034_page_8040_trace/trace.jsonl`;
- `build/run034_page_9656_trace/trace.jsonl`;
- `static_template_selector_groups_run034_page_{4252,8040,9656}.json`.
