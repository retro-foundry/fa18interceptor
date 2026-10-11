# Source boundaries inside long native C candidates

Updated 2026-10-01. Evidence: original translated instructions on sealed native
`demo01`, with optional CSV observations in `port/machine/bus.c` and
`port/machine/machine.c`. No cycle charges or activation changes are proposed
by these measurements.

## Reproduce

The trace uses an inclusive low address and exclusive high address. Rows read
CPU and machine storage directly, without extra bus reads or cycle charges.
`instruction` observes the boundary before the opcode; `flow_target` observes
a completed branch/call/return after its deferred fetches. `event_before` and
`event_after` bracket chipset service, and `frame_end` records a service that
ends the execution slice. `log_mode=1` marks sandboxed observations, excluded
by the summarizer. CPU registers, SR, pending interrupt masks, blit count,
frame/line, current cycle and next event accompany each row.

For the whole demo region probe:

```powershell
$env:FA18_BOUNDARY_TRACE = 'build/recomp/region_boundaries_demo01.csv'
$env:FA18_BOUNDARY_RANGE = 'C2B042-C2B3B4'
./build/recomp/fa18_recomp.exe --state captures/native/demo01/state.bin `
  --input captures/native/demo01/input.fa18in --to-end `
  --rom local/system/kick13.rom --ports off `
  --ram-out build/recomp/ram_boundary_demo01.bin
Remove-Item Env:FA18_BOUNDARY_TRACE
Remove-Item Env:FA18_BOUNDARY_RANGE
python tools/recomp/summarize_boundary_trace.py `
  build/recomp/region_boundaries_demo01.csv --entry C2B05A `
  --returns C2B3AA C2B3B2 --out build/recomp/region_boundaries_demo01.json
```

For early renderer calls, repeat with `--frames 600`, the following ranges,
and unique CSV/output paths:

| Range | Entry | Returns |
| --- | --- | --- |
| C2AB34-C2AFFA | C2AB34 | C2AFF8 |
| C2AB34-C2AFFA | C2AB5A | C2AFF8 |
| C2FD8C-C2FF48 | C2FD8C | C2FF3A C2FF44 |

## Observed live calls

Costs run from the entry boundary to the completed return. They include child
calls, bus waits and interrupt work. They differ from sandbox-only instruction
costs and must not be used as fixed replacement charges.

| Entry | Window | Completed calls | Cycle range | Mean cycles | Calls spanning frames | Calls with an interrupt observed inside the selected range |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| C2B05A | complete demo | 17 | 20,214-47,376 | 37,633 | 14 | 14 |
| C2AB34 | first 600 frames | 22 | 146,286-1,690,130 | 443,381 | 22 | 15 |
| C2AB5A | first 600 frames | 5 | 267,236-393,668 | 316,221 | 5 | 4 |
| C2FD8C | first 600 frames | 22 | 69,026-145,272 | 99,301 | 20 | 7 |

The complete traced demo ended at 20,833 frames, 4,892 iterations and 555,658
blits; its final RAM/register artifact matched the sealed SHA-256. The map
and active-plane 600-frame traces ended with identical execution statistics:
85,261,216 CPU cycles, 4,815 blits and 2,185 iterations.

Region probe's first interrupted call starts at frame 19,443. At elapsed
2,560 cycles, service at `$C2B19E` changes PC to `$FC0D14`, SP to `$C7FFFA`
and SR to $2308. The call finally returns in frame 19,444 after 38,500 cycles.
Later interrupts enter at `$C2B0C4`, `$C2B0CA`, `$C2B0CC`, `$C2B0CE`,
`$C2B0D0`, `$C2B0E0`, `$C2B0EE`, `$C2B15C`, `$C2B1B0`, `$C2B1B6`,
`$C2B1B8` and `$C2B1F2`, at different stages of segment arithmetic.

For the wide map, early calls starting at frames 299, 327, 349, 366 and 384
each include 416 blits. A charge at their completed return cannot preserve
that sequence's intermediate display state.

No `frame_end` row occurs inside the selected routine ranges in these traces.
The call start/end frames show frame crossings while a child or interrupt is
executing outside the filter. Thus the reported interrupt counts are lower
bounds for the entire parent call, and zero filtered frame-end rows is not
evidence that a call stayed in one frame.

## Artifacts

Ignored files under `build/recomp/` can be regenerated with the commands above.

| CSV | SHA-256 |
| --- | --- |
| region_boundaries_demo01.csv | 17b658188b414489fa7d328328eda632202a51e4be8e025c70a2f4f412690295 |
| map_boundaries_demo01_600.csv | 5af4485812c2a3cea930d327da619d2b7467ef6a886294a8909e7abf671c7fd4 |
| active_planes_boundaries_demo01_600.csv | 0b3c674901cb02870d0609a03afa300eff4930b5915bec88ff47bdd41dd22656 |

## Required next implementation

The recorded register refactor establishes completed-call contracts only;
its observation hooks are not resumable execution boundaries. Before any
activation, introduce explicit C continuation state that survives both an
interrupt and the end of `fa18_machine_run_frame`. Keep the translated
interrupt return from dispatching into a generated routine's middle after
the C path has already performed its writes. A call's polygon children must
likewise suspend without being repeated.

Use these CSVs to choose source checkpoints, measure their path-dependent
instruction and bus costs, and reproduce the order of record writes,
BLTSIZE writes, chipset service and interrupt delivery. First validate the
region probe's interrupt at `$C2B19E`, then its other segment-stage entries;
extend the same mechanism to map polygon submission and active-plane busy
polling. Require full live ON RGB444 equality before registering the batch.
