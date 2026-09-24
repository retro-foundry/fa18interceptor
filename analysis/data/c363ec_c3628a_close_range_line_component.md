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

## Run034 filled-face path

The sealed run034 near-terrain checkpoint independently reaches `$C1F4AC`
with source `$C363EC`; its bounded interval ends at the next matrix source
after 3,968 instructions. Within that one interval, `$C2469E` is entered and
the later `$C24D60 -> $C2FF48 -> $C301F6` submission path executes. Before
the submit wrapper, `$C24D10` and `$C24D58/$C24D5A` build projected pairs in
the mutable `$C4B390` workspace. The renderer then reads that workspace for
the polygon list.

This proves that the source-bounded near-terrain work associated with
`$C363EC` can feed a filled-face renderer path as well as its previously
observed `$C366D4` line record. `$C4B390` remains a transient projected
workspace, not exported source geometry. The trace does not identify the
resulting face with the user-described grey city region, nor does it show that
the component alone selects a distance/LOD tier.

## Status

This is an exportable **unnamed static line component**: preserve the four
source triples, the matrix transform, and the `$C366D4` line record; exclude
`$C48390` as mutable output.  It is observed only in the close-range matrix
inventory, so it is a concrete range-specific component lead, not proof of a
Golden Gate LOD level or of a semantic object identity.

Authority: `build/run031_frame11850_c363ec_transform_trace/trace.jsonl` and
`build/run031_frame11850_golden_gate_close_line_submissions_64f/line_submissions.json`.
The additional face-path authority is
`build/run034_near_c363ec_source_interval/trace.jsonl` (not versioned; it is
reproducible with `scripts/trace_matrix_source_interval.py` from the sealed
run034 checkpoint).
