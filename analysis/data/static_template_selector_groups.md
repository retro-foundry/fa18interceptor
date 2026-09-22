# Static template-selector groups

Classification: **scenario-backed static selector dataflow**.  This records which `$C42390` table entries selected template streams in one bounded replay; it is not a decoded world grid, an LOD table, or a terrain mesh.

Authority: `build/run033_placement_bulk_404_trace/trace.jsonl`.  At `$C1D400` the helper doubles `D0`; `$C1D402` adds the signed word at `$C42390+D0*2` to that table base; `$C1D406` tests the resulting static group record.  On the observed accepted path, `$C1D426` passes a bit gate, `$C1D43A` resolves a stream pointer, and `$C1D442` begins the byte stream consumed by the workspace copier.

The trace executes **27** group selections; **10** reach a template stream before returning.  Entries without a stream either take a gate/exit path in this trace or lack an observed stream before the next helper entry.

| Frame | workspace band | selector index | table word | static group record | bit gate passed | first stream byte |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | $C48390 | `$0012` | $C423B4 | $C42704 | False |  |
| 1 | $C48990 | `$0012` | $C423B4 | $C42704 | False |  |
| 1 | $C48F90 | `$0012` | $C423B4 | $C42704 | False |  |
| 1 | $C49590 | `$0011` | $C423B2 | $C426F0 | True | $C42B3E |
| 1 | $C49B90 | `$0010` | $C423B0 | $C426DC | True |  |
| 1 | $C4A190 | `$0011` | $C423B2 | $C426F0 | True |  |
| 1 | $C4A790 | `$0011` | $C423B2 | $C426F0 | True | $C427C8 |
| 1 | $C4AD90 | `$0011` | $C423B2 | $C426F0 | True | $C427B4 |
| 1 | $C4B390 | `$0010` | $C423B0 | $C426DC | True |  |
| 1 | $C4B990 | `$0010` | $C423B0 | $C426DC | True | $C4274A |
| 1 | $C4BF90 | `$000F` | $C423AE | $C426CE | True |  |
| 1 | $C4C590 | `$000F` | $C423AE | $C426CE | True |  |
| 1 | $C4CB90 | `$000F` | $C423AE | $C426CE | True | $C4272E |
| 1 | $C4D190 | `$0010` | $C423B0 | $C426DC | True | $C42788 |
| 3 | $C48390 | `$0041` | $C42412 | $C4288E | True |  |
| 3 | $C48990 | `$0041` | $C42412 | $C4288E | True |  |
| 3 | $C48F90 | `$0042` | $C42414 | $C42896 | True |  |
| 3 | $C49590 | `$0043` | $C42416 | $C428A4 | True |  |
| 3 | $C49B90 | `$0042` | $C42414 | $C42896 | True | $C429D0 |
| 3 | $C4A190 | `$0042` | $C42414 | $C42896 | True |  |
| 3 | $C4A790 | `$0042` | $C42414 | $C42896 | True |  |
| 3 | $C4AD90 | `$0043` | $C42416 | $C428A4 | True | $C429DE |
| 3 | $C4B390 | `$0043` | $C42416 | $C428A4 | True |  |
| 3 | $C4B990 | `$0044` | $C42418 | $C428B2 | True |  |
| 3 | $C4BF90 | `$0044` | $C42418 | $C428B2 | True | $C4298A |
| 3 | $C4C590 | `$0044` | $C42418 | $C428B2 | True |  |
| 3 | $C4CB90 | `$0043` | $C42416 | $C428A4 | True | $C42B5C |

The static group records and their selected streams are a stronger upstream boundary than the mutable workspace: the same path later reaches the static-to-workspace copy contract.  The selector index is not yet tied to a player world coordinate, distance, course cell, or visual LOD state, so none of those meanings are assigned here.

See [the workspace-template inventory](workspace_template_copies.md) for the copied records and their later placement outputs.
