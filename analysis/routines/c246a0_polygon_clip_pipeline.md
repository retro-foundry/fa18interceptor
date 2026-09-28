# `$C246A0-$C24CFD`: polygon clip pipeline

Authority: `build/run075_frame382_c246_stage/`, captured during the ordinary
run075 render-page preparation pass. The invocation begins at `$C246A0` and
returns to `$C2AFE8` after 3,129 instructions.

The source reads 13 triples from `$C4BF94` with coordinate shift two
(`D3=2` at entry) and leaves 14 triples at `$C4B990` before `$C24CFE`
projects and submits them.
The traced path enters the outer loop, both tuple-cache children, and all four
post-loop closures. Its result is source-order clipping against the four
planes `y=z`, `-y=z`, `x=z`, and `-x=z`; the first and final crossings are
retained at the tail rather than rotating the polygon at the closing edge.

`polygon_clip_pipeline.{c,h}` composes those bounded leaves with the source
word-width rounding rule. Its contract uses the observed 13 input triples and
checks the exact 14-tuple `$C4B990` result. It is a producer for the existing
`$C24CFE` projection/submission path, not yet a game scheduler or page hook.

`polygon_display_pipeline.{c,h}` now composes that result through the exact
positive-depth projection and `$C2FF48` DMA-enabled list wrapper. Its same
witness checks all 14 projected pairs at `$C4B390`, including `(0,89)`,
`(319,91)`, and the closing `(0,179)`, and confirms one `$8400` DMA write.

`map_packet_polygon_display.{c,h}` is the direct `$C2AFE2` callback adapter:
it copies the preceding map packet's three-word records into the clip input,
passes the caller-owned live shift, and routes the result to the already bound
`$C2FF48` submission state.  It does not select a page, retain a shift, or
schedule rendering.

Its page-level contract binds the existing submission callbacks to a native
five-plane page with the traced renderer values (bound 144, vertical 89,
horizontal 0). The raw 13-record, shift-two witness produces the 14 projected
pairs, one `$8400` DMA write, page-blitter work, and nonzero lower-plane bytes.
