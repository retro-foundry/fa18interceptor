# `$C363EC -> $C3628A/$C366D4`: close-range line component

At the sealed run031 frame-11,850 overhead Golden Gate checkpoint,
`$C363EC` enters the alternate matrix path at `$C1F4AC` with
`A3=$C48390`.  Four immutable triples at `$C363EC-$C36403` are transformed
into workspace slots 0--3.  The next walker entry has `A1=$C36406` and
`A3=$C483A8`, proving the four-triple boundary; `$C36404-$C36405` is the
following non-coordinate control word and is not coordinate data.

`$C1F708` loads the static controller `$C36288`, which advances to
`$C3628A`.  Its line dispatch reaches `$C212B0` with `A5=$C3628A` and
`A2=$C366D4`.  That immutable record decodes exactly as:

| Selector | Renderer-observed segments | Workspace slots |
| ---: | --- | --- |
| 8 | 0--1; 2--3 | `$C48390-$C483A2` |

The four source triples are `[724, 172, -724]`, `[-724, 172, 724]`,
`[0, 416, 0]`, and `[0, 0, -1]`.  The transformed segments recur five times
in the 64-frame close checkpoint window under the same static line context.

## Status

This is an exportable **unnamed static line component**: preserve the four
source triples, the matrix transform, and the `$C366D4` line record; exclude
`$C48390` as mutable output.  It is observed only in the close-range matrix
inventory, so it is a concrete range-specific component lead, not proof of a
Golden Gate LOD level or of a semantic object identity.

Authority: `build/run031_frame11850_c363ec_transform_trace/trace.jsonl` and
`build/run031_frame11850_golden_gate_close_line_submissions_64f/line_submissions.json`.
