# `$C2159E`: conditional three-point projection-record preparation

Classification: **runtime-backed generic projection-record preparation**.

`source_amiga/observed/prepare_conditional_three_point_projection_record.asm`
is byte exact for `$C2159E-$C21689`.

## Contract

The helper selects a record-table item from the `A2` control stream and
initializes the three-point workspace at `$C4BF92`. It compares two record
components through the current component base and workspace transform fields,
then chooses one of two table-derived three-point layouts based on the signed
cross-term and bit 15 of the supplied record offset. Both layouts reject when
their relevant depth-word AND is negative; accepted layouts call `$C2469E`.

It saves and restores `A1`, `A2`, and `A5` across this projection call. A
rejection returns `D0=0`.

## Runtime anchor

The frame-12,000 run031 Golden Gate trace reaches this helper from `$C1F942`,
between `$C21490` and `$C211DC`. It establishes a generic transformed
three-point path used by that landmark frame; no direct output-to-pixel match
yet supports a bridge-specific geometry label.
