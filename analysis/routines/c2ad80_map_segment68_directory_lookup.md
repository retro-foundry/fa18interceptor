# `$C2AD80` segment-68 map packet directory lookup

Classification: **traced two-dimensional static packet selector**.

In the map-page renderer, `$C2ADBA` supplies `$C42CA8` as the selected static
base. `$C2ADC0` doubles the live X term; `$C2ADC2` multiplies the live Y term
by 16; `$C2ADCE` reads a 16-bit relative offset at their sum from that base;
and `$C2ADD4` adds the offset to recover the packet pointer in `A3`.

The prefix `$C42CA8-$C42D27` is therefore an 8-column, 8-row relative-offset
directory with a 16-byte row stride. In the bounded run003 map build, twelve
selector cells, with both terms in `3..6`, are observed. They select packet
starts including `$C42DC4`, `$C42DFC`, and `$C42E1A`, which subsequently enter
the static pair-transform/display path.

The directory has seven distinct targets: `$C42E6A` occurs in 31 cells and
`$C42E52` in 28, while five additional targets occur once each. This is direct
static reuse evidence; it does not identify either repeated target as empty,
terrain, a detail level, or a global map boundary.

The full 64-cell static directory and the twelve observed accesses are in the
[segment-68 directory inventory](../data/run003_m_map_segment68_directory.md),
with a [colour-coded selector grid](../plots/run003_m_map_segment68_directory.png)
for visual inspection.

The immediately preceding control walker has a separate, explicit
threshold-based detail gate at [`$C2AD00`](c2ad00_map_control_record_detail_gate.md).
It changes record fields before this lookup, but has not been correlated to a
line-versus-polygon primitive replacement.
This proves a local two-dimensional packet selector, not global world axes,
physical cell size, complete terrain coverage, or LOD.
