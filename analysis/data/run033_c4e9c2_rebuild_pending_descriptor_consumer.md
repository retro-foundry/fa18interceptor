# Run033 `$C4E9C2` rebuild and missing later descriptor consumer

Classification: **scenario-backed producer reconstruction with a bounded
missing consumer edge**. This document records trace order, rather than
equating repeated use of the same mutable record address with a producer to
an earlier consumer.

Authority: `build/run033_placement_bulk_404_thirtyframe_trace/trace.jsonl`
and `analysis/data/workspace_template_copies_404_426.json`.

The primary placement-loop record at `$C4E9C2` is a particularly high-value
join point: it is reused by the run041 primary selector fixtures and its
descriptor is `$C22408`. In the run033 30-frame trace, the following two
facts are both directly observed:

| Trace order | Event | Evidence |
| ---: | --- | --- |
| 165,488--165,497 | Earlier primary-loop pass uses `$C4E9C2`: `$C1CC60` has `A0=$C4E9CE`; `$C1CC70` loads descriptor fields through `A1=$C22410`; `$C1CC86` dispatches `$C1EE14`. | `$C1EE14` then publishes cursor `$C35598`, selects immutable source `$C35BAA`, and loads walker `A5=$C35598`. |
| 208,345 | `$C1D488` copies static entry `$C4264D` (`$20`, `$0980`, `$0580`) into mutable workspace cell `$C4A970`. | Recorded by the template-copy inventory. |
| 215,807--215,816 | `$C1DD36` reads that workspace cell and `$C1DD54` begins rebuilding runtime record `$C4E9C2`. | `A2=$C4E9C2` at the selector store. |
| later in the same builder iteration | The builder writes the descriptor pointer `$C22408` and the three generated coordinate words `(76, 0, 428)` into that record. | Existing producer inventory links the record, descriptor, and generated tuple. |

The record's descriptor field is therefore not a static label attached by
analysis: it is rebuilt from the workspace path, and the primary loop later
consumes this record layout on other observed intervals. However, the only
`$C1EE14` invocation for `$C4E9C2` in this trace occurs *before* this shown
rebuild. It cannot prove that the reconstructed contents fed that earlier
stage invocation. The trace ends without a post-rebuild `$C1CC86`/
`$C1EE14` consumer for this record.

## Exact highest-priority next capture

Continue from a state-changing placement refresh, retaining uninterrupted
trace rows from the `$C1D488` copy and `$C1DC1C-$C1E0B0` builder stores for
`A2=$C4E9C2` through the next primary-loop `$C1CC60/$C1CC70` use of
`A0=$C4E9CE`, provided `$C1CC86` dispatches `$C1EE14`, `$C1ED3C`, or
`$C1ED4C`. Preserve the target's return and its `$C1EF10`, `$C1F4AC`, and
`$C1F6F8` rows. This one per-record chain joins the producer to a shared
descriptor-stage consumer and is more informative than another isolated
side-effect-free helper or a broad call census.

No physical coordinate, object identity, terrain-topology, or LOD claim is
made here.
