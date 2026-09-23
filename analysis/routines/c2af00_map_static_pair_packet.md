# `$C2AF00` map static-pair packet transform

Classification: **scenario-backed static-packet-to-polygon-workspace path**.

During preparation of the pending `M`-map page, direct entries from
`$C2AEFC` to `$C2AF00` begin with `A3` in verified original Hunk 68,
`$C42CA8-$C444F7`; all captured packet and pair reads are within that same
6,224-byte segment. It has zero non-relocation byte differences from the
original executable in the resolved runtime mapping. The routine reads its source through `A3`, applies
the live fixed-point display matrix rooted at `$C45BDA`, and writes only
mutable `$C4BFxx` polygon-input workspace. `$C246A0` consumes that workspace;
its projected results reach `$C2FF48` on observed packets.

The bounded map trace contains 26 such direct packet entries and 55 completed
transform batches. The transform loop reads 353 exact signed two-word source
pairs at `$C2AF9C/$C2AF9E`; all 55 batches reach the `$C2AFE2 -> $C246A0`
display-stage call. The direct header is only captured on 26 batches because
the remaining batches resume through an enclosing packet path. The packet
source/header fields and every consumed pair
are retained in the [static-packet inventory](../data/run003_m_map_polygon_static_packets.md).

Each packet begins with a signed longword.  `$C2AF40` tests the culling/detail
result in `D7`: zero retains the inline stream immediately after that header;
non-zero replaces `A3` with the header longword before reading the next count.
This is a real two-stream selection point.  The selection condition is driven
by the preceding visibility/detail preparation, but it has not been tied to a
physical distance or to a specific LOD representation.  In particular, some
observed packet headers point at their own inline stream, so the two routes
need not differ.

Of the 26 directly entered headers in the bounded run003 trace, 8 have a
header pointer different from `header + 4`; the other 18 point straight to
their inline stream.  At every one of those 26 entries, the trace reaches
`$C2AF40` with `D7 = 0` and then reaches `$C2AF46` with `A3 = header + 4`.
Thus the run003 map visibly exercises only the inline route.  The eight
distinct alternate streams are static candidates for a detail/visibility
variant, but none is dynamically selected by this evidence.

The later run035 M-map appearance trace supplies the missing dynamic half:
of its 18 direct packet entries, 10 reach `$C2AF40` with non-zero `D7` and
then reach `$C2AF46` with `A3` equal to the packet header's alternate pointer.
The other 8 take the inline route.  By contrast, its sealed stable M-map
trace has 6 direct entries, all with `D7 = 0` and the inline route.  This
proves the alternate stream is a live renderer variant.  It does not make it
distance LOD: the appearance and stable captures differ in transition/control
state as well as any possible map scale or viewing variables.

Live state sampling at `$C2AF40` sharpens that qualification.  The run035
appearance state produces 12 inline and 12 alternate samples over three
passes; its metric is `1113`, `1118`, then `1124` (all below `$C80`).  Its
alternate selections occur only with the non-zero alternate-mode word.  The
stable state produces 31 inline and zero alternate samples, with metric
`196608`, including samples whose alternate-mode word is non-zero.  This
directly ties the route to the renderer's threshold metric and mode state;
it is strong evidence for map display/detail scaling or transition behavior,
but still not a flight-world-distance measurement.

The metric is no longer opaque: `$C2AAD2` loads the current projection-depth
longword at `$C45A78`, negates it, and `$C2AB0C` stores the result (or its
optional scale-normalized form) at `-$28(A6)`.  `$C2AD00` then applies its
`$400`/`$C80` detail bands to that exact field.  The alternate-stream choice
is therefore **projection-depth driven**.  This establishes a depth LOD-style
mechanism for map packet geometry; it does not by itself establish that the
depth is physical flight-world distance rather than the M-map renderer's own
camera/scale depth.

Upstream, `$C1C636` publishes `$C45A78` from the shifted `D1` element of a
transformed coordinate triple, as documented in the
[projection-component publisher](c1c5e0_projection_component_publish.md).

The exact route, header, and consumed-pair evidence is retained in the
[run035 appearance inventory](../data/run035_m_map_polygon_static_packets.md)
and [run035 stable inventory](../data/run035_m_map_stable_polygon_static_packets.md).
The live selector samples are in the
[appearance state](../data/run035_m_map_appearance_packet_runtime_state.md)
and [stable state](../data/run035_m_map_stable_packet_runtime_state.md).
Across the four bounded inventories, the
[segment-68 trace-coverage report](../data/m_map_segment68_trace_coverage.md)
records 24 distinct direct packet headers, 28 selected streams, and 387 unique
coordinate pairs (24.87% of the segment's bytes as exact pair payload). Live
`$C2AF40` selector samples add two headers without a direct `$C2AF00` trace:
`$C43FD0` and `$C440BC`. Together the 26 live headers structurally expose 34
inline/alternate streams and 489 pair records (31.43% payload coverage); those
additional records are reachable-format data, not a claim that they were
rendered. See the [static stream inventory](../data/static_m_map_packet_streams.md).
The immutable packet payload is explicitly
[two-dimensional](../data/m_map_packet_source_dimensionality.md); its third
renderer workspace component is computed, not read as source elevation.
The run003 trace-state arithmetic calculation records all 353 source-pair to
three-word workspace calculations, including instruction matrix/translation
inputs and the six-byte `A5` output stride within every completed batch. Its
calculated third word ranges from `384` to `6144` in this bounded draw; the
source loop establishes transform depth rather than a stored map-height field.
See the [pair-transform output
inventory](../data/run003_m_map_pair_transform_outputs.md).
The depth-metric producer is byte-exact in
[`prepare_map_depth_detail_metric.asm`](../../source_amiga/observed/prepare_map_depth_detail_metric.asm).
For the four headers observed on both routes, the
[inline/alternate comparison](../data/run003_run035_map_packet_variant_comparison.md)
shows distinct source ranges and reduced first-batch pair counts for three
of them (`13→7`, `9→8`, and `6→3`).
The accompanying [raw-coordinate sheet](../plots/run003_m_map_static_pair_packets.png)
is a visual inspection aid only; it preserves read order without inferring
closed faces or screen/world alignment.
The [OBJ line export](../exports/run003_m_map_static_pair_batches.obj) provides
the same batches to 3-D viewers using a zero middle axis only as a carrier.

This establishes that `$C4BFxx` workspace records are derived from immutable
map-mode packet input. It does **not** decode the packet's full grammar, prove
that every pair is terrain/coastline rather than an overlay, place the pairs
in global flight coordinates, or establish that the bounded 26 packets are
the complete map.

Authority: sealed `captures/run003`,
`build/run003_m_map_appearance_trace/{trace.jsonl,slow.bin}`, and:

```text
python scripts/inventory_map_polygon_static_packets.py \
  --trace build/run003_m_map_appearance_trace/trace.jsonl \
  --slow build/run003_m_map_appearance_trace/slow.bin \
  --output analysis/data/run003_m_map_polygon_static_packets.json
```

The header/stream selection is reconstructed exactly in
[`select_map_packet_stream_variant.asm`](../../source_amiga/observed/select_map_packet_stream_variant.asm).
