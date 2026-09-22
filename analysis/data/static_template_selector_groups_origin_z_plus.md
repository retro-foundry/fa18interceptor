# Static template-selector groups

Classification: **scenario-backed static selector dataflow**.  This records which `$C42390` table entries selected template streams in one bounded replay; it is not a decoded world grid, an LOD table, or a terrain mesh.

Authority: `build/run033_origin_z_plus_trace/trace.jsonl` and its slow-RAM snapshot.  At `$C1D400` the helper doubles `D0`; `$C1D402` adds the signed word at `$C42390+D0*2` to that table base; `$C1D406` tests the resulting static group record.  On the observed accepted path, `$C1D426` passes a bit gate, `$C1D43A` resolves a stream pointer, and `$C1D442` begins the byte stream consumed by the workspace copier.  Between those steps, `$C1D4E4-$C1D50C` binary-searches the group's static sorted word list using live `D1`, returning the pointer-table index used at `$C1D43A`. For accepted records, the first word is an even byte offset equal to twice the threshold count; it is followed by that many threshold words and then a same-sized long-pointer table. The JSON inventory preserves this exact decoded layout.

The trace executes **41** group selections; **14** reach a template stream before returning.  Entries without a stream either take a gate/exit path in this trace or lack an observed stream before the next helper entry.

| Frame | workspace band | selector index | call-time row key | table word | static group record | row thresholds / live key / selected index | first stream byte |
| ---: | --- | ---: | --- | --- | --- | --- | --- |
|  | $C48390 | `$0013` | $0011 | $C423B6 | $C42642 |  |  |
|  | $C48990 | `$0013` | $0010 | $C423B6 | $C42642 |  |  |
|  | $C48F90 | `$0013` | $000F | $C423B6 | $C42642 |  |  |
|  | $C49590 | `$0012` | $0012 | $C423B4 | $C42642 |  |  |
|  | $C49B90 | `$0011` | $0012 | $C423B2 | $C42622 | 5 / $0012 / 4 | $C42B46 |
|  | $C4A190 | `$0012` | $0011 | $C423B4 | $C42642 |  |  |
|  | $C4A790 | `$0012` | $0010 | $C423B4 | $C42642 |  |  |
|  | $C4AD90 | `$0012` | $000F | $C423B4 | $C42642 |  |  |
|  | $C4B390 | `$0011` | $0011 | $C423B2 | $C42622 |  |  |
|  | $C4B990 | `$0011` | $000F | $C423B2 | $C42622 | 5 / $000F / 2 | $C42B66 |
|  | $C4BF90 | `$0010` | $0011 | $C423B0 | $C42608 |  |  |
|  | $C4C590 | `$0010` | $0010 | $C423B0 | $C42608 | 4 / $0010 / 3 | $C42BD4 |
|  | $C4CB90 | `$0010` | $000F | $C423B0 | $C42608 | 4 / $000F / 2 | $C42706 |
|  | $C4D190 | `$0011` | $0010 | $C423B2 | $C42622 | 5 / $0010 / 3 | $C42646 |
|  | $C48390 | `$0013` | $0011 | $C423B6 | $C42704 |  |  |
|  | $C48990 | `$0013` | $0010 | $C423B6 | $C42704 |  |  |
|  | $C48F90 | `$0013` | $000F | $C423B6 | $C42704 |  |  |
|  | $C49590 | `$0012` | $0012 | $C423B4 | $C42704 |  |  |
|  | $C49B90 | `$0011` | $0012 | $C423B2 | $C426F0 | 3 / $0012 / 2 | $C42B3E |
|  | $C4A190 | `$0012` | $0011 | $C423B4 | $C42704 |  |  |
|  | $C4A790 | `$0012` | $0010 | $C423B4 | $C42704 |  |  |
|  | $C4AD90 | `$0012` | $000F | $C423B4 | $C42704 |  |  |
|  | $C4B390 | `$0011` | $0011 | $C423B2 | $C426F0 |  |  |
|  | $C4B990 | `$0011` | $000F | $C423B2 | $C426F0 | 3 / $000F / 0 | $C427B4 |
|  | $C4BF90 | `$0010` | $0011 | $C423B0 | $C426DC |  |  |
|  | $C4C590 | `$0010` | $0010 | $C423B0 | $C426DC | 3 / $0010 / 2 | $C42788 |
|  | $C4CB90 | `$0010` | $000F | $C423B0 | $C426DC | 3 / $000F / 1 | $C4274A |
|  | $C4D190 | `$0011` | $0010 | $C423B2 | $C426F0 | 3 / $0010 / 1 | $C427C8 |
|  | $C48390 | `$0041` | $0041 | $C42412 | $C4288E |  |  |
|  | $C48990 | `$0041` | $0042 | $C42412 | $C4288E |  |  |
|  | $C48F90 | `$0042` | $0040 | $C42414 | $C42896 |  |  |
|  | $C49590 | `$0043` | $0040 | $C42416 | $C428A4 |  |  |
|  | $C49B90 | `$0042` | $0041 | $C42414 | $C42896 | 2 / $0041 / 1 | $C429D0 |
|  | $C4A190 | `$0042` | $0042 | $C42414 | $C42896 |  |  |
|  | $C4A790 | `$0042` | $0043 | $C42414 | $C42896 |  |  |
|  | $C4AD90 | `$0043` | $0041 | $C42416 | $C428A4 | 2 / $0041 / 0 | $C429DE |
|  | $C4B390 | `$0043` | $0043 | $C42416 | $C428A4 |  |  |
|  | $C4B990 | `$0044` | $0041 | $C42418 | $C428B2 |  |  |
|  | $C4BF90 | `$0044` | $0042 | $C42418 | $C428B2 | 8 / $0042 / 6 | $C4298A |
|  | $C4C590 | `$0044` | $0043 | $C42418 | $C428B2 |  |  |
|  | $C4CB90 | `$0043` | $0042 | $C42416 | $C428A4 | 2 / $0042 / 1 | $C42B5C |

The static group records and their selected streams are a stronger upstream boundary than the mutable workspace: the same path later reaches the static-to-workspace copy contract.  The selector index is not yet tied to a player world coordinate, distance, course cell, or visual LOD state, so none of those meanings are assigned here.  The two-stage static selector plus live search key resembles a two-axis lookup structure, but resemblance is not sufficient to call it a world-map grid.

See [the workspace-template inventory](workspace_template_copies.md) for the copied records and their later placement outputs.
