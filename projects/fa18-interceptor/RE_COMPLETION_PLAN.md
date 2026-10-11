# F/A-18 analysis entry points

This project supports original-game investigation and evidence for the
[native checkout](native/README.md). The decompilation method, proof ladder,
report templates and capture-retention policy are maintained in the
[framework process manual](../../REVERSE_ENGINEERING.md).

## Start from the question and existing case evidence

| Investigation | Starting evidence |
| --- | --- |
| A loaded address or code version | [Hunk inventory](analysis/hunk_inventory.json), [resolved map](analysis/hunk_runtime_resolved.json), [mapping report](analysis/hunk_runtime_mapping.md). |
| A global/record field | [Memory map](analysis/memory_map.md), callers and consumers in [routine reports](analysis/routines/). |
| A model/geometry boundary | [Model geometry findings](analysis/model_geometry_boundaries.md) and its per-Hunk evidence. |
| Missing source representation | [Observed slices](source_amiga/README.md), [coverage report](analysis/coverage.json), [Ghidra coverage](analysis/ghidra_function_coverage.md) and the matching raw exports. |
| Gameplay, transitions or sound | The corresponding scenario/milestone in [analysis/](analysis/) and its preserved original recording. |

Choose a concrete unknown from the current native work or evidence report.
Capture its original trigger/caller under a declared bound, update the field or
routine contract, then qualify a replacement in the native repository.
Unvisited paths remain open; static names and historical counts do not close them.

The user's accepted scope does not require a single uninterrupted campaign;
repeat individual mission scenarios and investigate transitions as needed.
Moving residual byte-array state into named C structures is a separate cleanup
step outside the playable-completion goal. Preserve these decisions when
continuing qualification rather than reviving an older checklist.

## Historical plan

[RE_COMPLETION_PLAN.history.md](RE_COMPLETION_PLAN.history.md) preserves the old
plan byte for byte. Its unavailable-Ghidra assumption, translated runtime paths,
routine counts and porting order describe that earlier checkout. The runtime
and CPU comparison harness were retired here; original capture/analysis tools
and the native submodule are the active paths. See [README.md](README.md).
