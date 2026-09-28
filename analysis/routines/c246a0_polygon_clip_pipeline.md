# `$C246A0-$C24CFD`: polygon clip pipeline

Authority: `build/run075_frame382_c246_stage/`, captured during the ordinary
run075 render-page preparation pass. The invocation begins at `$C246A0` and
returns to `$C2AFE8` after 3,129 instructions.

The source reads 13 triples from `$C4BF94` with zero coordinate shift and
leaves 14 triples at `$C4B990` before `$C24CFE` projects and submits them.
The traced path enters the outer loop, both tuple-cache children, and all four
post-loop closures. Its result is source-order clipping against the four
planes `y=z`, `-y=z`, `x=z`, and `-x=z`; the first and final crossings are
retained at the tail rather than rotating the polygon at the closing edge.

`polygon_clip_pipeline.{c,h}` composes those bounded leaves with the source
word-width rounding rule. Its contract uses the observed 13 input triples and
checks the exact 14-tuple `$C4B990` result. It is a producer for the existing
`$C24CFE` projection/submission path, not yet a game scheduler or page hook.
