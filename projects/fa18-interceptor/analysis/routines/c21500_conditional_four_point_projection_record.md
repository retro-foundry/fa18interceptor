# `$C21500`: conditional four-point projection-record preparation

Classification: **runtime-backed generic projection-record preparation**.

`source_amiga/observed/prepare_conditional_four_point_projection_record.asm`
is byte exact for `$C21500-$C2159D`.

## Contract

After selecting a `$C48390` record-table item, this variant rejects when the
current depth at `$C45A78` is below `-$80`. It builds four points at
`$C4BF90` using two table triples and the incoming `D5-D7` delta, with the
last point represented relative to the saved initial delta. A four-way AND of
the output depth words gates submission to `$C2469E`.

`A1`, `A2`, and `A5` are preserved around the projection call. Rejections
return `D0=0`.

## Runtime anchor

The run031 frame-12,000 Golden Gate control block reaches this routine through
selector `$8028` at payload `$C3576C` (trace index 1138). It is an active
generic scene-record handler, not a proven individual bridge member.
