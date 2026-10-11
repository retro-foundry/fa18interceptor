# `$C2AD80` segment-68 map packet directory lookup

Classification: **byte-exact, traced two-dimensional static packet selector**.

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
`$C42E52` in 28, while five additional targets occur once each. `$C42E6A`
begins with the negative longword `$FFFF0800`; the byte-exact `$C2AEFC` entry
tests that loaded longword and immediately branches to its rejection exit.
Those 31 cells are therefore proven no-packet selections for this directory.
The other targets have non-negative entry longwords, but remain unclassified
as terrain, a detail level, or a global map boundary.

The full 64-cell static directory and the twelve observed accesses are in the
[segment-68 directory inventory](../data/run003_m_map_segment68_directory.md),
with a [colour-coded selector grid](../plots/run003_m_map_segment68_directory.png)
for visual inspection.

The complete selector slice is now byte-exact at `$C2AD80-$C2AE59`.
`D2` indexes a signed two-byte control pair at `$C29F00`; each byte is added
to the caller's row/column minima and checked against the corresponding
maxima. The mode word at `-$3E(A6)` selects a 16-byte or 64-byte row stride
before the 16-bit relative offset is read from the caller-supplied base in
`-$34(A6)`. A positive offset becomes `A3`; a non-positive offset restarts the
record walker after publishing status `$40`. Thus packet selection is a
bounded relative-offset lookup, not an ordinary header-pointer table.

The same source slice preserves the 18-word `$C2ADF8-$C2AE1B` detail-limit
lookup immediately before `$C2AE1C` prepares the coordinate terms consumed by
the later visibility/pair-transform code. `$C2AE5A` independently constrains
its lookup index to 0--17. This table is renderer detail support; it is not
terrain elevation or an LOD map.

Across the sealed run003, run035, and run037 map traces, `$C2AD80` reaches
selector values 0--24 at `$C29F00-$C29F30`. Their signed byte pairs comprise
every combination in the local `{-2,-1,0,1,2} × {-2,-1,0,1,2}` lattice. This
is scenario-backed evidence for a 5×5 local selector stencil. It does not
prove `$C29F32` is a table boundary, map-cell size, global orientation, or
physical terrain extent. See the [observed byte-pair inventory](../data/m_map_selector_byte_pairs.md).

Joining each observed selector entry to the first `$C2AEFC` packet-header setup
before the next selector entry accounts for 77 of 104 entries. All 25 signed
pair offsets appear in that join; an individual offset can resolve to multiple
static headers in different sampled frames/scenarios. This is expected for a
local state-dependent lookup and rejects treating the pair as a fixed global
coordinate. The 27 bounded misses only show that no packet setup was reached
in that trace interval. See the [selector-to-header join](../data/m_map_selector_pair_header_join.md).

The 16-byte normal-stride prefix is not the only observed selector base.
Across the four map traces, `$C2ADCE` reads 44 unique table slots from both
`$C42CA8` and `$C42E6C`; the latter contributes 28 observed slots, including
eight targets rejected immediately at `$C2AEFC`. This is direct evidence of a
second static packet-directory region used by the same selector. The recorded
table slots do not yet establish world-cell dimensions, ordering, or terrain
meaning. See the [observed directory-access inventory](../data/m_map_observed_directory_accesses.md).

The mode word is dynamically exercised both ways: paired trace/snapshot reads
show zero (16-byte rows) in run003 and run037 map draws, and non-zero
(64-byte rows) in both run035 map traces. Both bases occur under the observed
modes where reached. This is live alternative-layout evidence, not yet a
physical-distance LOD result because the captures also differ in transition
and map state. See the [selector-stride inventory](../data/m_map_selector_modes.md).

The immediately preceding control walker has a separate, explicit
threshold-based detail gate at [`$C2AD00`](c2ad00_map_control_record_detail_gate.md).
It changes record fields before this lookup, but has not been correlated to a
line-versus-polygon primitive replacement.
This proves a local two-dimensional packet selector, not global world axes,
physical cell size, complete terrain coverage, or LOD.

Authority: byte-exact
[`select_map_packet_relative_offset.asm`](../../source_amiga/observed/select_map_packet_relative_offset.asm),
the run003/run035 M-map traces, and the directory inventory above.
