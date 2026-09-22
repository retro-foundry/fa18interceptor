# Static template-selector groups

Classification: **scenario-backed static selector dataflow**.  This records which `$C42390` table entries selected template streams in one bounded replay; it is not a decoded world grid, an LOD table, or a terrain mesh.

Authority: `build/run034_page_9656_trace/trace.jsonl` and its slow-RAM snapshot.  At `$C1D400` the helper doubles `D0`; `$C1D402` adds the signed word at `$C42390+D0*2` to that table base; `$C1D406` tests the resulting static group record.  On the observed accepted path, `$C1D426` passes a bit gate, `$C1D43A` resolves a stream pointer, and `$C1D442` begins the byte stream consumed by the workspace copier.  Between those steps, `$C1D4E4-$C1D50C` binary-searches the group's static sorted word list using live `D1`, returning the pointer-table index used at `$C1D43A`. For accepted records, the first word is an even byte offset equal to twice the threshold count; it is followed by that many threshold words and then a same-sized long-pointer table. The JSON inventory preserves this exact decoded layout.

The trace executes **1** group selections; **1** reach a template stream before returning.  Entries without a stream either take a gate/exit path in this trace or lack an observed stream before the next helper entry.

| Frame | workspace band | selector index | call-time row key | table word | static group record | row thresholds / live key / selected index | first stream byte |
| ---: | --- | ---: | --- | --- | --- | --- | --- |
|  | $C48390 | `$000F` | $000F | $C423AE | $C425F4 | 3 / $000F / 2 | $C42ADA |

The static group records and their selected streams are a stronger upstream boundary than the mutable workspace: the same path later reaches the static-to-workspace copy contract.  The selector index is not yet tied to a player world coordinate, distance, course cell, or visual LOD state, so none of those meanings are assigned here.  The two-stage static selector plus live search key resembles a two-axis lookup structure, but resemblance is not sufficient to call it a world-map grid.

See [the workspace-template inventory](workspace_template_copies.md) for the copied records and their later placement outputs.
