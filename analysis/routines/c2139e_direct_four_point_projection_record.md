# `$C2139E`: direct four-point projection-record preparation

Classification: **runtime-backed generic projection-record preparation**.

`source_amiga/observed/prepare_direct_four_point_projection_record.asm` is
byte exact for `$C2139E-$C21411`.

## Contract

The control stream supplies one selector and four table offsets. Three table
triples are copied directly into the four-point workspace at `$C4BF90`; the
fourth is made relative to the first selected table triple. The AND of the
four output depth words rejects the workspace if negative, otherwise `$C2469E`
projects it while `A1`, `A2`, and `A5` are preserved.

## Runtime anchor

The run031 frame-12,000 Golden Gate control block invokes this helper from
selector `$803C` at payload `$C35756` (trace index 689). Its live use proves
the packet format, not a named bridge-subcomponent identity.
