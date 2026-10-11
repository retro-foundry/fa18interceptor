# Run033 `$C4E9C2` builder-to-descriptor-stage chain

Classification: **scenario-backed, same-record producer-to-consumer chain**.
The trace rows below are in one continuous run033 frame-5,250 placement-refresh
trace; they establish dataflow and stage routing, not object identity, physical
coordinates, terrain topology, or LOD.

Authority: `build/run033_frame05250_placement_thirtyframe_trace/trace.jsonl`.
All indexes below refer to that trace's monotonic instruction index, rather
than the frontend frame counter.

| Trace index | Frame | Event | Direct evidence |
| ---: | ---: | --- | --- |
| 27,710 | 5,253 | Static template copied to workspace | `$C1D488` has `A5=$C4264D` and `A2=$C4A370`. |
| 30,112 | 5,253 | Same workspace cell read for record construction | `$C1DD36` has `A3=$C4A370`, with output cursor `A2=$C4E9C2`. |
| 30,121 | 5,253 | Record selector written | `$C1DD54` writes to `A2=$C4E9C2`. |
| 30,135 | 5,253 | Descriptor pointer written | `$C1DD88` writes `A1=$C22408` to `$C4E9C4`. |
| 30,227--30,233 | 5,253 | Generated placement tuple written | `$C1E04A/$C1E054/$C1E05E` write `$2980`, `$0000`, `$1580` to `$C4E9C8/$C4E9CA/$C4E9CC`: signed tuple `(10624, 0, 5504)`. |
| 282,503 | 5,278 | Same record's primary-loop value publication | `$C1CC60` has `A0=$C4E9CE`, the record's `+12` cursor. |
| 282,507--282,512 | 5,278 | Descriptor fields and indirect stage target consumed | `$C1CC70` reads through `A1=$C22410`; `$C1CC86` dispatches `A2=$C1EE14`. |
| 282,602 | 5,278 | Stage publishes selected control cursor | `$C1EF10` uses `$C35568` and writes `$C35598` to `$C45A36`. |
| 282,668 | 5,278 | Immutable transform input selected | `$C1F4AC` reads `A1=$C35BAA`. |
| 282,767--282,770 | 5,278 | Walker starts on the selected cursor | `$C1F6F8` is reached and `$C1F70E` loads `A5=$C35598`. |
| 283,109 | 5,278 | Stage return verified | The independently rerun call collector matches the `$C1CC86` stack frame and returns to `$C1CC88`; this invocation emits one `$C2FA7E` line entry. |

This directly resolves the prior record-provenance gap: the same `$C4E9C2`
record constructed from workspace `$C4A370` is later consumed by the primary
placement loop and reaches the conditional descriptor/control stage. The
stage's cursor, immutable triple, and walker values also match the existing
run041 `$C22408`-family fixtures, corroborating this record/descriptor family
across separate replay windows.

The static source `$C4264D` is a reusable template, not a fixed placement:
the independently traced earlier run033 window constructs `$C4E9C2/$C22408`
from the same template with a different tuple. This chain therefore proves a
template/workspace/record/stage pipeline; it does not assign a world object or
geographical interpretation to the entry.

## Next boundary unlocked

The next highest-leverage unknown is the upstream selector path that chooses
which static entry `$C1D488` copies into workspace (including `$C1D10C`,
`$C1D3F4`, and `$C1D442` where applicable). It can explain many generated
placement records at once. The local builder-to-renderer boundary no longer
needs another capture unless a distinct builder branch appears.
