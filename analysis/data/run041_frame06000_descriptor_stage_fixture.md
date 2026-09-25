# Run041 frame-6000 returned descriptor-stage fixtures

Classification: **scenario-backed producer-to-renderer dataflow**. These are
two complete returned invocations from the sealed run041 frame-6000 trace;
they do not identify a world object, landmark, or LOD policy.

Authority: `build/run041_frame06000_12f_reverse_stack_trace/trace.jsonl`.

The trace contains adjacent placement-loop iterations whose `$C1CC70` stores
select descriptor control fields, then whose indirect calls run through
`$C1EE14`, both the immutable-triple transform branch and the control-stream
walker, before returning to `$C1CC88`:

| Trace rows | Placement-loop field at `$C1CC70` | `$C1EF10` cursor written to `$C45A36` | `$C1F4AC` source `A1` | `$C1F708`/`$C1F70E` walker `A5` | Return |
| --- | --- | --- | --- | --- | --- |
| 5,395--6,941, frame 6,001 | `$C22410` | `$C35594` | `$C35B56` | `$C35594` | `$C1CC88` |
| 7,000--9,147, frames 6,001--6,002 | `$C22424` | `$C355CC` | `$C35B80` | `$C355CC` | `$C1CC88` |

For the first fixture, the decisive instructions are: `$C1CC70` stores its
selected control field at trace row 5,395; the caller enters `$C1EE14` at
5,401; `$C1EF10` publishes `A2=$C35594` at 5,487; `$C1F4AC` reads from
`A1=$C35B56` at 5,553; and `$C1F708` loads `$C45A36` before row 5,769 shows
`A5=$C35594`. The enclosing call returns to `$C1CC88` at row 6,941.

The second fixture has the same exact producer/consumer shape with distinct
control and immutable-source addresses. It reaches `$C1EE14` at row 7,006,
publishes `$C355CC` at 7,092, uses `$C35B80` at `$C1F4AC` row 7,158, loads
that cursor into `A5` at row 7,374, and returns at 9,147.

This joins placement-loop selection to both downstream renderer-input
families in an uninterrupted replay interval. It does not establish that the
two address families are a mesh, that either field owns the same object across
frames, or what visible feature they represent.
