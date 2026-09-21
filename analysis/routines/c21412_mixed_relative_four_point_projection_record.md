# `$C21412`: mixed relative four-point projection-record preparation

Classification: **runtime-backed generic projection-record preparation**.

`source_amiga/observed/prepare_mixed_relative_four_point_projection_record.asm`
is byte exact for `$C21412-$C2148F`.

## Contract

The stream supplies a selector and three table offsets. The first and last
triples are copied directly to the four-point workspace at `$C4BF90`; the
middle two are made relative to the triple selected by the second stream
offset. The helper ANDs all four resulting depth words and calls `$C2469E`
only if the result is nonnegative. It preserves `A1`, `A2`, and `A5` around
that projection call.

## Runtime anchor

The run031 frame-12,000 Golden Gate control block enters this helper through
selector `$8040` with payload `$C35762` (trace index 1085). This establishes
the live record format but not an individual visual bridge-element label.
