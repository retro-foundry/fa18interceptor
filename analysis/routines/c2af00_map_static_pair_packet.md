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
