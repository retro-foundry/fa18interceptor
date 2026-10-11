# `$C21490`: four-point projected-record preparation

Classification: **runtime-backed generic projection-record preparation**.

`source_amiga/observed/prepare_four_point_projected_record.asm` is byte exact
for `$C21490-$C214FF`.

## Contract

The stream at `A2` supplies a selector and an offset into `$C48390`; the
selector is published to `$C45954`.  Unless the current depth at `$C45A78` is
less than `-$80`, six words from that table are copied to `$C4BF90` as a
four-point workspace.  A following three-word point is then adjusted by the
incoming `D5-D7` delta and appended.  The helper rejects when the high-bit
AND of the three adjusted depth words is negative.  Otherwise it delegates to
`$C2469E`, preserving `A1`, `A2`, and `A5` around that call.

## Runtime anchor

The run031 frame-12,000 Golden Gate no-input trace calls this helper from the
`$C1F942` record-dispatch route before `$C2159E` and the counted `$C211DC`
projection-record submitter.  This demonstrates that it is part of the live
scene stream, but neither its data-table ownership nor its screen geometry
has been assigned a landmark-specific name.
