# `$C35932`: mixed transform/control stream boundary

Classification: **trace-proven mixed static packet; not a contiguous vertex array**.

The bounded `$C1F4AC` trace enters with `A1=$C35932` and transforms exactly
seven consecutive triples at `$C35932-$C3595B` into `$C48390-$C483B9`.  The
loop ends after the seventh store with `A1=$C3595C`.

`$C3595C` is then read as a signed mode/control word at `$C1F58A`, not as a
coordinate.  In the observed mode, `$C1F598` reads one further triple starting
at `$C3595E`; subsequent execution advances through the mixed packet and
reaches `$C1F6F8` with `A1=$C35996`, where the renderer/control route begins.

The data boundary is therefore:

| Range | Observed role |
| --- | --- |
| `$C35932-$C3595B` | seven direct source triples |
| `$C3595C` | mode/control word |
| `$C3595E-$C35963` | conditionally transformed triple in the sampled mode |
| `$C35964-$C35995` | control/renderer stream; not classified as raw vertices |

This explains why the `$C35932 -> $C355D8` result is correctly documented as a
renderer-bounded bridge face-family component rather than an exportable
contiguous static mesh.  Any future extractor must preserve the control stream
and trace its mode alternatives instead of reading `$C35932-$C35995` as a
simple triple table.

Authority: `build/run031_frame12000_c35932_following_trace_v2/trace.jsonl`,
instructions 0--994.
